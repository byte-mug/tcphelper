CFLAGS+=-O3

all: tcploop tcpsrv

tcploop: tcploop.c
	$(CC) $(CFLAGS) tcploop.c -o tcploop

tcpsrv: tcpsrv.c
	$(CC) $(CFLAGS) tcpsrv.c -o tcpsrv


