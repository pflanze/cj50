#ifndef CJMATH_H_
#define CJMATH_H_


typedef struct Vec2 {
    float x;
    float y;
} Vec2;

#define Vec2(x, y) ((Vec2) { (x), (y) })


typedef struct Vec3 {
    float x;
    float y;
    float z;
} Vec3;

#define Vec3(x, y, z) ((Vec3) { (x), (y), (z) })


Vec2 add_Vec2(Vec2 a, Vec2 b) {
    return Vec2(a.x + b.x, a.y + b.y);
}
Vec2 sub_Vec2(Vec2 a, Vec2 b) {
    return Vec2(a.x - b.x, a.y - b.y);
}
Vec2 neg_Vec2(Vec2 a) {
    return Vec2(-a.x, -a.y);
}

Vec3 add_Vec3(Vec3 a, Vec3 b) {
    return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}
Vec3 sub_Vec3(Vec3 a, Vec3 b) {
    return Vec3(a.x - b.x, a.y - b.y, a.z - b.z);
}
Vec3 neg_Vec3(Vec3 a) {
    return Vec3(-a.x, -a.y, -a.z);
}


Vec2 mul_Vec2_float(Vec2 a, float b) {
    return Vec2(a.x * b, a.y * b);
}
Vec3 mul_Vec3_float(Vec3 a, float b) {
    return Vec3(a.x * b, a.y * b, a.z * b);
}

void print_debug_Vec2(Vec2 a) {
    printf("Vec2(%g, %g)", a.x, a.y);
}
void print_debug_Vec3(Vec3 a) {
    printf("Vec3(%g, %g, %g)", a.x, a.y, a.z);
}


typedef struct Line2 {
    Vec2 start;
    Vec2 end;
} Line2;


#define add(a, b)                               \
    _Generic((a)                                \
             , Vec2: add_Vec2                   \
             , Vec3: add_Vec3                   \
        )((a), (b))

#define sub(a, b)                               \
    _Generic((a)                                \
             , Vec2: sub_Vec2                   \
             , Vec3: sub_Vec3                   \
        )((a), (b))

#define neg(a)                                  \
    _Generic((a)                                \
             , Vec2: neg_Vec2                   \
             , Vec3: neg_Vec3                   \
        )(a)

#define mul(a, b)                                               \
    _Generic((a)                                                \
             , Vec2: _Generic((b)                               \
                              , int: mul_Vec2_float             \
                              , float: mul_Vec2_float           \
                              , double: mul_Vec2_float          \
                 )                                              \
             , Vec3: _Generic((b)                               \
                              , int: mul_Vec3_float             \
                              , float: mul_Vec3_float           \
                              , double: mul_Vec3_float          \
                 )                                              \
        )((a), (b))


#endif /* CJMATH_H_ */
