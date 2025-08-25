#pragma once

#include "math.h"
#include "window.h"
#include "vulkan.h"

enum {
    VKAPP_MESH_TOP_RECTANGLE = 0,
    VKAPP_MESH_BOTTOM_RECTANGLE,
    VKAPP_MESH_COUNT,
};

const void *const *vkapp_get_meshes();
const unsigned *vkapp_get_mesh_sizes();
const unsigned *const *vkapp_get_indices();
const unsigned *vkapp_get_index_count();
