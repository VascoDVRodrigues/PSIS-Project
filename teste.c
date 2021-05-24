#include "linkedList-lib.h"

int main() {
	Node *list1, *list2;
	char str1[4] = "ola", str2[4] = "ola", str3[4] = "aaa";
	list1 = create_LinkedList();
	list2 = create_LinkedList();

	list1 = insertNode(str1, str2, list1);
	list1 = insertNode("sike", "sike", list1);
	list1 = insertNode("sike", "sike", list1);

	printList(list1);
	printf("-------------------\n");
	list1 = deleteNode(str1, list1);
	printList(list1);
}