/*semaphore.c*/
#include <fcntl.h> /* For O_* constants */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/stat.h> /* For mode constants */
#include <unistd.h>

pthread_cond_t data_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mymutex;

int n = 0;

void func() {
	pthread_mutex_lock(&mymutex);
	printf("Sending signal in 5secs\n");
	sleep(5);
	pthread_cond_broadcast(&data_cond);
	pthread_mutex_unlock(&mymutex);
}

int main(int argc, char* argv[]) {
	char* name = "my_semaphore";
	int VALUE = 1;
	sem_t* sema;

	pthread_t t1;

	pthread_mutex_init(&mymutex, NULL);

	// sem_close(sema);
	sem_unlink(name);

	// If semaphore with name does not exist, then create it with VALUE
	printf("Open or Create a named semaphore, %s, its value is %d\n", name, VALUE);
	sema = sem_open(name, O_CREAT, 0666, VALUE);

	pthread_create(&t1, NULL, (void*)func, NULL);

	pthread_mutex_lock(&mymutex);
	// wait on semaphore sema and decrease it by 1
	sem_wait(sema);
	printf("ENTERED SEMAPHORE\n");
	printf("I will unlock when i receive the broadcast\n");
	pthread_cond_wait(&data_cond, &mymutex);
	// waiting for the cond variable to broadcast

	printf("I will leave crit region\n");
	// add semaphore sema by 1
	sem_post(sema);
	pthread_mutex_unlock(&mymutex);

	// Before exit, you need to close semaphore and unlink it, when all  processes have
	// finished using the semaphore, it can be removed from the system using sem_unlink
	sem_close(sema);
	sem_unlink(name);
	return 0;
}