#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <pthread.h>
#include <time.h>

#include "at/at_log.h"
#include "at/at_util.h"
#include "at_misc.h"

#include "at_cmd_table_wrapper.h"
#include "at_cmd_parser_3gpp.h"
#include "at_cmd_parser_3gpp_utils.h"

int noti_proc_3gpp(const char* buffer, int len)
{
	//ATLOGT("<atd> 3gpp (%s) - (%d)\r\n",__func__,__LINE__);
	char* result_cmd = NULL;
	char write_cmd[AT_MAX_BUFF_SIZE] = {0,};
	
	int i = 0;
	
	ATLOGT("<atd> 3gpp (%s) - (%d) : noti [%s]\r\n",__func__,__LINE__, buffer);

	// model sms proc
	// CMTI 명령어가 오면, CMGR 명령어를 새로 보낸다. (sms 읽기 위해.)
	if ((result_cmd = strstr(buffer, "+CMTI: \"ME\"")) != NULL)
	{
		i = 0;
		
		if (at_get_sms_idx_from_cmti(result_cmd, &i) == AT_RET_SUCCESS)
		{
			sprintf(write_cmd,"AT+CMGR=%d",i);
			send_at_cmd(write_cmd);
		}
	}
	// 실제 SMS 가 왔을때..
	// 간혹 READ MARK 관련해서 꼬이는경우도 있다. 
	// 그냥 CMGR 일때는 모두 파싱하도록 한다.
	// else if((result_cmd = strstr(buffer, "+CMGR: \"REC UNREAD\"")) != NULL)
	else if((result_cmd = strstr(buffer, "+CMGR: ")) != NULL)
	{
		char phone_num[AT_MAX_BUFF_SIZE] = {0,};
		char msg_time[AT_MAX_BUFF_SIZE] = {0,};
		char msg[AT_MAX_BUFF_SIZE] = {0,};
		
		if (at_parse_sms_from_cmgr(result_cmd, phone_num, msg_time, msg) != AT_RET_SUCCESS)
			return AT_RET_FAIL;

		check_sms_sendto_client();
		
		if (g_model_proc_sms_read != NULL)
		{
			g_model_proc_sms_read(phone_num, msg_time, msg);
		}
	}
	else
	{
		// model notification proc
		if (g_model_proc_noti != NULL)
		{
			g_model_proc_noti(buffer,len);
		}
	}
	return AT_RET_SUCCESS;
}

int get_phonenum_3gpp(char *pnumber, int buf_len)
{	
	char ret_buff[AT_MAX_BUFF_SIZE] = {0,};
	static char phone_num[AT_MAX_BUFF_SIZE] = {0,};
	
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	
	// 인자 체크
	// 만약에 잘못된 인자가 들어온다면, 기존의 저장값을 초기화하는데 쓰인다.
	if ( ( pnumber == NULL ) || ( buf_len <= 0 ) )
	{
		memset(phone_num, 0x00, sizeof(phone_num));
		ATLOGT("<atd> 3gpp [%s] saved data init\r\n",__func__);
		return AT_RET_FUNC_DATA_INIT;
	}
	
	// 기존의 이미 데이터가 있는지 확인
	// 기존의 데이터에 대해서 길이 체크 : 길이가 잘못되어있으면, 다시 읽어온다.
	if (strlen(phone_num) < AT_LEN_PHONENUM )
	{
		if (send_at_cmd_singleline_resp("AT+CNUM", "+CNUM: ", ret_buff, 3) != AT_RET_SUCCESS)
		{
			memset(phone_num, 0x00, sizeof(phone_num));
			ATLOGE("<atd> 3gpp [%s] CNUM command fail - 1\r\n",__func__);
			return AT_RET_FAIL;
		}

		if ( at_get_phonenum_cnum(ret_buff, phone_num) != AT_RET_SUCCESS)
		{
			memset(phone_num, 0x00, sizeof(phone_num));
			ATLOGE("<atd> 3gpp [%s] CNUM command fail - 2\r\n",__func__);
			return AT_RET_FAIL;
		}
	}
	
	// 읽어온 데이터에 대해서 유효성 다시 체크
	if (strlen(phone_num) < AT_LEN_PHONENUM )
	{
		memset(phone_num, 0x00, sizeof(phone_num));
		ATLOGE("<atd> 3gpp [%s] CNUM command fail - 3\r\n",__func__);
		return AT_RET_FAIL;
	}
	
	ATLOGT("<atd> 3gpp [%s] : phone num [%s]\r\n",__func__, phone_num);
	
	if ( ( pnumber == NULL ) || (buf_len < (int)strlen(phone_num)) )
	{
		ATLOGE("<atd> 3gpp [%s] func argument invalid\r\n",__func__);
		return AT_RET_FAIL;
	}
	
	strcpy(pnumber, phone_num);
	return AT_RET_SUCCESS;

}

