<!-- THIS FILE WAS AUTO-GENERATED by gendoc. DO NOT EDIT! -->


Operations on slices and `Vec`s.

The `VEC` type is either `Vec`, `slice` or `mutslice`. It is
called "vectorlike" from here onwards.

# Normal functions

## len_VEC_$T {#one_len_VEC_QDT}

```C
size_t len_VEC_$T(const VEC(T) *self)
```

The number of elements the vectorlike is currently holding.

## is_empty_VEC_$T {#one_is_empty_VEC_QDT}

```C
size_t is_empty_VEC_$T(const VEC(T) *self)
```

Whether the vectorlike has exactly 0 elements.

## equal_VEC_$T {#one_equal_VEC_QDT}

```C
bool equal_VEC_$T(const VEC(T) *a, const VEC(T) *b)
```

Whether the two vectorlikes have the same number of elements with equal
elements in every position.

## print_debug_VEC_$T {#one_print_debug_VEC_QDT}

```C
int print_debug_VEC_$T(const VEC(T) *self)
```

Print in C code syntax.

## print_debug_move_VEC_$T {#one_print_debug_move_VEC_QDT}

```C
int print_debug_move_VEC_$T(VEC(T) self)
```

Print in C code syntax, consuming the argument.

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