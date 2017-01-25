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

int get_adc_main_pwr_tl500(int* main_pwr_volt)
{
	// send_at_cmd_singleline_resp(send_cmd, ">", NULL, 3) != AT_RET_SUCCESS 
	return AT_RET_SUCCESS;
}


