# tcphelper
TCP/IP server Helper for Linux

## Build 
```
make
```

## Run

```
#IPv4
tcpsrv 4 $IP $PORT tcploop cmd args...

#IPv4 IN_ANY
tcpsrv 4 0.0.0.0 $PORT tcploop cmd args...

#IPv6
tcpsrv 6 $IP $PORT tcploop cmd args...

#IPv6 IN_ANY
tcpsrv 6 . $PORT tcploop cmd args...
```

## How it works:

### 'tcpsrv'

```
tcpsrv $FAMILY $IP $PORT cmd args...
```

`tcpsrv` creates a socket, binds it, calls `listen` and execs the next command. The File-descriptor number of the socket
is stored in the Environment variable `SOCKET`

* $FAMILY : A string containing a '4' (for IPv4) or '6' (for IPv6)
* $IP : An IP address. If the address family is IPv6, and $IP is '.', an IN_ANY equivalent is being used.
* $PORT : The port number.

### 'unixsrv'

```
unixsrv $SOCKET_FILE cmd args...
```

`unixsrv` creates a socket, binds it, calls `listen` and execs the next command. The File-descriptor number of the socket
is stored in the Environment variable `SOCKET`


