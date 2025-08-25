#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "core/vulkan.h"

VulkanCode vulkan_setup_objects(Vulkan *self, const VulkanObjectsInfo *info) {
    assert(self != NULL);
    assert(info != NULL);

    for (int i = 0; i < info->count; ++i) {
        self->object_counts[info->types[i]]++;
    }

    for (int i = 0; i < VULKAN_OBJECT_TYPE_COUNT; ++i) {
        self->objects[i] = calloc(self->object_counts[i], sizeof(VulkanObject));
        if (self->objects[i] == NULL) return VULKAN_CODE_ALLOCATE_ERROR;
    }

    int offsets[VULKAN_OBJECT_TYPE_COUNT];
    memset(offsets, 0, VULKAN_OBJECT_TYPE_COUNT * sizeof(int));

    for (int i = 0; i < info->count; ++i) {
        VulkanObject *obj = self->objects[info->types[i]] + offsets[info->types[i]];
        obj->mesh_index = info->mesh_indices[i];
        obj->vertex_count = info->vertex_count[i];

        offsets[info->types[i]]++;
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_objects(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_OBJECT_TYPE_COUNT; ++i) {
        free(self->objects[i]);
    }
}
