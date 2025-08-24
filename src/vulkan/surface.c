#include <assert.h>

#include "core/vulkan.h"
#include "core/math.h"

VulkanCode vulkan_get_surface_capabilities(Vulkan *self, Window *window) {
    assert(self != NULL);
    assert(window != NULL);

    if (vulkan_throw_api(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(self->physical, self->surface, &self->capabilities))) return VULKAN_CODE_GET_SURFACE_CAPABILITIES_ERROR;

    self->extent = &self->capabilities.currentExtent;

    if (self->extent->width == -1) {
        self->extent->width = math_clamp_int(window->width, self->capabilities.minImageExtent.width, self->capabilities.maxImageExtent.width);
        self->extent->height = math_clamp_int(window->height, self->capabilities.minImageExtent.height, self->capabilities.maxImageExtent.height);
    }

    return VULKAN_CODE_SUCCESS;
}

VulkanCode vulkan_load_surface_format(Vulkan *self) {
    assert(self != NULL);

    unsigned count;
    if (vulkan_throw_api(vkGetPhysicalDeviceSurfaceFormatsKHR(self->physical, self->surface, &count, NULL))) return VULKAN_CODE_LOAD_SURFACE_FORMATS_ERROR;

    VkSurfaceFormatKHR formats[count];
    if (vulkan_throw_api(vkGetPhysicalDeviceSurfaceFormatsKHR(self->physical, self->surface, &count, formats))) return VULKAN_CODE_LOAD_SURFACE_FORMATS_ERROR;

    self->format = formats[0];

    for (int i = 1; i < count; ++i) {
        if (formats[i].format != VK_FORMAT_B8G8R8A8_SRGB || formats[i].colorSpace != VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) continue;

        self->format = formats[i];
    }

    return VULKAN_CODE_SUCCESS;
}

VulkanCode vulkan_load_surface_present_mode(Vulkan *self) {
    assert(self != NULL);

    unsigned count;
    if (vulkan_throw_api(vkGetPhysicalDeviceSurfacePresentModesKHR(self->physical, self->surface, &count, NULL))) return VULKAN_CODE_LOAD_SURFACE_PRESENT_MODES_ERROR;

    VkPresentModeKHR modes[count];
    if (vulkan_throw_api(vkGetPhysicalDeviceSurfacePresentModesKHR(self->physical, self->surface, &count, modes))) return VULKAN_CODE_LOAD_SURFACE_PRESENT_MODES_ERROR;

    self->mode = VK_PRESENT_MODE_FIFO_KHR;

    for (int i = 0; i < count; ++i) {
        if (modes[i] != VK_PRESENT_MODE_MAILBOX_KHR) continue;

        self->mode = modes[i];
    }

    return VULKAN_CODE_SUCCESS;
}
