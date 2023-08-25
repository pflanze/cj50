#include <cj50.h>

typedef struct Ctx {
    float t; // time in seconds
    VertexRenderer rdr;
} Ctx;

static
void drop_Ctx(Ctx self) {
    drop_VertexRenderer(self.rdr);
}


// Luminosity wave; f1 and f2 are holding x for period time, y for
// amplitude.

static
float wavef(float t, Vec2(float) f1, Vec2(float) f2, float low, float high) {
    float y1 = sinf(math_pi * t / f1.x) * f1.y;
    float y2 = sinf(math_pi * t / f2.x) * f2.y;
    float wanted_range = high - low;
    float effective_range = (f1.y + f2.y) * 4.f;
    return (wanted_range * (y1 + y2) / effective_range) + low + (high-low)/2;
}

static
int wave(float t, Vec2(float) f1, Vec2(float) f2, int low, int high) {
    int res = wavef(t, f1, f2, low, high);
    assert(res <= high);
    assert(res >= low);
    return res;
}

static
void render_floating_triangle(VertexRenderer *rdr, Vec2(int) window_dimensions, float t,
                              Vec2(float) fx1, Vec2(float) fx2,
                              Vec2(float) fy1, Vec2(float) fy2,
                              float size, float rotation_speed,
                              SDL_Color color) {
    Vec2(float) center = vec2_float(
        wavef(t, fx1, fx2, -size, window_dimensions.x + size),
        wavef(t, fy1, fy2, -size, window_dimensions.y + size));
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

bool render(SDL_Renderer* renderer, void* _ctx, Vec2(int) window_dimensions) {
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
#define X(v) ((v)*window_dimensions.x)
#define Y(v) ((v)*window_dimensions.y)
#define POS(x, y) vec2_float(X(x), Y(y))
    int v1 = push_vertex(&ctx->rdr,
                         vertex_2(vec2_float(wavef(t,
                                                   vec2_float(11, 1),
                                                   vec2_float(5.53, 1),
                                                   X(.12), X(.16)),
                                             wavef(t,
                                                   vec2_float(13.2, 1),
                                                   vec2_float(6.61, 1),
                                                   Y(.19), Y(.23))),
                                  c1));
    int v2 = push_vertex(&ctx->rdr,
                         vertex_2(vec2_float(wavef(t,
                                                   vec2_float(12, 1),
                                                   vec2_float(11.123, 1.1),
                                                   X(.53), X(.68)),
                                             wavef(t,
                                                   vec2_float(11.913, 1),
                                                   vec2_float(8.71, 0.5),
                                                   Y(.33), Y(.5))),
                                  c2));
    int v3 = push_vertex(&ctx->rdr,
                         vertex_2(vec2_float(wavef(t,
                                                   vec2_float(12, 1),
                                                   vec2_float(11.123, 1.1),
                                                   X(.21), X(.34)),
                                             wavef(t,
                                                   vec2_float(11.913, 1),
                                                   vec2_float(8.71, 0.5),
                                                   Y(.54), Y(.71))),
                                  c2));
    push_triangle(&ctx->rdr, vec3_int(v1, v2, v3));
    SDL_Color c3 = ColorA(255 - c1.r,
                          255 - c1.g,
                          255 - c1.b,
                          255);
    int v4 = push_vertex(&ctx->rdr,
                         vertex_2(vec2_float(wavef(t,
                                                   vec2_float(11, 1),
                                                   vec2_float(5.53, 1),
                                                   X(.76), X(.80)),
                                             wavef(t,
                                                   vec2_float(13.2, 1),
                                                   vec2_float(6.61, 1),
                                                   Y(.77), Y(.81))),
                                  c3));
    push_triangle(&ctx->rdr, vec3_int(v2, v3, v4));

    // Floating triangles:
    render_floating_triangle(&ctx->rdr, window_dimensions, t,
                             vec2_float(4.6, 1.0), vec2_float(0.6, 0.4),
                             vec2_float(7.6, 1.0), vec2_float(0.7, 0.4),
                             100, 3.6,
                             ColorA(255, 20, 10, 150));
    render_floating_triangle(&ctx->rdr, window_dimensions, t - 4.0,
                             // XX change these
                             vec2_float(4.6, 1.0), vec2_float(0.6, 0.4),
                             vec2_float(7.6, 1.0), vec2_float(0.7, 0.4),
                             90, -2.4,
                             ColorA(0, 255, 30, 120));
    render_floating_triangle(&ctx->rdr, window_dimensions, t - 2.0,
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
                    vec2_int(640, 480), render, &ctx);
    drop_Ctx(ctx);
}
