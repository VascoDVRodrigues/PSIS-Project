#include "linkedList-lib.h"

typedef struct _node {
	Item *item;
	struct _node *next;
} Node;

typedef struct _head {
	Node *Real_head;
} Head;

Head *create_LinkedList() {
	Head *new_list = (Head *)malloc(sizeof(Head));
	new_list->Real_head = (Node *)malloc(sizeof(Node));
	new_list->Real_head->item = NULL;
	new_list->Real_head->next = NULL;
	return new_list;
}

// receives the head and a function to print de elements
void printList(Head *head, void printfunc(Item *)) {
	Node *current = head->Real_head;
	if (current->next == NULL) {
		printf("Empty list!!:(\n");
	}
	printf("HEAD->");
	while (current->next != NULL) {
		printfunc(current->item);
		printf("-->\n-->");
		current = current->next;
	}
	printf("NULL\n");
}

Head *insertNode(Item *item, Head *head) {
	Node *node = (Node *)malloc(sizeof(Node));
	node->item = item;
	node->next = head->Real_head;
	head->Real_head = node;
	return head;
}

// recebe a cabeca da lista, o item a encontrar e uma funcao que compara 2 itens, se 2 itens forem
// iguais, essa funcao deve retornar 1, se forem diferentes deve retornar 0.
// A função searchNode retorna o item se existir, se não existir retorna NULL
Item *searchNode(Head *head, Item *item, int compare(Item *, Item *)) {
	/*if (isEmpty(head)) {
		return head;
	}*/
	Node *current = head->Real_head;
	while (current->next != NULL) {
		if (compare(current->item, item)) {	 // Encontramos o nó
			return current->item;
		}
		current = current->next;
	}
	return NULL;
}

// Recebe a cabeca e o item a apagar
// Recebe também a funcao para comparar itens, que deve funcionar como na func searchNode
// Recebe ainda uma funcao para apagar o item
// Retorna a cabeca
Head *deleteNode(Head *head, Item *item, int compare(Item *, Item *), void freeItem(Item *item)) {
	Node *current_node = head->Real_head;
	Node *aux_node = NULL;

	if (compare(item, current_node->item)==1) {  // Se o nó a apagar é a cabeça
		head->Real_head = head->Real_head->next;
		freeItem(current_node->item);
		free(current_node);
		return head;
	} else {
		
		while (current_node->next->next != NULL) {
			if (compare(item, current_node->next->item)) {	// no a apagar encontrado
				// o no a apagar é o no aseguir a current
				aux_node = current_node->next;
				current_node->next = current_node->next->next;
				freeItem(aux_node->item);
				free(aux_node);
			}
			// se ainda n foi encontrado continuar para o prox
			current_node = current_node->next;
		}
	}
	return head;
}

/*
int isEmpty(Node *node) {
	if (node == NULL) {
		return 1;
	}
	return 0;
}

void printNode(Node *node) {
	printf("Node-> Key: %s\tValue: %s\n", node->key, node->value);
	return;
}

void updateValue(Node *node, char *newValue) {
	strcpy(node->value, newValue);
	return;
}
*/