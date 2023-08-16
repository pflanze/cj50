#include <cj50.h>

float average(const float* vs, size_t len) {
    float total = 0;
    for (size_t i = 0; i < len; i++) {
        total += vs[i];
    }
    return total / len;
}

void cancel() {
    print("You cancelled.\n");
    exit(0);
}

void run_name() {
    print("What is your name? ");
    Option(CStr) name = get_CStr(); 
    DBG(name);
    if (!name.is_some) {
        cancel();
    }
    print("Hi ");
    print(name.value);
    print("!\n");
    print("What is your age? ");
    int age = unwrap(get_nat0());
    print("In a year, you will be ");
    print_nat0(age + 1);
    print("!\n");
}

void run_float() {
    print("Width: ");
    if_let_Some(w, get_float()) {
        print("Height: ");
        if_let_Some(h, get_float()) {
            print("Area = ");
            println(w * h);
        } else_None {
            cancel();
        }
    } else_None {
        cancel();
    }
}

void run_array() {
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
        String ans = unwrap(get_String());
        // check the first character
        ucodepoint ans0 = unwrap(get_ucodepoint(&ans, 0));
        if (equal(ans0, uchar("e"))) {
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

        } else if (equal(ans0, uchar("r"))) {
            print("To which length? ");
            int newlen = unwrap(get_nat());
            grades = resize(grades, n, newlen);
            n = newlen;

        } else if (equal(ans0, uchar("c"))) {
            cont = true;
        } else {
            print("Invalid answer.\n");
        }
        drop(ans);
    }
    drop(grades);
}

void run_names() {
    print("How many people do we have? ");
    int n = unwrap(get_nat0());
    Option_String* ss = new_Option_Strings(n);
    for (int i = 0; i < n; i++) {
        print("What is the name of person no. ");
        print(i + 1);
        print("? ");
        ss[i] = some(unwrap(get_String()));
    }
    print("Our people are:\n");
    print_debug_array(ss, n);
    print("\n");
    drop_array(ss, n);
}

void run_math() {
    Vec2 a = { 30, 44.3 };
    DBG(a);
    Vec2 b = vec2(-7.1, 10);
    DBG(b);
    Vec2 c = add(a, b);
    DBG(c);
    DBG(mul(c, 3));
    DBG(neg(c));
}


int main() {
    if (getenv("NAME")) run_name();
    else if (getenv("FLOAT")) run_float();
    else if (getenv("ARRAY")) run_array();
    else if (getenv("NAMES")) run_names();
    else if (getenv("MATH")) run_math();
    else {
        if (!getenv("NONAME")) run_name();
        if (!getenv("NOFLOAT")) run_float();
        if (!getenv("NOARRAY")) run_array();
        if (!getenv("NONAMES")) run_names();
        if (!getenv("NOMATH")) run_math();
    }
    return 0;
}
