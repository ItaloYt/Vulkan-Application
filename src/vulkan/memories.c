#include <assert.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_mesh_memory(Vulkan *self) {
    assert(self != NULL);

    VkMemoryRequirements requirement;
    vkGetBufferMemoryRequirements(self->device, self->buffers[VULKAN_BUFFERS_MESH], &requirement);

    VkPhysicalDeviceMemoryProperties properties;
    vkGetPhysicalDeviceMemoryProperties(self->physical, &properties);

    const VkMemoryPropertyFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
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
        .allocationSize = requirement.size,
        .memoryTypeIndex = memory_index,
    };

    if (vulkan_throw_api(vkAllocateMemory(self->device, &info, NULL, self->memories + VULKAN_MEMORIES_MESH))) return VULKAN_CODE_CREATE_MEMORY_ERROR;

    if (vulkan_throw_api(vkBindBufferMemory(self->device, self->buffers[VULKAN_BUFFERS_MESH], self->memories[VULKAN_MEMORIES_MESH], 0))) return VULKAN_CODE_BIND_BUFFER_MEMORY_ERROR;

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_memories(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_MEMORIES_COUNT; ++i) {
        vkFreeMemory(self->device, self->memories[i], NULL);
    }
}
