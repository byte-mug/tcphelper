CFLAGS+=-O3

all: tcploop tcpsrv unixsrv

tcploop: tcploop.c
	$(CC) $(CFLAGS) tcploop.c -o tcploop

tcpsrv: tcpsrv.c
	$(CC) $(CFLAGS) tcpsrv.c -o tcpsrv

unixsrv: unixsrv.c
	$(CC) $(CFLAGS) unixsrv.c -o unixsrv


