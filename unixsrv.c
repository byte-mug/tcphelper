/*
 * Copyright (c) 2016 Simon Schmidt
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/un.h>

static char NAME[4090];

struct sockaddr* build_address(const char* name,socklen_t *al){
	static struct sockaddr_un stun;
	size_t siz = strlen(name)+1;
	size_t asiz = sizeof(stun)-sizeof(stun.sun_path)+siz;
	struct sockaddr_un *un;
	if(asiz>sizeof(stun)) un = malloc(asiz);
	else {
		*al = asiz;
		un = &stun;
	}
	un->sun_family = AF_UNIX;
	memcpy(un->sun_path,name,siz);
	return (struct sockaddr*)un;
}

int main(int argc,const char* const * argv){
	socklen_t al;
	int server;
	int fam;
	int worked;
	uid_t uid;
	if(argc<=2){printf("argc<=2 %d \n",argc);return 1;}
	struct sockaddr* addr = build_address(argv[1],&al);

	server = socket(AF_UNIX, SOCK_STREAM, 0);
	if(server<0){ perror("socket"); return 1; }
	unlink(argv[1]);
	if(bind(server,addr,al)<0){ perror("bind"); return 1; }
	if(listen(server,10)<0){ perror("listen"); return 1; }
	snprintf(NAME,sizeof(NAME)-1,"%d",server);
	setenv("SOCKET",NAME,1);
	execvp(argv[2],(char*const*)argv+2);
	perror("execvp");
	return 1;
}


