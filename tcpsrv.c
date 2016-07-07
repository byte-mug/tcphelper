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

static int isinv(char c){
	switch(c){
	case 0:
	case '.':
	case '-':
	return 1;
	}
	return 0;
}

static int findit(const char* chr){
	if(!chr) return 0;
	while(*chr) switch(*chr++){
	case '4':return 4;
	case '6':return 6;
	}
	return 0;
}
static char NAME[4090];

int main(int argc,const char* const * argv){
	union {
		struct sockaddr_in i4;
		struct sockaddr_in6 i6;
	} au;
	socklen_t al;
	int server;
	int fam;
	int worked;
	uid_t uid;
	if(argc<=4){printf("argc<=4 %d \n",argc);return 1;}
	sscanf(argv[3],"%d",&server);
	switch(findit(argv[1])){
		case 4:
			inet_pton(AF_INET,argv[2],&(au.i4.sin_addr.s_addr));
			au.i4.sin_family = AF_INET;
			fam = PF_INET;
			au.i4.sin_port = htons(server);	
			al = sizeof(struct sockaddr_in);
			break;
		case 6:
			if(!isinv(*argv[2]))
				inet_pton(AF_INET6,argv[2],&(au.i6.sin6_addr.s6_addr));
			else
				au.i6.sin6_addr = in6addr_any;
			au.i6.sin6_family = AF_INET6;
			fam = PF_INET6;
			au.i6.sin6_port = htons(server);
			al = sizeof(struct sockaddr_in6);
			break;
		default: printf("invalid protocol type\n"); return 1;
	}
	server = socket(fam, SOCK_STREAM, 0);
	if(server<0){ perror("socket"); return 1; }
	uid = getuid();

	/*
	 * We try to gain Root privileges. Useful for bind(). If `tcpsrv' is a
	 * setuid-executable, it shall be able to bind onto any TCP port.
	 */
	worked = setuid(0)>=0;

	if(bind(server,(struct sockaddr*)&au,al)<0){ perror("bind"); return 1; }

	/*
	 * Revert the setuid(0)-Step.
	 */
	if(worked){
		if(setuid(uid)<0){
			perror("setuid");
			printf("unable to drop root privileges\n");
			printf("to fix that: chmod -s %s\n",argv[0]);
			return 1;
		}
	}

	if(listen(server,10)<0){ perror("listen"); return 1; }
	snprintf(NAME,sizeof(NAME)-1,"%d",server);
	setenv("SOCKET",NAME,1);
	execvp(argv[4],(char*const*)argv+4);
	perror("execvp");
	return 1;
}


