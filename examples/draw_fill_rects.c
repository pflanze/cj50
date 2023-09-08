#include <cj50.h>

#define FLOAT double

// -1..1 to 0..1
static
float halfscale(float x) {
    return (x + 1.0f) * 0.5f;
}

// The position at time t; each component is in the range 0..1
static
Vec2(float) position(FLOAT t) {
    return vec2_float(halfscale(sin(t * 0.8)),
                      halfscale(sin(0.3 + t * 0.8102)));
}

typedef struct Actor {
    FLOAT t; // local time, argument to `position`
    float halfwidth; // half the width of the rectangle
} Actor;

static
Rect2(float) rect_Actor(const Actor *self, Vec2(int) window_dimensions) {
    AUTO pos = position(self->t);
    float hw = self->halfwidth;
    return rect2_float(vec2_float(pos.x * (window_dimensions.x - 40) + 20.f - hw,
                                  pos.y * (window_dimensions.y - 40) + 20.f - hw),
                       vec2_float(hw * 2.f, hw * 2.f));
}

// Implement Vec(Actor) (and first the necessary functions and types for that)
static bool equal_Actor(UNUSED const Actor *a, UNUSED const Actor *b) { UNIMPLEMENTED }
static void drop_Actor(UNUSED Actor self) { }
static int print_debug_Actor(UNUSED const Actor* self) { UNIMPLEMENTED }
GENERATE_Option(Actor);
#define T Actor
#include <cj50/gen/template/Vec.h>
#undef T

typedef struct Ctx {
    FLOAT t;
    FLOAT last_actor_creation_t;
    FLOAT dt_per_actor;
    FLOAT current_halfwidth;
    Vec(Actor) actors;
    Vec(Rect2(float)) rects;
} Ctx;

void drop_Ctx(Ctx self) {
    drop_Vec_Actor(self.actors);
    drop_Vec_Rect2_float(self.rects);
}

const FLOAT t60th = 1./60.;

bool render(SDL_Renderer *rdr, void *_ctx, Vec2(int) window_dimensions) {
    Ctx *ctx = _ctx;

    ctx->t += t60th;

    if (ctx->actors.len < 300) {
        FLOAT t_since_last = ctx->t - ctx->last_actor_creation_t;
        if (t_since_last > ctx->dt_per_actor) {
            Actor a = {
                // Be precise with regards to how far behind the last actor this
                // one is (don't just start at 0.):
                .t = t_since_last - ctx->dt_per_actor,
                .halfwidth = ctx->current_halfwidth,
            };
            push_Vec_Actor(&ctx->actors, a);
            ctx->current_halfwidth *= 0.991f; // 0.991**300 * 10 = ~0.6
            ctx->last_actor_creation_t = ctx->t;
            // Use numbers like 1., 0.999, 1.001
            //ctx->dt_per_actor *= 1.0002;
        }
    }

    set_draw_color(rdr, color(255, 255, 255));
    clear(rdr);

    clear(&ctx->rects);
    FOR_EACH(actor, &ctx->actors, {
            push(&ctx->rects, rect_Actor(actor, window_dimensions));
            actor->t += t60th;
        });

    set_draw_color(rdr, color(20, 30, 100));
    draw_fill_rects(rdr, deref(&ctx->rects));

    return true;
}

Result(Unit, String) run(UNUSED slice(cstr) argv) {
    BEGIN_Result(Unit, String);

    if (getenv("NOGRAPHICS")) {
        RETURN_Ok(Unit(), cleanup);
    }

    Ctx ctx = {
        .t = 1.,
        .last_actor_creation_t = 0.,
        .dt_per_actor = 2.,
        .current_halfwidth = 10.,
        .actors = new_Vec_Actor(),
        .rects = new_Vec_Rect2_float(),
    };
    graphics_render("Draw filled rectangles",
                    vec2_int(640, 480),
                    render,
                    &ctx);

    drop_Ctx(ctx);

    RETURN_Ok(Unit(), cleanup);
cleanup:
    END_Result();
}

MAIN(run);
