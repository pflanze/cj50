#include <cj50.h>

int main () {
#define SIZ_VALS 4
    int vals[SIZ_VALS] = { -3, 5, 8, 133 };
    DA(vals, SIZ_VALS);

    int vals2[SIZ_VALS] = { -3, 5, 8, 133 };
    int vals3[SIZ_VALS - 1] = { -3, 5, 8 };
    int vals4[SIZ_VALS] = { -3, 5, 8, 132 };

    assert(equal_array(vals, SIZ_VALS, vals2, SIZ_VALS) == true);
    assert(equal_array(vals, SIZ_VALS, vals3, SIZ_VALS - 1) == false);
    assert(equal_array(vals, SIZ_VALS, vals4, SIZ_VALS) == false);

#undef SIZ_VALS
}
