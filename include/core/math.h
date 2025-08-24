#pragma once

static inline int math_max_int(int a, int b) {
    return (a > b ? a : b);
}

static inline int math_min_int(int a, int b) {
    return (a < b ? a : b);
}

static inline int math_clamp_int(int value, int min, int max) {
    return math_max_int(math_min_int(value, max), min);
}
