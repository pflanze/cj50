#include <cj50.h>

int main() {
    if (!getenv("NONAME")) {
        print_string("What is your name? ");
        string name = get_string();
        print_string("Hi ");
        print_string(name);
        print_string("!\n");
        print_string("What is your age? ");
        nat0 age = get_nat0();
        print_string("In a year, you will be ");
        print_nat0(age + 1);
        print_string("!\n");
    }

    if (!getenv("NOFLOAT")) {
        print_string("Width: ");
        float w = get_float();
        print_string("Height: ");
        float h = get_float();
        print_string("Area = ");
        print_float(w * h);
        print_string("\n");
    }
}
