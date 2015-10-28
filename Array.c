#include <stdlib.h>
#include <stdio.h>

#include "Array.h"

void AddEndOfList(unsigned int *A[], unsigned int *size, unsigned int val)
{
    printf("Check 1 %u\n", *size + 1);

    printf("The pointer passed into AddEndOfList is %p\n", *A);

    //FIXME for some reason the program is getting stuck here
    // I know that *A is the same pointer that was returned from malloc
    // I know that *size + 1 is the correct new size
    // realloc isn't failing because perror isn't showing up
    // process is getting hung somewhere

    // makes an array of one larger size and copies elements from *A over to it
    unsigned int *L;
    if ((L = (unsigned int *)realloc(*A, *size + 1)) == NULL)
    {
        perror("realloc failed\n");
        exit(EXIT_FAILURE);
    }

    A = &L;

    printf("Check 2\n");

    // the extra spot we added equals val
    *A[*size] = val;

    printf("Check 3 %u", *A[*size]);

    printf("%u\n", **A);
    ++*size;
    printf("%u\n", *size);
}

void DeleteFirstRequest(unsigned int *A, unsigned int *size)
{
    int i;
    // from the first element to the second to last element
    for(i = 0; i < (*size - 1); i++)
    {
        A[i] = A[i+1];
    }
    *size -= 1;
}

int FirstRequestAmount(unsigned int *A, unsigned int *size)
{
    return *A;
}
