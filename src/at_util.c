#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <pthread.h>
#include <time.h>

#include "at/at_log.h"
#include "at_cmd_table_wrapper.h"
#include "at/watchdog.h"
#include "at_thread_poll_sms.h"
#include "at_misc.h"
#include "at_thread.h"

#define AT_CMD_ECHO
//#define AT_DBG_VERBOS

char g_dev_path[256] = {0,};
FILE *g_debug_file = NULL;


void at_listener_open()
{
	g_debug_file = stderr;

	init_watchdog();

	create_watchdog("at_listener_open", 60);
	while(1) {
		if(_at_start() == AT_RET_SUCCESS)
			break;
		ATLOGE("%s> _at_start fail\n", __func__);
		sleep(5);
	}
	watchdog_delete_id("at_listener_open");
}

int at_open(AT_DEV_TYPE dev, void (*p_noti_proc)(const char* buffer, int len),  void (*p_sms_proc)(const char* phone_num, const char* recv_time, const char* msg), char *debug_file )
{
	int ret = AT_RET_SUCCESS;

	if(debug_file != NULL) {
		if(!strcmp(debug_file, "console")) {
			g_debug_file = stderr;
		}
		else {
			g_debug_file = fopen(debug_file, "w");
		}
	} else {
		g_debug_file = NULL;
	}

	ATLOGD("%s ++\n", __func__);

	if(init_watchdog() < 0) {
		ATLOGE("%s> watchdog create error\n", __func__);
	}

	//if ( thread_type == e_NO_NOTI_POLL_SMS ) 
	if(p_noti_proc != NULL || p_sms_proc != NULL)
	{
		create_watchdog("_at_poll_sms_start", 60);
		while(1) {
			if(_at_poll_sms_start() == AT_RET_SUCCESS)
				break;
			ATLOGE("%s> _at_start fail\n", __func__);
			sleep(5);
		}
		watchdog_delete_id("_at_poll_sms_start");
	}
	
	//strcpy(g_dev_path,dev_path);
	strcpy(g_dev_path, SMD_CMD_DEV);
	
	at_func_init(dev, p_noti_proc, p_sms_proc);

	ATLOGD("%s --\n", __func__);
		
	return ret;
}

int at_close()
{
	_at_stop();
	return AT_RET_SUCCESS;

}

// ---------------------------------------------------------
// at cmd util
// ---------------------------------------------------------
static int _wait_read(int fd, unsigned char *buf, int buf_len, int ftime)
{
	fd_set reads;
	struct timeval tout;
	int result = 0;
	
	int ret_byte = 0;

	FD_ZERO(&reads);
	FD_SET(fd, &reads);

	while (1) {
		tout.tv_sec = ftime;
		tout.tv_usec = 0;
		result = select(fd + 1, &reads, 0, 0, &tout);
		if(result <= 0) //time out & select error
			return -1;
		
		if ( (ret_byte = read(fd, buf, buf_len)) <= 0)
			return -1;

		break; //success
	}

	return ret_byte;
}




int send_at_cmd(const char* cmd)
{
	int ret;
	ATLOGD("%s> cmd : %s ++\r\n", __func__, cmd);
	
	ret = send_at_cmd_singleline_resp(cmd, NULL, NULL, 3);;

	ATLOGD("%s> cmd : %s --\r\n", __func__, cmd);
	return ret;
}


