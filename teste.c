#include "defs.h"
#include "linkedList-lib.h"
int main() {
	char* line = NULL;
	size_t size = 0;
	if (getline(&line, &size, stdin) == -1) {
		printf("no line");
	} else {
		printf("Line: %s\n", line);
		free(line);
	}

	/*
	time_t start = time(NULL);
	char str[100];
	strftime(str, 100, "%A, %x - %X", localtime(&start));
	printf("%s\n", str);

	sleep(10);
	start = time(NULL);

	strftime(str, 100, "%A, %x - %X", localtime(&start));
	printf("%s\n", str); */
}