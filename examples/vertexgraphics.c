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


bool render(SDL_Renderer* renderer, void* _ctx) {
    Ctx* ctx = _ctx;

    clear_VertexRenderer(&ctx->rdr);
    SDL_Color c = ColorA(250, 200, 30, 255);
    int v1 = push_vertex(&ctx->rdr, vertex_2(vec2_float(90, 100), c));
    int v2 = push_vertex(&ctx->rdr, vertex_2(vec2_float(380, 200), c));
    int v3 = push_vertex(&ctx->rdr, vertex_2(vec2_float(180, 300), c));
    push_triangle(&ctx->rdr, vec3_int(v1, v2, v3));
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
