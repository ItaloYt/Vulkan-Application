#pragma once

#include <stdbool.h>

#include <vulkan/vulkan.h>

#include "window.h"
#include "shaders.h"

enum {
    VULKAN_QUEUE_GRAPHIC = 0,
    VULKAN_QUEUE_PRESENT,
    VULKAN_QUEUE_COUNT,

    VULKAN_RENDER_PASS_BASE_UI = 0,
    VULKAN_RENDER_PASS_COUNT,

    VULKAN_PIPELINE_LAYOUT_BASE_UI = 0,
    VULKAN_PIPELINE_LAYOUT_COUNT,

    VULKAN_PIPELINE_BASE_UI = 0,
    VULKAN_PIPELINE_COUNT,

    VULKAN_COMMAND_POOL_GRAPHIC = 0,
    VULKAN_COMMAND_POOL_COUNT,

    VULKAN_GRAPHIC_COMMAND_DRAW = 0,
    VULKAN_GRAPHIC_COMMAND_COUNT,

    VULKAN_SEMAPHORE_IMAGE_AVAILABLE = 0,
    VULKAN_SEMAPHORE_RENDER_FINISHED,
    VULKAN_SEMAPHORE_COUNT,

    VULKAN_FENCE_FRAME_ENDED = 0,
    VULKAN_FENCE_COUNT,

    VULKAN_BUFFERS_MESH = 0,
    VULKAN_BUFFERS_INDEX,
    VULKAN_BUFFERS_COUNT,

    VULKAN_MEMORIES_MESH = 0,
    VULKAN_MEMORIES_COUNT,

    VULKAN_FRAMES_IN_FLIGHT = 1,
};

typedef struct Vulkan Vulkan;

struct Vulkan {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR format;

    VkQueue queues[VULKAN_QUEUE_COUNT];
    unsigned queue_indices[VULKAN_QUEUE_COUNT];

    VkShaderModule shaders[EMBED_SHADERS_COUNT];
    VkPipelineLayout pipeline_layouts[VULKAN_PIPELINE_LAYOUT_COUNT];
    VkRenderPass render_passes[VULKAN_RENDER_PASS_COUNT];
    VkFramebuffer *framebuffers[VULKAN_RENDER_PASS_COUNT];
    VkPipeline pipelines[VULKAN_PIPELINE_COUNT];
    VkCommandPool command_pools[VULKAN_COMMAND_POOL_COUNT];
    VkCommandBuffer *command_buffers[VULKAN_COMMAND_POOL_COUNT];

    VkSemaphore *semaphores[VULKAN_SEMAPHORE_COUNT];
    VkFence *fences[VULKAN_FENCE_COUNT];

    VkBuffer buffers[VULKAN_BUFFERS_COUNT];
    VkDeviceMemory memories[VULKAN_MEMORIES_COUNT];
    unsigned buffer_sizes[VULKAN_BUFFERS_COUNT];
    unsigned *memory_offsets[VULKAN_MEMORIES_COUNT];

    VkExtent2D *extent;
    VkImage *images;
    VkImageView *views;

    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical;
    VkDevice device;
    VkSwapchainKHR swapchain;

    VkPresentModeKHR mode;
    unsigned image_count;
    unsigned frame_index;

    bool should_resize;
};

struct VulkanMeshInfo {
    const void *const *meshes;
    const unsigned *mesh_sizes;
    const unsigned mesh_count;
};
typedef struct VulkanMeshInfo VulkanMeshInfo;

