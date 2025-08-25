#include <assert.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_buffers_memory(Vulkan *self, unsigned *buffers, unsigned count, unsigned memory, VkMemoryPropertyFlags flags) {
    assert(self != NULL);
    assert(buffers != NULL);
    assert(count > 0);
    assert(buffers[0] < VULKAN_BUFFER_COUNT);
    assert(memory < VULKAN_MEMORY_COUNT);

    VkMemoryRequirements requirement;
    unsigned previous_size = 0;
    unsigned memory_type = -1;

    for (int i = 0; i < count; ++i) {
        assert(buffers[i] < VULKAN_BUFFER_COUNT);

        vkGetBufferMemoryRequirements(self->device, self->buffers[buffers[i]], &requirement);

        memory_type &= requirement.memoryTypeBits;

        if (i > 0) self->buffer_offsets[buffers[i]] = (self->buffer_offsets[buffers[i - 1]] + previous_size + requirement.size - 1) / requirement.alignment * requirement.alignment;

        previous_size = requirement.size;
    }

    if (memory_type == 0) return VULKAN_CODE_NO_MEMORY_TYPE_ERROR;

    unsigned size = self->buffer_offsets[buffers[count - 1]] + requirement.size;

    VkPhysicalDeviceMemoryProperties properties;
    vkGetPhysicalDeviceMemoryProperties(self->physical, &properties);

    int memory_index = -1;

    for (int i = 0; i < properties.memoryTypeCount; ++i) {
        if (requirement.memoryTypeBits & (1 << i) && (properties.memoryTypes[i].propertyFlags & flags) == flags) {
            memory_index = i;
            break;
        }
    }

    if (memory_index == -1) return VULKAN_CODE_NO_MEMORY_TYPE_ERROR;

    VkMemoryAllocateInfo info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = size,
        .memoryTypeIndex = memory_index,
    };

    if (vulkan_throw_api(vkAllocateMemory(self->device, &info, NULL, self->memories + memory))) return VULKAN_CODE_CREATE_MEMORY_ERROR;

    for (int i = 0; i < count; ++i) {
        if (vulkan_throw_api(vkBindBufferMemory(self->device, self->buffers[buffers[i]], self->memories[memory], self->buffer_offsets[buffers[i]]))) return VULKAN_CODE_BIND_BUFFER_MEMORY_ERROR;
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_memories(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_MEMORY_COUNT; ++i) {
        vkFreeMemory(self->device, self->memories[i], NULL);
    }
}
