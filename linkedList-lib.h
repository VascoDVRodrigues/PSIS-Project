#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

/* COMO USAR
1o cria-se uma lista de grupos com createMainList()

2o Cria-se uma lista de key|values correspondente a um grupo

3o Adiciona-se a lista de Key|values a lista de grupos com insertGroup()
ou então passa-se NULL a insertGroup(), fica assim um grupo vazio

Para atualizar/apagar/adicionar um key|value primeiro de um certo grupo primeiro temos q se procurar esse grupo,
com o objeto retornado já se pode fazer a operação desejada, por exemplo:
	Apagar a key "CHAVE1" do grupo "GRUPOXX":
		grupoXX = searchGroup("GRUPOXX", grupoXX);
		grupoXX->GroupHead = deleteNode("CHAVE1", grupoXX->GroupHead);
*/

typedef struct _node {
	char *key;
	char *value;
	struct _node *next;
} SubNode;

typedef struct _main_node {
	//Talvez por aqui malloc?
	//Visto que quando este package e construido já se sabe qual o nome do grupo?
	char groupID[1024];
	struct _node *GroupHead;
	struct _main_node *next;
} MainNode;

MainNode *createMainList();

MainNode *insertGroup(SubNode *data, char groupID[], MainNode *head);

MainNode *searchGroup(char groupID[], MainNode *head);

MainNode *deleteGroup(char groupID[], MainNode *head);

void printGroups(MainNode *head);

///////////////////////////////////////////////////////

SubNode *create_LinkedList();

SubNode *insertNode(char *key, char *value, SubNode *head);

SubNode *searchNode(char *key, SubNode *head);

SubNode *deleteNode(char *key, SubNode *head);

int isEmpty(SubNode *node);

void printList(SubNode *head);

void printNode(SubNode *node);

void updateValue(SubNode *node, char *newValue);