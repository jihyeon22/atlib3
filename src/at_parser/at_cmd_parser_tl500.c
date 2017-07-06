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
#include "at_cmd_parser_tl500.h"
#include "at_cmd_parser_tl500_utils.h"


int set_gps_on_tl500(AT_GPS_ON_TYPE type, AT_GPS_BOOT_TYPE bootmode)
{	
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	char number_buf[AT_MAX_BUFF_SIZE] ={0,};
	int nvr_val = 0;

	// only support agps env setting!!
	if ( ( type != e_GPS_ON_TYPE_SET_ENV_AGPS ) || ( bootmode != e_GPS_BOOT_TYPE_NULL ))
		return AT_RET_CMD_NOT_SUPPORT;
	
	send_at_cmd("AT$$SPC=000000");

	// ---------------------------------------------
	// step 1 : nvr setting.
	// ---------------------------------------------
	memset(&result_buf, 0x00, AT_MAX_BUFF_SIZE);
	memset(&number_buf, 0x00, AT_MAX_BUFF_SIZE);
	if ( send_at_cmd_singleline_resp("AT$$NVR=3758", "$$NVR: ", result_buf, 5) != AT_RET_SUCCESS )
	{
		printf("<atd> tl500 [%s] send cmd fail\r\n",__func__);
		return AT_RET_FAIL;
	}

	at_get_number(number_buf, result_buf);
	nvr_val = atoi(number_buf);

	ATLOGI("<atd> tl500 [%s] nvr val is [%d] \r\n",__func__, nvr_val);

	if( nvr_val != 0 )
	{
		ATLOGI("<atd> tl500 [%s] nvr setting val : 00\r\n",__func__);
		send_at_cmd("AT$$NVW=3758,00");
	}

	// ---------------------------------------------
	// step 2 : apn settting
	// ---------------------------------------------
	/*
	memset(&result_buf, 0x00, AT_MAX_BUFF_SIZE);
	memset(&number_buf, 0x00, AT_MAX_BUFF_SIZE);
	if ( send_at_cmd_singleline_resp("AT+CGDCONT?", "+CGDCONT: ", result_buf, 5) != AT_RET_SUCCESS )
	{
		printf("<atd> tl500 [%s] send cmd fail\r\n",__func__);
		return AT_RET_FAIL;
	}

	if ( strstr(result_buf, "+CGDCONT: 3") == NULL)
	{
		ATLOGI("<atd> tl500 [%s] apn index 3 is not setting. setting 3 apn\r\n",__func__);
		send_at_cmd("at+cgdcont=3,\"IP\",\"lte-internet.sktelecom.com\"");
	}

	if ( strstr(result_buf, "+CGDCONT: 4") == NULL)
	{
		ATLOGI("<atd> tl500 [%s] apn index 4 is not setting. setting 4 apn\r\n",__func__);
		send_at_cmd("at+cgdcont=3,\"IP\",\"lte-internet.sktelecom.com\"");
	}

	//printf("<atd> tl500 [%s] apn list is [%s] \r\n",__func__, result_buf);
	*/
	return AT_RET_SUCCESS;
}

int set_gps_off_tl500()
{	
	// not implement
	return AT_RET_CMD_NOT_SUPPORT;
}

int get_gps_ant_tl500(char *ant, int buf_len)
{	
	// not implement
	return AT_RET_CMD_NOT_SUPPORT;
}

//at$$car_batt?
//$$CAR_BATT: 12

//OK
int get_adc_main_pwr_tl500(int* main_pwr_volt)
{
	// send_at_cmd_singleline_resp(send_cmd, ">", NULL, 3) != AT_RET_SUCCESS
	//char send_cmd[AT_MAX_BUFF_SIZE] ={0,};
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	char number_buf[AT_MAX_BUFF_SIZE] ={0,};
	
	printf("<atd> tl500 [%s] start ??? \r\n",__func__);
	
	if ( send_at_cmd_singleline_resp("AT$$CAR_BATT?", "$$CAR_BATT: ", result_buf, 5) != AT_RET_SUCCESS )
	{
		printf("<atd> tl500 [%s] send cmd fail\r\n",__func__);
		return AT_RET_FAIL;
	}

	at_get_number(number_buf, result_buf);

	printf("get adc result is [%s] !!! \r\n", number_buf);
	*main_pwr_volt = atoi(number_buf);
	return AT_RET_SUCCESS;
}

