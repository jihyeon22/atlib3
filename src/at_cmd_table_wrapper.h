#ifndef _AT_CMD_TABLE_H_
#define _AT_CMD_TABLE_H_

#include "at/at_util.h"

extern void  (*g_model_proc_noti)(const char* buffer, int len);
extern void (*g_model_proc_sms_read)(const char* phone_num, const char* recv_time, const char* msg);

typedef struct AT_FUNC
{
	int (*noti_proc)(const char* buffer, int len);
	
	// modem info
	int (*get_phonenum)(char *pnumber, int buf_len);
	int (*get_imei)(char *imei, int buf_len);
	
	// sms
	int (*send_sms)(const char* dest, const char* msg);

	// modem status
	int (*get_rssi)(int *rssi);
	int (*get_netstat)(AT_RET_NET_STAT* stat);
	int (*get_dbgmsg)(unsigned char* result_buff, const int buff_size);
	int (*get_modemtime)(time_t *out, int flag);
	
	// gps function
	int  (*get_gps_ant)(char *ant, int buf_len);
	int (*set_gps_on)(int type, int bootmode);
	int (*set_gps_off)(void);
	int  (*get_agps_addr)(char *addr, int buf_len);
	int  (*set_agps_addr)(const char *addr);
	
	// modem mode
	int  (*set_modem_mode)(AT_MODEM_MODE op);
	int  (*get_modem_mode)(AT_MODEM_MODE* op);
	int  (*reset_modem)(void);
	int  (*set_at_channel_recovery)(void);
	int  (*get_modem_status)(char *buf);
	int (*set_alive)(int time);
	int  (*get_3g_qulify_profile)(network_qos_info_t* qos_info);
	int  (*set_3g_qulify_profile)(network_qos_info_t qos_info);
	int (*get_qos_info)(int* max_uplink, int* max_downlink);
	int (*set_qos_info)(int max_uplink, int max_downlink);

	int (*get_sms_unread)(SMS_MSG_STAT_T* p_sms_msg_stat);
}AT_FUNC_T;


#endif





