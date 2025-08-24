#include <assert.h>

#include "core/vulkan.h"

void vulkan_draw_base_ui(Vulkan *self, unsigned image_index) {
    assert(self != NULL);

    const VkCommandBuffer cmd = self->command_buffers[VULKAN_COMMAND_POOL_GRAPHIC][VULKAN_GRAPHIC_COMMAND_DRAW];

    VkRenderPassBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = NULL,
        .renderPass = self->render_passes[VULKAN_RENDER_PASS_BASE_UI],
        .framebuffer = self->framebuffers[VULKAN_RENDER_PASS_BASE_UI][image_index],
        .renderArea = {
            .offset = { .x = 0, .y = 0, },
            .extent = *self->extent,
        },
        .clearValueCount = 1,
        .pClearValues = (VkClearValue[]) {
            (VkClearValue) { .color = { .float32 = { 0, 0, 0, 0, }, }, },
        },
    };

    vkCmdBeginRenderPass(cmd, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, self->pipelines[VULKAN_PIPELINE_BASE_UI]);

    vkCmdSetViewport(cmd, 0, 1, (VkViewport[]) {
        (VkViewport) {
            .x = 0, .y = 0,
            .width = self->extent->width,
            .height = self->extent->height,
            .minDepth = 0, .maxDepth = 1,
        },
    });

    vkCmdSetScissor(cmd, 0, 1, (VkRect2D[]) {
        (VkRect2D) {
            .offset = { .x = 0, .y = 0, },
            .extent = *self->extent,
        },
    });

    vkCmdDraw(cmd, 3, 1, 0, 0);

    vkCmdEndRenderPass(cmd);
}
