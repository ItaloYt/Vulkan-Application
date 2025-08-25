#pragma once

#include "math.h"
#include "window.h"
#include "vulkan.h"

enum {
    VKAPP_MESHES_BASE_UI = 0,
    VKAPP_MESHES_COUNT,
};

const void *const *vkapp_get_meshes();
const unsigned *vkapp_get_mesh_sizes();
