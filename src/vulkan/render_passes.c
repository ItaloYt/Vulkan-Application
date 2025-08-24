#include <assert.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_render_passes(Vulkan *self) {
    assert(self != NULL);

    VkRenderPassCreateInfo infos[VULKAN_RENDER_PASS_COUNT] = {
        // VULKAN_RENDER_PASS_BASE_UI
        (VkRenderPassCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .attachmentCount = 1,
            .pAttachments = (VkAttachmentDescription[]) {
                (VkAttachmentDescription) {
                    .flags = 0,
                    .format = self->format.format,
                    .samples = VK_SAMPLE_COUNT_1_BIT,
                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                },
            },
            .subpassCount = 1,
            .pSubpasses = (VkSubpassDescription[]) {
                (VkSubpassDescription) {
                    .flags = 0,
                    .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                    .inputAttachmentCount = 0,
                    .pInputAttachments = NULL,
                    .colorAttachmentCount = 1,
                    .pColorAttachments = (VkAttachmentReference[]) {
                        (VkAttachmentReference) {
                            .attachment = 0,
                            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                        },
                    },
                    .pResolveAttachments = NULL,
                    .pDepthStencilAttachment = NULL,
                    .preserveAttachmentCount = 0,
                    .pPreserveAttachments = NULL,
                },
            },
            .dependencyCount = 1,
            .pDependencies = (VkSubpassDependency[]) {
                (VkSubpassDependency) {
                    .srcSubpass = VK_SUBPASS_EXTERNAL,
                    .dstSubpass = 0,
                    .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .srcAccessMask = 0,
                    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                    .dependencyFlags = 0,
                },
            },
        },
    };

    for (int i = 0; i < VULKAN_RENDER_PASS_COUNT; ++i) {
        if (vulkan_throw_api(vkCreateRenderPass(self->device, infos + i, NULL, self->render_passes + i))) return VULKAN_CODE_CREATE_RENDER_PASSES_ERROR;
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_render_passes(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_RENDER_PASS_COUNT; ++i) {
        vkDestroyRenderPass(self->device, self->render_passes[i], NULL);
    }
}
