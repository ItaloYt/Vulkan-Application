#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vulkan/vk_enum_string_helper.h>

#include "core/vulkan.h"

static void on_resize(Vulkan *self, Window *window, unsigned width, unsigned height) {
    self->should_resize = true;
}

VulkanCode vulkan_init(Vulkan *self, Window *window) {
    assert(self != NULL);
    assert(window != NULL);

    if (
        vulkan_throw(vulkan_create_instance(self, window)) ||
        vulkan_throw_api(window->create_vulkan_surface(window, &self->surface, self->instance, NULL)) ||
        vulkan_throw(vulkan_load_physical(self, window)) ||
        vulkan_throw(vulkan_get_surface_capabilities(self, window)) ||
        vulkan_throw(vulkan_load_surface_format(self)) ||
        vulkan_throw(vulkan_load_surface_present_mode(self)) ||
        vulkan_throw(vulkan_create_device(self)) ||
        vulkan_throw(vulkan_create_swapchain(self)) ||
        vulkan_throw(vulkan_load_swapchain_images(self)) ||
        vulkan_throw(vulkan_create_swapchain_views(self)) ||
        vulkan_throw(vulkan_create_shaders(self)) ||
        vulkan_throw(vulkan_create_pipeline_layouts(self)) ||
        vulkan_throw(vulkan_create_render_passes(self)) ||
        vulkan_throw(vulkan_create_framebuffers(self)) ||
        vulkan_throw(vulkan_create_pipelines(self)) ||
        vulkan_throw(vulkan_create_command_pools(self)) ||
        vulkan_throw(vulkan_create_command_buffers(self)) ||
        vulkan_throw(vulkan_create_semaphores(self)) ||
        vulkan_throw(vulkan_create_fences(self))
    ) {
        vulkan_destroy(self);

        return VULKAN_CODE_FAILED_ERROR;
    }

    window->set_resize_callback(window, WINDOW_CALLBACK_TYPE_RENDERER, (void *) on_resize, self);

    return VULKAN_CODE_SUCCESS;
}

void vulkan_destroy(Vulkan *self) {
    if (self == NULL) return;

    if (self->device != NULL) (void) vkDeviceWaitIdle(self->device);
    
    vulkan_destroy_memory_offsets(self);
    vulkan_destroy_memories(self);
    vulkan_destroy_buffers(self);
    vulkan_destroy_fences(self);
    vulkan_destroy_semaphores(self);
    vulkan_destroy_command_buffers(self);
    vulkan_destroy_command_pools(self);
    vulkan_destroy_pipelines(self);
    vulkan_destroy_framebuffers(self);
    vulkan_destroy_render_passes(self);
    vulkan_destroy_pipeline_layouts(self);
    vulkan_destroy_shaders(self);
    vulkan_destroy_swapchain_views(self);
    free(self->images);
    vkDestroySwapchainKHR(self->device, self->swapchain, NULL);
    vkDestroyDevice(self->device, NULL);
    vkDestroySurfaceKHR(self->instance, self->surface, NULL);
    vkDestroyInstance(self->instance, NULL);

    memset(self, 0, sizeof(Vulkan));
}

