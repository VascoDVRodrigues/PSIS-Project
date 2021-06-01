#ifndef _LISTA_H
#define _LISTA_H

/* def for pointer casts */
#define Item void *

/* type definition for structure to hold list item */
typedef struct _LinkedList LinkedList;

LinkedList *createList(void);

LinkedList *insertNode(LinkedList *head, Item data);

LinkedList *deleteNode(LinkedList *head, Item data_to_delete, int compareItem(Item, Item), void freeItem(Item));

LinkedList *updateNode(LinkedList *head, Item data_to_update, Item update, int compareItem(Item, Item), void freeItem(Item));

Item *searchNode(LinkedList *head, Item data_to_find, int compareItem(Item, Item));

int numItens(LinkedList *head);

void printList(LinkedList *head, void printItem(Item), int tabs);

LinkedList *clearList(LinkedList *head, void freeItem(Item));

#endif