int get_auto_ota_tl500()
{
	// send_at_cmd_singleline_resp(send_cmd, ">", NULL, 3) != AT_RET_SUCCESS
	//char send_cmd[AT_MAX_BUFF_SIZE] ={0,};
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	char number_buf[AT_MAX_BUFF_SIZE] ={0,};
	
	printf("<atd> tl500 [%s] start ??? \r\n",__func__);
	
	if ( send_at_cmd_singleline_resp("at$$auto_ota?", "$$AUTO_OTA: 0,", result_buf, 5) != AT_RET_SUCCESS )
	{
		printf("<atd> tl500 [%s] send cmd fail\r\n",__func__);
		return AT_RET_FAIL;
	}

	at_get_number(number_buf, result_buf+strlen("$$AUTO_OTA: 0,"));

	printf("get ota result is [%s] !!! \r\n", number_buf);
	return atoi(number_buf);
}

int set_auto_ota_tl500(int mode)
{
	// send_at_cmd_singleline_resp(send_cmd, ">", NULL, 3) != AT_RET_SUCCESS
	//char send_cmd[AT_MAX_BUFF_SIZE] ={0,};
//	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
//	char number_buf[AT_MAX_BUFF_SIZE] ={0,};
	
//	char write_cmd[AT_MAX_BUFF_SIZE] = {0,};

	int current_ota_mode = get_auto_ota_tl500();

	if ( (current_ota_mode != 1) && ( mode == 1 ) )
		return send_at_cmd("at$$auto_ota=1");
	else
		// return send_at_cmd("at$$auto_ota=0");

	return AT_RET_SUCCESS;
}


int get_apps_port_tl500(int* port)
{
	// send_at_cmd_singleline_resp(send_cmd, ">", NULL, 3) != AT_RET_SUCCESS
	//char send_cmd[AT_MAX_BUFF_SIZE] ={0,};
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	char number_buf[AT_MAX_BUFF_SIZE] ={0,};
	
	printf("<atd> tl500 [%s] start ??? \r\n",__func__);
	
	if ( send_at_cmd_singleline_resp("at$$apps_port?", "$$APPS_PORT: 0,", result_buf, 5) != AT_RET_SUCCESS )
	{
		printf("<atd> tl500 [%s] send cmd fail\r\n",__func__);
		return AT_RET_FAIL;
	}

	at_get_number(number_buf, result_buf+strlen("$$APPS_PORT: 0,"));

	printf("get apps port result is [%s] !!! \r\n", number_buf);
	*port = atoi(number_buf);
	return AT_RET_SUCCESS;
}

int set_apps_port_tl500(int port)
{
	// send_at_cmd_singleline_resp(send_cmd, ">", NULL, 3) != AT_RET_SUCCESS
	//char send_cmd[AT_MAX_BUFF_SIZE] ={0,};
	char cmd_buf[AT_MAX_BUFF_SIZE] ={0,};

	int current_port = -1;
	
	if ( get_apps_port_tl500(&current_port) != AT_RET_SUCCESS )
		return AT_RET_FAIL;
	
	if ( (current_port != port) )
	{
		sprintf(cmd_buf, "at$$apps_port=%d", port);
		return send_at_cmd(cmd_buf);
	}
	
	return AT_RET_SUCCESS;
}


