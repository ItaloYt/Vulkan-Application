#include <assert.h>
#include <dlfcn.h>
#include <string.h>

#define VK_USE_PLATFORM_WAYLAND_KHR
#include "backend/wayland/backend.h"

WindowCode wayland_init(Window *self) {
    assert(self != NULL);

    WaylandBackend *const backend = self->backend;

    backend->display = wl_connect_display(&backend->loader);
    if (
        (backend->display == NULL && wayland_throw(WAYLAND_CODE_CONNECT_DISPLAY_ERROR)) ||
        wayland_throw(wayland_setup_registry(self)) ||
        wayland_throw(wayland_setup_surface(self))
    ) {
        wayland_destroy(self);

        return WINDOW_CODE_BACKEND_ERROR;
    }

    self->open = true;

    return WINDOW_CODE_SUCCESS;
}

WindowCode wayland_update(Window *self) {
    WaylandBackend *const backend = self->backend;

    if (wl_roundtrip_display(&backend->loader, backend->display) == -1 && wayland_throw(WAYLAND_CODE_ROUNDTRIP_DISPLAY_ERROR)) return WINDOW_CODE_BACKEND_ERROR;

    return WINDOW_CODE_SUCCESS;
}

void wayland_destroy(Window *self) {
    assert(self != NULL);

    if (self->backend == NULL) return;

    WaylandBackend *const backend = self->backend;

    if (backend->xdg_toplevel != NULL) wl_destroy_xdg_toplevel(&backend->loader, backend->xdg_toplevel);
    if (backend->xdg_surface != NULL) wl_destroy_xdg_surface(&backend->loader, backend->xdg_surface);
    if (backend->xdg_base != NULL) wl_destroy_xdg_base(&backend->loader, backend->xdg_base);
    if (backend->surface != NULL) wl_destroy_surface(&backend->loader, backend->surface);
    if (backend->compositor != NULL) wl_destroy_proxy(&backend->loader, backend->compositor);
    if (backend->registry != NULL) wl_destroy_proxy(&backend->loader, backend->registry);
    if (backend->display != NULL) wl_disconnect_display(&backend->loader, backend->display);
    if (backend->loader.lib != NULL) (void) dlclose(backend->loader.lib);

    memset(backend, 0, sizeof(WaylandBackend));
    memset(self, 0, sizeof(Window));
}

void wayland_resize(Window *self, unsigned width, unsigned height) {
    assert(self != NULL);

    WaylandBackend *const backend = self->backend;

    for (int i = 0; i < WINDOW_CALLBACK_TYPE_COUNT; ++i) {
        if (backend->resizes[i] == NULL) continue;

        backend->resizes[i](backend->datas[i], self, width, height);
    }
}

void wayland_set_resize_callback(Window *self, WindowCallbackType type, WindowResizeCallback callback, void *data) {
    assert(self != NULL);
    assert(type >= WINDOW_CALLBACK_TYPE_USER && type < WINDOW_CALLBACK_TYPE_COUNT);

    WaylandBackend *const backend = self->backend;

    if (backend->resizes[type] != NULL) return;

    backend->resizes[type] = callback;
    backend->datas[type] = data;
}

unsigned wayland_get_vulkan_instance_extensions(Window *self, const char ***extensions) {
    static const char *exts[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
    };

    static const unsigned count = sizeof(exts) / sizeof(char *);

    *extensions = exts;

    return count;
}

VkResult wayland_create_vulkan_surface(Window *self, VkSurfaceKHR *surface, VkInstance instance, const VkAllocationCallbacks *allocator) {
    assert(self != NULL);
    assert(instance != NULL);

    WaylandBackend *const backend = self->backend;

    VkWaylandSurfaceCreateInfoKHR info = {
        .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .display = backend->display,
        .surface = backend->surface,
    };

    return vkCreateWaylandSurfaceKHR(instance, &info, allocator, surface);
}

VkResult wayland_get_vulkan_presentation_support(Window *self, bool *support, VkPhysicalDevice physical, unsigned index) {
    WaylandBackend *const backend = self->backend;

    *support = vkGetPhysicalDeviceWaylandPresentationSupportKHR(physical, index, backend->display);

    return VK_SUCCESS;
}
