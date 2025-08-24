#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>

#include "backend/wayland/backend.h"

static inline bool resolve_ref(void *sym, void *lib, const char *name) {
    *(void **) sym = dlsym(lib, name);

    return *(void **) sym == NULL;
}

WaylandCode wayland_load(Window *window) {
    static WaylandBackend backend = {0};

    assert(window != NULL);

    window->backend = &backend;

    backend.loader.lib = dlopen("libwayland-client.so", RTLD_LAZY);
    if (backend.loader.lib == NULL) return WAYLAND_CODE_LOAD_LIB_ERROR;
    if (
        resolve_ref(&backend.loader.connect_display, backend.loader.lib, "wl_display_connect") ||
        resolve_ref(&backend.loader.disconnect_display, backend.loader.lib, "wl_display_disconnect") ||
        resolve_ref(&backend.loader.marshal_flags, backend.loader.lib, "wl_proxy_marshal_flags") ||
        resolve_ref(&backend.loader.get_version, backend.loader.lib, "wl_proxy_get_version") ||
        resolve_ref(&backend.loader.create, backend.loader.lib, "wl_proxy_create") ||
        resolve_ref(&backend.loader.destroy, backend.loader.lib, "wl_proxy_destroy") ||
        resolve_ref(&backend.loader.add_listener, backend.loader.lib, "wl_proxy_add_listener") ||
        resolve_ref(&backend.loader.roundtrip_display, backend.loader.lib, "wl_display_roundtrip") ||

        resolve_ref(&backend.loader.registry_interface, backend.loader.lib, "wl_registry_interface") ||
        resolve_ref(&backend.loader.compositor_interface, backend.loader.lib, "wl_compositor_interface") ||
        resolve_ref(&backend.loader.surface_interface, backend.loader.lib, "wl_surface_interface")
    ) {
        wayland_destroy(window);

        return WAYLAND_CODE_RESOLVE_REF_ERROR;
    }

    window->init = wayland_init;
    window->update = wayland_update;
    window->destroy = wayland_destroy;

    window->set_resize_callback = wayland_set_resize_callback;

    window->get_vulkan_instance_extensions = wayland_get_vulkan_instance_extensions;
    window->create_vulkan_surface = wayland_create_vulkan_surface;
    window->get_vulkan_presentation_support = wayland_get_vulkan_presentation_support;

    return WAYLAND_CODE_SUCCESS;
}

bool wayland_throw(WaylandCode code) {
    switch (code) {
        case WAYLAND_CODE_SUCCESS: {
            return false;
        }

        case WAYLAND_CODE_UNDEFINED_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Undefined\n");
            return true;
        }

        case WAYLAND_CODE_LOAD_LIB_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to load library: %s\n", dlerror());
            return true;
        }

        case WAYLAND_CODE_RESOLVE_REF_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to resolve reference: %s\n", dlerror());
            return true;
        }

        case WAYLAND_CODE_CONNECT_DISPLAY_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to connect display\n");
            return true;
        }

        case WAYLAND_CODE_GET_REGISTRY_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to get registry\n");
            return true;
        }

        case WAYLAND_CODE_ADD_REGISTRY_LISTENER_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to add registry listener\n");
            return true;
        }

        case WAYLAND_CODE_ROUNDTRIP_DISPLAY_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to roundtrip display\n");
            return true;
        }

        case WAYLAND_CODE_INVALID_COMPOSITOR_VERSION_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Compositor version is smaller than supported\n");
            return true;
        }

        case WAYLAND_CODE_BIND_COMPOSITOR_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to bind compositor\n");
            return true;
        }

        case WAYLAND_CODE_BIND_GLOBALS_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to bind globals\n");
            return true;
        }

        case WAYLAND_CODE_BIND_XDG_BASE_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to bind xdg base\n");
            return true;
        }

        case WAYLAND_CODE_GET_XDG_SURFACE_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to get xdg surface\n");
            return true;
        }

        case WAYLAND_CODE_INVALID_XDG_BASE_VERSION_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Xdg base version is smaller than supported\n");
            return true;
        }

        case WAYLAND_CODE_NO_COMPOSITOR_ERROR: {
            (void) fprintf(stderr, "Wayland Error: No compositor interface\n");
            return true;
        }

        case WAYLAND_CODE_NO_XDG_BASE_ERROR: {
            (void) fprintf(stderr, "Wayland Error: No xdg base interface\n");
            return true;
        }

        case WAYLAND_CODE_CREATE_SURFACE_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to create compositor surface\n");
            return true;
        }

        case WAYLAND_CODE_ADD_XDG_BASE_LISTENER_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to add xdg base listener\n");
            return true;
        }

        case WAYLAND_CODE_ADD_XDG_SURFACE_LISTENER_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to add xdg surface listener\n");
            return true;
        }

        case WAYLAND_CODE_GET_XDG_TOPLEVEL_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to get xdg toplevel\n");
            return true;
        }

        case WAYLAND_CODE_ADD_XDG_TOPLEVEL_LISTENER_ERROR: {
            (void) fprintf(stderr, "Wayland Error: Failed to add xdg toplevel listener\n");
            return true;
        }
    }

    (void) fprintf(stderr, "Wayland Error: Invalid code %#02x\n", code);

    return true;
}
