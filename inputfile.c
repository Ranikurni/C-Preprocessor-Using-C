#include <stdio.h>
#include <myheader.h>

#define PI 3.14
#define MAX 100
#define SQUARE(x) ((x)*(x))

int main()
{
    // Calculate square
    float area = PI * 5 * 5;

    int a = MAX;
    int result = SQUARE(5);

    printf("Area = %f\n", area);
    printf("Maximum = %d\n", a);
    printf("Square = %d\n", result);

    display();

    return 0;
}
