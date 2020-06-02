#ifndef __AT_CMD_PARSER_3GPP__
#define __AT_CMD_PARSER_3GPP__

int noti_proc_3gpp(const char* buffer, int len);
int get_phonenum_3gpp(char *pnumber, int buf_len);;
int get_imei_3gpp(char *imei, int buf_len);
int send_sms_3gpp(const char* dest, const char* msg);
int get_rssi_3gpp(int *rssi);
int get_netstat_3gpp(AT_RET_NET_STAT *stat);
int get_modemtime_3gpp(time_t *out, int flag);
int set_modem_mode_3gpp(AT_MODEM_MODE op);
int get_modem_mode_3gpp(AT_MODEM_MODE* op);
int reset_modem_3gpp();
int at_channel_recovery_3gpp();
int get_3g_qulify_profile_3gpp(network_qos_info_t* qos_info);
int set_3g_qulify_profile_3gpp(network_qos_info_t qos_info);
int get_qos_info_3gpp(int* max_uplink, int* max_downlink);
int set_qos_info_3gpp(int max_uplink, int max_downlink);

int get_apn_info_3gpp(int cid, int* attr, char* apn);
int set_apn_info_3gpp(int cid, int attr, char* apn);

int get_sms_unread_3gpp(SMS_MSG_STAT_T* p_sms_msg_stat);

int get_csq_3gpp(int *csq);

int at_set_clear_all_sms();

#endif
