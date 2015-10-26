#ifndef LinkedList_h
#define LinkedList_h

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

LinkedList NewLinkedList();

void AddEndOfList(LinkedList *, unsigned int);

void DeleteFirstRequest(LinkedList *);

int FirstRequestAmount(LinkedList *);

#endif