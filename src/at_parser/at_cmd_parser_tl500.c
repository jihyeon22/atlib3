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


