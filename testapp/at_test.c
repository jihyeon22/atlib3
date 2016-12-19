#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h> 

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <termios.h>
#include "at/at_util.h"
#include "at/at_log.h"

// ---------------------------------------------

// -------------------------------------
void sigint_handler( int signo)
{
	printf("signo = [%d]\n", signo);
	if(signo == 2) //ctrl+c
	   exit(0);
}	
	
void noti_proc(const char* buffer, int len)
{
	printf("test app noti proc +++++++++++++++++++++++++++\r\n");
	printf("%s : [%s](%d)\r\n", __func__, buffer, len);
	printf("test app noti proc ---------------------------\r\n");	
}

void sms_proc(const char* phone_num, const char* recv_time, const char* msg)
{
	printf("test app sms proc +++++++++++++++++++++++++++\r\n");
	printf("%s :  phone_num [%s]\r\n", __func__,  phone_num);	
	printf("%s : recv_time [%s]\r\n", __func__,  recv_time);	
	printf("%s : msg [%s]\r\n", __func__,  msg);	
	printf("test app sms proc ---------------------------\r\n");
	//_parse_sms(buffer);
}

void main(int argc, char *argv[])
{
	int ret;
	signal( SIGINT, sigint_handler);

	if(argc < 2) {
		printf("usage...\n");
		printf("-s : server create\n");
		printf("-c : client create\n");
		return 0;


	}
	printf("argc = [%d]\n", argc);
	
	if(!strcmp(argv[1], "-s")) {
		printf("at_listener_open call()\n");
		at_listener_open();
	}
	else {
		printf("at_open call()\n");
		ret = at_open(e_DEV_TX500, noti_proc, sms_proc, "console");
		printf("at_open ret[%d]\n", ret);
	}

	#if 0
	// get phone number
	{
		char phone_num[32] = {0,};
		
		printf("\r\n[ phone number ] ++++++++++++++++++++++++++++++++\r\n");
		if ( at_get_phonenum(phone_num, sizeof(phone_num)) == AT_RET_SUCCESS )
			printf("test app => phone_num is [%s]\r\n",phone_num);
		printf("\r\n[ phone number ] --------------------------------\r\n");
	}
	
	/*
	{
		if ( at_send_sms("01086687577","test msg") == AT_RET_SUCCESS )
			printf("test app => sms send success \r\n");
	}
	*/
	
	// get imei test
	{
		char phone_imei[32] = {0,};
		
		printf("\r\n[ phone imei ] ++++++++++++++++++++++++++++++++\r\n");
		if ( at_get_imei(phone_imei, sizeof(phone_imei) ) == AT_RET_SUCCESS )
			printf("test app => phone imei is [%s]\r\n",phone_imei);
		printf("\r\n[ phone imei ] --------------------------------\r\n");
	}
	
	
	// rssi test
	{
		int rssi = 0;
		
		printf("\r\n[ phone rssi ] ++++++++++++++++++++++++++++++++\r\n");
		if ( at_get_rssi(&rssi) == AT_RET_SUCCESS )
			printf("test app => rssi is [%d]\r\n", rssi);
		printf("\r\n[ phone rssi ] --------------------------------\r\n");
	}
	
	
	// netstat test
	{
		AT_RET_NET_STAT netstat = 0;
		
		printf("\r\n[ net stat ] ++++++++++++++++++++++++++++++++\r\n");
		if ( at_get_netstat(&netstat) == AT_RET_SUCCESS )
		{
			if ( netstat == AT_RET_NET_REGI_SUCCESS)
				printf("test app => netstat is OK\r\n");
			else
				printf("test app => netstat is not OK\r\n");
		}
		printf("\r\n[ net stat ] ---------------------------------\r\n");
	}
	
	
	// get modem time
	{
		time_t modem_time = {0,};
		
		printf("\r\n[ get modem time ] ++++++++++++++++++++++++++++++++\r\n");
		//if ( at_get_modemtime(&modem_time, 1) == AT_RET_SUCCESS ) // set linux modem time to linux time
		if ( at_get_modemtime(&modem_time, 0) == AT_RET_SUCCESS ) // not set set linux modem time
		{
			printf ( "test app => Current modem time : [%s]", ctime (&modem_time) );
		}
		printf("\r\n[ get modem time ] --------------------------------\r\n");
	}

	
	{
		AT_MODEM_MODE modem_mode = 0;
		
		printf("\r\n[ get modem mode ] ++++++++++++++++++++++++++++++++\r\n");
		if ( at_get_modem_mode(&modem_mode)  == AT_RET_SUCCESS )
		{
			printf ( "test app => Current modem mode : [%d]", modem_mode );
		} 
		printf("\r\n[ get modem mode ] --------------------------------\r\n");
		
	}
	
	{
		network_qos_info_t qos_info_tmp = {0,};
		
		printf("\r\n[ get modem qos info 1 ] ++++++++++++++++++++++++++++++++++++++\r\n");
		if ( at_get_3g_qulify_profile(&qos_info_tmp) == AT_RET_SUCCESS )
		{
			printf("test app ==> qos_info_tmp.cid is [%d]\r\n", qos_info_tmp.cid);
			printf("test app ==> qos_info_tmp.traffic_class is [%d]\r\n", qos_info_tmp.traffic_class);
			printf("test app ==> qos_info_tmp.max_bitrate_uplink is [%d]\r\n", qos_info_tmp.max_bitrate_uplink);
			printf("test app ==> qos_info_tmp.max_bitrate_downlink is [%d]\r\n", qos_info_tmp.max_bitrate_downlink);
			printf("test app ==> qos_info_tmp.granteed_bitrate_uplink is [%d]\r\n", qos_info_tmp.granteed_bitrate_uplink);
			printf("test app ==> qos_info_tmp.granteed_bitrate_downlink is [%d]\r\n", qos_info_tmp.granteed_bitrate_downlink);
			printf("test app ==> qos_info_tmp.delivery_order is [%d]\r\n", qos_info_tmp.delivery_order);
			printf("test app ==> qos_info_tmp.max_sdu_size is [%d]\r\n", qos_info_tmp.max_sdu_size);
			printf("test app ==> qos_info_tmp.sdu_err_ratio is [%s]\r\n", qos_info_tmp.sdu_err_ratio);
			printf("test app ==> qos_info_tmp.residual_bit_err_ratio is [%s]\r\n", qos_info_tmp.residual_bit_err_ratio);
			printf("test app ==> qos_info_tmp.delivery_of_err_sdu is [%d]\r\n", qos_info_tmp.delivery_of_err_sdu);
			printf("test app ==> qos_info_tmp.transfer_delay is [%d]\r\n", qos_info_tmp.transfer_delay);
			printf("test app ==> qos_info_tmp.traffic_handing_priority is [%d]\r\n", qos_info_tmp.traffic_handing_priority);
		}
		printf("\r\n[ get modem qos info 1 ] --------------------------------\r\n");
	}
	
	{
		int uplink, downlink;
		printf("\r\n[ get modem qos info 2 ] ++++++++++++++++++++++++++++++++++++++\r\n");
		if ( at_get_qos_info(&uplink, &downlink) == AT_RET_SUCCESS )
		{
			printf("test app ==> uplink is [%d]\r\n",uplink);
			printf("test app ==> downlink is [%d]\r\n",downlink);
		}
		printf("\r\n[ get modem qos info 2 ] ---------------------------------------\r\n");
		
		
	}
	
	
	
	at_send_sms("01086687577", "hello world");
	
	#endif
	
	/*
	{
		int i = 0;
		SMS_MSG_STAT_T sms_mgs_stat = {0,};
		at_get_sms_unread(&sms_mgs_stat);
		for ( i = 0 ; i < sms_mgs_stat.total_unread_cnt ; i++)
		{
			printf("sms_mgs_stat.total_unread_cnt [%d]\r\n",sms_mgs_stat.total_unread_cnt);
			printf("sms_mgs_stat.msg[%d].time_stamp => [%s]\r\n",i, sms_mgs_stat.msg[i].time_stamp);
			printf("sms_mgs_stat.msg[%d].msg_from => [%s]\r\n",i, sms_mgs_stat.msg[i].msg_from);
			printf("sms_mgs_stat.msg[%d].msg_buff => [%s]\r\n",i, sms_mgs_stat.msg[i].msg_buff);
		}
	}
	*/
	
	while(1)
	{
		printf(".\r\n");
		sleep(1);
	}

	return 0;
}
