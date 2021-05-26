#include "linkedList-lib.h"

int main() {
	MainNode *grupos = createMainList();

	SubNode *g1 = create_LinkedList();

	g1 = insertNode("key99", "value1", g1);
	g1 = insertNode("key2456", "value2", g1);
	g1 = insertNode("key111233", "value3", g1);

	grupos = insertGroup(g1, "grupo1", grupos);

	SubNode *g2 = create_LinkedList();

	g2 = insertNode("key1", "value1", g2);
	g2 = insertNode("key2", "value2", g2);
	g2 = insertNode("key3", "value3", g2);

	grupos = insertGroup(g1, "grupo2", grupos);

	printGroups(grupos);

	printf("\n---------------------\n");

	MainNode *procura = searchGroup("grupo1", grupos);

	printList(procura->GroupHead);
}