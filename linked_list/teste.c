#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linkedList-lib.h"

typedef struct _dados {
	int a, b, c, d;
} Dados;

void printfunction(Item* item) {
	if (item == NULL) {
		printf("NULL, nothing here to print... :(\n");
		return;
	}

	Dados meusDados = *(Dados*)item;
	printf("a=%d, b=%d", meusDados.a, meusDados.b);
}

int compareItens(Item* item1, Item* item2) {
	Dados meusDados1 = *(Dados*)item1;
	Dados meusDados2 = *(Dados*)item2;

	if (meusDados1.a == meusDados2.a) {
		if (meusDados1.b == meusDados2.b) {
			return 1;
		}
	}
	return 0;
}

void freeItem(Item* item) { return; }

int main() {
	Head* mylist1 = create_LinkedList();
	// Head* mylist2 = create_LinkedList();

	Dados exemplo;
	exemplo.a = 1;
	exemplo.b = 2;

	mylist1 = insertNode((Item*)&exemplo, mylist1);

	Dados exemplo1;
	exemplo1.a = 66;
	exemplo1.b = 734;

	mylist1 = insertNode((Item*)&exemplo1, mylist1);

	Dados exemplo2;
	exemplo2.a = 6;
	exemplo2.b = 6;

	mylist1 = insertNode((Item*)&exemplo2, mylist1);

	printList(mylist1, printfunction);

	// mylist1 = deleteNode(mylist1, (Item*)&exemplo1, compareItens, freeItem);
	// printList(mylist1, printfunction);
	// printfunction(searchNode(mylist1, (Item*)&exemplo3, compareItens));

	return 0;
}