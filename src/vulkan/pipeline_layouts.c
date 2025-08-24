#include <assert.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_pipeline_layouts(Vulkan *self) {
    assert(self != NULL);

    VkPipelineLayoutCreateInfo infos[VULKAN_PIPELINE_LAYOUT_COUNT] = {
        // VULKAN_PIPELINE_LAYOUT_BASE_UI
        (VkPipelineLayoutCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .setLayoutCount = 0,
            .pSetLayouts = NULL,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges = NULL,
        },
    };

    for (int i = 0; i < VULKAN_PIPELINE_LAYOUT_COUNT; ++i) {
        if (vulkan_throw_api(vkCreatePipelineLayout(self->device, infos + i, NULL, self->pipeline_layouts + i))) return VULKAN_CODE_CREATE_PIPELINE_LAYOUTS_ERROR;
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_pipeline_layouts(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_PIPELINE_LAYOUT_COUNT; ++i) {
        vkDestroyPipelineLayout(self->device, self->pipeline_layouts[i], NULL);
    }
}
