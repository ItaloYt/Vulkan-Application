#pragma once

#include <stdbool.h>
#include <vulkan/vulkan.h>

typedef struct Window Window;

enum WindowCode {
    WINDOW_CODE_SUCCESS = 0,
    WINDOW_CODE_BACKEND_ERROR,
};
typedef enum WindowCode WindowCode;

enum WindowCallbackType {
    WINDOW_CALLBACK_TYPE_USER = 0,
    WINDOW_CALLBACK_TYPE_RENDERER,
    WINDOW_CALLBACK_TYPE_COUNT,
};
typedef enum WindowCallbackType WindowCallbackType;

typedef void (*WindowResizeCallback)(void *data, Window *window, unsigned width, unsigned height);

struct Window {
    void *backend;

    WindowCode (*init)(Window *self);
    WindowCode (*update)(Window *self);
    void (*destroy)(Window *self);

    void (*set_resize_callback)(Window *self, WindowCallbackType type, WindowResizeCallback callback, void *data);

    unsigned (*get_vulkan_instance_extensions)(Window *self, const char ***extensions);
    VkResult (*create_vulkan_surface)(Window *self, VkSurfaceKHR *surface, VkInstance instance, const VkAllocationCallbacks *allocator);
    VkResult (*get_vulkan_presentation_support)(Window *self, bool *support, VkPhysicalDevice physical, unsigned index);

    unsigned width, height;
    bool open;
};

WindowCode window_load_backend(Window **window);

bool window_throw(WindowCode code);
