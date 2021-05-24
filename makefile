CC=gcc
CFLAGS= -lpthread

all: KVS-localServer KVS-authServer apps

# $^ significa todas as dependencias
# $@ significa o nome da task, aqui representa KVS-localServer
KVS-localServer: KVS-localServer.c linkedList-lib.c
	$(CC) $^ -o $@ $(CFLAGS) 

KVS-authServer: KVS-authServer.c
	$(CC) $^ -o $@ $(CFLAGS)

#METER AQUI TODAS AS APPS A COMPILAR
apps: $(addprefix app, 1 2)

app%: app%.c KVS-lib.c
	$(CC) $^ -o $@ $(CFLAGS) 

teste: teste.c
	$(CC) $^ -o $@ $(CFLAGS) 

clean:
	rm KVS-localServer app1 app2 teste -f

