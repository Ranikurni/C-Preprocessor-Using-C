#include <stdio.h>

#define A 10
#define ADD(x, y) ((x) + (y))
#define RESULT ADD(A, 20)

int main()
{
    printf("Result = %d\n", RESULT);
    return 0;
}
