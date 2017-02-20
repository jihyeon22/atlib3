#ifndef _AT_UTIL_H_
#define _AT_UTIL_H_

#define SMD_CMD_DEV					"/dev/smd7"
#define SMD_LISTENER_DEV			"/dev/smd8"

#define AT_CTRL_CHAR(x) (#x[0]-'a'+1)

// define
#define AT_INVAILD_FD 		-1
#define AT_MAX_BUFF_SIZE 	1024
#define AT_MAX_WAIT_READ_SEC    1

#define AT_LEN_PHONENUM			11
#define AT_LEN_PHONENUM_BUFF	(AT_LEN_PHONENUM + 1)
#define AT_LEN_IMEI				15
#define AT_LEN_IMEI_BUFF		(AT_LEN_IMEI + 1)

// max 5 sec.. block func..
#define AT_MAX_OPEN_RETRY_CNT	10
// device list...
 
typedef enum {
	e_DEV_3GPP = 0,
	e_DEV_TX501_BASE = 2,
	e_DEV_TL500_BASE = 3,
	e_DEV_TX501S = e_DEV_TX501_BASE,
	e_DEV_TX501K = e_DEV_TX501_BASE,
	e_DEV_TX501L = e_DEV_TX501_BASE,
	e_DEV_TL500S = e_DEV_TL500_BASE,
	e_DEV_TL500K = e_DEV_TL500_BASE,
	e_DEV_TL500L = e_DEV_TL500_BASE,
	// TODO...
	e_DEV_MAX,
} AT_DEV_TYPE;

typedef enum {
	e_NONE_THREAD_MODE = 0,
	e_NO_NOTI_POLL_SMS = 1,
	//e_NOTI_SOCK_API = 1,
	// TODO...
	e_MAX_THREAD_TYPE,
} AT_THREAD_TYPE;


typedef struct network_qos_info network_qos_info_t ;
struct network_qos_info {
    int cid;
    int traffic_class;
    int max_bitrate_uplink;
    int max_bitrate_downlink;
    int granteed_bitrate_uplink;
    int granteed_bitrate_downlink;
    int delivery_order;
    int max_sdu_size;
    unsigned char sdu_err_ratio[8];
    unsigned char residual_bit_err_ratio[8];
    int delivery_of_err_sdu;
    int transfer_delay;
    int traffic_handing_priority;
};

// sms check dev
#define MAX_SMS_SAVE	20

typedef struct sms_msg_content
{
	char time_stamp[128];
	char msg_from[512];
	char msg_buff[1024];
}SMS_MSG_CONTENT_T;

typedef struct sms_msg_stat
{
	int total_unread_cnt;
	SMS_MSG_CONTENT_T msg[MAX_SMS_SAVE];
}SMS_MSG_STAT_T;


// -------------------------
typedef enum {
	AT_RET_NET_REGI_FAIL_CASE_1 	= 0,
	AT_RET_NET_REGI_SUCCESS 		= 1,
	AT_RET_NET_REGI_FAIL_CASE_2	= 2,
	AT_RET_NET_REGI_FAIL_DENINED	= 3,
	AT_RET_NET_REGI_FAIL_UNKNOWN	= 4,
	AT_RET_NET_REGI_FAIL_ROAMING	= 5
} AT_RET_NET_STAT;

typedef enum {
	MODEM_MODE_PWR_SAVE 	= 0,
	MODEM_MODE_ONLINE 	= 1,
	MODEM_MODE_NO_TX 		= 2,
	MODEM_MODE_NO_RX 		= 3,
	MODEM_MODE_NO_TX_RX 	= 4,
	MODEM_MODE_OFFLINE	= 4
} AT_MODEM_MODE;

typedef enum {
	AT_RET_FAIL = -1,
	AT_RET_SUCCESS = 0,
	AT_RET_CMD_NOT_SUPPORT = 1,
	AT_RET_CMD_NOT_INIT_MODEL = 2,
	AT_RET_FUNC_DATA_INIT = 3,
	AT_RET_STAT_NET_STAT_OK = 4,
	AT_RET_STAT_NET_STAT_NOK = 5,
} AT_RET_TYPE;

//jwrho ++
typedef enum {
	UNKNOWN_UART_PORT_USED = -1,
	UART1_PORT_USED = 1,
	UART2_PORT_USED = 2,
} UART_PORT_TYPE;
//jwrho --



// at cmd util...
int send_at_cmd(const char* cmd);
int send_at_cmd_singleline_resp(const char* cmd, const char* resp, char* ret_cmd, const int retry_cnt);
int send_at_cmd_numeric(const char* cmd, char* ret_cmd, const int retry_cnt);

// at open..
int at_open(AT_DEV_TYPE dev, void (*p_noti_proc)(const char* buffer, int len),  void (*p_sms_proc)(const char* phone_num, const char* recv_time, const char* msg), char *debug_file );
int at_close();
void at_read_flush();

//at main listener : **caution** general app dont use this function.
void at_listener_open();

// function wrapper
int at_func_init(AT_DEV_TYPE dev,void (*p_noti_proc)(const char* buffer, int len),  void (*p_sms_proc)(const char* phone_num, const char* recv_time, const char* msg) );
int at_noti_proc(const char* buffer, int len);
int at_get_phonenum(char *pnumber, int buf_len);
int at_get_imei(char *imei, int buf_len);
int at_send_sms(const char* dest, const char* msg);
int at_get_rssi(int* rssi);
int at_get_netstat(AT_RET_NET_STAT* stat);
int at_get_dbgmsg(unsigned char* result_buff, const int buff_size);
int at_get_modemtime(time_t *out, int flag);

int at_get_gps_ant(char *ant, int buf_len);
int at_set_gps_on(int type, int bootmode);
int at_set_gps_off(void);
int at_get_agps_addr(char *addr, int buf_len);
int at_set_agps_addr(const char *addr);

int at_set_modem_mode(AT_MODEM_MODE op);
int at_get_modem_mode(AT_MODEM_MODE* op);

int at_reset_modem(void);

int at_set_at_channel_recovery(void);
int at_get_modem_status(char *buf);

int at_set_alive(int time);

int at_get_3g_qulify_profile(network_qos_info_t* qos_info);
int at_set_3g_qulify_profile(network_qos_info_t qos_info);
int at_get_qos_info(int* max_uplink, int* max_downlink);
int at_set_qos_info(int max_uplink, int max_downlink);
int at_get_sms_unread(SMS_MSG_STAT_T* p_sms_msg_stat);

int at_get_adc_main_pwr(int* main_pwr_volt);	// suport only tl500
int at_get_adc_internal_batt(int* internal_batt_volt); // not support tl500 5x500

int at_get_csq(int* csq);	// use test mode

//jwrho ++
int get_used_uart_channel_3gpp(int *uart_ch_code);
int set_used_uart_channel_3gpp(UART_PORT_TYPE utype);
//jwrho --
#endif