int get_imei_3gpp(char *imei, int buf_len)
{
	static char phone_imei[AT_MAX_BUFF_SIZE] = {0,};
	
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	
	if ( ( imei == NULL ) || ( buf_len <= 0 ) )
	{
		memset(phone_imei, 0x00, sizeof(phone_imei));
		ATLOGT("<atd> 3gpp [%s] saved data init\r\n",__func__);
		return AT_RET_FUNC_DATA_INIT;
	}
	
	// 기존의 이미 데이터가 있는지 확인
	// 기존의 데이터에 대해서 길이 체크 : 길이가 잘못되어있으면, 다시 읽어온다.
	if (strlen(phone_imei) < AT_LEN_IMEI )
	{
		if ( send_at_cmd_numeric("AT+GSN", phone_imei, 4) != AT_RET_SUCCESS )
		{
			memset(phone_imei, 0x00, sizeof(phone_imei));
			ATLOGE("<atd> 3gpp [%s] GSN command fail - 1 \r\n",__func__);
			return AT_RET_FAIL;
		}
	}
	
	// 읽어온 데이터에 대해서 유효성 다시 체크
	if (strlen(phone_imei) < AT_LEN_IMEI )
	{
		memset(phone_imei, 0x00, sizeof(phone_imei));
		ATLOGE("<atd> 3gpp [%s] GSN command fail - 2\r\n",__func__);
		return AT_RET_FAIL;
	}
	
	if ( ( imei == NULL ) || (buf_len < (int)strlen(phone_imei)) )
	{
		ATLOGE("<atd> 3gpp [%s] func argument invalid\r\n",__func__);
		return AT_RET_FAIL;
	}
	
	strcpy(imei, phone_imei);
	ATLOGT("<atd> 3gpp [%s] : imei [%s]\r\n",__func__, phone_imei);
	
	return AT_RET_SUCCESS;
	
	
}

int send_sms_3gpp(const char* dest, const char* msg)
{
	char send_cmd[AT_MAX_BUFF_SIZE] ={0,};
	
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	ATLOGT("<atd> 3gpp [%s] dest [%s], msg [%s]\r\n", __func__ , dest, msg);
	
	// 강제로 text mode 로 변환
	send_at_cmd("AT+CMGF=1");
	
	// 첫째로 보내는 at cmd 를 보낸다. 이때 응답은 ">" 가 나와야한다.
	memset(send_cmd, 0x00, sizeof(send_cmd));
	sprintf(send_cmd,"AT+CMGS=\"%s\"",dest);
	if ( send_at_cmd_singleline_resp(send_cmd, ">", NULL, 3) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] send CMGS cmd fail\r\n", __func__);
		memset(send_cmd, 0x00, sizeof(send_cmd));
		sprintf(send_cmd,"%c", AT_CTRL_CHAR(z));
		send_at_cmd_singleline_resp(send_cmd, NULL, NULL, 3);
		return AT_RET_FAIL;
	}
	
	// 이후에 실제 메시지를 ctrl + z 문자와 함께 보낸다.
	memset(send_cmd, 0x00, sizeof(send_cmd));
	sprintf(send_cmd,"%s%c",msg, AT_CTRL_CHAR(z));
	if ( send_at_cmd_singleline_resp(send_cmd, NULL, NULL, 3) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] send msg fail\r\n");
		return AT_RET_FAIL;
	}
	
	// 여기까지 왔으면 성공
	return AT_RET_SUCCESS;
}


int get_rssi_3gpp(int *rssi)
{
	//char send_cmd[AT_MAX_BUFF_SIZE] ={0,};
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	
	int phone_rssi = 0;
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	
	if ( send_at_cmd_singleline_resp("AT+CSQ", "+CSQ:", result_buf, 5) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] send CSQ fail\r\n",__func__);
		return AT_RET_FAIL;
	}
	
	if ( at_get_rssi_from_csq(result_buf, &phone_rssi) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] rssi get fail\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	ATLOGT("<atd> 3gpp [%s] rssi is [%d]\r\n",__func__, phone_rssi);

	*rssi = phone_rssi;
	return AT_RET_SUCCESS;
	
}


