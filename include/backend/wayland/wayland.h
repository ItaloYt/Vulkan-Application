#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef void *WlProxy;
typedef void *WlDisplay;
typedef void *WlRegistry;
typedef void *WlListener;
typedef void *WlCompositor;
typedef void *WlXdgBase;
typedef void *WlSurface;
typedef void *WlXdgSurface;
typedef void *WlXdgToplevel;
typedef void *WlCallback;

struct WlArray {
    size_t size;
    size_t capacity;
    void *data;
};
typedef struct WlArray *WlArray;

typedef struct WlInterface WlInterface;

struct WlMessage {
    const char *name;
    const char *signature;
    const WlInterface **types;
};
typedef struct WlMessage WlMessage;

struct WlInterface {
    const char *name;
    int version;
    int method_count;
    const WlMessage *methods;
    int event_count;
    const WlMessage *events;
};

struct WlRegistryListener {
    void (*global)(void *user, WlRegistry registry, unsigned name, const char *interface, unsigned version);
    void (*global_remove)(void *user, WlRegistry registry, unsigned name);
};
typedef struct WlRegistryListener WlRegistryListener;

struct WlXdgBaseListener {
    void (*ping)(void *user, WlXdgBase xdg_base, unsigned serial);
};
typedef struct WlXdgBaseListener WlXdgBaseListener;

struct WlXdgSurfaceListener {
    void (*configure)(void *user, WlXdgSurface xdg_surface, unsigned serial);
};
typedef struct WlXdgSurfaceListener WlXdgSurfaceListener;

struct WlXdgToplevelListener {
    void (*configure)(void *user, WlXdgToplevel xdg_toplevel, int width, int height, WlArray states);
    void (*close)(void *user, WlXdgToplevel xdg_toplevel);
    void (*configure_bounds)(void *user, WlXdgToplevel xdg_toplevel, int width, int height);
    void (*wm_capabilities)(void *user, WlXdgToplevel xdg_toplevel, WlArray capabilities);
};
typedef struct WlXdgToplevelListener WlXdgToplevelListener;

// We need to ensure this is 32-bit
enum WlProxyOpcode : unsigned {
    // WlDisplay
    WL_PROXY_OPCODE_DISPLAY_GET_REGISTRY = 1,
    
    // WlRegistry
    WL_PROXY_OPCODE_REGISTRY_BIND = 0,

    // WlCompositor
    WL_PROXY_OPCODE_COMPOSITOR_CREATE_SURFACE = 0,

    // WlSurface
    WL_PROXY_OPCODE_SURFACE_DESTROY = 0,
    WL_PROXY_OPCODE_SURFACE_COMMIT = 6,

    // XdgBase
    WL_PROXY_OPCODE_XDG_BASE_DESTROY = 0,
    WL_PROXY_OPCODE_XDG_BASE_GET_SURFACE = 2,
    WL_PROXY_OPCODE_XDG_BASE_PONG = 3,

    // XdgSurface
    WL_PROXY_OPCODE_XDG_SURFACE_DESTROY = 0,
    WL_PROXY_OPCODE_XDG_SURFACE_GET_TOPLEVEL = 1,
    WL_PROXY_OPCODE_XDG_SURFACE_SET_GEOMETRY = 3,
    WL_PROXY_OPCODE_XDG_SURFACE_ACK_CONFIGURE = 4,

    // XdgToplevel
    WL_PROXY_OPCODE_XDG_TOPLEVEL_DESTROY = 0,
    WL_PROXY_OPCODE_XDG_TOPLEVEL_SET_TITLE = 2,
    WL_PROXY_OPCODE_XDG_TOPLEVEL_SET_APP_ID = 3,
};
typedef enum WlProxyOpcode WlProxyOpcode;

enum {
    WL_REGISTRY_VERSION = 1,
    WL_COMPOSITOR_VERSION = 1,
    WL_SURFACE_VERSION = 1,
    WL_XDG_BASE_VERSION = 1,
    WL_XDG_SURFACE_VERSION = 1,
    WL_XDG_TOPLEVEL_VERSION = 1,
};

enum {
    WL_MARSHAL_FLAG_DESTROY = 1,
};

struct WaylandLoader {
    void *lib;

    WlDisplay (*connect_display)(const char *name);
    void (*disconnect_display)(WlDisplay display);

    WlProxy (*marshal_flags)(WlProxy proxy, WlProxyOpcode opcode, const WlInterface *interface, unsigned version, unsigned flags, ...);
    unsigned (*get_version)(WlProxy proxy);

    WlProxy (*create)(WlProxy factory, const WlInterface *inteface);
    void (*destroy)(WlProxy proxy);

    int (*add_listener)(WlProxy proxy, WlListener listener, void *user);

    int (*roundtrip_display)(WlDisplay display);

    const WlInterface *registry_interface;
    const WlInterface *compositor_interface;
    const WlInterface *surface_interface;
};
typedef struct WaylandLoader WaylandLoader;

const WlInterface *wl_get_xdg_base_interface(WaylandLoader *loader);
const WlInterface *wl_get_xdg_surface_interface(WaylandLoader *loader);
const WlInterface *wl_get_xdg_toplevel_interface(WaylandLoader *loader);

// WlProxy
static inline void wl_destroy_proxy(WaylandLoader *loader, WlProxy proxy) {
    loader->destroy(proxy);
}

static inline int wl_add_proxy_listener(WaylandLoader *loader, WlProxy proxy, WlListener listener, void *user) {
    return loader->add_listener(proxy, listener, user);
}

// WlDisplay
static inline WlDisplay wl_connect_display(WaylandLoader *loader) {
    return loader->connect_display(NULL);
}

