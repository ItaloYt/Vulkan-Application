#include <assert.h>
#include <stdlib.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_command_buffers(Vulkan *self) {
    assert(self != NULL);

    const unsigned *count = vulkan_get_command_buffers_count(self);

    int total = 0;
    for (int i = 0; i < VULKAN_COMMAND_BUFFER_COUNT; ++i) {
        total += count[i];
    }

    self->command_buffers = calloc(total, sizeof(VkCommandBuffer));
    if (self->command_buffers == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

    VkCommandBufferAllocateInfo infos[VULKAN_COMMAND_POOL_COUNT] = {
        // VULKAN_COMMAND_POOL_GRAPHIC
        (VkCommandBufferAllocateInfo) {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = NULL,
            .commandPool = self->command_pools[VULKAN_COMMAND_POOL_GRAPHIC],
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = count[VULKAN_GRAPHIC_COMMAND_DRAW] + count[VULKAN_GRAPHIC_COMMAND_EXTRA],
        },
    };

    for (int i = 0, offset = 0; i < VULKAN_COMMAND_POOL_COUNT; ++i, offset += infos[i].commandBufferCount) {
        if (vulkan_throw_api(vkAllocateCommandBuffers(self->device, infos + i, self->command_buffers + offset))) return VULKAN_CODE_CREATE_COMMAND_BUFFERS_ERROR;
    }

    return VULKAN_CODE_SUCCESS;
}

VkCommandBuffer *vulkan_get_command_buffers(Vulkan *self, unsigned cmd) {
    assert(self != NULL);
    assert(cmd < VULKAN_COMMAND_BUFFER_COUNT);

    static unsigned offsets[VULKAN_COMMAND_BUFFER_COUNT] = {
        0,
    };

    static bool cached = false;
    if (cached) return self->command_buffers + offsets[cmd];

    const unsigned *count = vulkan_get_command_buffers_count(self);

    for (int i = 1; i < VULKAN_COMMAND_BUFFER_COUNT; ++i) {
        offsets[i] += offsets[i - 1] + count[i - 1];
    }

    cached = true;
    return self->command_buffers + offsets[cmd];
}

const unsigned *vulkan_get_command_buffers_count(Vulkan *self) {
    assert(self != NULL);

    static unsigned count[VULKAN_COMMAND_BUFFER_COUNT] = {
        VULKAN_FRAMES_IN_FLIGHT, // VULKAN_GRAPHIC_COMMAND_DRAW
        1, // VULKAN_GRAPHIC_COMMAND_EXTRA
    };

    static bool cached = false;
    if (cached) return count;

    cached = true;
    return count;
}

VulkanCode vulkan_copy_buffers(Vulkan *self, unsigned dst, unsigned src) {
    assert(self != NULL);
    assert(dst < VULKAN_BUFFER_COUNT);
    assert(src < VULKAN_BUFFER_COUNT);

    const VkCommandBuffer cmd = vulkan_get_command_buffers(self, VULKAN_GRAPHIC_COMMAND_EXTRA)[0];

    if (vulkan_throw_api(vkResetCommandBuffer(cmd, 0))) return VULKAN_CODE_RESET_COMMAND_BUFFER_ERROR;

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    if (vulkan_throw_api(vkBeginCommandBuffer(cmd, &begin_info))) return VULKAN_CODE_BEGIN_COMMAND_BUFFER_ERROR;

    VkBufferCopy regions[1] = {
        (VkBufferCopy) {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = self->buffer_sizes[src],
        },
    };

    vkCmdCopyBuffer(cmd, self->buffers[src], self->buffers[dst], 1, regions);

    if (vulkan_throw_api(vkEndCommandBuffer(cmd))) return VULKAN_CODE_END_COMMAND_BUFFER_ERROR;

    VkSubmitInfo submit_infos[1] = {
        (VkSubmitInfo) {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = NULL,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = NULL,
            .pWaitDstStageMask = NULL,
            .commandBufferCount = 1,
            .pCommandBuffers = (VkCommandBuffer[]) { cmd },
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = NULL,
        },
    };

    if (vulkan_throw_api(vkQueueSubmit(self->queues[VULKAN_QUEUE_GRAPHIC], 1, submit_infos, NULL))) return VULKAN_CODE_SUBMIT_QUEUE_ERROR;
    if (vulkan_throw_api(vkQueueWaitIdle(self->queues[VULKAN_QUEUE_GRAPHIC]))) return VULKAN_CODE_WAIT_QUEUE_ERROR;

    return VULKAN_CODE_SUCCESS;
}
