#include <cj50.h>

typedef double floatingpoint_t;

typedef struct Ctx {
    floatingpoint_t r;
    floatingpoint_t dr;
    Vec(Vec2(float)) points;
} Ctx;

static
void drop_Ctx(Ctx self) {
    drop(self.points);
}

static
floatingpoint_t logistic_growth(floatingpoint_t y1, floatingpoint_t r) {
    return r*y1*(1 - y1);
}


bool render(SDL_Renderer* renderer, void* _ctx,
            Vec2(int) window_dimensions) {
    Ctx* ctx = _ctx;

    AUTO points = &ctx->points;
    clear(points);
    
    floatingpoint_t r = ctx->r;
    if (! getenv("SILENT"))
        DBG(r);
    floatingpoint_t y = 0.01;
    float yscreen = window_dimensions.y;
    for (int x = 0; x < window_dimensions.x; x++) {
        /* DBG(y); */
        push(points, vec2_float(x, (1. - y) * yscreen));
        y = logistic_growth(y, r);
    }

    set_draw_color(renderer, color(0, 0, 0));
    clear(renderer);
    set_draw_color(renderer, color(255, 255, 255));
    draw_points_float(renderer, deref(points));

    ctx->r += ctx->dr;

    return true;
}

Result(Unit, String) run(slice(cstr) argv) {
    BEGIN_Result(Unit, String);

    if (len(&argv) != 3) {
        AUTO s =
            String("Usage: ");
        append_move(
            &s,
            String(at(&argv, 0)));
        append_move(
            &s,
            String(
                " r dr\n"
                "  Show the logistic function with growth parameter `r` and\n"
                "  change of `r` per frame of `dr` (could be 0), both floats."));
        RETURN_Err(s, end);
    }

    AUTO r = TRY(parse_float(*at(&argv, 1)), end);
    AUTO dr = TRY(parse_float(*at(&argv, 2)), end);

    if (getenv("NOGRAPHICS")) 
        RETURN_Ok(Unit(), end);

    Ctx ctx = {
        r,
        dr,
        new_Vec_Vec2_float(),
    };
    
    graphics_render("Logistic map",
                    vec2_int(800, 600), render, &ctx);
    drop_Ctx(ctx);
    
    RETURN_Ok(Unit(), end);
end:
    END_Result();
}

MAIN(run);
