#include <assert.h>
#include <stdlib.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_semaphores(Vulkan *self) {
    assert(self != NULL);

    const unsigned *count = vulkan_get_semaphores_count(self);

    int total = 0;
    for (int i = 0; i < VULKAN_SEMAPHORE_COUNT; ++i) {
        total += count[i];
    }

    self->semaphores = calloc(total, sizeof(VkSemaphore));
    if (self->semaphores == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

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

    for (int i = 0, offset = 0; i < VULKAN_SEMAPHORE_COUNT; ++i) {
        for (int ii = 0; ii < count[i]; ++ii, ++offset) {
            if (vulkan_throw_api(vkCreateSemaphore(self->device, infos + i, NULL, self->semaphores + offset))) return VULKAN_CODE_CREATE_SEMAPHORES_ERROR;
        }
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_semaphores(Vulkan *self) {
    assert(self != NULL);

    if (self->semaphores == NULL) return;

    const unsigned *count = vulkan_get_semaphores_count(self);

    for (int i = 0, offset = 0; i < VULKAN_SEMAPHORE_COUNT; ++i) {
        for (int ii = 0; ii < count[i]; ++ii, ++offset) {
            vkDestroySemaphore(self->device, self->semaphores[offset], NULL);
        }
    }

    free(self->semaphores);
}

VkSemaphore *vulkan_get_semaphores(Vulkan *self, unsigned semaphore) {
    assert(self != NULL);
    assert(semaphore < VULKAN_SEMAPHORE_COUNT);

    static unsigned offsets[VULKAN_SEMAPHORE_COUNT] = {
        0,
    };
    
    static bool cached = false;

    if (cached) return self->semaphores + offsets[semaphore];

    const unsigned *count = vulkan_get_semaphores_count(self);

    for (int i = 1; i < VULKAN_SEMAPHORE_COUNT; ++i) {
        offsets[i] += offsets[0] + count[i - 1];
    }

    cached = true;
    return self->semaphores + offsets[semaphore];
}

const unsigned *vulkan_get_semaphores_count(Vulkan *self) {
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
