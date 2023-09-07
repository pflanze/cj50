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

typedef struct Ctx {
    float t;
    u32 framenumber;
    int draw_every_nth_frame;
    bool slowly;
} Ctx;

bool render_snail(SDL_Renderer *rdr, void *_ctx, Vec2(int) window_dimensions) {
    Ctx* ctx = _ctx;
    ctx->t += 0.04;

    set_draw_color(rdr, color(100, 140, 246));
    clear(rdr);
    set_draw_color(rdr, color(110, 100, 50));

    if (ctx->framenumber == 0) {
        ctx->framenumber = ctx->draw_every_nth_frame;

        float x = -1.01;
        for (int i = 0; i < 480; i++) {
            if (ctx->slowly) {
                SDL_RenderPresent(rdr);
                sleep_float(0.01);
            }

            if_let_Some(y, snail_y(x)) {
                // adding waves
                float yi = y * (200 + sinf(-ctx->t + x * 40) * 5.);
                draw_rect(rdr, rect2(vec2_float(100. + i, 240. + yi),
                                     vec2_float(1, -yi * 2.)));
            } else_None;
            x += 2./400.;
        }
    }
    ctx->framenumber--;

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
        int draw_every_nth_frame;
        char *nth = getenv("NTH");
        if (nth) {
            draw_every_nth_frame = TRY(parse_nat(nth), cleanup);
        } else {
            draw_every_nth_frame = 1;
        }

        Ctx ctx = {
            0.,
            0,
            draw_every_nth_frame,
            getenv("SLOWLY") ? true : false,
        };
        graphics_render("Not a snail", vec2_int(640, 480), render_snail, &ctx);
    }

    RETURN_Ok(Unit(), cleanup);
cleanup:
    END_Result();
}

MAIN(run);
