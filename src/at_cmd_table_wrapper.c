#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>
#include <time.h>

#include "at/at_log.h"
#include "at/at_util.h"
#include "at_cmd_table_wrapper.h"
#include "board_system.h"

static AT_FUNC_T g_at_func_table = {0,};

void (*g_model_proc_noti)(const char* buffer, int len) = NULL;
void (*g_model_proc_sms_read)(const char* phone_num, const char* recv_time, const char* msg) = NULL;
	
int at_func_init(AT_DEV_TYPE dev,void (*p_noti_proc)(const char* buffer, int len),  void (*p_sms_proc)(const char* phone_num, const char* recv_time, const char* msg))
{
	memset(&g_at_func_table, 0x00, sizeof(g_at_func_table));
	
	g_model_proc_noti = p_noti_proc;
	g_model_proc_sms_read = p_sms_proc;

	if ( dev == e_DEV_3GPP )
	{
		ATLOGT("<atd> init : dev is 3gpp\r\n");
	}
	else if ( dev == e_DEV_TX501_BASE )
	{
		ATLOGT("<atd> init : dev is tx501\r\n");
		dev_table_init_tx501 (&g_at_func_table);
	}
	else if ( dev == e_DEV_TL500_BASE )
	{
		ATLOGT("<atd> init : dev is tl500\r\n");
		dev_table_init_tl500 (&g_at_func_table);
	}
	else
	{
		ATLOGE("<atd> init : dev is not support\r\n");
	}

	return AT_RET_SUCCESS;
	
}

