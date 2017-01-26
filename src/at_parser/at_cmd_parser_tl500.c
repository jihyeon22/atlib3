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


int set_gps_on_tl500(AT_MODEM_MODE op)
{	
	// not implement
	return AT_RET_SUCCESS;
}

int set_gps_off_tl500()
{	
	// not implement
	return AT_RET_SUCCESS;
}

int get_gps_ant_tl500(char *ant, int buf_len)
{	
	// not implement
	return AT_RET_SUCCESS;
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
	int phone_rssi = 0;
	
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


