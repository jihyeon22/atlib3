#pragma once

#define SOCKNAME					"/tmp/smd.panout"
typedef enum {
	e_NOTIFICATION_MSG,
	e_SMS_MSG,
	e_UNKNOWN_MSG,
} IPC_MSG_TYPE;


typedef struct soc_ipc_data
{
	IPC_MSG_TYPE msg_type;
	char recv_time[20];
	char src_phone_num[15];
	char data[256];
}SOC_IPC_DATA_T;

//////////////////////////////////////////////////////
//socket ipc server function
//////////////////////////////////////////////////////
#define MAX_CLIENT_SOCKET_COUNT		20

int init_server_socket_ipc();
int server_socket_listener_loop();
void send_smd_data_broadcast(SOC_IPC_DATA_T msg);

//////////////////////////////////////////////////////
//socket ipc client function
//////////////////////////////////////////////////////
void retry_loop_until_ipc_session_ok();
int client_socket_ipc_listener(SOC_IPC_DATA_T *msg, int msg_len);

