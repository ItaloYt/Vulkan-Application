#include <assert.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_instance(Vulkan *self, Window *window) {
    assert(self != NULL);
    assert(window != NULL);

    VkInstanceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &(VkApplicationInfo) {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = NULL,
            .pApplicationName = "Vulkan Application",
            .applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 0),
            .pEngineName = NULL,
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_0,
        },
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 0, // Later definition
        .ppEnabledExtensionNames = NULL, // Later definition
    };

    info.enabledExtensionCount = window->get_vulkan_instance_extensions(window, (void *) &info.ppEnabledExtensionNames);

    return (vulkan_throw_api(vkCreateInstance(&info, NULL, &self->instance)) ? VULKAN_CODE_CREATE_INSTANCE_ERROR : VULKAN_CODE_SUCCESS);
}
