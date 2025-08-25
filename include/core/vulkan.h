#pragma once

#include <stdbool.h>

#include <vulkan/vulkan.h>

#include "window.h"
#include "shaders.h"

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
    VULKAN_CODE_CREATE_BUFFER_ERROR,
    VULKAN_CODE_NO_MEMORY_TYPE_ERROR,
    VULKAN_CODE_CREATE_MEMORY_ERROR,
    VULKAN_CODE_BIND_BUFFER_MEMORY_ERROR,
    VULKAN_CODE_MAP_MEMORY_ERROR,
    VULKAN_CODE_RESET_COMMAND_BUFFER_ERROR,
    VULKAN_CODE_WAIT_QUEUE_ERROR,
};
typedef enum VulkanCode VulkanCode;

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
    VULKAN_GRAPHIC_COMMAND_EXTRA,
    VULKAN_GRAPHIC_COMMAND_LAST,

    VULKAN_COMMAND_BUFFER_COUNT,
    VULKAN_GRAPHIC_COMMAND_COUNT = VULKAN_GRAPHIC_COMMAND_LAST - VULKAN_GRAPHIC_COMMAND_DRAW,

    VULKAN_SEMAPHORE_IMAGE_AVAILABLE = 0,
    VULKAN_SEMAPHORE_RENDER_FINISHED,
    VULKAN_SEMAPHORE_COUNT,

    VULKAN_FENCE_FRAME_ENDED = 0,
    VULKAN_FENCE_COUNT,

    VULKAN_MESH_BUFFER_VERTEX = 0,
    VULKAN_MESH_BUFFER_INDEX,
    VULKAN_MESH_BUFFER_LAST,

    VULKAN_TEMP_BUFFER_STAGE_VERTEX = VULKAN_MESH_BUFFER_LAST,
    VULKAN_TEMP_BUFFER_STAGE_INDEX,
    VULKAN_TEMP_BUFFER_LAST,

    VULKAN_BUFFER_COUNT = VULKAN_TEMP_BUFFER_LAST,
    VULKAN_MESH_BUFFER_COUNT = VULKAN_MESH_BUFFER_LAST - VULKAN_MESH_BUFFER_VERTEX,
    VULKAN_TEMP_BUFFER_COUNT = VULKAN_TEMP_BUFFER_LAST - VULKAN_TEMP_BUFFER_STAGE_VERTEX,

    VULKAN_MEMORY_MESH = 0,
    VULKAN_MEMORY_TEMP,
    VULKAN_MEMORY_COUNT,

    VULKAN_FRAMES_IN_FLIGHT = 1,
};

struct VulkanObject {
    unsigned mesh;
};
typedef struct VulkanObject VulkanObject;

struct Vulkan {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR format;

    VkQueue queues[VULKAN_QUEUE_COUNT];
    unsigned queue_indices[VULKAN_QUEUE_COUNT];

    VkShaderModule shaders[EMBED_SHADERS_COUNT];
    VkPipelineLayout pipeline_layouts[VULKAN_PIPELINE_LAYOUT_COUNT];
    VkRenderPass render_passes[VULKAN_RENDER_PASS_COUNT];
    VkFramebuffer *framebuffers; // [VULKAN_RENDER_PASS_COUNT][self->image_count]
    VkPipeline pipelines[VULKAN_PIPELINE_COUNT];
    VkCommandPool command_pools[VULKAN_COMMAND_POOL_COUNT];
    VkCommandBuffer *command_buffers;

    VkSemaphore *semaphores;
    VkFence *fences;

    VkBuffer buffers[VULKAN_BUFFER_COUNT];
    VkDeviceMemory memories[VULKAN_MEMORY_COUNT];
    unsigned buffer_sizes[VULKAN_BUFFER_COUNT];
    unsigned *mesh_offsets;
    unsigned *index_offsets;
    unsigned buffer_offsets[VULKAN_BUFFER_COUNT];
    unsigned *index_count;

    VulkanObject *objects;
    unsigned object_count[VULKAN_PIPELINE_COUNT];

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
    unsigned mesh_count;

    bool should_resize;
};
typedef struct Vulkan Vulkan;

struct VulkanMeshInfo {
    const void *const *meshes;
    const unsigned *mesh_sizes;
    const unsigned *const *indices;
    const unsigned *index_count;
    const unsigned count;
};
typedef struct VulkanMeshInfo VulkanMeshInfo;

struct VulkanObjectsInfo {
    const unsigned *pipelines;
    const unsigned *meshes;
    unsigned count;
};
typedef struct VulkanObjectsInfo VulkanObjectsInfo;

VulkanCode vulkan_init(Vulkan *self, Window *window);
VulkanCode vulkan_render(Vulkan *self, Window *window);
VulkanCode vulkan_bind_mesh(Vulkan *self, const VulkanMeshInfo *info);
VulkanCode vulkan_bind_objects(Vulkan *self, const VulkanObjectsInfo *info);
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
VkFramebuffer *vulkan_get_framebuffers(Vulkan *self, unsigned render_pass);

VulkanCode vulkan_create_pipelines(Vulkan *self);
void vulkan_destroy_pipelines(Vulkan *self);

VulkanCode vulkan_create_command_pools(Vulkan *self);
void vulkan_destroy_command_pools(Vulkan *self);

VulkanCode vulkan_create_command_buffers(Vulkan *self);
VkCommandBuffer *vulkan_get_command_buffers(Vulkan *self, unsigned cmd);
const unsigned *vulkan_get_command_buffers_count(Vulkan *self);
VulkanCode vulkan_copy_buffers(Vulkan *self, unsigned dst, unsigned src);

VulkanCode vulkan_create_semaphores(Vulkan *self);
void vulkan_destroy_semaphores(Vulkan *self);
VkSemaphore *vulkan_get_semaphores(Vulkan *self, unsigned semaphore);
const unsigned *vulkan_get_semaphores_count(Vulkan *self);

VulkanCode vulkan_create_fences(Vulkan *self);
void vulkan_destroy_fences(Vulkan *self);
VkFence *vulkan_get_fences(Vulkan *self, unsigned fence);
const unsigned *vulkan_get_fences_count(Vulkan *self);

VulkanCode vulkan_create_mesh_buffers(Vulkan *self, const VulkanMeshInfo *mesh_info);
void vulkan_destroy_buffers(Vulkan *self);

VulkanCode vulkan_create_buffers_memory(Vulkan *self, unsigned *buffers, unsigned count, unsigned memory, VkMemoryPropertyFlags flags);
void vulkan_destroy_memories(Vulkan *self);

VulkanCode vulkan_map_mesh(Vulkan *self, const VulkanMeshInfo *mesh_info);

VulkanObject *vulkan_get_objects(Vulkan *self, unsigned pipeline);

void vulkan_draw_base_ui(Vulkan *self, unsigned image_index);

VulkanCode vulkan_resize_swapchain(Vulkan *self, Window *window);

bool vulkan_throw(VulkanCode code);
bool vulkan_throw_api(VkResult result);
