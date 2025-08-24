#include <assert.h>

#include "core/vulkan.h"
#include "core/shaders.h"

VulkanCode vulkan_create_pipelines(Vulkan *self) {
    assert(self != NULL);

    VkGraphicsPipelineCreateInfo infos[VULKAN_PIPELINE_COUNT] = {
        // VULKAN_PIPELINE_BASE_UI
        (VkGraphicsPipelineCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = NULL,
            .flags = 0,
            .stageCount = 2,
            .pStages = (VkPipelineShaderStageCreateInfo[]) {
                (VkPipelineShaderStageCreateInfo) {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = NULL,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = self->shaders[EMBED_SHADERS_BASE_UI_VERT],
                    .pName = "main",
                    .pSpecializationInfo = NULL,
                },
                (VkPipelineShaderStageCreateInfo) {
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .pNext = NULL,
                    .flags = 0,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = self->shaders[EMBED_SHADERS_BASE_UI_FRAG],
                    .pName = "main",
                    .pSpecializationInfo = NULL,
                },
            },
            .pVertexInputState = &(VkPipelineVertexInputStateCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .vertexBindingDescriptionCount = 0,
                .pVertexBindingDescriptions = NULL,
                .vertexAttributeDescriptionCount = 0,
                .pVertexAttributeDescriptions = NULL,
            },
            .pInputAssemblyState = &(VkPipelineInputAssemblyStateCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitiveRestartEnable = VK_FALSE,
            },
            .pTessellationState = NULL,
            .pViewportState = &(VkPipelineViewportStateCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .viewportCount = 1,
                .pViewports = NULL, // Later definition(at record command buffer)
                .scissorCount = 1,
                .pScissors = NULL, // Later definition(at record command buffer)
            },
            .pRasterizationState = &(VkPipelineRasterizationStateCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .depthClampEnable = VK_FALSE,
                .rasterizerDiscardEnable = VK_FALSE,
                .polygonMode = VK_POLYGON_MODE_FILL,
                .cullMode = VK_CULL_MODE_BACK_BIT,
                .frontFace = VK_FRONT_FACE_CLOCKWISE,
                .depthBiasEnable = VK_FALSE,
                .depthBiasConstantFactor = 0,
                .depthBiasClamp = 0,
                .depthBiasSlopeFactor = 0,
                .lineWidth = 1,
            },
            .pMultisampleState = &(VkPipelineMultisampleStateCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                .sampleShadingEnable = VK_FALSE,
                .minSampleShading = 0,
                .pSampleMask = NULL,
                .alphaToCoverageEnable = VK_FALSE,
                .alphaToOneEnable = VK_FALSE,
            },
            .pDepthStencilState = NULL,
            .pColorBlendState = &(VkPipelineColorBlendStateCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .logicOpEnable = VK_FALSE,
                .logicOp = VK_LOGIC_OP_COPY,
                .attachmentCount = 1,
                .pAttachments = (VkPipelineColorBlendAttachmentState[]) {
                    (VkPipelineColorBlendAttachmentState) {
                        .blendEnable = VK_FALSE,
                        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
                        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                        .colorBlendOp = VK_BLEND_OP_ADD,
                        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                        .alphaBlendOp = VK_BLEND_OP_ADD,
                        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
                    },
                },
                .blendConstants = { 0, 0, 0, 0, },
            },
            .pDynamicState = &(VkPipelineDynamicStateCreateInfo) {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .pNext = NULL,
                .flags = 0,
                .dynamicStateCount = 2,
                .pDynamicStates = (VkDynamicState[]) {
                    VK_DYNAMIC_STATE_VIEWPORT,
                    VK_DYNAMIC_STATE_SCISSOR,
                },
            },
            .layout = self->pipeline_layouts[VULKAN_PIPELINE_LAYOUT_BASE_UI],
            .renderPass = self->render_passes[VULKAN_RENDER_PASS_BASE_UI],
            .subpass = 0,
            .basePipelineHandle = NULL,
            .basePipelineIndex = -1,
        },
    };

    for (int i = 0; i < VULKAN_PIPELINE_COUNT; ++i) {
        if (vulkan_throw_api(vkCreateGraphicsPipelines(self->device, NULL, VULKAN_PIPELINE_COUNT, infos, NULL, self->pipelines))) return VULKAN_CODE_CREATE_PIPELINES_ERROR;
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_pipelines(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < VULKAN_PIPELINE_COUNT; ++i) {
        vkDestroyPipeline(self->device, self->pipelines[i], NULL);
    }
}
