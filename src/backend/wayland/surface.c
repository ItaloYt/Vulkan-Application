#include <assert.h>
#include <stdio.h>

#include "core/math.h"
#include "backend/wayland/backend.h"

static void ping(Window *self, WlXdgBase xdg_base, unsigned serial);
static void configure(Window *self, WlXdgSurface xdg_surface, unsigned serial);
static void toplevel_configure(Window *self, WlXdgToplevel xdg_toplevel, int width, int height, WlArray states);
static void close(Window *self, WlXdgToplevel xdg_toplevel);
static void configure_bounds(Window *self, WlXdgToplevel xdg_toplevel, int width, int height);
static void wm_capabilities(Window *self, WlXdgToplevel xdg_toplevel, WlArray capabilities);

WaylandCode wayland_setup_surface(Window *self) {
    static WlXdgBaseListener xdg_base_listener = {
        .ping = (void *) ping,
    };

    static WlXdgSurfaceListener xdg_surface_listener = {
        .configure = (void *) configure,
    };

    static WlXdgToplevelListener xdg_toplevel_listener = {
        .configure = (void *) toplevel_configure,
        .close = (void *) close,
        .configure_bounds = (void *) configure_bounds,
        .wm_capabilities = (void *) wm_capabilities,
    };

    assert(self != NULL);

    WaylandBackend *const backend = self->backend;

    backend->surface = wl_create_surface(&backend->loader, backend->compositor);
    if (backend->surface == NULL) return WAYLAND_CODE_CREATE_SURFACE_ERROR;

    if (wl_add_proxy_listener(&backend->loader, backend->xdg_base, &xdg_base_listener, self) == -1) return WAYLAND_CODE_ADD_XDG_BASE_LISTENER_ERROR;

    backend->xdg_surface = wl_get_xdg_surface(&backend->loader, backend->xdg_base, backend->surface);
    if (backend->xdg_surface == NULL) return WAYLAND_CODE_GET_XDG_SURFACE_ERROR;

    if (wl_add_proxy_listener(&backend->loader, backend->xdg_surface, &xdg_surface_listener, self) == -1) return WAYLAND_CODE_ADD_XDG_SURFACE_LISTENER_ERROR;

    backend->xdg_toplevel = wl_get_xdg_toplevel(&backend->loader, backend->xdg_surface);
    if (backend->xdg_toplevel == NULL) return WAYLAND_CODE_GET_XDG_TOPLEVEL_ERROR;

    wl_set_xdg_toplevel_title(&backend->loader, backend->xdg_toplevel, "Vulkan Application");
    wl_set_xdg_toplevel_app_id(&backend->loader, backend->xdg_toplevel, "vkapp");

    if (wl_add_proxy_listener(&backend->loader, backend->xdg_toplevel, &xdg_toplevel_listener, self) == -1) return WAYLAND_CODE_ADD_XDG_TOPLEVEL_LISTENER_ERROR;
    
    wl_commit_surface(&backend->loader, backend->surface);

    if (wl_roundtrip_display(&backend->loader, backend->display) == -1) return WAYLAND_CODE_ROUNDTRIP_DISPLAY_ERROR;

    return WAYLAND_CODE_SUCCESS;
}

static void toplevel_configure(Window *self, WlXdgToplevel xdg_toplevel, int width, int height, WlArray states) {
    WaylandBackend *const backend = self->backend;

    self->width = (width == 0 ? (backend->width_bound == 0 ? 1000 : math_min_int(1000, backend->width_bound)) : width);
    self->height = (height == 0 ? (backend->height_bound == 0 ? 700 : math_min_int(700, backend->height_bound)) : height);
}

static void close(Window *self, WlXdgToplevel xdg_toplevel) {
    self->open = false;
}

static void configure_bounds(Window *self, WlXdgToplevel xdg_toplevel, int width, int height) {
    WaylandBackend *const backend = self->backend;

    backend->width_bound = width;
    backend->height_bound = height;
}

static void wm_capabilities(Window *self, WlXdgToplevel xdg_toplevel, WlArray capabilities) {

}

static void configure(Window *self, WlXdgSurface xdg_surface, unsigned serial) {
    static unsigned width = 0;
    static unsigned height = 0;

    WaylandBackend *const backend = self->backend;

    if (width != self->width || height != self->height) {
        width = self->width;
        height = self->height;

        wl_set_xdg_surface_geometry(&backend->loader, backend->xdg_surface, 0, 0, width, height);

        wayland_resize(self, width, height);
    }

    wl_ack_configure_xdg_surface(&backend->loader, xdg_surface, serial);
}

static void ping(Window *self, WlXdgBase xdg_base, unsigned serial) {
    WaylandBackend *const backend = self->backend;

    wl_pong_xdg_base(&backend->loader, xdg_base, serial);
}
