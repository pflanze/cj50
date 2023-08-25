#include <cj50.h>


/// -1..1 -> -1..1
Option(float) circle_y(float x) {
    float y = sinf(acosf(x));
    return isnan(y) ? None(float) : some(y);
}

// one side of the body line
Option(float) snail_y(float x) {
    if_let_Some(y, circle_y(x)) {
        return some(y * -(x - 3.0f) / 16.0f);
    } else_None {
        return None(float);
    }
}

bool render_snail(SDL_Renderer *rdr, void *ctx, Vec2(int) window_dimensions) {
    float *t = ctx;
    *t += 0.04;

    set_draw_color(rdr, color(100, 140, 246));
    clear(rdr);
    set_draw_color(rdr, color(110, 100, 50));

    float x = -1.01;
    for (int i = 0; i < 480; i++) {
        if_let_Some(y, snail_y(x)) {
            // adding waves
            float yi = y * (200 + sinf(-*t + x * 40) * 5.);
            draw_rect(rdr, rect2(vec2_float(100. + i, 240. + yi),
                                 vec2_float(1, -yi * 2.)));
        } else_None;
        x += 2./400.;
    }                      
    return true;
}

Result(Unit, String) run(UNUSED slice(cstr) argv) {
    BEGIN_Result(Unit, String);

    if (getenv("NOGRAPHICS")) {
        RETURN_Ok(Unit(), cleanup);
    }

    if (getenv("PLOT")) {
        DEF_SLICE(ColorFunction_float, fs, {
                { color(255, 0, 0), circle_y },
                { color(0, 255, 0), snail_y },
            });
        plot_functions_float(fs, rect2(vec2_float(-1.4, -1.1),
                                       vec2_float(2.8, 2.2)));
    } else {
        float t = 0.0;
        graphics_render("Snail", vec2_int(640, 480), render_snail, &t);
    }

    RETURN_Ok(Unit(), cleanup);
cleanup:
    END_Result();
}

MAIN(run);
