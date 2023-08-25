#include <cj50.h>

const int window_width = 640;
const int window_height = 480;

typedef struct Ctx {
    float t; // time in seconds
    VertexRenderer rdr;
} Ctx;

void drop_Ctx(Ctx self) {
    drop_VertexRenderer(self.rdr);
}


// Luminosity wave; f1 and f2 are holding x for period time, y for
// amplitude.

float wavef(float t, Vec2(float) f1, Vec2(float) f2, float low, float high) {
    float y1 = sinf(math_pi * t / f1.x) * f1.y;
    float y2 = sinf(math_pi * t / f2.x) * f2.y;
    float wanted_range = high - low;
    float effective_range = (f1.y + f2.y) * 4.f;
    return (wanted_range * (y1 + y2) / effective_range) + low + (high-low)/2;
}

int wave(float t, Vec2(float) f1, Vec2(float) f2, int low, int high) {
    int res = wavef(t, f1, f2, low, high);
    assert(res <= high);
    assert(res >= low);
    return res;
}

void render_floating_triangle(VertexRenderer *rdr, float t,
                              Vec2(float) fx1, Vec2(float) fx2,
                              Vec2(float) fy1, Vec2(float) fy2,
                              float size, float rotation_speed,
                              SDL_Color color) {
    Vec2(float) center = vec2_float(
        wavef(t, fx1, fx2, -size, window_width + size),
        wavef(t, fy1, fy2, -size, window_height + size));
    float angle = t * rotation_speed;
    int p1 = push_vertex(rdr,
                         vertex_2(add(center, vec2_float(size * sinf(angle),
                                                         size * cosf(angle))),
                                  color));
    angle += math_pi * 2. / 3.;
    int p2 = push_vertex(rdr,
                         vertex_2(add(center, vec2_float(size * sinf(angle),
                                                         size * cosf(angle))),
                                  color));
    angle += math_pi * 2. / 3.;
    int p3 = push_vertex(rdr,
                         vertex_2(add(center, vec2_float(size * sinf(angle),
                                                         size * cosf(angle))),
                                  color));
    push_triangle(rdr, vec3_int(p1, p2, p3));
}

bool render(SDL_Renderer* renderer, void* _ctx) {
    Ctx* ctx = _ctx;

    clear_VertexRenderer(&ctx->rdr);

    float t = ctx->t;

    // Diamond shape
    SDL_Color c1 = ColorA(
        wave(t, vec2_float(3., 1.), vec2_float(2.9, 0.99), 0, 255),
        wave(t, vec2_float(4.13, 0.5), vec2_float(5.11, 0.7), 90, 255),
        wave(t, vec2_float(0.23, 0.5), vec2_float(9.3, 0.7), 0, 255),
        255);
    SDL_Color c2 = ColorA(250, 200, 30, 255);
    int v1 = push_vertex(&ctx->rdr, vertex_2(vec2_float(90, 100), c1));
    int v2 = push_vertex(&ctx->rdr,
                         vertex_2(vec2_float(wavef(t,
                                                   vec2_float(12, 1),
                                                   vec2_float(11.123, 1.1),
                                                   340, 440),
                                             wavef(t,
                                                   vec2_float(11.913, 1),
                                                   vec2_float(8.71, 0.5),
                                                   160, 240)),
                                  c2));
    int v3 = push_vertex(&ctx->rdr,
                         vertex_2(vec2_float(wavef(t,
                                                   vec2_float(12, 1),
                                                   vec2_float(11.123, 1.1),
                                                   140, 220),
                                             wavef(t,
                                                   vec2_float(11.913, 1),
                                                   vec2_float(8.71, 0.5),
                                                   260, 340)),
                                  c2));
    push_triangle(&ctx->rdr, vec3_int(v1, v2, v3));
    SDL_Color c3 = ColorA(255 - c1.r,
                          255 - c1.g,
                          255 - c1.b,
                          255);
    int v4 = push_vertex(&ctx->rdr, vertex_2(vec2_float(500, 380), c3));
    push_triangle(&ctx->rdr, vec3_int(v2, v3, v4));

    // Floating triangles:
    render_floating_triangle(&ctx->rdr, t,
                             vec2_float(4.6, 1.0), vec2_float(0.6, 0.4),
                             vec2_float(7.6, 1.0), vec2_float(0.7, 0.4),
                             100, 3.6,
                             ColorA(255, 20, 10, 150));
    render_floating_triangle(&ctx->rdr, t - 4.0,
                             // XX change these
                             vec2_float(4.6, 1.0), vec2_float(0.6, 0.4),
                             vec2_float(7.6, 1.0), vec2_float(0.7, 0.4),
                             90, -2.4,
                             ColorA(0, 255, 30, 120));
    render_floating_triangle(&ctx->rdr, t - 2.0,
                             // XX change these
                             vec2_float(3.6, 1.0), vec2_float(1.6, 0.4),
                             vec2_float(8.6, 1.0), vec2_float(0.8, 0.4),
                             105, -2.8,
                             ColorA(0, 30, 255, 140));

    clear(renderer);
    asserting_sdl(SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND));
    render_VertexRenderer(renderer, &ctx->rdr);
    
    ctx->t += 1./60.;
    return true;
}

int main() {
    if (getenv("NOGRAPHICS")) {
        return 0;
    }

    Ctx ctx = {
        0,
        new_VertexRenderer(),
    };
    graphics_render("Vertex Graphics",
                    window_width, window_height, render, &ctx);
    drop_Ctx(ctx);
}
