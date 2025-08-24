#include <assert.h>

#include "core/vulkan.h"

VulkanCode vulkan_create_shaders(Vulkan *self) {
    assert(self != NULL);

    VkShaderModuleCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .codeSize = 0, // Later definition
        .pCode = NULL, // Later definition
    };

    const unsigned *sizes = embed_get_shader_sizes();
    const char *const *codes = embed_get_shaders();

    for (int i = 0; i < EMBED_SHADERS_COUNT; ++i) {
        info.codeSize = sizes[i];
        info.pCode = (void *) codes[i];

        if (vulkan_throw_api(vkCreateShaderModule(self->device, &info, NULL, self->shaders + i))) return VULKAN_CODE_CREATE_SHADERS_ERROR;
    }

    return VULKAN_CODE_SUCCESS;
};

void vulkan_destroy_shaders(Vulkan *self) {
    assert(self != NULL);

    for (int i = 0; i < EMBED_SHADERS_COUNT; ++i) {
        vkDestroyShaderModule(self->device, self->shaders[i], NULL);
    }
}