int get_netstat_3gpp(AT_RET_NET_STAT *stat)
{
	//char send_cmd[AT_MAX_BUFF_SIZE] ={0,};
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	
	int netstat = 0;
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	
	if ( send_at_cmd_singleline_resp("AT+CGREG?", "+CGREG:", result_buf, 3) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] send CGREG fail\r\n");
		return AT_RET_FAIL;
	}
	
	if ( at_get_netstat_from_cgreg(result_buf, &netstat) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] netstat get fail\r\n");
		return AT_RET_FAIL;
	}
	
	ATLOGT("<atd> 3gpp [%s] netstat is [%d]\r\n",__func__, netstat);

	*stat = netstat;
	return AT_RET_SUCCESS;
	
}

// +CCLK: "16/03/16,14:36:51"
int get_modemtime_3gpp(time_t *out, int flag)
{
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	char time_str[AT_MAX_BUFF_SIZE] = {0,};
	
	//int netstat = 0;
	
	struct tm tm_tmp;
	memset(&tm_tmp, 0x00, sizeof(struct tm));
	
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	
	if (out == NULL)
	{
		ATLOGE("<atd> 3gpp [%s] invalid argument\r\n");
		return AT_RET_FAIL;
	}
	
	if ( send_at_cmd_singleline_resp("AT+CCLK?", "+CCLK:", result_buf, 3) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] send CCLK fail\r\n");
		return AT_RET_FAIL;
	}
	
	if ( at_get_time_from_cclk(result_buf, time_str) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] modem time get fail\r\n");
		return AT_RET_FAIL;
	}
	
	ATLOGT("<atd> 3gpp [%s] modem time is [%s]\r\n",__func__, time_str);
		
	if (strptime(time_str, "%y/%m/%d,%H:%M:%S", &tm_tmp) == NULL)
		return AT_RET_FAIL;
	
	//ATLOGD( "tm --> %d %d %d %d %d %d\n", t.tm_year + 1900 , t.tm_mon + 1 , t.tm_mday , t.tm_hour , t.tm_min , t.tm_sec);
	
	*out = mktime(&tm_tmp);
	
	if (flag == 1)
	{
		time_t t_tmp = {0,};
		t_tmp = mktime(&tm_tmp);
		ATLOGT("<atd> 3gpp [%s] set linux system time\r\n",__func__);
		stime(&t_tmp);
	}
	
	return AT_RET_SUCCESS;
}

int set_modem_mode_3gpp(AT_MODEM_MODE op)
{
	char send_cmd[AT_MAX_BUFF_SIZE] ={0,};
	
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	ATLOGT("<atd> 3gpp [%s] set mode [%d]\r\n", __func__ , op);

	// 첫째로 보내는 at cmd 를 보낸다. 이때 응답은 ">" 가 나와야한다.
	memset(send_cmd, 0x00, sizeof(send_cmd));
	sprintf(send_cmd,"AT+CFUN=%d", op);
	if ( send_at_cmd_singleline_resp(send_cmd, "OK", NULL, 3) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] send CFUN cmd fail\r\n",__func__);
		return AT_RET_FAIL;
	}
	
	return AT_RET_SUCCESS;
	
}

// +CFUN: 1
int get_modem_mode_3gpp(AT_MODEM_MODE* op)
{
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	int mode_temp = 0;
	
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);

	if ( send_at_cmd_singleline_resp("AT+CFUN?", "+CFUN:", result_buf, 3) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] send CFUN cmd fail\r\n",__func__);;
		return AT_RET_FAIL;
	}
	
	if ( at_get_mode_from_cfun(result_buf, &mode_temp) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] CFUN parse fail\r\n",__func__);
		return AT_RET_FAIL;
	}
	
	ATLOGT("<atd> 3gpp [%s] cur mode [%d]\r\n", __func__ , mode_temp);
	
	*op = mode_temp;
	
	return AT_RET_SUCCESS;
}

int reset_modem_3gpp()
{
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	set_modem_mode_3gpp(MODEM_MODE_OFFLINE);
	sleep(1);
	set_modem_mode_3gpp(MODEM_MODE_ONLINE);
	
	return AT_RET_SUCCESS;
}

int at_channel_recovery_3gpp()
{
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	send_at_cmd("AT");
	return AT_RET_SUCCESS;
}
	
