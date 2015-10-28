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

    list->head = head;
    list->tail = tail;

    // This point reached
    printf("checkpoint 1 reached\n");

    /* 
    FIXME
    when I allocated memory for the list it should
    initialize the head and tail so there can be pointers to them
    */
    // this should dereference list and get pointer to head
    // dereference head and get pointer to nextNode
    // make the pointer to nextNode
    // dereference list and get pointer to tail
    // make head's pointer to nextNode equal the pointer to tail
    list->head->nextNode = list->tail;
    list->tail->prevNode = list->head;

    printf("check2\n");

    // There will be a NULL node before the first node is added
    //list->head->nextNode = NULL;
    //list->tail->prevNode = NULL;

    printf("checkpoint 2 reached\n");

    //LinkedList *returnList = (LinkedList *)malloc(sizeof(*list));
    //printf("checkpoint 3 reached\n");
    //*returnList = *list;
    printf("checkpoint 4 reached\n");;

    return list;
}

void DestroyLinkedList(LinkedList *A)
{
    free(A);
}

/* Add the element val to the end of the linked list A */
void AddEndOfList(LinkedList *A, unsigned int val)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->value = val;
    newNode->nextNode = A->tail;
    newNode->prevNode = A->tail->prevNode;
    A->tail->prevNode = newNode;
    newNode->prevNode->nextNode = newNode;
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
int FirstRequestAmount(LinkedList *A)
{
    if (A->head->nextNode != A->tail)
    {
        return A->head->nextNode->value;
    }
    return -1;
}
