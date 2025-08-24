#include <stdio.h>

#include "core/window.h"
#include "backend/wayland/backend.h"

WindowCode window_load_backend(Window **window) {
    static Window win = {0};
    
    if (wayland_throw(wayland_load(&win))) return WINDOW_CODE_BACKEND_ERROR;

    *window = &win;

    return WINDOW_CODE_SUCCESS;
}

bool window_throw(WindowCode code) {
    switch(code) {
        case WINDOW_CODE_SUCCESS: {
            return false;
        }

        case WINDOW_CODE_BACKEND_ERROR: {
            (void) fprintf(stderr, "Window Error: Failed to load backend\n");
            return true;
        }
    }

    (void) fprintf(stderr, "Window Error: Invalid code %#02x\n", code);
    return true;
}
