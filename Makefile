CFLAGS = -Wall -Werror -g

all: server client

Rule.o: Rule.c
	$(CC) $(CFLAGS) -c Rule.c

QueryList.o: QueryList.c
	$(CC) $(CFLAGS) -c QueryList.c

RuleList.o: RuleList.c
	$(CC) $(CFLAGS) -c RuleList.c

RuleHandler.o: RuleHandler.c
	$(CC) $(CFLAGS) -c RuleHandler.c RuleList.c

server.o: server.c
	$(CC) $(CFLAGS) -c server.c

server: server.o Rule.o QueryList.o RuleList.o RuleHandler.o
	$(CC) $(CFLAGS) -o server server.o Rule.o QueryList.o RuleList.o RuleHandler.o -lpthread


client: client.o
	$(CC) $(CFLAGS) -o client client.o

client.o: client.c
	$(CC) $(CFLAGS) -c client.c

clean:
	rm -f *.o
