#include <stdlib.h>
#include <stdio.h>

#include "LinkedList.h"

/* Initialize a new empty linked list */
LinkedList * NewLinkedList()
{
    // Allocating memory for list should
    // initialize a list by
    // allocate memory for the pointer to head
    // allocate memory for the pointer to tail
    // need to allocate the memory for actual head and tail nodes
    // then make the pointers in list point to those allocated spots
    LinkedList *list = malloc(sizeof(*list));
    // allocates memory for the head node
    // initializes a head node at that memory
    Node *head = malloc(sizeof(*head));
    // allocates memory for the tail node
    // initializes a tail node at that memory
    Node *tail = malloc(sizeof(*tail));

    // this pointer should equal the list->tail pointer which
    // should equal the list->head->nextNode pointer
    printf("pointer to the tail allocated by malloc: %p\n", tail);

    list->head = head;
    list->tail = tail;

    printf("pointer to the tail inside the LinkedList struct: %p\n", list->tail);

    list->head->nextNode = list->tail;
    list->tail->prevNode = list->head;

    printf("pointer from head to the nextNode: %p\n", list->head->nextNode);

    printf("pointer to the list allocated by malloc: %p\n", list);

    // this should return the allocated spot in memory that linked list was initialized
    return list;
}

void DestroyLinkedList(LinkedList *A)
{
    free(A);
}

/* Add the element val to the end of the linked list A */
void AddEndOfList(LinkedList *A, unsigned int val)
{
    printf("pointer to the list allocated by malloc: %p\n", A);

    /* 
    FIXME
    When adding the first element the process seems to hang up at this point
    unable to access A->tail
    which should be the pointer to the tail element
    which was allocated and printed in the init method
    */
    printf("pointer to the tail inside of the list: %p\n", A->tail);
    /* 
    FIXME
    The pointer to head also can't be reached
    the items inside the linked list must not have been
    allocated?
    */
    printf("pointer from the head to the nextNode: %p\n", A->head->nextNode);

    // allocates memory for a new node and initializes that node
    // includes allocating/initializing pointers to a nextNode and prevNode
    // includes allocating/initializing an unsigned int
    Node *newNode = malloc(sizeof(Node));
    newNode->value = val;

    // this is printing fine meaning that memory was allocated fine and unsigned int val was initialized
    printf("%u\n", newNode->value);

    printf("Check1\n");

    // it can't print this even though tail should have a value
    printf("%p\n", A->tail);
    printf("%u\n", A->tail->value);

    // the pointer nextNode was initialized but currently has no value
    // A->tail should be initialized
    newNode->nextNode = A->tail;

    printf("Check2\n");

    newNode->prevNode = A->tail->prevNode;

    printf("Check3\n");

    A->tail->prevNode = newNode;

    printf("Check4\n");

    newNode->prevNode->nextNode = newNode;

    printf("Check end\n");
}

/* Remove the first element from linked list A */
void DeleteFirstRequest(LinkedList *A)
{
    if (A->head->nextNode != A->tail)
    {
        Node *temp = A->head->nextNode->nextNode;
        free(A->head->nextNode);
        A->head->nextNode = temp;
        temp->prevNode = A->head;
    }
}

/* Get the value of the first node in linked list A */
/* Returns -1 if the list is empty */
unsigned int FirstRequestAmount(LinkedList *A)
{
    if (A->head->nextNode != A->tail)
    {
        return A->head->nextNode->value;
    }
    return -1;
}
