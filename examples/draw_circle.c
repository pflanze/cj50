#include <cj50.h>

bool render(SDL_Renderer *rdr, UNUSED void *_ctx, Vec2(int) window_dimensions) {
    set_draw_color(rdr, color(30, 240, 40));
    clear(rdr);
    set_draw_color(rdr, color(240, 30, 10));
    draw_fill_circle(rdr, vec2_int(500, 400), 120);
    return true;
}

Result(Unit, String) run(UNUSED slice(cstr) argv) {
    BEGIN_Result(Unit, String);

    if (getenv("NOGRAPHICS"))
        RETURN_Ok(Unit(), cleanup);

    graphics_render("Circle", vec2_int(1000, 800), render, NULL);

    RETURN_Ok(Unit(), cleanup);
cleanup:
    END_Result();
}

MAIN(run);
