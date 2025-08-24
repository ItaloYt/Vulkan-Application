#include <assert.h>
#include <stdlib.h>

#include "core/vulkan.h"

static const unsigned *get_semaphore_counts(Vulkan *self);

VulkanCode vulkan_create_semaphores(Vulkan *self) {
    assert(self != NULL);

    VkSemaphoreCreateInfo infos[VULKAN_SEMAPHORE_COUNT] = {
        // VULKAN_SEMAPHORE_IMAGE_AVAILABLE
        (VkSemaphoreCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
        },
        // VULKAN_SEMAPHORE_RENDER_FINISHED
        (VkSemaphoreCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
        },
    };

    const unsigned *counts = get_semaphore_counts(self);

    for (int i = 0; i < VULKAN_SEMAPHORE_COUNT; ++i) {
        self->semaphores[i] = calloc(counts[i], sizeof(VkSemaphore));
        if (self->semaphores[i] == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

        for (int ii = 0; ii < counts[i]; ++ii) {
            if (vulkan_throw_api(vkCreateSemaphore(self->device, infos + i, NULL, self->semaphores[i] + ii))) return VULKAN_CODE_CREATE_SEMAPHORES_ERROR;
        }
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_semaphores(Vulkan *self) {
    assert(self != NULL);

    const unsigned *counts = get_semaphore_counts(self);

    for (int i = 0; i < VULKAN_SEMAPHORE_COUNT; ++i) {
        if (self->semaphores[i] == NULL) continue;

        for (int ii = 0; ii < counts[i]; ++ii) {
            vkDestroySemaphore(self->device, self->semaphores[i][ii], NULL);
        }

        free(self->semaphores[i]);
    }
}

static const unsigned *get_semaphore_counts(Vulkan *self) {
    static unsigned counts[VULKAN_SEMAPHORE_COUNT] = {
        VULKAN_FRAMES_IN_FLIGHT,
        VULKAN_FRAMES_IN_FLIGHT,
    };

    static bool cached = false;
    if (cached) return counts;

    assert(self != NULL);

    counts[1] *= self->image_count;

    cached = true;
    return counts;
}
