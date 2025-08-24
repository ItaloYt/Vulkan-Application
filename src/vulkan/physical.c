#include <assert.h>

#include "core/vulkan.h"

VulkanCode vulkan_load_physical(Vulkan *self, Window *window) {
    assert(self != NULL);
    assert(window != NULL);

    unsigned count;
    if (vulkan_throw_api(vkEnumeratePhysicalDevices(self->instance, &count, NULL))) return VULKAN_CODE_LOAD_PHYSICALS_ERROR;

    VkPhysicalDevice physicals[count];
    if (vulkan_throw_api(vkEnumeratePhysicalDevices(self->instance, &count, physicals))) return VULKAN_CODE_LOAD_PHYSICALS_ERROR;

    self->physical = physicals[0];

    vkGetPhysicalDeviceQueueFamilyProperties(self->physical, &count, NULL);

    VkQueueFamilyProperties families[count];
    vkGetPhysicalDeviceQueueFamilyProperties(self->physical, &count, families);

    for (unsigned i = 0; i < count && (self->queue_indices[VULKAN_QUEUE_GRAPHIC] == 0 || self->queue_indices[VULKAN_QUEUE_PRESENT]); ++i) {
        if (self->queue_indices[VULKAN_QUEUE_GRAPHIC] == 0 && families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            // We need to remeber to subtract 1 everytime we use a queue indice
            self->queue_indices[VULKAN_QUEUE_GRAPHIC] = i + 1;

        if (self->queue_indices[VULKAN_QUEUE_PRESENT] > 0) continue;

        bool is_presentation_supported = false;
        if (vulkan_throw_api(window->get_vulkan_presentation_support(window, &is_presentation_supported, self->physical, i))) return VULKAN_CODE_GET_PRESENTATION_SUPPORT_ERROR;

        if (!is_presentation_supported) continue;

        self->queue_indices[VULKAN_QUEUE_PRESENT] = i + 1;
    }

    for (unsigned i = 0; i < VULKAN_QUEUE_COUNT; ++i) {
        if (self->queue_indices[i] == 0) return VULKAN_CODE_NO_GRAPHIC_QUEUE_ERROR + i;
    }

    return VULKAN_CODE_SUCCESS;
}
