
#ifndef __AT_CMD_PARSER_UTIL_3GPP__
#define __AT_CMD_PARSER_UTIL_3GPP__

int at_get_sms_idx_from_cmti(const char* cmd, int* idx);
int at_parse_sms_from_cmgr(char* buff, char* phone_num, char* time, char* msg);
int at_get_phonenum_cnum(const char* cmd, char* phone_num);
int at_get_rssi_from_csq(const char* cmd, int* rssi);
int at_get_csq_from_csq(const char* cmd, int* csq);
int at_get_netstat_from_creg(const char* cmd, int* netstat);
int at_get_time_from_cclk(const char* cmd, char* time_str);
int at_get_mode_from_cfun(const char* cmd, int* mode);
int at_get_unread_sms_from_cmgl(const char* cmd, SMS_MSG_STAT_T* p_sms_msg_stat);
int at_get_mode_from_cgeqreq(const char* cmd, network_qos_info_t* qos_info);


#endif

