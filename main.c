#include <stdlib.h>

int main() {
    int *numbers = malloc(10 * sizeof(int));

    free(numbers);
    return 0; 
}