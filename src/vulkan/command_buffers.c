#include <assert.h>
#include <stdlib.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_command_buffers(Vulkan *self) {
    assert(self != NULL);

    VkCommandBufferAllocateInfo infos[VULKAN_COMMAND_POOL_COUNT] = {
        // VULKAN_COMMAND_POOL_GRAPHIC
        (VkCommandBufferAllocateInfo) {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = NULL,
            .commandPool = self->command_pools[VULKAN_COMMAND_POOL_GRAPHIC],
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = VULKAN_GRAPHIC_COMMAND_COUNT,
        },
    };

    for (int i = 0; i < VULKAN_COMMAND_POOL_COUNT; ++i) {
        self->command_buffers[i] = calloc(infos[i].commandBufferCount, sizeof(VkCommandBuffer));
        if (self->command_buffers[i] == NULL) return VULKAN_CODE_ALLOCATE_ERROR;
        if (vulkan_throw_api(vkAllocateCommandBuffers(self->device, infos + i, self->command_buffers[i]))) return VULKAN_CODE_CREATE_COMMAND_BUFFERS_ERROR;
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_command_buffers(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_COMMAND_POOL_COUNT; ++i) {
        free(self->command_buffers[i]);
    }
}
