#include <assert.h>

#include "backend/wayland/wayland.h"

const WlInterface *wl_get_xdg_base_interface(WaylandLoader *loader) {
    static const WlInterface *types[] = {
        NULL, // primitive type
        NULL, // xdg surface
        NULL, // wl surface
    };

    static const WlMessage methods[] = {
        (WlMessage) { .name = "destroy", .signature = "", .types = types + 0 },
        (WlMessage) { 0 },
        (WlMessage) { .name = "get_xdg_surface", .signature = "no", .types = types + 1 },
        (WlMessage) { .name = "pong", .signature = "u", .types = types + 0 },
    };

    static const WlMessage events[] = {
        (WlMessage) { .name = "ping", .signature = "u", .types = types + 0 },
    };

    static const WlInterface interface = {
        .name = "xdg_wm_base",
        .version = WL_XDG_BASE_VERSION,
        .method_count = sizeof(methods) / sizeof(WlMessage),
        .methods = methods,
        .event_count = sizeof(events) / sizeof(WlMessage),
        .events = events,
    };

    static bool cached = false;
    if (cached) return &interface;

    assert(loader != NULL);

    types[1] = wl_get_xdg_surface_interface(loader);
    types[2] = loader->surface_interface;

    cached = true;
    return &interface;
}

const WlInterface *wl_get_xdg_surface_interface(WaylandLoader *loader) {
    static const WlInterface *types[] = {
        NULL, // primitive type
        NULL, // primitive type
        NULL, // primitive type
        NULL, // primitive type
        NULL, // xdg toplevel
    };

    static const WlMessage methods[] = {
        (WlMessage) { .name = "destroy", .signature = "", .types = types + 0 },
        (WlMessage) { .name = "get_toplevel", .signature = "n", .types = types + 4 },
        (WlMessage) { 0 },
        (WlMessage) { .name = "set_window_geometry", .signature = "iiii", .types = types + 0 },
        (WlMessage) { .name = "ack_configure", .signature = "u", .types = types + 0 },
    };

    static const WlMessage events[] = {
        (WlMessage) { .name = "configure", .signature = "u", .types = types + 0 },
    };

    static const WlInterface interface = {
        .name = "xdg_surface",
        .version = WL_XDG_SURFACE_VERSION,
        .method_count = sizeof(methods) / sizeof(WlMessage),
        .methods = methods,
        .event_count = sizeof(events) / sizeof(WlMessage),
        .events = events,
    };

    static bool cached = false;
    if (cached) return &interface;

    assert(loader != NULL);

    types[4] = wl_get_xdg_toplevel_interface(loader);

    cached = true;
    return &interface;
}

const WlInterface *wl_get_xdg_toplevel_interface(WaylandLoader *loader) {
    static const WlInterface *types[] = {
        NULL, // primitive type
        NULL, // primitive type
        NULL, // primitive type
    };

    static const WlMessage methods[] = {
        (WlMessage) { .name = "destroy", .signature = "", .types = types + 0 },
        (WlMessage) { 0 },
        (WlMessage) { .name = "set_title", .signature = "s", .types = types + 0 },
        (WlMessage) { .name = "set_app_id", .signature = "s", .types = types + 0 },
        (WlMessage) { 0 },
        (WlMessage) { 0 },
        (WlMessage) { 0 },
        (WlMessage) { 0 },
        (WlMessage) { 0 },
        (WlMessage) { 0 },
        (WlMessage) { 0 },
        (WlMessage) { 0 },
        (WlMessage) { 0 },
        (WlMessage) { 0 },
    };

    static const WlMessage events[] = {
        (WlMessage) { .name = "configure", .signature = "iia", .types = types + 0 },
        (WlMessage) { .name = "close", .signature = "", .types = types + 0 },
        (WlMessage) { .name = "configure_bounds", .signature = "ii", .types = types + 0 },
        (WlMessage) { .name = "wm_capabilities", .signature = "a", .types = types + 0 },
    };

    static const WlInterface interface = {
        .name = "xdg_toplevel",
        .version = WL_XDG_TOPLEVEL_VERSION,
        .method_count = sizeof(methods) / sizeof(WlMessage),
        .methods = methods,
        .event_count = sizeof(events) / sizeof(WlMessage),
        .events = events,
    };

    static bool cached = false;
    if (cached) return &interface;

    assert(loader != NULL);

    cached = true;
    return &interface;
}