// --------------------------------------
// noti proc
// --------------------------------------
int at_noti_proc(const char* buffer, int len)
{
	if ((g_at_func_table.noti_proc == NULL) || (g_at_func_table.noti_proc == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}

	if ( buffer == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	g_at_func_table.noti_proc(buffer,len);

	return AT_RET_SUCCESS;
}

// --------------------------------------
// modem info
// --------------------------------------
int at_get_phonenum(char *pnumber, int buf_len)
{
	
	if ((g_at_func_table.get_phonenum == NULL) || (g_at_func_table.get_phonenum == 0x00))
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( pnumber == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_phonenum(pnumber,buf_len);
}

int at_get_imei(char *imei, int buf_len)
{
	if ((g_at_func_table.get_imei == NULL) || (g_at_func_table.get_imei == 0x00))
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( imei == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_imei(imei,buf_len);
}

// --------------------------------------
// modem sms
// --------------------------------------
int at_send_sms(const char* dest, const char* msg)
{	
	if ((g_at_func_table.send_sms == NULL) || (g_at_func_table.send_sms == 0x00))
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if (( dest == NULL ) || ( msg == NULL ) )
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.send_sms(dest,msg);
}


// --------------------------------------
// modem status
// --------------------------------------
int at_get_rssi(int *rssi)
{	
	if ((g_at_func_table.get_rssi == NULL) || (g_at_func_table.get_rssi == 0x00))
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( rssi == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_rssi(rssi);
} 

int at_get_netstat(AT_RET_NET_STAT* netstat)
{
	if ((g_at_func_table.get_netstat == NULL) || (g_at_func_table.get_netstat == 0x00))
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( netstat == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_netstat(netstat);
} 

int at_get_dbgmsg(unsigned char* result_buff, const int buff_size)
{
	if ((g_at_func_table.get_dbgmsg == NULL) || (g_at_func_table.get_dbgmsg == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( result_buff == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_dbgmsg(result_buff, buff_size);
} 

int at_get_modemtime(time_t *out, int flag)
{
	if ((g_at_func_table.get_modemtime == NULL) || (g_at_func_table.get_modemtime == 0x00))
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( out == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_modemtime(out, flag);
} 

// --------------------------------------
// gps function
// --------------------------------------

int at_get_gps_ant(char *ant, int buf_len)
{

	if ((g_at_func_table.get_gps_ant == NULL) || (g_at_func_table.get_gps_ant == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( ant == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_gps_ant(ant,buf_len);
} 

int at_set_gps_on(AT_GPS_ON_TYPE type, AT_GPS_BOOT_TYPE bootmode)
{
	if ((g_at_func_table.set_gps_on == NULL) || (g_at_func_table.set_gps_on == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	return g_at_func_table.set_gps_on(type,bootmode);
} 

int at_set_gps_off(void)
{
	if ((g_at_func_table.set_gps_off == NULL) || (g_at_func_table.set_gps_off == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	return g_at_func_table.set_gps_off();
}

int at_get_agps_addr(char *addr, int buf_len)
{	
	if ((g_at_func_table.get_agps_addr == NULL) || (g_at_func_table.get_agps_addr == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( addr == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}

	return g_at_func_table.get_agps_addr(addr,buf_len);
}

int at_set_agps_addr(const char *addr)
{
	if ((g_at_func_table.set_agps_addr == NULL) || (g_at_func_table.set_agps_addr == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( addr == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.set_agps_addr(addr);
}


// --------------------------------------
// modem mode
// --------------------------------------
int at_set_modem_mode(AT_MODEM_MODE op)
{
	if ((g_at_func_table.set_modem_mode == NULL) || (g_at_func_table.set_modem_mode == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	//if ( op == NULL ) 
	//{
	//	ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
	//	return AT_RET_FAIL;
	//}
	
	return g_at_func_table.set_modem_mode(op);
}

int at_get_modem_mode(AT_MODEM_MODE* op)
{
	if ((g_at_func_table.get_modem_mode == NULL) || (g_at_func_table.get_modem_mode == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( op == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_modem_mode(op);
}

int at_reset_modem(void)
{
	if ((g_at_func_table.reset_modem == NULL) || (g_at_func_table.reset_modem == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	return g_at_func_table.reset_modem();
}

int at_set_at_channel_recovery(void)
{
	if ((g_at_func_table.set_at_channel_recovery == NULL) || (g_at_func_table.set_at_channel_recovery == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	return g_at_func_table.set_at_channel_recovery();
}

int at_get_modem_status(char *buf)
{	
	if ((g_at_func_table.get_modem_status == NULL) || (g_at_func_table.get_modem_status == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( buf == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_modem_status(buf);
}

int at_set_alive(int time)
{
	if ((g_at_func_table.set_alive == NULL) || (g_at_func_table.set_alive == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( time <= 0 ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.set_alive(time);
}

int at_get_3g_qulify_profile(network_qos_info_t* qos_info)
{
	if ((g_at_func_table.get_3g_qulify_profile == NULL) || (g_at_func_table.get_3g_qulify_profile == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( qos_info == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_3g_qulify_profile(qos_info);
}

int at_set_3g_qulify_profile(network_qos_info_t qos_info)
{
	if ((g_at_func_table.set_3g_qulify_profile == NULL) || (g_at_func_table.set_3g_qulify_profile == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	return g_at_func_table.set_3g_qulify_profile(qos_info);
}

int at_get_qos_info(int* max_uplink, int* max_downlink)
{
	if ((g_at_func_table.get_qos_info == NULL) || (g_at_func_table.get_qos_info == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( ( max_uplink == NULL ) || ( max_downlink == NULL ) )
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_qos_info(max_uplink,max_downlink);
}

int at_set_qos_info(int max_uplink, int max_downlink)
{
	if ((g_at_func_table.set_qos_info == NULL) || (g_at_func_table.set_qos_info == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( ( max_uplink < 0 ) || ( max_downlink < 0 ) )
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	
	return g_at_func_table.set_qos_info(max_uplink,max_downlink);
}


int at_set_apn_addr(int cid, int attr, const char* apn)
{
	if ((g_at_func_table.set_apn_addr == NULL) || (g_at_func_table.set_apn_addr == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( ( cid < 0 ) || ( apn == NULL ) )
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	
	return g_at_func_table.set_apn_addr(cid,attr,apn);
}

int at_get_apn_addr(int cid, int* attr, char* apn)
{
	if ((g_at_func_table.get_apn_addr == NULL) || (g_at_func_table.get_apn_addr == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( ( cid < 0 ) || ( apn == NULL ) )
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	
	return g_at_func_table.get_apn_addr(cid,attr,apn);
}


int at_get_sms_unread(SMS_MSG_STAT_T* p_sms_msg_stat)
{
	if ((g_at_func_table.get_sms_unread == NULL) || (g_at_func_table.get_sms_unread == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( p_sms_msg_stat == NULL )
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	
	return g_at_func_table.get_sms_unread(p_sms_msg_stat);
}

int at_get_adc_main_pwr(int* main_pwr_volt)
{
	if ((g_at_func_table.get_adc_main_pwr == NULL) || (g_at_func_table.get_adc_main_pwr == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( main_pwr_volt == NULL )
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	
	return g_at_func_table.get_adc_main_pwr(main_pwr_volt);
}

int at_get_adc_internal_batt(int* internal_batt_volt)
{
	if ((g_at_func_table.get_adc_internal_batt == NULL) || (g_at_func_table.get_adc_internal_batt == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( internal_batt_volt == NULL )
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	
	return g_at_func_table.get_adc_internal_batt(internal_batt_volt);
}


int at_get_csq(int* csq)
{
	if ((g_at_func_table.get_csq == NULL) || (g_at_func_table.get_csq == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( csq == NULL )
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_csq(csq);
}

int at_get_modem_swver(char *buf, int buf_len)
{	
	if ((g_at_func_table.get_modem_swver == NULL) || (g_at_func_table.get_modem_swver == 0x00) )
	{
		ATLOGE("<atd> err [%s]: this cmd not support\r\n", __func__);
		return AT_RET_CMD_NOT_SUPPORT;
	}
	
	if ( buf == NULL ) 
	{
		ATLOGE("<atd> err [%s]: argument is not valid\r\n", __func__);
		return AT_RET_FAIL;
	}
	
	return g_at_func_table.get_modem_swver(buf, buf_len);
}






