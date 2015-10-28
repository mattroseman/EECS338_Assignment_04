#include <stdlib.h>
#include <stdio.h>

#include "Array.h"

/* 
TODO
Have this method take in a pointer to an array and 
copy over the items so that there is enought allocated memory*/
void AddEndOfList(unsigned int *A[], unsigned int *size, unsigned int val)
{
    printf("Check 1\n");

    // makes an array of one larger size and copies elements from *A over to it
    if ((*A = realloc(*A, *size + 1)) < 0)
    {
        perror("realloc failed\n");
        exit(EXIT_FAILURE);
    }

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
