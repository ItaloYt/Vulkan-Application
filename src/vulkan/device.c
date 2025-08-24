#include <assert.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_device(Vulkan *self) {
    assert(self != NULL);   

    VkDeviceCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .queueCreateInfoCount = (self->queue_indices[VULKAN_QUEUE_GRAPHIC] == self->queue_indices[VULKAN_QUEUE_PRESENT] ? VULKAN_QUEUE_COUNT - 1 : VULKAN_QUEUE_COUNT),
        .pQueueCreateInfos = (VkDeviceQueueCreateInfo[]) {
            (VkDeviceQueueCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = self->queue_indices[VULKAN_QUEUE_GRAPHIC] - 1,
                .queueCount = 1,
                .pQueuePriorities = (float[]) { 1 },
            },
            (VkDeviceQueueCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .queueFamilyIndex = self->queue_indices[VULKAN_QUEUE_PRESENT] - 1,
                .queueCount = 1,
                .pQueuePriorities = (float[]) { 1 },
            },
        },
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = (const char *[]) {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        },

        .pEnabledFeatures = &(VkPhysicalDeviceFeatures) {0},
    };

    if (vulkan_throw_api(vkCreateDevice(self->physical, &info, NULL, &self->device))) return VULKAN_CODE_CREATE_DEVICE_ERROR;

    for (int i = 0; i < VULKAN_QUEUE_COUNT; ++i) {
        vkGetDeviceQueue(self->device, self->queue_indices[i] - 1, 0, self->queues + i);
    }

    return VULKAN_CODE_SUCCESS;
}