VulkanCode vulkan_render(Vulkan *self, Window *window) {
    assert(self != NULL);
    
    VkResult result = vkGetFenceStatus(self->device, self->fences[VULKAN_FENCE_FRAME_ENDED][self->frame_index]);
    if (result == VK_NOT_READY) goto _next_frame;
    if (vulkan_throw_api(result)) return VULKAN_CODE_GET_FENCE_STATUS_ERROR;

    if (self->should_resize && vulkan_throw(vulkan_resize_swapchain(self, window))) return VULKAN_CODE_RESIZE_SWAPCHAIN_ERROR;

    unsigned image_index;
    result = vkAcquireNextImageKHR(self->device, self->swapchain, -1, self->semaphores[VULKAN_SEMAPHORE_IMAGE_AVAILABLE][self->frame_index], NULL, &image_index);

    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) return vulkan_resize_swapchain(self, window);
    if (vulkan_throw_api(result)) return VULKAN_CODE_ACQUIRE_NEXT_IMAGE_ERROR;

    if (vulkan_throw_api(vkResetFences(self->device, 1, self->fences[VULKAN_FENCE_FRAME_ENDED] + self->frame_index))) return VULKAN_CODE_RESET_FENCES_ERROR;

    const VkCommandBuffer cmd = self->command_buffers[VULKAN_COMMAND_POOL_GRAPHIC][VULKAN_GRAPHIC_COMMAND_DRAW];

    VkCommandBufferBeginInfo begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = 0,
        .pInheritanceInfo = NULL,
    };

    if (vulkan_throw_api(vkBeginCommandBuffer(cmd, &begin_info))) return VULKAN_CODE_BEGIN_COMMAND_BUFFER_ERROR;

    vulkan_draw_base_ui(self, image_index);

    if (vulkan_throw_api(vkEndCommandBuffer(cmd))) return VULKAN_CODE_END_COMMAND_BUFFER_ERROR;

    VkSubmitInfo submit_infos[1] = {
        (VkSubmitInfo) {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = NULL,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = self->semaphores[VULKAN_SEMAPHORE_IMAGE_AVAILABLE] + self->frame_index,
            .pWaitDstStageMask = (VkPipelineStageFlags[]) {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            },
            .commandBufferCount = 1,
            .pCommandBuffers = self->command_buffers[VULKAN_COMMAND_POOL_GRAPHIC] + VULKAN_GRAPHIC_COMMAND_DRAW,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = self->semaphores[VULKAN_SEMAPHORE_RENDER_FINISHED] + image_index + self->frame_index * self->image_count,
        },
    };

    if (vulkan_throw_api(vkQueueSubmit(self->queues[VULKAN_QUEUE_GRAPHIC], 1, submit_infos, self->fences[VULKAN_FENCE_FRAME_ENDED][self->frame_index]))) return VULKAN_CODE_SUBMIT_QUEUE_ERROR;

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = self->semaphores[VULKAN_SEMAPHORE_RENDER_FINISHED] + image_index + self->frame_index * self->image_count,
        .swapchainCount = 1,
        .pSwapchains = (VkSwapchainKHR[]) {
            self->swapchain,
        },
        .pImageIndices = (unsigned[]) {
            image_index,
        },
        .pResults = NULL,
    };

    result = vkQueuePresentKHR(self->queues[VULKAN_QUEUE_PRESENT], &present_info);
    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) return vulkan_resize_swapchain(self, window);
    if (vulkan_throw_api(result)) return VULKAN_CODE_PRESENT_QUEUE_ERROR;

_next_frame:
    self->frame_index = (self->frame_index + 1) % VULKAN_FRAMES_IN_FLIGHT;

    return VULKAN_CODE_SUCCESS;
}

VulkanCode vulkan_bind_mesh(Vulkan *self, const VulkanMeshInfo *mesh_info) {
    VulkanCode code = vulkan_create_mesh_buffer(self, mesh_info);
    if (code != VULKAN_CODE_SUCCESS) return code;

    code = vulkan_create_mesh_memory(self);
    if (code != VULKAN_CODE_SUCCESS) return code;

    code = vulkan_map_mesh(self, mesh_info);
    if (code != VULKAN_CODE_SUCCESS) return code;
    
    return VULKAN_CODE_SUCCESS;
}

