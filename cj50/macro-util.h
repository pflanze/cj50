/*
  Copyright (C) 2021-2023 Christian Jaeger, <ch@christianjaeger.ch>
  Published under the terms of the MIT License, see the LICENSE file.
*/

/*
  Utilities for writing macros
*/

#pragma once


#define STR(s) #s
#define XSTR(s) STR(s)

#define _CAT(a, b) a##b
#define XCAT(a,b) _CAT(a,b)
#define _CAT3(a,b,c) a##b##c
#define XCAT3(a,b,c) _CAT3(a,b,c)

// (Can't use XCAT3(var, _, __LINE__) safely, right? Sigh. Anyway it's
// good to have a larger distance, anyway.)
#define HYGIENIC(var) XCAT3(var, _hygienic,  __LINE__)

