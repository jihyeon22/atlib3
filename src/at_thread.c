#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>

#include <signal.h>
#include <errno.h>

#include "at/at_util.h"
#include "at/at_log.h"
#include "at/socket_ipc.h"
#include "at/watchdog.h"

static int at_fd = AT_INVAILD_FD;
static int at_thread_run_at_loop_flag = 1;
static int at_thread_run_socket_listener_flag = 1;

pthread_t at_thread_var_at_loop;
pthread_t at_thread_var_ipc_soc_mng;
pthread_mutex_t mutex_at = PTHREAD_MUTEX_INITIALIZER;
/*
void _at_noti_proc(char* buffer, int len)
{
	// first filter for sms
	at_noti_proc(buffer,len);
}
*/

void check_sms_sendto_client()
{
	SOC_IPC_DATA_T msg;
	SMS_MSG_STAT_T sms_mgs_stat;
	int i;

	memset(&sms_mgs_stat, 0x00, sizeof(SMS_MSG_STAT_T));
	at_get_sms_unread(&sms_mgs_stat);
	
	if(sms_mgs_stat.total_unread_cnt <= 0)
		return;

	for(i = 0; i < sms_mgs_stat.total_unread_cnt; i++) {
		memset(&msg, 0x00, sizeof(SOC_IPC_DATA_T));
		msg.msg_type = e_SMS_MSG;
		strncpy(msg.src_phone_num, sms_mgs_stat.msg[i].msg_from, sizeof(msg.src_phone_num)-1);
		strncpy(msg.recv_time, sms_mgs_stat.msg[i].time_stamp, sizeof(msg.recv_time)-1);
		strncpy(msg.data, sms_mgs_stat.msg[i].msg_buff, sizeof(msg.data)-1);
		send_smd_data_broadcast(msg);
	}


}

void check_notification_sendto_client(char *buffer)
{
	SOC_IPC_DATA_T msg;
	
	if(buffer == NULL)
		return;
	if(isascii(buffer[0]) == 0)
		return;
	if(buffer[0] == '\n' && buffer[1] == NULL)
		return;

	
	memset(&msg, 0x00, sizeof(SOC_IPC_DATA_T));
	msg.msg_type = e_NOTIFICATION_MSG;
	strncpy(msg.data, buffer, sizeof(msg.data)-1);
	send_smd_data_broadcast(msg);
}

void _at_loop(void* arg)
{
	arg = NULL; //for remove build warning
/* //for aging test
	SOC_IPC_DATA_T msg;
	int sms_cnt = 0;
	int noti_cnt = 0;
	int msg_toggle = 0;
	while(1) {
		if(msg_toggle == 0) {
			msg_toggle = 1;
			msg.msg_type = e_NOTIFICATION_MSG;
			sprintf(msg.data, "NOTI DATA-%04d", noti_cnt++);
		}
		else {
			msg_toggle = 0;
			msg.msg_type = e_SMS_MSG;
			strcpy(msg.src_phone_num, "01012345678");
			sprintf(msg.recv_time, "%s", "20161117104400");
			sprintf(msg.data, "SMS DATA-%04d", sms_cnt++);
		}
		send_smd_data_broadcast(msg);
		sleep(3);
	}
*/

//*
	int result;
	char buffer[AT_MAX_BUFF_SIZE];
	fd_set reads, temps;
	struct  timeval tv;
	int read_cnt;
	

	if(at_fd < 0)
	{
		ATLOGE( "<atd> %s : %d, AT Channel Open Error errno[%d]\r\n", __func__, __LINE__, errno);
		return;
	}

	FD_ZERO(&reads);
	FD_SET(at_fd, &reads);
	
	ATLOGT("PID %s : %d\n", __FUNCTION__, getpid());

	while(at_thread_run_at_loop_flag) 
	{
		//watchdog_process();

		tv.tv_sec = 120;
		tv.tv_usec = 0;	
		temps = reads;
		result = select(at_fd + 1, &temps, NULL, NULL, &tv);

		if(result < 0)
		{
			if(errno != EINTR)
			{
				ATLOGE( "at select Error: %s\n", __FUNCTION__);
				return;
			}
			continue;
		}

		if(result > 0)
		{
			if(!FD_ISSET(at_fd, &temps))
			{
				continue;
			}
			
			memset(buffer, 0x00, sizeof(buffer));
			read_cnt = read(at_fd, buffer, AT_MAX_BUFF_SIZE-1);
			if( read_cnt <= 0) {
				continue;
			}
			
			check_sms_sendto_client();
			check_notification_sendto_client(buffer);
		}
	}

	if ( at_fd != AT_INVAILD_FD )
		close(at_fd);
//*/	
	return;
}

void _socket_listener_loop(void* arg)
{
	arg = NULL; //for remove build warning
	while(at_thread_run_socket_listener_flag) {
		if(init_server_socket_ipc() < 0) {
			ATLOGT("<atd> %s (%d): init_server_socket_ipc fail\n", __FUNCTION__, __LINE__);
			sleep(5);
			continue;
		}

		server_socket_listener_loop(); //block function
	}
}


int _at_start()
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 300000);
	int open_retry_cnt = AT_MAX_OPEN_RETRY_CNT;

	ATLOGD("%s> start\n", __func__);

	if(at_fd > 0) {
		close(at_fd);
		at_fd = AT_INVAILD_FD;
	}
	create_watchdog("_at_start", 10);
	while (open_retry_cnt--)
	{
		at_fd = open(SMD_LISTENER_DEV, O_RDWR | O_NOCTTY | O_NONBLOCK);
		if (at_fd > 0 )
		{
			ATLOGD("%s> [%s] open success [%d]/[%d]\r\n",__func__, SMD_LISTENER_DEV, (AT_MAX_OPEN_RETRY_CNT-open_retry_cnt), AT_MAX_OPEN_RETRY_CNT);
			break;
		}
		watchdog_keepalive_id("_at_start");
		ATLOGE("%s> [%s] open fail and retry.. [%d]/[%d]\r\n",__func__, SMD_LISTENER_DEV, (AT_MAX_OPEN_RETRY_CNT-open_retry_cnt), AT_MAX_OPEN_RETRY_CNT);
		sleep(1);
	}
	watchdog_delete_id("_at_start");

	
	if(pthread_create(&at_thread_var_at_loop, &attr, (void *)_at_loop, NULL) != 0) {
		return AT_RET_FAIL;
	}

	if(pthread_create(&at_thread_var_ipc_soc_mng, &attr, (void *)_socket_listener_loop, NULL) != 0) {
		pthread_cancel(at_thread_var_at_loop);
		return AT_RET_FAIL;
	}
	
	return AT_RET_SUCCESS;
	
}

void _at_stop()
{
	at_thread_run_at_loop_flag = 0;
	at_thread_run_socket_listener_flag = 0;
}
