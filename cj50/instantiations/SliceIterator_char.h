#pragma once

#include <cj50/gen/SliceIterator.h>
#include <cj50/instantiations/Vec_char.h> /* slice(char) */
#include <cj50/ref_char.h> /* Option(ref(char)) */
// #include <cj50/char.h> /* Option(char) */

#define T char
#include <cj50/gen/template/SliceIterator.h>
#undef T

