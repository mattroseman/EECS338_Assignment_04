#include <stdlib.h>
#include <stdio.h>

#include "LinkedList.h"

/* Initialize a new empty linked list */
LinkedList * NewLinkedList()
{
    LinkedList *list;

    printf("checkpoint 1 reached\n");

    list->head->nextNode = list->tail;
    list->tail->prevNode = list->head;

    printf("checkpoint 2 reached\n");

    // do it in this order so that sizeof(list) is correct
    LinkedList *returnList = (LinkedList *)malloc(sizeof(*list));
    printf("checkpoint 3 reached\n");
    *returnList = *list;
    printf("checkpoint 4 reached\n");;

    return returnList;
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
