#include <stdio.h>

#define X 5
#define Y 10
#define MULTIPLY(a, b) ((a) * (b))

int main()
{
    int a = X;
    int b = Y;

    printf("X = %d\n", X);
    printf("Y = %d\n", Y);
    printf("Multiplication = %d\n", MULTIPLY(a, b));

    return 0;
}
