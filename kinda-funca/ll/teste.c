#include <stdio.h>
#include <stdlib.h>

#include "linkedList-lib.h"

typedef struct teste {
	int a, b;
} Dados;

void printitem(Item i) {
	Dados d = *(Dados*)i;

	printf("%d %d\n", d.a, d.b);
}

void freeitem(Item i) {
	free(i);
	return;
}

int main() {
	Dados* d1 = malloc(sizeof(Dados));
	d1->a = 6;
	d1->b = 6;
	LinkedList* lista = createList();
	lista = insertNode(lista, (Item)d1);

	printList(lista, printitem, 0);

	clearList(lista, freeitem);

	// free(d1);
	return 0;
}