
#include <cj50/unicode.h> /* ucodepoint, well cycle */

#pragma once /* placed late to avoid cycle, ? */

#include <cj50/unicodeError.h>
#include <cj50/gen/Result.h>
#include <cj50/size_t.h>

GENERATE_Result(ucodepoint, UnicodeError);

