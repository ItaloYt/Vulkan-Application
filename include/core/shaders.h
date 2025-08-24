#pragma once

enum {
    EMBED_SHADERS_BASE_UI_VERT,
    EMBED_SHADERS_BASE_UI_FRAG,
    EMBED_SHADERS_COUNT,
};

const char *const *embed_get_shaders();
const unsigned *embed_get_shader_sizes();