static inline void wl_disconnect_display(WaylandLoader *loader, WlDisplay display) {
    loader->disconnect_display(display);
}

static inline WlRegistry wl_get_display_registry(WaylandLoader *loader, WlDisplay display) {
    return loader->marshal_flags(display, WL_PROXY_OPCODE_DISPLAY_GET_REGISTRY, loader->registry_interface, WL_REGISTRY_VERSION, 0, NULL);
} 

static inline int wl_roundtrip_display(WaylandLoader *loader, WlDisplay display) {
    return loader->roundtrip_display(display);
}

// WlRegistry
static inline WlProxy wl_bind_interface(WaylandLoader *loader, WlRegistry registry, unsigned name, const WlInterface *interface, unsigned version) {
    return loader->marshal_flags(registry, WL_PROXY_OPCODE_REGISTRY_BIND, interface, version, 0, name, interface->name, version, NULL);
}

// WlCompositor
static inline WlSurface wl_create_surface(WaylandLoader *loader, WlCompositor compositor) {
    return loader->marshal_flags(compositor, WL_PROXY_OPCODE_COMPOSITOR_CREATE_SURFACE, loader->surface_interface, WL_SURFACE_VERSION, 0, NULL);
}

// WlSurface
static inline void wl_destroy_surface(WaylandLoader *loader, WlSurface surface) {
    (void) loader->marshal_flags(surface, WL_PROXY_OPCODE_SURFACE_DESTROY, NULL, WL_SURFACE_VERSION, WL_MARSHAL_FLAG_DESTROY);
}

static inline void wl_commit_surface(WaylandLoader *loader, WlSurface surface) {
    (void) loader->marshal_flags(surface, WL_PROXY_OPCODE_SURFACE_COMMIT, NULL, WL_SURFACE_VERSION, 0);
}

// WlXdgBase
static inline void wl_destroy_xdg_base(WaylandLoader *loader, WlXdgBase xdg_base) {
    (void) loader->marshal_flags(xdg_base, WL_PROXY_OPCODE_XDG_BASE_DESTROY, NULL, WL_XDG_BASE_VERSION, WL_MARSHAL_FLAG_DESTROY);
}

static inline WlXdgSurface wl_get_xdg_surface(WaylandLoader *loader, WlXdgBase xdg_base, WlSurface surface) {
    return loader->marshal_flags(xdg_base, WL_PROXY_OPCODE_XDG_BASE_GET_SURFACE, wl_get_xdg_surface_interface(loader), WL_XDG_SURFACE_VERSION, 0, NULL, surface);
}

static inline void wl_pong_xdg_base(WaylandLoader *loader, WlXdgBase xdg_base, unsigned serial) {
    (void) loader->marshal_flags(xdg_base, WL_PROXY_OPCODE_XDG_BASE_PONG, NULL, WL_XDG_BASE_VERSION, 0, serial);
}

// WlXdgSurface
static inline void wl_destroy_xdg_surface(WaylandLoader *loader, WlXdgSurface xdg_surface) {
    (void) loader->marshal_flags(xdg_surface, WL_PROXY_OPCODE_XDG_SURFACE_DESTROY, NULL, WL_XDG_SURFACE_VERSION, WL_MARSHAL_FLAG_DESTROY);
}

static inline WlXdgToplevel wl_get_xdg_toplevel(WaylandLoader *loader, WlXdgSurface xdg_surface) {
    return loader->marshal_flags(xdg_surface, WL_PROXY_OPCODE_XDG_SURFACE_GET_TOPLEVEL, wl_get_xdg_toplevel_interface(loader), WL_XDG_TOPLEVEL_VERSION, 0, NULL);
}

static inline void wl_set_xdg_surface_geometry(WaylandLoader *loader, WlXdgSurface xdg_surface, int x, int y, int width, int height) {
    (void) loader->marshal_flags(xdg_surface, WL_PROXY_OPCODE_XDG_SURFACE_SET_GEOMETRY, NULL, WL_XDG_SURFACE_VERSION, 0, x, y, width, height);
}

static inline void wl_ack_configure_xdg_surface(WaylandLoader *loader, WlXdgSurface xdg_surface, unsigned serial) {
    (void) loader->marshal_flags(xdg_surface, WL_PROXY_OPCODE_XDG_SURFACE_ACK_CONFIGURE, NULL, WL_XDG_SURFACE_VERSION, 0, serial);
}

// WlXdgToplevel
static inline void wl_destroy_xdg_toplevel(WaylandLoader *loader, WlXdgToplevel xdg_toplevel) {
    (void) loader->marshal_flags(xdg_toplevel, WL_PROXY_OPCODE_XDG_TOPLEVEL_DESTROY, NULL, WL_XDG_TOPLEVEL_VERSION, WL_MARSHAL_FLAG_DESTROY);
}

static inline void wl_set_xdg_toplevel_title(WaylandLoader *loader, WlXdgToplevel xdg_toplevel, const char *title) {
    (void) loader->marshal_flags(xdg_toplevel, WL_PROXY_OPCODE_XDG_TOPLEVEL_SET_TITLE, NULL, WL_XDG_TOPLEVEL_VERSION, 0, title);
}

static inline void wl_set_xdg_toplevel_app_id(WaylandLoader *loader, WlXdgToplevel xdg_toplevel, const char *id) {
    (void) loader->marshal_flags(xdg_toplevel, WL_PROXY_OPCODE_XDG_TOPLEVEL_SET_APP_ID, NULL, WL_XDG_TOPLEVEL_VERSION, 0, id);
}
