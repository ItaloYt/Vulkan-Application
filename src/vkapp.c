#include "core/vkapp.h"

const void *const *vkapp_get_meshes() {
    static const Vector2 base_ui[] = {
        (Vector2) { .x = -1, .y = -1, },
        (Vector2) { .x = 1, .y = -1, },
        (Vector2) { .x = 1, .y = -0.8 },
    };

    static const void *const meshes[VKAPP_MESHES_COUNT] = {
        base_ui,
    };

    return meshes;
}

const unsigned *vkapp_get_mesh_sizes() {
    static const unsigned sizes[VKAPP_MESHES_COUNT] = {
        3 * sizeof(Vector2),
    };

    return sizes;
}
