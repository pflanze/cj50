#include <cj50.h>

static
void render1(bool do_rect, VertexRenderer *vertexrenderer,
             SDL_Renderer *rdr, Vec2(int) window_dimensions) {
    int x = 10;
    for (int i = 0; ; i++) {
        int diameter = square(i);
        int next_x = x + 10 + diameter;
        if (next_x > window_dimensions.x) {
            break;
        }
        float diameter_y = diameter * (1.2f + 1.1f * sinf(i));
        AUTO r = rect2_float(vec2_float(x, window_dimensions.y - 10 - diameter_y),
                             vec2_float(diameter, diameter_y));
        if (do_rect) {
            draw_fill_rect(rdr, r);
        } else {
            // XX A not working
            draw_fill_ellipsis(vertexrenderer,
                               r,
                               0.f,
                               ColorA(255, 255, 255, 255),
                               20);
        }
        x = next_x;
    }
}


bool render(SDL_Renderer *rdr, UNUSED void *_ctx, Vec2(int) window_dimensions) {
    float *t = _ctx;
    *t += 1./60.;

    set_draw_color(rdr, color(250, 240, 40));
    clear(rdr);
    set_draw_color(rdr, color(240, 30, 10));

    // Old style circle around the center:
    draw_fill_circle(rdr, vec2_int(500, 400), 120);

    AUTO vertexrenderer = new_VertexRenderer();

    // Rectangles at the bottom
    render1(true, &vertexrenderer, rdr, window_dimensions);
    // Ellipses on top of the rectangles at the bottom
    render1(false, &vertexrenderer, rdr, window_dimensions);

    // Spinning ellipsis
    draw_fill_ellipsis(&vertexrenderer,
                       rect2_float(vec2_float(300, 10),
                                   vec2_float(100, 400)),
                       *t,
                       ColorA(0, 0, 0, 255),
                       30);

    render_VertexRenderer(rdr, &vertexrenderer);
    drop(vertexrenderer);
    return true;
}

Result(Unit, String) run(UNUSED slice(cstr) argv) {
    BEGIN_Result(Unit, String);

    if (getenv("NOGRAPHICS"))
        RETURN_Ok(Unit(), cleanup);

    float t = 0.;
    graphics_render("Circle", vec2_int(1000, 800), render, &t);

    RETURN_Ok(Unit(), cleanup);
cleanup:
    END_Result();
}

MAIN(run);
