/*semaphore.c*/
#include <fcntl.h> /* For O_* constants */
#include <semaphore.h>
#include <stdio.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>

int main(int argc, char* argv[]) {
	char* name = "my_semaphore";
	int VALUE = 1;
	sem_t* sema;
	
	// If semaphore with name does not exist, then create it with VALUE
	printf("Open or Create a named semaphore, %s, its value is %d\n", name, VALUE);
	sema = sem_open(name, O_CREAT, 0666, VALUE);
	
	// wait on semaphore sema and decrease it by 1
	sem_wait(sema);
	printf("ENTERED SEMAPHORE\n");
	sleep(4);
	printf("I will leave crit region\n");
	// add semaphore sema by 1
	sem_post(sema);


	// Before exit, you need to close semaphore and unlink it, when all  processes have
	// finished using the semaphore, it can be removed from the system using sem_unlink
	sem_close(sema);
	sem_unlink(name);
	return 0;
}