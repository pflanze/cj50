#include <cj50.h>

float average(const float* vs, size_t len) {
    float total = 0;
    for (size_t i = 0; i < len; i++) {
        total += vs[i];
    }
    return total / len;
}


int main() {
    if (!getenv("NONAME")) {
        print("What is your name? ");
        Option(string) name = get_string();
        D(name);
        if (!name.is_some) {
            DIE("You cancelled.");
        }
        print("Hi ");
        print(name.value);
        print("!\n");
        print("What is your age? ");
        int age = unwrap(get_nat0());
        print("In a year, you will be ");
        print_nat0(age + 1);
        print("!\n");
        drop(name);
    }

    if (!getenv("NOFLOAT")) {
        print("Width: ");
        float w = unwrap(get_float());
        print("Height: ");
        float h = unwrap(get_float());
        print("Area = ");
        print(w * h);
        print("\n");
    }

    if (!getenv("NOARRAY")) {
        print("How many tests have you done? ");
        int n = unwrap(get_nat0());
        float* grades = new_floats(n);
        for (int i = 0; i < n; i++) {
            print("What was your grade for test no. ");
            print(i + 1);
            print("? ");
            float grade = unwrap(get_float());
            grades[i] = grade;
        }

        bool cont = false;
        while (!cont) {
            float avg = average(grades, n);
            print("average(");
            print_debug_array(grades, n);
            print(") = ");
            print(avg);
            print("\n");

            print("Do you want to (e)dit, (r)esize, (c)ontinue? ");
            string ans = unwrap(get_string());
            if (ans[0] == 'e') {
                print("Which test (1-based)? ");
                int i = unwrap(get_nat()) - 1;
                if (i < n) {
                    print("What grade? ");
                    float g = unwrap(get_float());
                    grades[i] = g;
                } else {
                    print("Your test number is outside the range [1..");
                    print(n);
                    print("]. Try again.\n");
                }
                
            } else if (ans[0] == 'r') {
                print("To which length? ");
                int newlen = unwrap(get_nat());
                grades = resize(grades, n, newlen);
                n = newlen;

            } else if (ans[0] == 'c') {
                cont = true;
            } else {
                print("Invalid answer.\n");
            }
            drop(ans);
        }
        drop(grades);
    }

    if (!getenv("NONAMES")) {
        print("How many people do we have? ");
        int n = unwrap(get_nat0());
        string* ss = new_strings(n);
        for (int i = 0; i < n; i++) {
            print("What is the name of person no. ");
            print(i + 1);
            print("? ");
            ss[i] = unwrap(get_string());
        }
        print("Our people are:\n");
        print_debug_array(ss, n);
        print("\n");
        drop_array(ss, n);
    }

    if (!getenv("NOMATH")) {
        Vec2 a = { 30, 44.3 };
        D(a);
        Vec2 b = vec2(-7.1, 10);
        D(b);
        Vec2 c = add(a, b);
        D(c);
        D(mul(c, 3));
        D(neg(c));
    }
}
