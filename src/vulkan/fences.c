#include <assert.h>
#include <stdlib.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_fences(Vulkan *self) {
    assert(self != NULL);

    const unsigned *count = vulkan_get_fences_count(self);

    int total = 0;
    for (int i = 0; i < VULKAN_FENCE_COUNT; ++i) {
        total += count[i];
    }

    self->fences = calloc(total, sizeof(VkFence));
    if (self->fences == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

    VkFenceCreateInfo infos[VULKAN_FENCE_COUNT] = {
        // VULKAN_FENCE_FRAME_END
        (VkFenceCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = NULL,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        },
    };

    for (int i = 0, offset = 0; i < VULKAN_FENCE_COUNT; ++i) {
        for (int ii = 0; ii < count[i]; ++ii, ++offset) {
            if (vulkan_throw_api(vkCreateFence(self->device, infos + i, NULL, self->fences + offset))) return VULKAN_CODE_CREATE_FENCES_ERROR;
        }
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_fences(Vulkan *self) {
    assert(self != NULL);

    if (self->fences == NULL) return;

    const unsigned *count = vulkan_get_fences_count(self);

    for (int i = 0, offset = 0; i < VULKAN_FENCE_COUNT; ++i) {
        for (int ii = 0; ii < count[i]; ++ii, ++offset) {
            vkDestroyFence(self->device, self->fences[offset], NULL);
        }
    }

    free(self->fences);
}

VkFence *vulkan_get_fences(Vulkan *self, unsigned fence) {
    assert(self != NULL);
    assert(fence < VULKAN_FENCE_COUNT);

    static unsigned offsets[VULKAN_FENCE_COUNT] = {
        0,
    };

    static bool cached = false;
    if (cached) return self->fences + offsets[fence];

    const unsigned *count = vulkan_get_fences_count(self);

    for (int i = 1; i < VULKAN_FENCE_COUNT; ++i) {
        offsets[i] += offsets[i - 1] + count[i - 1];
    }

    cached = true;
    return self->fences + offsets[fence];
}

const unsigned *vulkan_get_fences_count(Vulkan *self) {
    static unsigned counts[VULKAN_FENCE_COUNT] = {
        VULKAN_FRAMES_IN_FLIGHT,
    };

    static bool cached = false;
    if (cached) return counts;

    cached = true;
    return counts;
}