bool vulkan_throw(VulkanCode code) {
    switch (code) {
        case VULKAN_CODE_SUCCESS: {
            return false;
        }

        case VULKAN_CODE_UNDEFINED_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Undefined\n");
            return true;
        }

        case VULKAN_CODE_CREATE_INSTANCE_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create instance\n");
            return true;
        }

        case VULKAN_CODE_FAILED_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed\n");
            return true;
        }

        case VULKAN_CODE_LOAD_PHYSICALS_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to load physical devices\n");
            return true;
        }

        case VULKAN_CODE_NO_GRAPHIC_QUEUE_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: No graphic queue found\n");
            return true;
        }

        case VULKAN_CODE_NO_PRESENT_QUEUE_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: No present queue found\n");
            return true;
        }

        case VULKAN_CODE_GET_PRESENTATION_SUPPORT_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to get queue presentation support\n");
            return true;
        }

        case VULKAN_CODE_CREATE_DEVICE_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create device\n");
            return true;
        }

        case VULKAN_CODE_GET_SURFACE_CAPABILITIES_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to get surface capabilities\n");
            return true;
        }

        case VULKAN_CODE_LOAD_SURFACE_FORMATS_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to load surface formats\n");
            return true;
        }

        case VULKAN_CODE_LOAD_SURFACE_PRESENT_MODES_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to load surface present modes\n");
            return true;
        }

        case VULKAN_CODE_CREATE_SWAPCHAIN_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create swapchain\n");
            return true;
        }

        case VULKAN_CODE_LOAD_SWAPCHAIN_IMAGES_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to load swapchain images\n");
            return true;
        }

        case VULKAN_CODE_ALLOCATE_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to allocate host memory\n");
            return true;
        }

        case VULKAN_CODE_CREATE_SWAPCHAIN_VIEWS_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create swapchain views\n");
            return true;
        }

        case VULKAN_CODE_CREATE_SHADERS_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create shaders\n");
            return true;
        }

        case VULKAN_CODE_CREATE_PIPELINE_LAYOUTS_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create pipeline layouts\n");
            return true;
        }

        case VULKAN_CODE_CREATE_RENDER_PASSES_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create render passes\n");
            return true;
        }

        case VULKAN_CODE_CREATE_FRAMEBUFFERS_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create framebuffers\n");
            return true;
        }

        case VULKAN_CODE_CREATE_PIPELINES_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create pipelines\n");
            return true;
        }

        case VULKAN_CODE_CREATE_COMMAND_POOLS_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create command pools\n");
            return true;
        }

        case VULKAN_CODE_CREATE_COMMAND_BUFFERS_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create command buffers\n");
            return true;
        }

        case VULKAN_CODE_CREATE_SEMAPHORES_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create semaphores\n");
            return true;
        }

        case VULKAN_CODE_CREATE_FENCES_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create fences\n");
            return true;
        }

        case VULKAN_CODE_GET_FENCE_STATUS_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to get fence status\n");
            return true;
        }

        case VULKAN_CODE_RESET_FENCES_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to reset fences\n");
            return true;
        }

        case VULKAN_CODE_RESIZE_SWAPCHAIN_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to resize swapchain\n");
            return true;
        }

        case VULKAN_CODE_ACQUIRE_NEXT_IMAGE_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to acquire next swapchain image\n");
            return true;
        }

        case VULKAN_CODE_BEGIN_COMMAND_BUFFER_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to begin command buffer\n");
            return true;
        }

        case VULKAN_CODE_END_COMMAND_BUFFER_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to end command buffer\n");
            return true;
        }

        case VULKAN_CODE_SUBMIT_QUEUE_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to submit queue\n");
            return true;
        }

        case VULKAN_CODE_PRESENT_QUEUE_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to present queue\n");
            return true;
        }

        case VULKAN_CODE_CREATE_MESH_BUFFER_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create mesh buffer\n");
            return true;
        }

        case VULKAN_CODE_NO_MEMORY_TYPE_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: No suitable memory type\n");
            return true;
        }

        case VULKAN_CODE_CREATE_MEMORY_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create device memory\n");
            return true;
        }

        case VULKAN_CODE_BIND_BUFFER_MEMORY_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to bind buffer memory\n");
            return true;
        }

        case VULKAN_CODE_MAP_MEMORY_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to map memory\n");
            return true;
        }
    }

    (void) fprintf(stderr, "Vulkan Error: invalid code %#02x\n", code);

    return true;
}

bool vulkan_throw_api(VkResult result) {
    if (result == VK_SUCCESS) return false;

    (void) fprintf(stderr, "Vulkan API Error: %s\n", string_VkResult(result));

    return true;
}