enum VulkanCode {
    VULKAN_CODE_SUCCESS = 0,
    VULKAN_CODE_UNDEFINED_ERROR,
    VULKAN_CODE_ALLOCATE_ERROR,
    VULKAN_CODE_CREATE_INSTANCE_ERROR,
    VULKAN_CODE_FAILED_ERROR,
    VULKAN_CODE_LOAD_PHYSICALS_ERROR,
    VULKAN_CODE_GET_PRESENTATION_SUPPORT_ERROR,
    VULKAN_CODE_NO_GRAPHIC_QUEUE_ERROR,
    VULKAN_CODE_NO_PRESENT_QUEUE_ERROR,
    VULKAN_CODE_CREATE_DEVICE_ERROR,
    VULKAN_CODE_GET_SURFACE_CAPABILITIES_ERROR,
    VULKAN_CODE_LOAD_SURFACE_FORMATS_ERROR,
    VULKAN_CODE_LOAD_SURFACE_PRESENT_MODES_ERROR,
    VULKAN_CODE_CREATE_SWAPCHAIN_ERROR,
    VULKAN_CODE_LOAD_SWAPCHAIN_IMAGES_ERROR,
    VULKAN_CODE_CREATE_SWAPCHAIN_VIEWS_ERROR,
    VULKAN_CODE_CREATE_SHADERS_ERROR,
    VULKAN_CODE_CREATE_PIPELINE_LAYOUTS_ERROR,
    VULKAN_CODE_CREATE_RENDER_PASSES_ERROR,
    VULKAN_CODE_CREATE_FRAMEBUFFERS_ERROR,
    VULKAN_CODE_CREATE_PIPELINES_ERROR,
    VULKAN_CODE_CREATE_COMMAND_POOLS_ERROR,
    VULKAN_CODE_CREATE_COMMAND_BUFFERS_ERROR,
    VULKAN_CODE_CREATE_SEMAPHORES_ERROR,
    VULKAN_CODE_CREATE_FENCES_ERROR,
    VULKAN_CODE_GET_FENCE_STATUS_ERROR,
    VULKAN_CODE_RESET_FENCES_ERROR,
    VULKAN_CODE_RESIZE_SWAPCHAIN_ERROR,
    VULKAN_CODE_ACQUIRE_NEXT_IMAGE_ERROR,
    VULKAN_CODE_BEGIN_COMMAND_BUFFER_ERROR,
    VULKAN_CODE_END_COMMAND_BUFFER_ERROR,
    VULKAN_CODE_SUBMIT_QUEUE_ERROR,
    VULKAN_CODE_PRESENT_QUEUE_ERROR,
    VULKAN_CODE_CREATE_MESH_BUFFER_ERROR,
    VULKAN_CODE_NO_MEMORY_TYPE_ERROR,
    VULKAN_CODE_CREATE_MEMORY_ERROR,
    VULKAN_CODE_BIND_BUFFER_MEMORY_ERROR,
    VULKAN_CODE_MAP_MEMORY_ERROR,
};
typedef enum VulkanCode VulkanCode;

VulkanCode vulkan_init(Vulkan *self, Window *window);
VulkanCode vulkan_render(Vulkan *self, Window *window);
VulkanCode vulkan_bind_mesh(Vulkan *self, const VulkanMeshInfo *info);
void vulkan_destroy(Vulkan *self);

VulkanCode vulkan_create_instance(Vulkan *self, Window *window);
VulkanCode vulkan_load_physical(Vulkan *self, Window *window);
VulkanCode vulkan_get_surface_capabilities(Vulkan *self, Window *window);
VulkanCode vulkan_load_surface_format(Vulkan *self);
VulkanCode vulkan_load_surface_present_mode(Vulkan *self);
VulkanCode vulkan_create_device(Vulkan *self);
VulkanCode vulkan_create_swapchain(Vulkan *self);
VulkanCode vulkan_load_swapchain_images(Vulkan *self);
VulkanCode vulkan_create_swapchain_views(Vulkan *self);
void vulkan_destroy_swapchain_views(Vulkan *self);
VulkanCode vulkan_create_shaders(Vulkan *self);
void vulkan_destroy_shaders(Vulkan *self);
VulkanCode vulkan_create_pipeline_layouts(Vulkan *self);
void vulkan_destroy_pipeline_layouts(Vulkan *self);
VulkanCode vulkan_create_render_passes(Vulkan *self);
void vulkan_destroy_render_passes(Vulkan *self);
VulkanCode vulkan_create_framebuffers(Vulkan *self);
void vulkan_destroy_framebuffers(Vulkan *self);
VulkanCode vulkan_create_pipelines(Vulkan *self);
void vulkan_destroy_pipelines(Vulkan *self);
VulkanCode vulkan_create_command_pools(Vulkan *self);
void vulkan_destroy_command_pools(Vulkan *self);
VulkanCode vulkan_create_command_buffers(Vulkan *self);
void vulkan_destroy_command_buffers(Vulkan *self);
VulkanCode vulkan_create_semaphores(Vulkan *self);
void vulkan_destroy_semaphores(Vulkan *self);
VulkanCode vulkan_create_fences(Vulkan *self);
void vulkan_destroy_fences(Vulkan *self);
VulkanCode vulkan_create_mesh_buffer(Vulkan *self, const VulkanMeshInfo *mesh_info);
void vulkan_destroy_buffers(Vulkan *self);
VulkanCode vulkan_create_mesh_memory(Vulkan *self);
void vulkan_destroy_memories(Vulkan *self);
VulkanCode vulkan_map_mesh(Vulkan *self, const VulkanMeshInfo *mesh_info);
void vulkan_destroy_memory_offsets(Vulkan *self);

void vulkan_draw_base_ui(Vulkan *self, unsigned image_index);

VulkanCode vulkan_resize_swapchain(Vulkan *self, Window *window);

bool vulkan_throw(VulkanCode code);
bool vulkan_throw_api(VkResult result);
