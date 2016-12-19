#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>

#include "at/at_log.h"
#include "at/socket_ipc.h"

static int g_client_sockfd = -1;

int client_socket_conection()
{
	struct sockaddr_un client_addr;

	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sun_family = AF_UNIX;
	strcpy(client_addr.sun_path, SOCKNAME);

	g_client_sockfd = socket(PF_FILE, SOCK_STREAM, 0);
	if(g_client_sockfd < 0) {
		ATLOGE("%s> socket  open error : %s\n", __func__, strerror(errno));
		return -1;

	}
	if(connect( g_client_sockfd, (struct sockaddr*)&client_addr, sizeof( client_addr)) < 0)
	{
		ATLOGE("%s> socket  connection error : %s\n", __func__, strerror(errno));
		return -1;
	}

	return 0;
}

void retry_loop_until_ipc_session_ok()
{
	while(1) {
		if(client_socket_conection() == 0)
			break;
			
		ATLOGT("%s> client_socket_conection error\n", __func__);
		sleep(5);
	}
	ATLOGI("%s> client session connection success!!\n", __func__);
}

//int client_socket_ipc_listener(char *msg, int msg_len)
int client_socket_ipc_listener(SOC_IPC_DATA_T *msg, int msg_len)
{
	int ret;
	memset(msg, 0x00, sizeof(msg_len));
	ret = recvfrom(g_client_sockfd, msg, msg_len, 0 , NULL, 0);
	if(ret <= 0) {
		close(g_client_sockfd);
		g_client_sockfd = -1;

	}
	return ret;
}

