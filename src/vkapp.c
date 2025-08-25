#include "core/vkapp.h"

enum {
    TOP_RECTANGLE_VERTEX_COUNT = 4,
    TOP_RECTANGLE_INDEX_COUNT = 6,
    BOTTOM_RECTANGLE_VERTEX_COUNT = 4,
    BOTTOM_RECTANGLE_INDEX_COUNT = 6,
};

const void *const *vkapp_get_meshes() {
    static const Vector2 top_rectangle[TOP_RECTANGLE_VERTEX_COUNT] = {
        (Vector2) { .x = -1, .y = -1, },
        (Vector2) { .x = 1, .y = -1, },
        (Vector2) { .x = 1, .y = -0.8 },
        (Vector2) { .x = -1, .y = -0.8 },
    };

    static const Vector2 bottom_rectangle[BOTTOM_RECTANGLE_VERTEX_COUNT] = {
        (Vector2) { .x = -1, .y = 0.8, },
        (Vector2) { .x = 1, .y = 0.8, },
        (Vector2) { .x = 1, .y = 1 },
        (Vector2) { .x = -1, .y = 1 },
    };

    static const void *const meshes[VKAPP_MESH_COUNT] = {
        top_rectangle,
        bottom_rectangle,
    };

    return meshes;
}

const unsigned *vkapp_get_mesh_sizes() {
    static const unsigned sizes[VKAPP_MESH_COUNT] = {
        TOP_RECTANGLE_VERTEX_COUNT * sizeof(Vector2),
        BOTTOM_RECTANGLE_VERTEX_COUNT * sizeof(Vector2),
    };

    return sizes;
}

const unsigned *const *vkapp_get_indices() {
    static const unsigned top_rectangle[TOP_RECTANGLE_INDEX_COUNT] = {
        0, 1, 2,
        2, 3, 0,
    };

    static const unsigned bottom_rectangle[BOTTOM_RECTANGLE_INDEX_COUNT] = {
        0, 1, 2,
        2, 3, 0,
    };

    static const unsigned *const indices[VKAPP_MESH_COUNT] = {
        top_rectangle,
        bottom_rectangle,
    };

    return indices;
}

const unsigned *vkapp_get_index_count() {
    static const unsigned count[VKAPP_MESH_COUNT] = {
        TOP_RECTANGLE_INDEX_COUNT,
        BOTTOM_RECTANGLE_INDEX_COUNT,
    };

    return count;
}
