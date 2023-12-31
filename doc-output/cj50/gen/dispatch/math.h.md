<!-- THIS FILE WAS AUTO-GENERATED by gendoc. DO NOT EDIT! -->

# Generic functions

## add {#two_add}

```C
add(a, b)
```

Add together two values, for which `+` is not defined, e.g. Vec2
or Vec3. Both values need to be of the same type.

Example:

```C
DBG(add(vec2_int(10, -3), vec2_int(3, 5)))  /* prints vec2(13, 2) */
```

Members:

```C
Vec2(double) : Vec2_double add_Vec2_double(Vec2_double a, Vec2_double b);
Vec2(float)  : Vec2_float add_Vec2_float(Vec2_float a, Vec2_float b);
Vec2(int)    : Vec2_int add_Vec2_int(Vec2_int a, Vec2_int b);
Vec3(double) : Vec3_double add_Vec3_double(Vec3_double a, Vec3_double b);
Vec3(float)  : Vec3_float add_Vec3_float(Vec3_float a, Vec3_float b);
Vec3(int)    : Vec3_int add_Vec3_int(Vec3_int a, Vec3_int b);
```

## sub {#two_sub}

```C
sub(a, b)
```

Subtraction of two values for which `-` is not defined, e.g. Vec2
or Vec3. Both values need to be of the same type.

Example:

```C
DBG(sub(vec2(10, -3), vec2(3, 5)))  /* prints vec2(7, -8) */
```

Members:

```C
Vec2(double) : Vec2_double sub_Vec2_double(Vec2_double a, Vec2_double b);
Vec2(float)  : Vec2_float sub_Vec2_float(Vec2_float a, Vec2_float b);
Vec2(int)    : Vec2_int sub_Vec2_int(Vec2_int a, Vec2_int b);
Vec3(double) : Vec3_double sub_Vec3_double(Vec3_double a, Vec3_double b);
Vec3(float)  : Vec3_float sub_Vec3_float(Vec3_float a, Vec3_float b);
Vec3(int)    : Vec3_int sub_Vec3_int(Vec3_int a, Vec3_int b);
```

## neg {#two_neg}

```C
neg(a)
```

Negate a value for which `-` is not defined, e.g. Vec2 or Vec3.

Example:

```C
DBG(neg(vec2(10, -3)))  /* prints vec2(-10, 3) */
```

Members:

```C
Vec2(double) : Vec2_double neg_Vec2_double(Vec2_double a);
Vec2(float)  : Vec2_float neg_Vec2_float(Vec2_float a);
Vec2(int)    : Vec2_int neg_Vec2_int(Vec2_int a);
Vec3(double) : Vec3_double neg_Vec3_double(Vec3_double a);
Vec3(float)  : Vec3_float neg_Vec3_float(Vec3_float a);
Vec3(int)    : Vec3_int neg_Vec3_int(Vec3_int a);
```

## mul {#two_mul}

```C
mul(a, b)
```

Multiply two values for which `*` is not defined, e.g. Vec2 or
Vec3. NOTE: the name of this operation might change! Currently
only `float` is supported for the second argument.

Example:

```C
DBG(mul(vec2(10, -3), 2))  /* prints vec2(20, -6) */
```

Members:

```C
double : Vec2_double mul_Vec2_double_float(Vec2_double a, float b);
double : Vec2_float mul_Vec2_float_float(Vec2_float a, float b);
double : Vec2_int mul_Vec2_int_float(Vec2_int a, float b);
double : Vec3_double mul_Vec3_double_float(Vec3_double a, float b);
double : Vec3_float mul_Vec3_float_float(Vec3_float a, float b);
double : Vec3_int mul_Vec3_int_float(Vec3_int a, float b);
float  : Vec2_double mul_Vec2_double_float(Vec2_double a, float b);
float  : Vec2_float mul_Vec2_float_float(Vec2_float a, float b);
float  : Vec2_int mul_Vec2_int_float(Vec2_int a, float b);
float  : Vec3_double mul_Vec3_double_float(Vec3_double a, float b);
float  : Vec3_float mul_Vec3_float_float(Vec3_float a, float b);
float  : Vec3_int mul_Vec3_int_float(Vec3_int a, float b);
int    : Vec2_double mul_Vec2_double_float(Vec2_double a, float b);
int    : Vec2_float mul_Vec2_float_float(Vec2_float a, float b);
int    : Vec2_int mul_Vec2_int_float(Vec2_int a, float b);
int    : Vec3_double mul_Vec3_double_float(Vec3_double a, float b);
int    : Vec3_float mul_Vec3_float_float(Vec3_float a, float b);
int    : Vec3_int mul_Vec3_int_float(Vec3_int a, float b);
```

<hr>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
<p>&nbsp;</p>
