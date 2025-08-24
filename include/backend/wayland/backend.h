#pragma once

#include <stdbool.h>

#include "core/window.h"
#include "wayland.h"

enum WaylandCode {
    WAYLAND_CODE_SUCCESS = 0,
    WAYLAND_CODE_UNDEFINED_ERROR, // Debug only
    WAYLAND_CODE_LOAD_LIB_ERROR,
    WAYLAND_CODE_RESOLVE_REF_ERROR,
    WAYLAND_CODE_CONNECT_DISPLAY_ERROR,
    WAYLAND_CODE_ROUNDTRIP_DISPLAY_ERROR,
    WAYLAND_CODE_GET_REGISTRY_ERROR,
    WAYLAND_CODE_ADD_REGISTRY_LISTENER_ERROR,
    WAYLAND_CODE_INVALID_COMPOSITOR_VERSION_ERROR,
    WAYLAND_CODE_INVALID_XDG_BASE_VERSION_ERROR,
    WAYLAND_CODE_BIND_COMPOSITOR_ERROR,
    WAYLAND_CODE_BIND_XDG_BASE_ERROR,
    WAYLAND_CODE_BIND_GLOBALS_ERROR,
    WAYLAND_CODE_NO_COMPOSITOR_ERROR,
    WAYLAND_CODE_NO_XDG_BASE_ERROR,
    WAYLAND_CODE_ADD_XDG_BASE_LISTENER_ERROR,
    WAYLAND_CODE_CREATE_SURFACE_ERROR,
    WAYLAND_CODE_GET_XDG_SURFACE_ERROR,
    WAYLAND_CODE_ADD_XDG_SURFACE_LISTENER_ERROR,
    WAYLAND_CODE_GET_XDG_TOPLEVEL_ERROR,
    WAYLAND_CODE_ADD_XDG_TOPLEVEL_LISTENER_ERROR,
};
typedef enum WaylandCode WaylandCode;

struct WaylandBackend {
    WaylandLoader loader;

    WindowResizeCallback resizes[WINDOW_CALLBACK_TYPE_COUNT];
    void *datas[WINDOW_CALLBACK_TYPE_COUNT];

    WlDisplay display;
    WlRegistry registry;
    WlCompositor compositor;
    WlXdgBase xdg_base;

    WlSurface surface;
    WlXdgSurface xdg_surface;
    WlXdgToplevel xdg_toplevel;

    WaylandCode code;

    unsigned width_bound;
    unsigned height_bound;
};
typedef struct WaylandBackend WaylandBackend;

WaylandCode wayland_load(Window *self);

WindowCode wayland_init(Window *self);
WindowCode wayland_update(Window *self);
void wayland_destroy(Window *self);

void wayland_resize(Window *self, unsigned width, unsigned height);
void wayland_set_resize_callback(Window *self, WindowCallbackType type, WindowResizeCallback callback, void *data);

unsigned wayland_get_vulkan_instance_extensions(Window *self, const char ***extensions);
VkResult wayland_create_vulkan_surface(Window *self, VkSurfaceKHR *surface, VkInstance instance, const VkAllocationCallbacks *allocator);
VkResult wayland_get_vulkan_presentation_support(Window *self, bool *support, VkPhysicalDevice physical, unsigned index);

WaylandCode wayland_setup_registry(Window *self);
WaylandCode wayland_setup_surface(Window *self);

bool wayland_throw(WaylandCode code);
