#include <assert.h>
#include <string.h>

#include "backend/wayland/backend.h"

static void global(WaylandBackend *backend, WlRegistry registry, unsigned name, const char *interface, unsigned version);

WaylandCode wayland_setup_registry(Window *window) {
    static WlRegistryListener listener = {
        .global = (void *) global,
        .global_remove = NULL,
    };

    assert(window != NULL);

    WaylandBackend *const backend = window->backend;

    backend->registry = wl_get_display_registry(&backend->loader, backend->display);
    if (backend->registry == NULL) return WAYLAND_CODE_GET_REGISTRY_ERROR;
    if (wl_add_proxy_listener(&backend->loader, backend->registry, &listener, backend) == -1) return WAYLAND_CODE_ADD_REGISTRY_LISTENER_ERROR;

    if (wl_roundtrip_display(&backend->loader, backend->display) == -1) return WAYLAND_CODE_ROUNDTRIP_DISPLAY_ERROR;
    if (wayland_throw(backend->code)) return WAYLAND_CODE_BIND_GLOBALS_ERROR;

    if (backend->compositor == NULL) return WAYLAND_CODE_NO_COMPOSITOR_ERROR;
    if (backend->xdg_base == NULL) return WAYLAND_CODE_NO_XDG_BASE_ERROR;

    return WAYLAND_CODE_SUCCESS;
}

static void global(WaylandBackend *backend, WlRegistry registry, unsigned name, const char *interface_name, unsigned version) {
    if (backend->code != WAYLAND_CODE_SUCCESS) return;

    const WlInterface *interface = backend->loader.compositor_interface;

    if (strcmp(interface_name, interface->name) == 0) {
        if (version < WL_COMPOSITOR_VERSION) {
            backend->code = WAYLAND_CODE_INVALID_COMPOSITOR_VERSION_ERROR;
            return;
        }

        backend->compositor = wl_bind_interface(&backend->loader, registry, name, interface, WL_COMPOSITOR_VERSION);
        if (backend->compositor == NULL) {
            backend->code = WAYLAND_CODE_BIND_COMPOSITOR_ERROR;
            return;
        }

        return;
    }

    interface = wl_get_xdg_base_interface(&backend->loader);

    if (strcmp(interface_name, interface->name) == 0) {
        if (version < WL_XDG_BASE_VERSION) {
            backend->code = WAYLAND_CODE_INVALID_XDG_BASE_VERSION_ERROR;
            return;
        }

        backend->xdg_base = wl_bind_interface(&backend->loader, registry, name, interface, WL_XDG_BASE_VERSION);
        if (backend->xdg_base == NULL) {
            backend->code = WAYLAND_CODE_BIND_XDG_BASE_ERROR;
            return;
        }

        return;
    }
}