// +CGEQREQ: 1,2,8,3648,0,0,0,1500,"1E4","1E5",3,0,0
int get_3g_qulify_profile_3gpp(network_qos_info_t* qos_info)
{
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	//int mode_temp = 0;
	network_qos_info_t qos_info_tmp;
	
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);

	if ( send_at_cmd_singleline_resp("AT+CGEQREQ?", "+CGEQREQ:", result_buf, 3) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] send CGEQREQ cmd fail\r\n",__func__);;
		return AT_RET_FAIL;
	}
	
	if ( at_get_mode_from_cgeqreq(result_buf, &qos_info_tmp) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] parse CGEQREQ cmd fail\r\n",__func__);;
		return AT_RET_FAIL;
	}
	
	/*
	ATLOGD("qos_info_tmp.cid is [%d]\r\n", qos_info_tmp.cid);
	ATLOGD("qos_info_tmp.traffic_class is [%d]\r\n", qos_info_tmp.traffic_class);
	ATLOGD("qos_info_tmp.max_bitrate_uplink is [%d]\r\n", qos_info_tmp.max_bitrate_uplink);
	ATLOGD("qos_info_tmp.max_bitrate_downlink is [%d]\r\n", qos_info_tmp.max_bitrate_downlink);
	ATLOGD("qos_info_tmp.granteed_bitrate_uplink is [%d]\r\n", qos_info_tmp.granteed_bitrate_uplink);
	ATLOGD("qos_info_tmp.granteed_bitrate_downlink is [%d]\r\n", qos_info_tmp.granteed_bitrate_downlink);
	ATLOGD("qos_info_tmp.delivery_order is [%d]\r\n", qos_info_tmp.delivery_order);
	ATLOGD("qos_info_tmp.max_sdu_size is [%d]\r\n", qos_info_tmp.max_sdu_size);
	ATLOGD("qos_info_tmp.sdu_err_ratio is [%s]\r\n", qos_info_tmp.sdu_err_ratio);
	ATLOGD("qos_info_tmp.residual_bit_err_ratio is [%s]\r\n", qos_info_tmp.residual_bit_err_ratio);
	ATLOGD("qos_info_tmp.delivery_of_err_sdu is [%d]\r\n", qos_info_tmp.delivery_of_err_sdu);
	ATLOGD("qos_info_tmp.transfer_delay is [%d]\r\n", qos_info_tmp.transfer_delay);
	ATLOGD("qos_info_tmp.traffic_handing_priority is [%d]\r\n", qos_info_tmp.traffic_handing_priority);
	*/
	
	memcpy(qos_info, &qos_info_tmp, sizeof(network_qos_info_t));
	
	return AT_RET_SUCCESS;
}

int set_3g_qulify_profile_3gpp(network_qos_info_t qos_info)
{
	char send_cmd[AT_MAX_BUFF_SIZE] ={0,};
	
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	//ATLOGT("<atd> 3gpp [%s] set mode [%d]\r\n", __func__ , op);

	// 첫째로 보내는 at cmd 를 보낸다. 이때 응답은 ">" 가 나와야한다.
	memset(send_cmd, 0x00, sizeof(send_cmd));
	
	// LOGD(LOG_TARGET, "set_3g_qulify_profile request!\n");
	sprintf(send_cmd,       "AT+CGEQREQ=%d,%d,%d,%d,%d,%d,%d,%d,%s,%s,%d,%d,%d\r\n",
				qos_info.cid,
				qos_info.traffic_class,
				qos_info.max_bitrate_uplink,
				qos_info.max_bitrate_downlink,
				qos_info.granteed_bitrate_uplink,
				qos_info.granteed_bitrate_downlink,
				qos_info.delivery_order,
				qos_info.max_sdu_size,
				qos_info.sdu_err_ratio,
				qos_info.residual_bit_err_ratio,
				qos_info.delivery_of_err_sdu,
				qos_info.transfer_delay,
				qos_info.traffic_handing_priority);
	if ( send_at_cmd_singleline_resp(send_cmd, "OK", NULL, 3) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] send CGEQREQ cmd fail\r\n",__func__);;
		return AT_RET_FAIL;
	}
	
	return AT_RET_SUCCESS;
}


int get_qos_info_3gpp(int* max_uplink, int* max_downlink)
{
	network_qos_info_t qos_info;
	
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	
	if ( get_3g_qulify_profile_3gpp(&qos_info) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] get qulify api fail\r\n",__func__);;
		return AT_RET_FAIL;
	}
	
	*max_uplink = qos_info.max_bitrate_uplink;
	*max_downlink= qos_info.max_bitrate_downlink;
	
	return AT_RET_SUCCESS;
}

int set_qos_info_3gpp(int max_uplink, int max_downlink)
{
	network_qos_info_t qos_info;
	
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	
	if ( get_3g_qulify_profile_3gpp(&qos_info) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] get qulify api fail\r\n",__func__);;
		return AT_RET_FAIL;
	}
	
	qos_info.max_bitrate_uplink = max_uplink;
	qos_info.max_bitrate_downlink = max_downlink;
	
	if ( set_3g_qulify_profile_3gpp(qos_info) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] set qulify api fail\r\n",__func__);;
		return AT_RET_FAIL;
	}
	
	return AT_RET_SUCCESS;
}

