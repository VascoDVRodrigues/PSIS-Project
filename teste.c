#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkedList-lib.h"

int main() {
	Node *head = create_LinkedList();

	head = insertNode("ola", "aaaaa", head);

	head = insertNode("23", "awegr", head);

	head = insertNode("123525t", "fdafaa", head);
	printList(head);
	printf("-------------\n");
	deleteNode("23", head);
	printList(head);
	printNode(searchNode("ola", head));

	return 0;
}