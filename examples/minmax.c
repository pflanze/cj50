#include <cj50.h>

int main() {
    DBG(MIN(10, 30));
    DBG(MIN3(10, 30, 3));
    DBG(MIN3(10, -30, 3));
    DBG(MAX(10, 30));
    DBG(MAX3(10, 30, 3));
    DBG(MAX3(10, -30, 3));
}
