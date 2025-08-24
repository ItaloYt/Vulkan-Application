// #include <time.h>

#include "core/window.h"
#include "core/vulkan.h"

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
