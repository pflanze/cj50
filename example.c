#include <cj50.h>

int main() {
    print_string("What is your name? ");
    string name = get_string();
    print_string("Hi ");
    print_string(name);
    print_string("!\n");
    print_string("What is your age? ");
    int age = get_int();
    print_string("In a year, you will be ");
    print_int(age + 1);
    print_string("!\n");
}
