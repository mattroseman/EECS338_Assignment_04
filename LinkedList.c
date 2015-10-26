#include "LinkedList.h"

/* Initialize a new empty linked list */
static LinkedList NewLinkedList()
{
    LinkedList list;
    // gets the pointer to the lists head, derefrence and get pointer to nextNode
    (*list.head).nextNode = list.tail;
    (*list.tail).prevNode = list.head;
}

/* Add the element val to the end of the linked list A */
static void AddEndOfList(LinkedList *A, unsigned int val)
{
    // list is the dereferenced list
    LinkedList list = *A;
    Node temp;
    Node newNode;
    newNode.value = val;
    // newNode pointer to next node equals list's pointer to tail
    newNode.nextNode = list.tail;
    // newNode pointer to previous node equals list's tail node's pointer to previous node
    newNode.prevNode = (*list.tail).prevNode;
    // list's tail node's previous pointer points to the newNode
    (*list.tail).prevNode = &newNode;
    // newNode's previous node points to newNode instead of tail
    (*newNode.prevNode).nextNode = &newNode;
}

/* Remove the first element from linked list A */
static void DeleteFirstRequest(LinkedList *A)
{
    LinkedList list = *A;
    // if the head doesn't point to the tail the list isn't empty
    if ((*list.head).nextNode != list.tail)
    {
        // temp equals a pointer to the second node in the list
        Node *temp = (*(*list.head).nextNode).nextNode;
        // head's nextNode equal the pointer to the second node
        (*list.head).nextNode = temp;
        // second nodes prevNode equals the pointer to the list's head
        (*temp).prevNode = list.head;
    }
    // If the list is empty do nothing
}

/* Get the value of the first node in linked list A */
/* Returns -1 if the list is empty */
static int FirstRequestAmount(LinkedList *A)
{
    LinkedList list = *A;
    // if the head doesn't point to the tail the list isn't empty
    if ((*list.head).nextNode != list.tail)
    {
        // the heads nextNodes value
        return (*(*list.head).nextNode).value;
    }
    return -1;
}
