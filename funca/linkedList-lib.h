#ifndef _LISTA_H
#define _LISTA_H

/* def for pointer casts */
#define Item void *

/* type definition for structure to hold list item */
typedef struct _LinkedList LinkedList;

LinkedList *createList(void);

/*
 * Function: insertNode
 * ----------------------------
 *   Inserts a node at the beginning
 *
 *   data: pointer to data to add
 *	 head: pointer to the head of the list to insert the new node
 *
 *   returns: Head of the list with the new node inserted
 *
 */
LinkedList *insertNode(LinkedList *head, Item data);

/*
 * Function: deleteNode
 * ----------------------------
 *   Deletes a node inside a certain linked list
 *
 *   data_to_delete: pointer to data to delete
 *	 compareItem: function that compares two items
 * 	 freeItem: function that frees a item
 *	 head: pointer to the head of the list
 *
 *   returns: Head of the list with the update
 *
 */
LinkedList *deleteNode(LinkedList *head, Item data_to_delete, int compareItem(Item, Item), void freeItem(Item));

/*
 * Function: updateNode
 * ----------------------------
 *   Updates a node inside a certain linked list
 *
 *   data_to_update: pointer to data to update
 * 	 update: pointer to data to replace the previous node
 *	 compareItem: function that compares two items
 * 	 freeItem: function that frees a item
 *	 head: pointer to the head of the list
 *
 *   returns: Head of the list with the update
 *
 */
LinkedList *updateNode(LinkedList *head, Item data_to_update, Item update, int compareItem(Item, Item), void freeItem(Item));

/*
 * Function: searchNode
 * ----------------------------
 *   Searches a node inside a certain linked list
 *
 *   data_to_find: pointer to data to find
 *	 compareItem: function that compares two items
 *	 head: pointer to the head of the list
 *
 *   returns: The item if found
 * 			  NULL otherwise
 *
 */
Item *searchNode(LinkedList *head, Item data_to_find, int compareItem(Item, Item));

/*
 * Function: printList
 * ----------------------------
 *   Calls the function that prints a item to every item in the list
 *
 *   head: pointer to the head of the list
 *	 printItem: function that prints a item
 *	 tabs: number of tabs to be placed before the print of the item
 *
 */
void printList(LinkedList *head, void printItem(Item), int tabs);

/*
 * Function: clearList
 * ----------------------------
 *   Deletes an entire linked list
 *
 *   head: pointer to the head of the list
 *	 freeItem: function that frees a item
 *
 *	 Returns: pointer to NULL
 */
LinkedList *clearList(LinkedList *head, void freeItem(Item));

#endif