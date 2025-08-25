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
    
    free(self->objects);
    free(self->index_count);
    free(self->index_offsets);
    free(self->mesh_offsets);
    vulkan_destroy_memories(self);
    vulkan_destroy_buffers(self);
    vulkan_destroy_fences(self);
    vulkan_destroy_semaphores(self);
    free(self->command_buffers);
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
    
    VkResult result = vkGetFenceStatus(self->device, vulkan_get_fences(self, VULKAN_FENCE_FRAME_ENDED)[self->frame_index]);
    if (result == VK_NOT_READY) goto _next_frame;
    if (vulkan_throw_api(result)) return VULKAN_CODE_GET_FENCE_STATUS_ERROR;

    if (self->should_resize && vulkan_throw(vulkan_resize_swapchain(self, window))) return VULKAN_CODE_RESIZE_SWAPCHAIN_ERROR;

    unsigned image_index;
    result = vkAcquireNextImageKHR(self->device, self->swapchain, -1, vulkan_get_semaphores(self, VULKAN_SEMAPHORE_IMAGE_AVAILABLE)[self->frame_index], NULL, &image_index);

    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) return vulkan_resize_swapchain(self, window);
    if (vulkan_throw_api(result)) return VULKAN_CODE_ACQUIRE_NEXT_IMAGE_ERROR;

    if (vulkan_throw_api(vkResetFences(self->device, vulkan_get_fences_count(self)[VULKAN_FENCE_FRAME_ENDED], vulkan_get_fences(self, VULKAN_FENCE_FRAME_ENDED) + self->frame_index))) return VULKAN_CODE_RESET_FENCES_ERROR;

    const VkCommandBuffer cmd = vulkan_get_command_buffers(self, VULKAN_GRAPHIC_COMMAND_DRAW)[self->frame_index];

    if (vulkan_throw_api(vkResetCommandBuffer(cmd, 0))) return VULKAN_CODE_RESET_COMMAND_BUFFER_ERROR;

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
            .pWaitSemaphores = vulkan_get_semaphores(self, VULKAN_SEMAPHORE_IMAGE_AVAILABLE) + self->frame_index,
            .pWaitDstStageMask = (VkPipelineStageFlags[]) {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            },
            .commandBufferCount = 1,
            .pCommandBuffers = (VkCommandBuffer[]) { cmd },
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = vulkan_get_semaphores(self, VULKAN_SEMAPHORE_RENDER_FINISHED) + (image_index + self->frame_index * self->image_count),
        },
    };

    if (vulkan_throw_api(vkQueueSubmit(self->queues[VULKAN_QUEUE_GRAPHIC], 1, submit_infos, vulkan_get_fences(self, VULKAN_FENCE_FRAME_ENDED)[self->frame_index]))) return VULKAN_CODE_SUBMIT_QUEUE_ERROR;

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = vulkan_get_semaphores(self, VULKAN_SEMAPHORE_RENDER_FINISHED) + (image_index + self->frame_index * self->image_count),
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
    VulkanCode code = vulkan_create_mesh_buffers(self, mesh_info);
    if (code != VULKAN_CODE_SUCCESS) return code;

    code = vulkan_create_buffers_memory(self, (unsigned[VULKAN_MESH_BUFFER_COUNT]) { VULKAN_MESH_BUFFER_VERTEX, VULKAN_MESH_BUFFER_INDEX }, VULKAN_MESH_BUFFER_COUNT, VULKAN_MEMORY_MESH, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (code != VULKAN_CODE_SUCCESS) return code;

    code = vulkan_create_buffers_memory(self, (unsigned[VULKAN_MESH_BUFFER_COUNT]) { VULKAN_TEMP_BUFFER_STAGE_VERTEX, VULKAN_TEMP_BUFFER_STAGE_INDEX }, VULKAN_TEMP_BUFFER_COUNT, VULKAN_MEMORY_TEMP, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (code != VULKAN_CODE_SUCCESS) return code;

    code = vulkan_map_mesh(self, mesh_info);
    if (code != VULKAN_CODE_SUCCESS) return code;

    self->mesh_count = mesh_info->count;

    vkFreeMemory(self->device, self->memories[VULKAN_MEMORY_TEMP], NULL);
    self->memories[VULKAN_MEMORY_TEMP] = NULL;

    vkDestroyBuffer(self->device, self->buffers[VULKAN_TEMP_BUFFER_STAGE_INDEX], NULL);
    self->buffers[VULKAN_TEMP_BUFFER_STAGE_INDEX] = NULL;

    vkDestroyBuffer(self->device, self->buffers[VULKAN_TEMP_BUFFER_STAGE_VERTEX], NULL);
    self->buffers[VULKAN_TEMP_BUFFER_STAGE_VERTEX] = NULL;
    
    return VULKAN_CODE_SUCCESS;
}

VulkanCode vulkan_bind_objects(Vulkan *self, const VulkanObjectsInfo *info) {
    assert(self != NULL);
    assert(info != NULL);

    self->objects = calloc(info->count, sizeof(VulkanObject));
    if (self->objects == NULL) return VULKAN_CODE_ALLOCATE_ERROR;

    for (int i = 0; i < info->count; ++i) {
        assert(info->pipelines[i] < VULKAN_PIPELINE_COUNT);
        assert(info->meshes[i] < self->mesh_count);

        self->object_count[info->pipelines[i]]++;
    }

    int offsets[VULKAN_PIPELINE_COUNT];
    (void) memset(offsets, 0, VULKAN_PIPELINE_COUNT * sizeof(int));

    for (int i = 0; i < info->count; ++i) {
        VulkanObject *obj = vulkan_get_objects(self, info->pipelines[i]) + offsets[info->pipelines[i]];
        obj->mesh = info->meshes[i];

        offsets[info->pipelines[i]]++;
    }

    return VULKAN_CODE_SUCCESS;
}

VulkanObject *vulkan_get_objects(Vulkan *self, unsigned pipeline) {
    assert(self != NULL);
    assert(pipeline < VULKAN_PIPELINE_COUNT);

    static unsigned offsets[VULKAN_PIPELINE_COUNT] = {
        0,
    };

    static bool cached = false;
    if (cached) return self->objects + offsets[pipeline];

    for (int i = 1; i < VULKAN_PIPELINE_COUNT; ++i) {
        offsets[i] += offsets[i - 1] + self->object_count[i - 1];
    }

    cached = true;
    return self->objects + offsets[pipeline];
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

        case VULKAN_CODE_CREATE_BUFFER_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to create buffer\n");
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

        case VULKAN_CODE_RESET_COMMAND_BUFFER_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to reset command buffer\n");
            return true;
        }

        case VULKAN_CODE_WAIT_QUEUE_ERROR: {
            (void) fprintf(stderr, "Vulkan Error: Failed to wait queue\n");
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
