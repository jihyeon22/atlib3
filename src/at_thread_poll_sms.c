#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>

#include <signal.h>
#include <errno.h>

#include "at_cmd_table_wrapper.h"

#include "at/at_util.h"
#include "at/at_log.h"
#include "at/socket_ipc.h"


#define SMS_CHK_INTERVAL	60
#define SMS_NEXT_SMS_READ_INTERVAL	10

static int at_poll_sms_thread_run = 1;

pthread_t at_poll_sms_thread_var;
//pthread_mutex_t mutex_at_poll_sms = PTHREAD_MUTEX_INITIALIZER;

void _at_poll_sms_noti_loop(void* arg)
{
	SOC_IPC_DATA_T msg;

	arg = NULL; //for remove build warning
	retry_loop_until_ipc_session_ok();

	while(1) {
		memset(&msg, 0x00, sizeof(SOC_IPC_DATA_T));
		if(client_socket_ipc_listener(&msg, sizeof(SOC_IPC_DATA_T)) <= 0) { //block function
			retry_loop_until_ipc_session_ok();
		}

		switch(msg.msg_type) {
			case e_NOTIFICATION_MSG:
				if(g_model_proc_noti != NULL) {
					g_model_proc_noti(msg.data, strlen(msg.data));
				}
				break;
			case e_SMS_MSG:
				if(g_model_proc_sms_read != NULL) {
					g_model_proc_sms_read(msg.src_phone_num, msg.recv_time, msg.data);
				}
				break;
			default:
				ATLOGE("atd %s> unknown msg type error \r\n", __func__);
		}

	}

/*
	int chk_cnt = 0;
	int i = 0;
	arg = NULL;
	while(at_poll_sms_thread_run) 
	{
		sleep(1);
		chk_cnt ++;
		
		if (chk_cnt >= SMS_CHK_INTERVAL)
		{
			int is_sms = 0;	
			SMS_MSG_STAT_T sms_mgs_stat;
			memset(&sms_mgs_stat, 0x00, sizeof(SMS_MSG_STAT_T));
			at_get_sms_unread(&sms_mgs_stat);
			ATLOGT("poll sms start. sms unread cnt => [%d]\r\n", sms_mgs_stat.total_unread_cnt);
			for ( i = 0 ; i < sms_mgs_stat.total_unread_cnt ; i++)
			{
				is_sms = 1;
				if ( g_model_proc_sms_read != NULL)
				{
					g_model_proc_sms_read(sms_mgs_stat.msg[i].msg_from, sms_mgs_stat.msg[i].time_stamp, sms_mgs_stat.msg[i].msg_buff);
					//ATLOGT("sms_mgs_stat.total_unread_cnt [%d]\r\n",sms_mgs_stat.total_unread_cnt);
					//ATLOGT("sms_mgs_stat.msg[%d].time_stamp => [%s]\r\n",i, sms_mgs_stat.msg[i].time_stamp);
					//ATLOGT("sms_mgs_stat.msg[%d].msg_from => [%s]\r\n",i, sms_mgs_stat.msg[i].msg_from);
					//ATLOGT("sms_mgs_stat.msg[%d].msg_buff => [%s]\r\n",i, sms_mgs_stat.msg[i].msg_buff);
					
				}
			}
			if ( is_sms )
				chk_cnt = SMS_CHK_INTERVAL - SMS_NEXT_SMS_READ_INTERVAL;
			else
				chk_cnt = 0;
		}

	}
*/	
	return;
}

int _at_poll_sms_start()
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 300000);
	
	if(pthread_create(&at_poll_sms_thread_var, &attr, (void *)_at_poll_sms_noti_loop, NULL) != 0)
		return AT_RET_FAIL;
	
	return AT_RET_SUCCESS;
	
}

void _at_poll_sms_stop()
{
	at_poll_sms_thread_run = 0;
}
