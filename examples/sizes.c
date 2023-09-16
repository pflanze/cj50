#include <cj50.h>

// This is not an example, but is currently placed here for ease of
// running as part of the tests: report the sizes of some structs.


GENERATE_Result(char, ParseError);
GENERATE_Result(double, ParseError);
GENERATE_Result(cstr, ParseError);
GENERATE_Result(String, ParseError);


#define S(T) do {                               \
        print(#T);                              \
        print("\t");                            \
        println(sizeof(T));                     \
    } while(0)

int main() {
    S(Vec2(float));
    S(Vec3(float));
    S(Rect2(float));
    S(Color);

    S(SyscallInfo);
    S(SystemError);

    S(ParseError);

    S(Option(char));
    S(Option(u8));
    S(Option(int));
    S(Option(float));
    S(Option(double));
    S(Option(cstr));

    S(Result(char, ParseError));
    S(Result(int, ParseError));
    S(Result(float, ParseError));
    S(Result(double, ParseError));
    S(Result(cstr, ParseError));
    S(Result(String, ParseError));

    S(Result(Option(u8), SystemError));
    S(Result(String, SystemError));
}
