#include <cj50.h>

// This is not an example, but is currently placed here for ease of
// running as part of the tests: report the sizes of some structs.


GENERATE_Result(char, ParseError);
GENERATE_Result(double, ParseError);
GENERATE_Result(string, ParseError);


#define S(T) do {                               \
        print(#T);                              \
        print("\t");                            \
        println(sizeof(T));                     \
    } while(0)

int main() {
    S(Vec2);
    S(Vec3);
    S(Line2);
    S(Rect2);
    S(Color);

    S(SyscallInfo);
    S(SystemError);

    S(ParseError);

    S(Option(char));
    S(Option(int));
    S(Option(float));
    S(Option(double));
    S(Option(string));

    S(Result(char, ParseError));
    S(Result(int, ParseError));
    S(Result(float, ParseError));
    S(Result(double, ParseError));
    S(Result(string, ParseError));

    S(Result(string, SystemError));
}
