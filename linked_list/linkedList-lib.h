#include <stdio.h>
#include <stdlib.h>

typedef void * Item;

typedef struct _head Head; 

Head *create_LinkedList();

void printList(Head *head, void printfunc(Item *));

Head *insertNode(Item* item, Head *head);

Item *searchNode(Head *head, Item *item, int compare(Item *, Item *));

Head *deleteNode(Head *head, Item *item, int compare(Item *, Item *), void freeItem(Item *item));
