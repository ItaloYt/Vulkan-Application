#include <assert.h>
#include <stdlib.h>

#include "core/vulkan.h"
#include "core/math.h"

VulkanCode vulkan_create_swapchain(Vulkan *self) {
    assert(self != NULL);

    VkSwapchainCreateInfoKHR info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .surface = self->surface,
        .minImageCount = (self->capabilities.maxImageCount == 0 ? self->capabilities.minImageCount + 1 : math_min_int(self->capabilities.minImageCount + 1, self->capabilities.maxImageCount)),
        .imageFormat = self->format.format,
        .imageColorSpace = self->format.colorSpace,
        .imageExtent = *self->extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = (self->queues[VULKAN_QUEUE_GRAPHIC] == self->queues[VULKAN_QUEUE_PRESENT] ? VK_SHARING_MODE_EXCLUSIVE :VK_SHARING_MODE_CONCURRENT),
        .queueFamilyIndexCount = (self->queues[VULKAN_QUEUE_GRAPHIC] == self->queues[VULKAN_QUEUE_PRESENT] ? VULKAN_QUEUE_COUNT - 1 : VULKAN_QUEUE_COUNT),
        .pQueueFamilyIndices = self->queue_indices,
        .preTransform = self->capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = self->mode,
        .clipped = VK_TRUE,
        .oldSwapchain = NULL,
    };

    return (vulkan_throw_api(vkCreateSwapchainKHR(self->device, &info, NULL, &self->swapchain)) ? VULKAN_CODE_CREATE_SWAPCHAIN_ERROR : VULKAN_CODE_SUCCESS);
}

VulkanCode vulkan_load_swapchain_images(Vulkan *self) {
    assert(self != NULL);

    if (vulkan_throw_api(vkGetSwapchainImagesKHR(self->device, self->swapchain, &self->image_count, NULL))) return VULKAN_CODE_LOAD_SWAPCHAIN_IMAGES_ERROR;

    self->images = malloc(self->image_count * sizeof(VkImage));
    if (self->images == NULL) return VULKAN_CODE_ALLOCATE_ERROR;
    if (vulkan_throw_api(vkGetSwapchainImagesKHR(self->device, self->swapchain, &self->image_count, self->images))) return VULKAN_CODE_LOAD_SWAPCHAIN_IMAGES_ERROR;

    return VULKAN_CODE_SUCCESS;
}

VulkanCode vulkan_create_swapchain_views(Vulkan *self) {
    assert(self != NULL);

    self->views = calloc(self->image_count, sizeof(VkImageView));
    if (self->views == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

    VkImageViewCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .image = NULL, // Later definition
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = self->format.format,
        .components = { VK_COMPONENT_SWIZZLE_IDENTITY },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    for (int i = 0; i < self->image_count; ++i) {
        info.image = self->images[i];

        if (vulkan_throw_api(vkCreateImageView(self->device, &info, NULL, self->views + i))) return VULKAN_CODE_CREATE_SWAPCHAIN_VIEWS_ERROR;
    }

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy_swapchain_views(Vulkan *self) {
    assert(self != NULL);

    if (self->views == NULL) return;

    for (int i = 0; i < self->image_count; ++i) {
        vkDestroyImageView(self->device, self->views[i], NULL);
    }

    free(self->views);
}

VulkanCode vulkan_resize_swapchain(Vulkan *self, Window *window) {
    assert(self != NULL);

    vulkan_destroy_framebuffers(self);
    vulkan_destroy_swapchain_views(self);
    free(self->images);
    vkDestroySwapchainKHR(self->device, self->swapchain, NULL);

    if (
        vulkan_throw(vulkan_get_surface_capabilities(self, window)) ||
        vulkan_throw(vulkan_create_swapchain(self)) ||
        vulkan_throw(vulkan_load_swapchain_images(self)) ||
        vulkan_throw(vulkan_create_swapchain_views(self)) ||
        vulkan_throw(vulkan_create_framebuffers(self))
    ) return VULKAN_CODE_RESIZE_SWAPCHAIN_ERROR;

    self->should_resize = false;

    return VULKAN_CODE_SUCCESS;
}
