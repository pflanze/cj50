#pragma once


#include <cj50/gen/Vec.h> /* for slice, except can't do parametrization automatically */
#include <cj50/gen/ref.h>

#define SliceIterator(T) XCAT(SliceIterator_, T)

