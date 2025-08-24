#include <assert.h>
#include <stdlib.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_framebuffers(Vulkan *self) {
    assert(self != NULL);

    VkFramebufferCreateInfo infos[VULKAN_RENDER_PASS_COUNT] = {
        // VULKAN_RENDER_PASS_BASE_UI
        (VkFramebufferCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .renderPass = self->render_passes[VULKAN_RENDER_PASS_BASE_UI],
            .attachmentCount = 1,
            .pAttachments = NULL, // Later definition
            .width = self->extent->width,
            .height = self->extent->height,
            .layers = 1,
        },
    };

    for (int i = 0; i < VULKAN_RENDER_PASS_COUNT; ++i) {
        self->framebuffers[i] = calloc(self->image_count, sizeof(VkFramebuffer));
        if (self->framebuffers[i] == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

        for (int ii = 0; ii < self->image_count; ++ii) {
            infos[i].pAttachments = self->views + ii;
            if (vulkan_throw_api(vkCreateFramebuffer(self->device, infos + i, NULL, self->framebuffers[i] + ii))) return VULKAN_CODE_CREATE_FRAMEBUFFERS_ERROR;
        }
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_framebuffers(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_RENDER_PASS_COUNT; ++i) {
        if (self->framebuffers[i] == NULL) continue;

        for (int ii = 0; ii < self->image_count; ++ii) {
            vkDestroyFramebuffer(self->device, self->framebuffers[i][ii], NULL);
        }

        free(self->framebuffers[i]);
    }
}
