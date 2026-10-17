#include "demo.h"

#define PI 3.14159
#define AREA(r) (PI * SQUARE(r))

int main(void)
{
    int n = MAX;
    float a = AREA(5);

    printf("n=%d area=%f\n", n, a);

    return 0;
}
