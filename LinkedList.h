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

LinkedList * NewLinkedList();

void DestroyLinkedList();

void AddEndOfList(LinkedList *, unsigned int);

void DeleteFirstRequest(LinkedList *);

unsigned int FirstRequestAmount(LinkedList *);

#endif
