# Compiler to use
CC = gcc


CFLAGS = -Wall -Wextra -g

TARGETS = udp_send udp_recv

all: $(TARGETS)

sender: sender.c
	$(CC) $(CFLAGS) -o udp_send udp_send.c

receiver: receiver.c
	$(CC) $(CFLAGS) -o udp_recv udp_recv.c

clean:
	rm -f $(TARGETS)