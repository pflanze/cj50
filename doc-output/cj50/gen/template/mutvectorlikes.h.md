<!-- THIS FILE WAS AUTO-GENERATED by gendoc. DO NOT EDIT! -->


Part of the [`cj50/gen/Vec.h`](../Vec.h.md) library: the parts
instantiated once per mutable vectorlike (Vec, mutslice).

# Normal functions

## set_\$VEC_\$T {#one_set_QordninetwoEQDVEC_QordninetwoEQDT}

```C
void set_$VEC_$T(VEC(T) *self, size_t index, T value)
```

Move the given `value` into the slot with the given `index`,
replacing the value that was there previously. Aborts if `index`
isn't smaller than the current `len` of the vector.

## mutslice_of_\$VEC_\$T {#one_mutslice_of_QordninetwoEQDVEC_QordninetwoEQDT}

```C
mutslice(T) mutslice_of_$VEC_$T(VEC(T) *self, Range range)
```

Get a mutable slice of the vectorlike. Aborts for invalid indices. (XX todo:
Use get_mutslice_* for a variant that returns failures instead.)

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
