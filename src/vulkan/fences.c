#include <assert.h>
#include <stdlib.h>

#include "core/vulkan.h"

static const unsigned *get_fence_counts(Vulkan *self);

VulkanCode vulkan_create_fences(Vulkan *self) {
    assert(self != NULL);

    VkFenceCreateInfo infos[VULKAN_FENCE_COUNT] = {
        (VkFenceCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = NULL,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        },
    };

    const unsigned *counts = get_fence_counts(self);

    for (int i = 0; i < VULKAN_FENCE_COUNT; ++i) {
        self->fences[i] = calloc(counts[i], sizeof(VkFence));
        if (self->fences[i] == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

        for (int ii = 0; ii < counts[i]; ++ii) {
            if (vulkan_throw_api(vkCreateFence(self->device, infos + i, NULL, self->fences[i] + ii))) return VULKAN_CODE_CREATE_FENCES_ERROR;
        }
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_fences(Vulkan *self) {
    assert(self != NULL);

    const unsigned *counts = get_fence_counts(self);

    for (int i = 0; i < VULKAN_FENCE_COUNT; ++i) {
        if (self->fences[i] == NULL) continue;

        for (int ii = 0; ii < counts[i]; ++ii) {
            vkDestroyFence(self->device, self->fences[i][ii], NULL);
        }

        free(self->fences[i]);
    }
}

static const unsigned *get_fence_counts(Vulkan *self) {
    static unsigned counts[VULKAN_FENCE_COUNT] = {
        VULKAN_FRAMES_IN_FLIGHT,
    };

    static bool cached = false;
    if (cached) return counts;

    cached = true;
    return counts;
}
