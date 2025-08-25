#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "core/vulkan.h"

VulkanCode vulkan_map_mesh(Vulkan *self, const VulkanMeshInfo *mesh_info) {
    assert(self != NULL);
    assert(mesh_info != NULL);

    self->mesh_offsets = calloc(mesh_info->count, sizeof(unsigned));
    if (self->mesh_offsets == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

    self->index_offsets = calloc(mesh_info->count, sizeof(unsigned));
    if (self->index_offsets == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

    self->index_count = calloc(mesh_info->count, sizeof(unsigned));
    if (self->index_count == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

    void *mapped;
    if (vulkan_throw_api(vkMapMemory(self->device, self->memories[VULKAN_MEMORY_TEMP], 0, VK_WHOLE_SIZE, 0, &mapped))) return VULKAN_CODE_MAP_MEMORY_ERROR;

    self->mesh_offsets[0] = 0;
    self->index_offsets[0] = 0;
    for (int i = 0; i < mesh_info->count; ++i) {
        (void) memcpy(mapped + (self->buffer_offsets[VULKAN_TEMP_BUFFER_STAGE_VERTEX] + self->mesh_offsets[i]), mesh_info->meshes[i], mesh_info->mesh_sizes[i]);
        (void) memcpy(mapped + (self->buffer_offsets[VULKAN_TEMP_BUFFER_STAGE_INDEX] + self->index_offsets[i]), mesh_info->indices[i], mesh_info->index_count[i] * sizeof(unsigned));

        self->index_count[i] = mesh_info->index_count[i];
        
        if (i >= mesh_info->count - 1) continue;

        self->mesh_offsets[i + 1] = self->mesh_offsets[i] + mesh_info->mesh_sizes[i];
        self->index_offsets[i + 1] = self->index_offsets[i] + mesh_info->index_count[i] * sizeof(unsigned);
    }

    vkUnmapMemory(self->device, self->memories[VULKAN_MEMORY_TEMP]);

    VulkanCode code = vulkan_copy_buffers(self, VULKAN_MESH_BUFFER_VERTEX, VULKAN_TEMP_BUFFER_STAGE_VERTEX);
    if (code != VULKAN_CODE_SUCCESS) return code;

    return vulkan_copy_buffers(self, VULKAN_MESH_BUFFER_INDEX, VULKAN_TEMP_BUFFER_STAGE_INDEX);
}