void at_read_flush()
{
	int fd = 0;
	char read_buf[AT_MAX_BUFF_SIZE] = {0,};

	create_watchdog("at_read_flush", 10);
	fd = open(g_dev_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
	
	if ( fd <= 0 )
	{
		ATLOGT("%s - return case 1\r\n", __func__);
		watchdog_delete_id("at_read_flush");
		return;
	}
	
	ATLOGD("<atd> dbg : > read flush\r\n");
	
	// send cmd
	_wait_read(fd, (unsigned char *)read_buf, AT_MAX_BUFF_SIZE, 1);
//	read(fd, read_buf, AT_MAX_BUFF_SIZE);
	
	close(fd);
	ATLOGT("%s - return case 2\r\n", __func__);
	watchdog_delete_id("at_read_flush");

}

//#define VERBOS_AT_CMD_DBG_MSG
int send_at_cmd_singleline_resp(const char* cmd, const char* resp, char* ret_cmd, const int retry_cnt)
{
	int ret = AT_RET_FAIL;
	int fd = 0;
	
	int write_cnt1 = 0;
	int write_cnt2 = 0;
	
	char write_buf[AT_MAX_BUFF_SIZE] = {0,};
	char buffer[AT_MAX_BUFF_SIZE] = {0,};
	//char buffer2[AT_MAX_BUFF_SIZE] = {0,};
	
	int write_cmd = 1;
	int retry = 0;
	
	int read_cnt = 0;
	int read_total = 0;
	
	char *p_chk_cmd = NULL;

	int open_retry_cnt = AT_MAX_OPEN_RETRY_CNT;
	
	ATLOGD("%s> g_dev_path<%s> ++\r\n", __func__, g_dev_path);
	create_watchdog("send_at_cmd_singleline_resp", 10);

	int sleep_time_ms = 1;

	while (open_retry_cnt--)
	{
		int wait_sec = 0;
		fd = open(g_dev_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
		if (fd > 0 )
		{
			ATLOGD("%s> [%s] open success [%d]/[%d]\r\n",__func__, g_dev_path, (AT_MAX_OPEN_RETRY_CNT-open_retry_cnt), AT_MAX_OPEN_RETRY_CNT);
			break;
		}
		watchdog_keepalive_id("send_at_cmd_singleline_resp");
		ATLOGE("%s> [%s] open fail and retry.. [%d]/[%d]\r\n",__func__, g_dev_path, (AT_MAX_OPEN_RETRY_CNT-open_retry_cnt), AT_MAX_OPEN_RETRY_CNT);
		wait_sec = sleep_time_ms++ * 10000;
		usleep(wait_sec);
	}

	if ( fd <= 0 ) {
		watchdog_delete_id("send_at_cmd_singleline_resp");
		ATLOGE("%s> g_dev_path<%s> open error\r\n", __func__, g_dev_path);
		return AT_RET_FAIL;
	}

	// Ŀ��??? ����??
	ATLOGD("%s> send cmd :: [%s] \r\n", __func__, cmd);
	sprintf(write_buf, "%s\r", cmd);

#ifdef VERBOS_AT_CMD_DBG_MSG
	ATLOGD("%s> at_strlen_with_cr call()\r\n", __func__);
#endif
	write_cnt1 = at_strlen_with_cr(write_buf);
#ifdef VERBOS_AT_CMD_DBG_MSG
	ATLOGD("%s> write_cnt1 = [%d]\r\n", __func__, write_cnt1);
#endif
	
	write_cmd = 1;
	
	// Ŀ��??? ������???.
	while(retry++ < retry_cnt) 
	{
		watchdog_keepalive_id("send_at_cmd_singleline_resp");
#ifdef VERBOS_AT_CMD_DBG_MSG
ATLOGD("%s> retry[%d/%d]\r\n", __func__, retry, retry_cnt);
		// 1. Ŀ��??? ����??
ATLOGD("%s> write_cmd[%d]\r\n", __func__, write_cmd);
#endif
		if ( write_cmd )
		{
			ATLOGD("<atd> dbg : > [%s] (%d)\r\n", write_buf, write_cnt1);

			write_cnt2 = write(fd, write_buf, write_cnt1);
			memset(buffer, 0, sizeof(buffer));
		}
#ifdef VERBOS_AT_CMD_DBG_MSG
ATLOGD("%s> write_cnt2[%d]\r\n", __func__, write_cnt2);		
#endif
		if ( write_cnt2 != write_cnt1 )
		{
			ATLOGD("<atd> dbg : write fail.. retry (%d),(%d)\r\n",write_cnt2,write_cnt1);
			continue;
		}
		
		write_cmd = 0;
		
		// 2. Ŀ��??����?k? �������ٸ�
#ifdef VERBOS_AT_CMD_DBG_MSG
ATLOGD("%s> _wait_read : read_total[%d]\r\n", __func__, read_total);				
#endif
		if( (read_cnt = _wait_read(fd, (unsigned char *)buffer+read_total, AT_MAX_BUFF_SIZE, AT_MAX_WAIT_READ_SEC)) < 0)
		{
			ATLOGD("<atd> dbg : read timeout fail.. retry\r\n");
			continue;
		}
#ifdef VERBOS_AT_CMD_DBG_MSG
ATLOGD("%s> _wait_read : read_cnt[%d]\r\n", __func__, read_cnt);				
#endif
		read_total += read_cnt;
#ifdef VERBOS_AT_CMD_DBG_MSG
		ATLOGD("<atd> dbg : < [%s]\r\n",buffer);
#endif
		// ���� ERROR ���� ???�l������break
		p_chk_cmd = strstr(buffer, "\r\nERROR");
		if (p_chk_cmd != NULL)
		{
			ret = AT_RET_FAIL;
			break;
		}
		
		// ���� OK ���� ???�� ???�l.. ??? ???��?????
		// ??���ڐb����??? ??? OK ?????.. �߅Y??%???..
		if (strncasecmp(cmd, "AT+CMGS", 7) != 0)
		{
			p_chk_cmd = strstr(buffer, "\r\nOK");
			if (p_chk_cmd == NULL)
			{
				write_cmd = 0;
				continue;
			}
		}
		else
		{
			p_chk_cmd = strstr(buffer, resp);
			if (p_chk_cmd == NULL)
			{
				write_cmd = 0;
				continue;
			}
		}
		
		ret = AT_RET_SUCCESS;
		// resp check..
		if ( resp != NULL ) 
		{
			p_chk_cmd = strstr(buffer, resp);
			if ( p_chk_cmd != NULL )
			{
				ret = AT_RET_SUCCESS;
				if ( ret_cmd != NULL )
					strcpy (ret_cmd, p_chk_cmd );
			}
			else
			{
				ret = AT_RET_FAIL;
			}
		}
		else
		{
			ret = AT_RET_SUCCESS;
			if ( ret_cmd != NULL )
				strcpy (ret_cmd, buffer );
		}

		break;
	}
	
	close(fd);
	
	watchdog_delete_id("send_at_cmd_singleline_resp");
	ATLOGD("%s> --\r\n", __func__);
	return ret;
}


int send_at_cmd_numeric(const char* cmd, char* ret_cmd, const int retry_cnt)
{
	int ret = AT_RET_FAIL;
	int tmp;
	char buffer[AT_MAX_BUFF_SIZE] = {0,};
	tmp = retry_cnt;
	
	ret = send_at_cmd_singleline_resp(cmd, NULL, buffer, 3);
	
	if (at_get_number(ret_cmd, buffer) > 0)
	{
		ret = AT_RET_SUCCESS;
		ATLOGD("<atd> dbg : numberic success [%s]\r\n", ret_cmd);
	}
	
	
	return ret;
}

//jwrho ++
int at_get_uart_port(const char* cmd, int *port_num)
{
    char *tr;
    char token_0[ ] = ",";
    char token_1[ ] = "\r\n";
    char *temp_bp = NULL;
    
    char *p_cmd = NULL;
    char buffer[AT_MAX_BUFF_SIZE] = {0,};
    char tmp[AT_MAX_BUFF_SIZE] = {0,};
	int tmp_port_num;
    
    memset(buffer, 0x00, sizeof(buffer));
    strcpy(buffer, cmd);
    
    p_cmd = strstr(buffer, "$$APPS_PORT:");
    
    if ( p_cmd == NULL)
        return AT_RET_FAIL;
    
    tr = strtok_r(p_cmd, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
	//printf("tr #1 = [%s]\n", tr);
    
    tr = strtok_r(NULL, token_1, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
	//printf("tr #2 = [%s]\n", tr);

	at_get_number(tmp, tr);
	tmp_port_num = atoi(tmp);

	//printf("tmp_port_num = [%d]\n", tmp_port_num);
	switch(tmp_port_num) {
		case 11:
			*port_num = UART2_PORT_USED;
			break;
		case 13:
			*port_num = UART1_PORT_USED;
			break;
		default:
			*port_num = UNKNOWN_UART_PORT_USED;
			break;
	}

    return AT_RET_SUCCESS;
}

int get_used_uart_channel_3gpp(int *uart_ch_code)
{	
	char ret_buff[AT_MAX_BUFF_SIZE] = {0,};
	int ret;
		
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	
	if ( uart_ch_code == NULL )
	{
		ATLOGE("<atd> 3gpp [%s] parameter NULL\r\n",__func__);
		return AT_RET_FUNC_DATA_INIT;
	}
	
	if (send_at_cmd_singleline_resp("at$$APPS_PORT?", "$$APPS_PORT: ", ret_buff, 3) != AT_RET_SUCCESS)
	{
		ATLOGE("command response error = [%s]\n", ret_buff);
		return AT_RET_FAIL;
	}

	ret = at_get_uart_port(ret_buff, uart_ch_code);

	return AT_RET_SUCCESS;

}


int set_used_uart_channel_3gpp(UART_PORT_TYPE utype)
{
	int ret = AT_RET_FAIL;
	if(utype == UART1_PORT_USED) {
		ret = send_at_cmd("at$$APPS_PORT=13");
	}
	else if(utype == UART2_PORT_USED) {
		ret = send_at_cmd("at$$APPS_PORT=11");
	}
	else {
		ATLOGE("%s> unknown uart setting parameter [%d]\r\n", __func__, utype);
	}

	return ret;
}

//jwrho --
