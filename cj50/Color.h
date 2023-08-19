#pragma once

/// A standard 8-bit RGB color representation.
typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

/// Constructing a color
static UNUSED
Color color(uint8_t r, uint8_t g, uint8_t b) {
    return (Color) { r, g, b };
}

static UNUSED
void drop_Color(UNUSED Color v) {}

static UNUSED
bool equal_Color(const Color *a, const Color *b) {
    return ((a->r == b->r) &&
            (a->g == b->g) &&
            (a->b == b->b));
}

static UNUSED
bool equal_move_Color(Color a, Color b) {
    return equal_Color(&a, &b);
}

static UNUSED
int print_debug_Color(const Color *v) {
    return printf("color(%i, %i, %i)", v->r, v->g, v->b);
}

static UNUSED
int print_debug_move_Color(Color v) {
    return print_debug_Color(&v);
}

