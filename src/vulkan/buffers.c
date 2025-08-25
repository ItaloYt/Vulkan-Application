#include <assert.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_mesh_buffers(Vulkan *self, const VulkanMeshInfo *mesh_info) {
    assert(self != NULL);
    assert(mesh_info != NULL);

    for (int i = 0; i < mesh_info->count; ++i) {
        self->buffer_sizes[VULKAN_MESH_BUFFER_VERTEX] += mesh_info->mesh_sizes[i];
        self->buffer_sizes[VULKAN_MESH_BUFFER_INDEX] += mesh_info->index_count[i] * sizeof(unsigned);
    }

    VkBufferCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = self->buffer_sizes[VULKAN_MESH_BUFFER_VERTEX],
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
    };
    
    if (vulkan_throw_api(vkCreateBuffer(self->device, &info, NULL, self->buffers + VULKAN_MESH_BUFFER_VERTEX))) return VULKAN_CODE_CREATE_BUFFER_ERROR;

    self->buffer_sizes[VULKAN_TEMP_BUFFER_STAGE_VERTEX] = self->buffer_sizes[VULKAN_MESH_BUFFER_VERTEX];
    info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    if (vulkan_throw_api(vkCreateBuffer(self->device, &info, NULL, self->buffers + VULKAN_TEMP_BUFFER_STAGE_VERTEX))) return VULKAN_CODE_CREATE_BUFFER_ERROR;

    self->buffer_sizes[VULKAN_TEMP_BUFFER_STAGE_INDEX] = self->buffer_sizes[VULKAN_MESH_BUFFER_INDEX];
    info.size = self->buffer_sizes[VULKAN_MESH_BUFFER_INDEX];
    if (vulkan_throw_api(vkCreateBuffer(self->device, &info, NULL, self->buffers + VULKAN_TEMP_BUFFER_STAGE_INDEX))) return VULKAN_CODE_CREATE_BUFFER_ERROR;

    info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    if (vulkan_throw_api(vkCreateBuffer(self->device, &info, NULL, self->buffers + VULKAN_MESH_BUFFER_INDEX))) return VULKAN_CODE_CREATE_BUFFER_ERROR;

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_buffers(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_BUFFER_COUNT; ++i) {
        vkDestroyBuffer(self->device, self->buffers[i], NULL);
    }
}
