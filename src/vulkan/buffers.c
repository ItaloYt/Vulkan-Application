#include <assert.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_mesh_buffer(Vulkan *self, const VulkanMeshInfo *mesh_info) {
    assert(self != NULL);
    assert(mesh_info != NULL);

    for (int i = 0; i < mesh_info->mesh_count; ++i) {
        self->buffer_sizes[VULKAN_BUFFERS_MESH] += mesh_info->mesh_sizes[i];
    }

    VkBufferCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = self->buffer_sizes[VULKAN_BUFFERS_MESH],
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
    };
    
    if (vulkan_throw_api(vkCreateBuffer(self->device, &info, NULL, self->buffers + VULKAN_BUFFERS_MESH))) return VULKAN_CODE_CREATE_MESH_BUFFER_ERROR;

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_buffers(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_BUFFERS_COUNT; ++i) {
        vkDestroyBuffer(self->device, self->buffers[i], NULL);
    }
}