int at_set_clear_all_sms()
{
    send_at_cmd("AT+CMGD=,4");
	return AT_RET_SUCCESS;
}

int get_sms_unread_3gpp(SMS_MSG_STAT_T* p_sms_msg_stat)
{
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	
	// unread 로만 하면 간혹 노치는경우발생
	// all 로 읽고 읽은데로 다 지운다.
	//if ( send_at_cmd_singleline_resp("AT+CMGL=\"REC UNREAD\"", NULL, result_buf, 3) != AT_RET_SUCCESS )
	if ( send_at_cmd_singleline_resp("AT+CMGL=\"ALL\"", NULL, result_buf, 3) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] send CMGL cmd fail\r\n",__func__);;
		return AT_RET_FAIL;
	}

	// 읽은다음 바로 지운다.
	at_set_clear_all_sms();
	
	if ( at_get_unread_sms_from_cmgl(result_buf, p_sms_msg_stat) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] parse CMGL cmd fail\r\n",__func__);;
		return AT_RET_FAIL;
	}
	

	return AT_RET_SUCCESS;
}
	

// ==============================================================
// at cmd utils
// ==============================================================
// +CMTI: "ME",0



int get_csq_3gpp(int *csq)
{
	//char send_cmd[AT_MAX_BUFF_SIZE] ={0,};
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	
	int phone_csq = 0;
	ATLOGT("<atd> 3gpp [%s] start\r\n",__func__);
	
	if ( send_at_cmd_singleline_resp("AT+CSQ", "+CSQ:", result_buf, 5) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] send CSQ fail\r\n",__func__);
		return AT_RET_FAIL;
	}
	
	if ( at_get_csq_from_csq(result_buf, &phone_csq) != AT_RET_SUCCESS )
	{
		ATLOGE("<atd> 3gpp [%s] csq get fail\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	ATLOGT("<atd> 3gpp [%s] csq is [%d]\r\n",__func__, phone_csq);

	*csq = phone_csq;
	return AT_RET_SUCCESS;
	
}

int set_apn_info_3gpp(int cid, int attr, char* apn)
{
	int cur_attr;
	int need_to_setting = 1;
	char cur_apn_addr[256] = {0,};

	ATLOGT("<atd> 3gpp [%s] ++ \r\n",__func__);
	if (get_apn_info_3gpp(cid, &cur_attr, cur_apn_addr) == AT_RET_SUCCESS)
	{
		need_to_setting = 0;
		ATLOGT("<atd> 3gpp [%s] already cid exist! check apn [%d][%s]\r\n",__func__, cur_attr, cur_apn_addr);
		if ( cur_attr != attr )
			need_to_setting = 1;
		if ( strcmp( cur_apn_addr, apn) != 0 )
			need_to_setting = 1;
	}

	ATLOGT("<atd> 3gpp [%s] need to seting [%d]\r\n",__func__, need_to_setting);

	if ( need_to_setting == 0 )
		return AT_RET_SUCCESS;
	else
		return at_set_apn_form_cgdcont(cid, attr, apn);
}

int get_apn_info_3gpp(int cid, int* attr, char* apn)
{
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	char tmp_chk_str[32] = {0,};

	char* p_cmd = NULL;

	int ip_attr = 0;
	char apn_addr[128] = {0,};

	ATLOGT("<atd> 3gpp [%s] ++ cid [%d]\r\n",__func__, cid);
	memset(&result_buf, 0x00, AT_MAX_BUFF_SIZE);

	if ( send_at_cmd_singleline_resp("AT+CGDCONT?", "+CGDCONT: ", result_buf, 5) != AT_RET_SUCCESS )
	{
		printf("<atd> tl500 [%s] send cmd fail\r\n",__func__);
		return AT_RET_FAIL;
	}

    sprintf(tmp_chk_str, "+CGDCONT: %d,",cid);
	p_cmd = strstr(result_buf, tmp_chk_str);

    if ( p_cmd == NULL)
    {
        ATLOGI("<atd> [%s()] cid [%d] is not exist\r\n",__func__, cid);
        return AT_RET_FAIL;
    }

	

	if ( at_get_apn_form_cgdcont(p_cmd, &ip_attr, apn_addr) == AT_RET_SUCCESS ) 
	{
		*attr = ip_attr;
		strcpy(apn, apn_addr);
		return AT_RET_SUCCESS;
	}
	else
		return AT_RET_FAIL;
}

