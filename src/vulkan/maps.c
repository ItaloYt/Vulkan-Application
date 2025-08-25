#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "core/vulkan.h"

VulkanCode vulkan_map_mesh(Vulkan *self, const VulkanMeshInfo *mesh_info) {
    assert(self != NULL);
    assert(mesh_info != NULL);

    unsigned *offsets = malloc(mesh_info->mesh_count * sizeof(unsigned));
    if (offsets == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

    self->memory_offsets[VULKAN_MEMORIES_MESH] = offsets;

    // TODO: I shouldn't be doing this, vulkan will probably throw an error
    if (mesh_info->mesh_count == 0) return VULKAN_CODE_SUCCESS;

    offsets[0] = 0;
    for (int i = 1; i < mesh_info->mesh_count; ++i) {
        offsets[i] = offsets[i - 1] + mesh_info->mesh_sizes[i];
    }

    void *mapped;
    if (vulkan_throw_api(vkMapMemory(self->device, self->memories[VULKAN_MEMORIES_MESH], 0, self->buffer_sizes[VULKAN_BUFFERS_MESH], 0, &mapped))) return VULKAN_CODE_MAP_MEMORY_ERROR;

    for (int i = 0; i < mesh_info->mesh_count; ++i) {
        (void) memcpy(mapped + offsets[i], mesh_info->meshes[i], mesh_info->mesh_sizes[i]);
    }

    vkUnmapMemory(self->device, self->memories[VULKAN_MEMORIES_MESH]);

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_memory_offsets(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_MEMORIES_COUNT; ++i) {
        free(self->memory_offsets[i]);
    }
}
