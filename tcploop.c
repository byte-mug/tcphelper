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

void sig_exit()
{
	int status;
	while(wait(&status)<=0);
}

void fail(const char* str){
	if(!errno)return;
	perror(str);
	_exit(1);
}

char NAME[4090];

int main(int argc,const char* const * argv){
	union {
		struct sockaddr_in i4;
		struct sockaddr_in6 i6;
		struct sockaddr_un un;
	} au;
	int sock;
	int client;
	pid_t pid;
	socklen_t al;
	const char* chr = getenv("SOCKET");
	unsetenv("SOCKET");
	signal (SIGCHLD, sig_exit);
	if(!chr)return 1;
	sscanf(chr,"%d",&sock);
	for(;;){
		al = sizeof(au);
		client = accept(sock, (struct sockaddr*)&au, &al);
		if(client<0) { usleep(100); continue; }
		pid = fork();
		if(pid==0){
			dup2(1,2);		fail("dup2 1,2");
			dup2(client,0); fail("dup2 client,0");
			dup2(client,1); fail("dup2 client,1");
			close(client);  fail("close");
			switch(au.i4.sin_family){
			case AF_INET:
				inet_ntop(AF_INET, &(au.i4.sin_addr), NAME, sizeof(NAME)-1);
				break;
			case AF_INET6:
				inet_ntop(AF_INET6, &(au.i6.sin6_addr), NAME, sizeof(NAME)-1);
				break;
			default:
				goto skip;
			}
			setenv("REMOTE_IP",NAME,1);
			sprintf(NAME,"%d",(int)ntohs(au.i4.sin_port) );
			setenv("REMOTE_PORT",NAME,1);
			skip:
			execvp(argv[1],(char*const*)argv+1);
			perror("execvp");
			return 1;
		}
		if(pid<0)perror("fork");
		close(client);
	}
}


