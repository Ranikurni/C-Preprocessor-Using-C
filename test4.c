#include <stdio.h>

#define SQUARE(x) ((x) * (x))
#define ADD(a,b) ((a) + (b))

int main()
{
    int a = SQUARE(5);
    int b = ADD(10, 20);

    printf("Square = %d\n", a);
    printf("Addition = %d\n", b);

    return 0;
}
