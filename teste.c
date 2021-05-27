#include "defs.h"
#include "linkedList-lib.h"
int main() {
	time_t start = time(NULL);
	char str[100];
	strftime(str, 100, "%A, %x - %X", localtime(&start));
	printf("%s\n", str);

	sleep(10);
	start = time(NULL);

	strftime(str, 100, "%A, %x - %X", localtime(&start));
	printf("%s\n", str);
}