<!-- THIS FILE WAS AUTO-GENERATED by gendoc. DO NOT EDIT! -->

# Types

## String

```C
typedef struct String {
    char* str;
} String
```

`String` is an owned type that holds a "C string", a string of
characters, more precisely, an array of `char`, that represents
the text, and a '\0' character after it to signal the end. There
is no length information, the length has to be determined by
walking the array until encountering the '\0' character (`strlen`
will do that on the contained array, or `len` on the String
wrapper type).

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