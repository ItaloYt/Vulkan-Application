#include "core/vkapp.h"

int main(void) {
    // clock_t t0 = clock();

    Window *window = NULL;
    if (window_throw(window_load_backend(&window))) return 1;

    if (window_throw(window->init(window))) {
        window->destroy(window);

        return 1;
    }

    Vulkan vulkan = {0};
    if (vulkan_throw(vulkan_init(&vulkan, window))) {
        window->destroy(window);

        return 1;
    }

    const VulkanMeshInfo mesh_info = {
        .meshes = vkapp_get_meshes(),
        .mesh_sizes = vkapp_get_mesh_sizes(),
        .indices = vkapp_get_indices(),
        .index_count = vkapp_get_index_count(),
        .count = VKAPP_MESH_COUNT,
    };

    const VulkanObjectsInfo objects_info = {
        .pipelines = (unsigned[]) {
            VULKAN_PIPELINE_BASE_UI,
            VULKAN_PIPELINE_BASE_UI,
        },
        .meshes = (unsigned[]) {
            VKAPP_MESH_TOP_RECTANGLE,
            VKAPP_MESH_BOTTOM_RECTANGLE,
        },
        .count = 2,
    };

    if (
        vulkan_throw(vulkan_bind_mesh(&vulkan, &mesh_info)) ||
        vulkan_throw(vulkan_bind_objects(&vulkan, &objects_info))
    ) {
        vulkan_destroy(&vulkan);
        window->destroy(window);

        return 1;
    }

    // clock_t t1 = clock();

    while (window->open) {
        // double delta = (double) (t1 - t0) / CLOCKS_PER_SEC;

        // (void) printf("FPS: %d\n", (int) (1 / delta));

        // t0 = clock();

        if (window_throw(window->update(window))) {
            vulkan_destroy(&vulkan);
            window->destroy(window);

            return 1;
        }

        if (vulkan_throw(vulkan_render(&vulkan, window))) {
            vulkan_destroy(&vulkan);
            window->destroy(window);

            return 1;
        }

        // t1 = clock();
    }

    vulkan_destroy(&vulkan);
    window->destroy(window);

    return 0;
}
