#include <assert.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_command_pools(Vulkan *self) {
    assert(self != NULL);

    VkCommandPoolCreateInfo infos[VULKAN_COMMAND_POOL_COUNT] = {
        (VkCommandPoolCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = NULL,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = self->queue_indices[VULKAN_QUEUE_GRAPHIC] - 1,
        },
    };

    for (int i = 0; i < VULKAN_COMMAND_POOL_COUNT; ++i) {
        if (vulkan_throw_api(vkCreateCommandPool(self->device, infos + i, NULL, self->command_pools + i))) return VULKAN_CODE_CREATE_COMMAND_POOLS_ERROR;
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_command_pools(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_COMMAND_POOL_COUNT; ++i) {
        vkDestroyCommandPool(self->device, self->command_pools[i], NULL);
    }
}
