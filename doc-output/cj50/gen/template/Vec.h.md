<!-- THIS FILE WAS AUTO-GENERATED by gendoc. DO NOT EDIT! -->


Part of the [`cj50/gen/Vec.h`](../Vec.h.md) library: the parts
instantiated only once per Vec parametrization.

# Types

## slice {#zero_slice}

```C
typedef struct slice(T)
```

A `slice` consists of two fields, a pointer to an array, and the
current length used out of that array. A slice is borrowing the
storage it is representing (either from the `Vec` it is taken from
(meaning, you can't modify or move the `Vec` while any `slice` of
it is in use), or from static storage).

## mutslice {#zero_mutslice}

```C
typedef struct mutslice(T)
```

A `mutslice` is like a `slice` but allows mutation of the items in
the slice. Only maximally one `mutslice` to the same storage
should exist at any time to avoid the risk of concurrent mutation.

## Vec {#zero_Vec}

```C
typedef struct Vec(T)
```

A Vec consists of three fields, a pointer to a heap-allocated
array, the current size of that array, and the current length used
out of that array.

Never mutate those fields directly, use accessor functions
instead!

# Normal functions

## drop_Vec_\$T {#one_drop_Vec_QordninetwoEQDT}

```C
void drop_Vec_$T(Vec(T) self)
```

Remove from existence, along with the owned elements.

## new_Vec_\$T {#one_new_Vec_QordninetwoEQDT}

```C
Vec(T) new_Vec_$T()
```

Construct a new, empty vector

## with_capacity_Vec_\$T {#one_with_capacity_Vec_QordninetwoEQDT}

```C
Vec(T) with_capacity_Vec_$T(size_t cap)
```

Construct a new, empty vector with at least the specified capacity.

## push_within_capacity_Vec_\$T {#one_push_within_capacity_Vec_QordninetwoEQDT}

```C
Result(Unit, VecError) push_within_capacity_Vec_$T(
    Vec(T) *self, T value)
```

Appends an element to the back of the vector if there's still
capacity left for it, otherwise returns a
`VecError_OutOfCapacity` error. I.e. never allocates additional
memory.

## reserve_Vec_\$T {#one_reserve_Vec_QordninetwoEQDT}

```C
void reserve_Vec_$T(Vec(T) *self, size_t additional)
```

Reserve space for `additional` more elements on top of the current
capacity (not len).

## push_Vec_\$T {#one_push_Vec_QordninetwoEQDT}

```C
void push_Vec_$T(Vec(T) *self, T value)
```

Appends an element to the back of the vector.

## pop_Vec_\$T {#one_pop_Vec_QordninetwoEQDT}

```C
Option(T) pop_Vec_$T(Vec(T) *self)
```

Removes the last element from a vector and returns it, or None if
it is empty.

## append_Vec_\$T {#one_append_Vec_QordninetwoEQDT}

```C
void append_Vec_$T(Vec(T) *self, Vec(T) *other)
```

Moves all the elements of `other` into `self`, leaving `other` empty.

## append_move_Vec_\$T {#one_append_move_Vec_QordninetwoEQDT}

```C
void append_move_Vec_$T(Vec(T) *self, Vec(T) other)
```

Moves all the elements of `other` into `self`, consuming `other`.

## clear_Vec_\$T {#one_clear_Vec_QordninetwoEQDT}

```C
void clear_Vec_$T(Vec(T) *self)
```

Clears the vector, removing all values.

Note that this method has no effect on the allocated capacity of
the vector.

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
