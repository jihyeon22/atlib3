
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

static int g_server_sockfd = 0;
typedef struct {
	int g_client_socket_fd;
}client_sock_mng_t;
static client_sock_mng_t g_csock_mng[MAX_CLIENT_SOCKET_COUNT];

void set_client_socket_id(int client_sock_fd)
{
	int i;
	for(i = 0; i < MAX_CLIENT_SOCKET_COUNT; i++) {
		if(g_csock_mng[i].g_client_socket_fd <= 0) {
			g_csock_mng[i].g_client_socket_fd = client_sock_fd;
			break;
		}
	}
}

ssize_t
send_timedwait(int sockfd, const void *buf, size_t len, int flags, int sec)
{
	int result;
	struct timeval tval;
	tval.tv_sec = sec;
	tval.tv_usec = 0;

	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tval, sizeof(struct timeval)) < 0) {
		ATLOGE("%s> setsockopt error : %s\n", __func__, strerror(errno));
		return -1;
	}

	result = send(sockfd, buf, len, flags);

	return result;
}

void send_smd_data_broadcast(SOC_IPC_DATA_T msg)
{
	int i;
	for(i = 0; i < MAX_CLIENT_SOCKET_COUNT; i++) {
		if(g_csock_mng[i].g_client_socket_fd > 0) {
			if(send_timedwait(g_csock_mng[i].g_client_socket_fd, &msg, sizeof(SOC_IPC_DATA_T), 0, 3) < 0) {
				ATLOGE("%s> client sock fd[%d] distory\n", __func__, g_csock_mng[i].g_client_socket_fd);
				close(g_csock_mng[i].g_client_socket_fd);
				g_csock_mng[i].g_client_socket_fd = 0;
			}
		}
	}
}



int init_server_socket_ipc()
{
	int option;
	struct sockaddr_un server_addr;
	signal(SIGPIPE, SIG_IGN);
	memset(g_csock_mng, 0x00, sizeof(client_sock_mng_t)*MAX_CLIENT_SOCKET_COUNT);

	if ( 0 == access(SOCKNAME, F_OK))
		unlink(SOCKNAME);

	g_server_sockfd = socket(PF_FILE, SOCK_STREAM, 0);
	if(g_server_sockfd == -1) {
		//perror("socket() err");
		ATLOGE("%s> socket open error : %s\n", __func__, strerror(errno));
		return -1;
	}

	setsockopt(g_server_sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCKNAME);

	if(bind(g_server_sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
		ATLOGE("%s> socket bind error : %s\n", __func__, strerror(errno));
		return -1;
	}

	return 0;
}

int server_socket_listener_loop()
{
	struct sockaddr_un client_addr;
	int client_socket;
	int client_addr_size;
	while(1) {
		ATLOGT("%s> waiting client socket listen\n", __func__);
		if(listen(g_server_sockfd, 5) < 0)
		{
			close(g_server_sockfd);
			g_server_sockfd = -1;
			ATLOGE("%s> socket listen error : %s\n", __func__, strerror(errno));
			return 0;
		}
		client_addr_size  = sizeof( client_addr);
		client_socket     = accept( g_server_sockfd, (struct sockaddr*)&client_addr, (socklen_t *)&client_addr_size);
		if(client_socket < 0) {
			ATLOGE("%s> client socket accept error : %s\n", __func__, strerror(errno));
			continue;
		}

		set_client_socket_id(client_socket);
	}
}