int get_swver_tl500(char* buf, int buf_len)
{
	// $$SWVER: 0, TL500S_1.1.0 [May 11 2017 11:41:26](TL500S)
	static char saved_modem_ver_str[AT_MAX_BUFF_SIZE] = {0,};

	char *tr;
    char token_0[ ] = ",\r\n";
	//char token_1[ ] = "\r\n";
    char *temp_bp = NULL;
    
    char *p_cmd = NULL;

	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	char swver_str_tmp[AT_MAX_BUFF_SIZE] ={0,};

	int str_start_idx = 0;
//	int i = 0;

	if (buf == NULL)
		return AT_RET_FAIL;

	if (strlen(saved_modem_ver_str) > 0)
	{
		int target_len = strlen(saved_modem_ver_str);

		if ( target_len > buf_len )
			target_len = buf_len;

		strncpy(buf, saved_modem_ver_str, target_len);
		printf("get_swver_tl500 ret-1 is [%s] \r\n", saved_modem_ver_str);
		return AT_RET_SUCCESS;
	}

	if ( send_at_cmd_singleline_resp("AT$$SWVER?", "$$SWVER: 0", result_buf, 5) != AT_RET_SUCCESS )
	{
		printf("<atd> tl500 [%s] send cmd fail\r\n",__func__);
		return AT_RET_FAIL;
	}

	p_cmd = strstr(result_buf, "$$SWVER: 0");
	if (p_cmd == NULL)
		return AT_RET_FAIL;

	tr = strtok_r(p_cmd, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;

	//printf(" >>>> tr is [%s]\r\n", tr);
		
	tr = strtok_r(NULL, token_0, &temp_bp);
	if(tr == NULL) return AT_RET_FAIL;
	//printf(" >>>> tr is [%s]\r\n", tr);

	strcpy(swver_str_tmp, tr);

	// first space char remove..
	while(1)
	{
		if ( swver_str_tmp[str_start_idx] == ' ')
			str_start_idx++;
		else
			break;
	}

	strcpy(saved_modem_ver_str, swver_str_tmp+str_start_idx);
	{
		int target_len = 0;

		if ( strlen(saved_modem_ver_str) > buf_len )
			target_len = buf_len;

		strncpy(buf, saved_modem_ver_str, target_len);
	}
	
	printf("get_swver_tl500 ret-2 is [%s] \r\n", saved_modem_ver_str);
	return AT_RET_SUCCESS;
}

#ifdef BOARD_TL500K
//lte : KTDEVSTAT:{"QMV":"2.0.0", "RSSI":"-59", "Tx":"-2", "APN":"privatelte.ktfwing.com", "QoS":"00/0000", "BR":"0", "SRVS":"2", "SS":"NS", "CS":"NS", "FREQ":"10812", "RAT":"WCDMA", "D/N":"TL500K", "VER":"1.1.0", "IMEI":"352992033762503", "ICCID":"8982300814008521900F", "RSRP":"NS", "RSRQ":"NS", "RMNET":"NS", "SINR":"NS", "PCID":"NS"}
//wcdma : KTDEVSTAT: "Modem_Qinfo":["1.1.8","-62","-67","-5","0","biz.ktfwing.com","64/3648","0","2","0","1","10836","HSDPA","210","NEO-W200K","0.0.5","356565040232595","8982300814008566236"]
int get_at_ktdevstat2_for_tl500k(char* buff)
{
	char *tr;
    char token_0[ ] = "{}";
    char *temp_bp = NULL;
    
    char *p_cmd = NULL;

	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
//    char result_buf2[AT_MAX_BUFF_SIZE] ={0,};

	if (buff == NULL)
		return AT_RET_FAIL;

	if ( send_at_cmd_singleline_resp("ATKTDEVSTAT2", "KTDEVSTAT:", result_buf, 5) != AT_RET_SUCCESS )
		return AT_RET_FAIL;


	p_cmd = strstr(result_buf, "KTDEVSTAT:");
	if (p_cmd == NULL)
		return AT_RET_FAIL;

	p_cmd+=strlen("KTDEVSTAT:");
    
	tr = strtok_r(p_cmd, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
//	printf(" >>>> tr is [%s]\r\n", tr);
	
//	tr = strtok_r(NULL, token_0, &temp_bp);
//    if(tr == NULL) return AT_RET_FAIL;
//	printf(" >>>> tr is [%s]\r\n", tr);
	
	sprintf(buff, "%s",tr);

	return AT_RET_SUCCESS;
}

//lte : KTDEVSTAT:{"QMV":"2.0.0", "RSSI":"-59", "Tx":"-2", "APN":"privatelte.ktfwing.com", "QoS":"00/0000", "BR":"0", "SRVS":"2", "SS":"NS", "CS":"NS", "FREQ":"10812", "RAT":"WCDMA", "D/N":"TL500K", "VER":"1.1.0", "IMEI":"352992033762503", "ICCID":"8982300814008521900F", "RSRP":"NS", "RSRQ":"NS", "RMNET":"NS", "SINR":"NS", "PCID":"NS"}
//wcdma : KTDEVSTAT: "Modem_Qinfo":["1.1.8","-62","-67","-5","0","biz.ktfwing.com","64/3648","0","2","0","1","10836","HSDPA","210","NEO-W200K","0.0.5","356565040232595","8982300814008566236"]
// <QMV>,<RSSI>,<RSCP>,<E/I>,<Tx>,<APN>,<QoS>,<BR>,<SRVC>,<SS>,<CS>,<FREQ>,<RAT>,<D/N>,<VER>,<IMEI>,<ICCID>,<RSRP>,<RSRQ>,<RMNET>
typedef enum {
	e_QMV,
	e_RSSI,
	e_Tx,
	e_APN,
	e_QoS,
	e_BR, 
	e_SRVS, 
	e_SS ,
	e_CS,
	e_FREQ,
	e_RAT,
	e_DN,
	e_VER,
	e_IMEI,
	e_ICCID,
	e_RSRP,
	e_RSRQ,
	e_RMNET,
	e_SINR,
	e_PCID,
	e_QTY_MAX
} e_QTY_ID;


int get_at_ktdevstat_for_tl500k(char* buff)
{
	char *tr;
    char token_0[ ] = ":,";
    char *temp_bp = NULL;
    
	char QTY_STR[64][30] = {0,};

    char *p_cmd = NULL;

	int i = 0;
	int idx_qty = 0;
	int str_len = 0;

	char ktdevstat2_str[AT_MAX_BUFF_SIZE] ={0,};
	char ktdevstat_str[AT_MAX_BUFF_SIZE] ={0,};	
	char tmp_str[AT_MAX_BUFF_SIZE] ={0,};

	if ( get_at_ktdevstat2_for_tl500k(ktdevstat2_str) == AT_RET_FAIL )
		return AT_RET_FAIL;
    
	at_remove_char(ktdevstat2_str, tmp_str, AT_MAX_BUFF_SIZE, '\"');

	p_cmd = tmp_str;

	while(1)
	{
		tr = strtok_r(p_cmd, token_0, &temp_bp);
    	if(tr == NULL) break;

		p_cmd = NULL;

		tr = strtok_r(p_cmd, token_0, &temp_bp);
    	if(tr == NULL) break;

		strcpy(QTY_STR[idx_qty], tr);
		//printf("----> qty str [%d]=>[%s]\r\n", idx_qty, QTY_STR[idx_qty]);
		idx_qty++;
	}

	for ( i = 0 ; i < e_QTY_MAX ; i ++ )
	{
		if ( ( strlen(QTY_STR[i]) == 0 ) || ( strcasecmp (QTY_STR[i], "NS") == 0 ) )
		{
			memset(QTY_STR[i], 0x00, 30);
			strcpy(QTY_STR[i], "0");
		}
	}

	str_len += sprintf(ktdevstat_str + str_len,"[\"%s\" ,", QTY_STR[e_QMV]); // <QMV>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_RSSI]); // <RSSI>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", "0"); // <RSCP>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", "0"); // <E/I>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_Tx]); // <Tx>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_APN]); // <APN>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_QoS]); // <QoS>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_BR]); // <BR>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_SRVS]); // <SRVC>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_SS]); // <SS>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_CS]); // <CS>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_FREQ]); // <FREQ>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_RAT]); // <RAT>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_DN]); // <D/N>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_VER]); // <VER>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_IMEI]); // <IMEI>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_ICCID]); // <ICCID>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_RSRP]); // <RSRP>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_RSRQ]); // <RSRQ>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\"]", QTY_STR[e_RMNET]); // <RMNET>
/*
	str_len += sprintf(ktdevstat_str + str_len,"[\"%s\" ,", QTY_STR[e_QMV]); // <QMV>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_RSSI]); // <RSSI>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_Tx]); // <RSCP>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_APN]); // <E/I>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_QoS]); // <Tx>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_BR]); // <APN>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_SRVS]); // <QoS>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_SS]); // <BR>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_CS]); // <SRVC>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_FREQ]); // <SS>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_RAT]); // <CS>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_DN]); // <FREQ>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_VER]); // <RAT>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_IMEI]); // <D/N>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_ICCID]); // <VER>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_RSRP]); // <IMEI>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_RSRQ]); // <ICCID>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_RMNET]); // <RSRP>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\" ,", QTY_STR[e_SINR]); // <RSRQ>
	str_len += sprintf(ktdevstat_str + str_len,"\"%s\"]", QTY_STR[e_PCID]); // <RMNET>
	//printf("ktdevstat_str is [%s]\r\n", ktdevstat_str);
*/
	strcpy(buff, ktdevstat_str);
//	sprintf(buff, "%s",tr);

	return AT_RET_SUCCESS;
}

int get_at_ktfota_ready_tl500k(int* result)
{
	// AT$$KFOTA_READY?
	// $$KFOTA_READY: 0, 1
	// OK
	char result_buf[AT_MAX_BUFF_SIZE] ={0,};
	char number_buf[AT_MAX_BUFF_SIZE] ={0,};

	char tmp_str[AT_MAX_BUFF_SIZE] ={0,};

//	char* p_tmp = NULL;

	if ( send_at_cmd_singleline_resp("AT$$KFOTA_READY?", "$$KFOTA_READY: ", result_buf, 5) != AT_RET_SUCCESS )
	{
		printf("<atd> tl500 [%s] send cmd fail\r\n",__func__);
		return AT_RET_FAIL;
	}

	if ( strstr(result_buf,"$$KFOTA_READY: 0,") == NULL )
		return AT_RET_FAIL;

	strcpy(tmp_str, result_buf+strlen("$$KFOTA_READY: 0,"));

	at_get_number(number_buf, tmp_str);

	printf("<atd> kt fota ready :: [%s] !!! \r\n", number_buf);
	*result = atoi(number_buf);

	return AT_RET_SUCCESS;
}

int set_modem_fota_testmode_for_tl500k(int mode)
{
	if (mode == TELADIN_DMS_SETTING_TEST_MODE)
		send_at_cmd("AT$$APPS_SEND=\"APPS_AT$$KT_DMS_FOTA_SERVER=0\"");
	else if (mode == TELADIN_DMS_SETTING_NONTEST_MODE)
		send_at_cmd("AT$$APPS_SEND=\"APPS_AT$$KT_DMS_FOTA_SERVER=1\"");
	else
		return AT_RET_FAIL;

	return AT_RET_SUCCESS;
}
#endif

