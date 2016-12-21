#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>
#include <time.h>

#include "at/at_log.h"
#include "at/at_util.h"
#include "at_misc.h"

#include "at_cmd_table_wrapper.h"
#include "at_cmd_parser_3gpp.h"


// function init
int dev_table_init_tx501 (AT_FUNC_T* p_func_table)
{

	ATLOGD("%s ++\r\n", __func__);

	if ( p_func_table == NULL) {
		ATLOGE("%s> p_func_table null error\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	memset(p_func_table, 0x00, sizeof(AT_FUNC_T));
	
	// noti proc
	p_func_table->noti_proc = noti_proc_3gpp;
	
	// model sub proc
	
	// sms proc
	p_func_table->send_sms = send_sms_3gpp;
	
	p_func_table->get_phonenum = get_phonenum_3gpp;
	p_func_table->get_imei = get_imei_3gpp;
	
	p_func_table->get_rssi = get_rssi_3gpp;
	p_func_table->get_netstat = get_netstat_3gpp;
	
	p_func_table->get_modemtime = get_modemtime_3gpp;
	
	p_func_table->set_modem_mode = set_modem_mode_3gpp;
	p_func_table->get_modem_mode = get_modem_mode_3gpp;
	p_func_table->reset_modem = reset_modem_3gpp;
	
	p_func_table->set_at_channel_recovery = at_channel_recovery_3gpp;
	
	p_func_table->get_3g_qulify_profile = get_3g_qulify_profile_3gpp;
	p_func_table->set_3g_qulify_profile = set_3g_qulify_profile_3gpp;
	
	p_func_table->get_sms_unread = get_sms_unread_3gpp;
	
	// init cmd..
//	send_at_cmd("AT+CNMI=2,1,0,0,0");
//	send_at_cmd("AT+CMGF=1");
//	send_at_cmd("AT$$SPC=000000");

	ATLOGD("%s> send_at_cmd() call\r\n", __func__);
	send_at_cmd("ATE1");

	ATLOGD("%s> at_read_flush() call\r\n", __func__);
	at_read_flush();

	ATLOGD("%s --\r\n", __func__);
	return AT_RET_SUCCESS;
	
}

