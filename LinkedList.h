#ifndef _LinkedList_h
#define _LinkedList_h

typedef struct Node
{
    struct Node *nextNode;
    struct Node *prevNode;
    unsigned int value;
}Node;

typedef struct LinkedList
{
    Node *head;
    Node *tail;
}LinkedList;

static LinkedList NewLinkedList();

static void AddEndOfList(LinkedList *, unsigned int);

static void DeleteFirstRequest(LinkedList *);

static int FirstRequestAmount(LinkedList *);

#endif
