#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>
#include <time.h>

#include "at/at_log.h"
#include "at/at_util.h"
#include "at_cmd_table_wrapper.h"
#include "at_misc.h"


// ----------------------------------------------------------
// at cmd parsing util
// ----------------------------------------------------------


static int _at_trans_format_phonenum(char **pn, char *buf)
{
    int i = 0;
    char *start = NULL;

    for(i = 0; buf[i] != 0 ; i++)
    {
        if(buf[i] < '0' || buf[i] > '9')
        {
            if(start != NULL)
            {
                buf[i] = 0;
            }
        }
        else
        {
            if(start == NULL)
            {
                start = &(buf[i]);
            }
        }
    }

    if(start == NULL)
    {
        return -1;
    }

    *pn = start;

    return 0;
}

int at_parse_sms_from_cmgr(char* buff, char* phone_num, char* time, char* msg)
{
    int i = 0;

    int argc = 0;
    char *argv[4] = {0};
    int len = 0;
    char *base = 0;

    char sms_trans_buffer[AT_MAX_BUFF_SIZE] = {0};
	char sms_msg[AT_MAX_BUFF_SIZE];
    char *pEnd = 0;

    pEnd = strstr(buff + 2, "$$");
    if(pEnd == NULL)
    {
        pEnd = strstr(buff + 2, "*S");
    }
    if(pEnd == NULL)
    {
        pEnd = buff + strlen(buff);
    }
    if(pEnd == NULL)
    {
        ATLOGE( "_parse_sms error (can't find carrage return symbol)\n");
        return -1;
    }

    memcpy(sms_trans_buffer, buff, pEnd - buff);

    ATLOGI( "\n\n RCV SMS RETURN  [%.180s]\n", sms_trans_buffer);

    len = strnlen(sms_trans_buffer, sizeof(sms_trans_buffer));
    
    base = sms_trans_buffer;
    while(argc <= 3 && len--) {
        switch(*base) {
            case ',':
                if(argc == 3)
                {
                    break;
                }
                *base = '\0';
                argv[argc] = base + 1;
                argc++;
                break;
            case 0x0a:
                *base = '\0';
                argv[argc] = base + 1;
                argc++;
                break;
            default:
                break;
        }
        base++;
    }
    argc--;

    for(i = 0; i <= argc; i++)
    {
        ATLOGD( "%d %s\n", i, argv[i]);
    }

    if(argc < 3)
    {
        ATLOGE( "Not enough SMS parameter\n");
        return -1;
    }
    if(argc < 3)
    {
        ATLOGE( "Not enough SMS parameter\n");
        return -1;
    }

    _at_trans_format_phonenum(&argv[0] , argv[0]);

    strcpy(phone_num, argv[0]);
    strcpy(time, argv[2]);

	strcpy(sms_msg, argv[3]);
	if(sms_msg[0] == 0x0a) {
	    strcpy(msg, &sms_msg[1]);
	}
	else {
		strcpy(msg, sms_msg);
	}
   
    // 0 : phone num
    // 2 : time
    // 3 : msg
    /*
    if(!strncmp(argv[3], "&MDS", 4)) {
        LOGD(LOG_TARGET, "MDS SMS!");
        return _parse_mds_sms(argv[2], argv[0], argv[3]);
    }
    else if(!strncmp(argv[3], "@", 1) || !strncmp(argv[3], "#", 1)) {
        LOGD(LOG_TARGET, "DM SMS!");
        return dm_sms_process(argv[2], argv[0], argv[3]);
    }
    else
    {
        LOGD(LOG_TARGET, "CALL MODEL SMS!");
        return parse_model_sms(argv[2], argv[0], argv[3]);
    }
*/
    return 0;
}


int at_get_sms_idx_from_cmti(const char* cmd, int* idx)
{
    char *tr;
    char token_0[ ] = ",";
    char token_1[ ] = "\r\n";
    char *temp_bp = NULL;
    
    char *p_cmd = NULL;
    char buffer[AT_MAX_BUFF_SIZE] = {0,};
    
    memset(buffer, 0x00, sizeof(buffer));
    strcpy(buffer, cmd);
    
    p_cmd = strstr(buffer, "+CMTI:");
    
    if ( p_cmd == NULL)
        return AT_RET_FAIL;
    
    tr = strtok_r(p_cmd, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
    
    tr = strtok_r(NULL, token_1, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
    
    *idx = atoi(tr);
    return AT_RET_SUCCESS;
}

// +CNUM: "Hello world","+821221273182",145

int at_get_phonenum_cnum(const char* cmd, char* phone_num)
{
    char *tr;
    char token_0[ ] = ",";
    //char token_1[ ] = "\r\n";
    char *temp_bp = NULL;
    
    char *p_cmd = NULL;
    char buffer[AT_MAX_BUFF_SIZE] = {0,};
    char tmp[AT_MAX_BUFF_SIZE] = {0,};
    
    memset(buffer, 0x00, sizeof(buffer));
    strcpy(buffer, cmd);
    
    p_cmd = strstr(buffer, "+CNUM:");
    
    if ( p_cmd == NULL)
        return AT_RET_FAIL;
    
    tr = strtok_r(p_cmd, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
    
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
    
    at_get_number(tmp, tr);

    //national code change into '0'.
    if (strncmp(tmp,"82",2) == 0)
    {
        sprintf(phone_num, "0%s",tmp+2);
    }
    else
    {
        strcpy(phone_num,tmp);
    }
    
    return AT_RET_SUCCESS;
}

// +CSQ: 7,99
int at_get_rssi_from_csq(const char* cmd, int* rssi)
{
    int csq_rssi[]={125, 109, 107, 105, 103, 101, 
                    99, 97, 95, 93, 91, 89, 87, 
                    85, 83, 81, 79, 77, 75, 73, 
                    71, 69, 67, 65, 63, 61, 59, 
                    57, 55, 53};
    
    char *tr;
    char token_0[ ] = ":,";
    //char token_1[ ] = "\r\n";
    char *temp_bp = NULL;
    
    char *p_cmd = NULL;
    char buffer[AT_MAX_BUFF_SIZE] = {0,};
    char tmp[AT_MAX_BUFF_SIZE] = {0,};
    
    int tmp_rssi = -1;
    
    memset(buffer, 0x00, sizeof(buffer));
    strcpy(buffer, cmd);
    
    p_cmd = strstr(buffer, "+CSQ:");
    
    if ( p_cmd == NULL)
        return AT_RET_FAIL;
    
    tr = strtok_r(p_cmd, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
    
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
       
	printf(" tr is [%s]\r\n", tr);
	
    at_get_number(tmp, tr);

    tmp_rssi = atoi(tmp);
    
    /*
        0     113 dBm or less 
        1     111 dBm   
        2~30  109~52dBm (109~108dBm= rssi 2, 107~106dBm= rssi 3 ,,, 55~54dBm= rssi 29 
                         53~52dBm= rssi 30) 
        31    51dBm or greater   
        99    unknown or not detectable 
    */
    // result change into rssi value.
	printf(" tmp_rssi is [%d]\r\n", tmp_rssi);
	
    if ( tmp_rssi == 0 )
        *rssi = 113;
    else if ( tmp_rssi == 1 )
        *rssi = 111;
    else if ( tmp_rssi == 31)
        *rssi = 51;
    else if ( tmp_rssi == 99)
        *rssi = 0;
    else 
        *rssi = csq_rssi[atoi(tmp)];
    
    return AT_RET_SUCCESS;
}

// +CREG: 0,1
int at_get_netstat_from_creg(const char* cmd, int* netstat)
{
    char *tr;
    char token_0[ ] = ",";
    char token_1[ ] = "\r\n";
    char *temp_bp = NULL;
    
    char *p_cmd = NULL;
    char buffer[AT_MAX_BUFF_SIZE] = {0,};
    char tmp[AT_MAX_BUFF_SIZE] = {0,};
    
    memset(buffer, 0x00, sizeof(buffer));
    strcpy(buffer, cmd);
    
    p_cmd = strstr(buffer, "+CREG:");
    
    if ( p_cmd == NULL)
        return AT_RET_FAIL;
    
    tr = strtok_r(p_cmd, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
    
    tr = strtok_r(NULL, token_1, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
    
    at_get_number(tmp, tr);

    // result change into stat.
    /*
    0 disable network registration unsolicited result code
    1 enable network registration unsolicited result code +CGREG: 
    2 enable network registration and location information unsolicited result code +CGREG:

    0 not registered, ME is not currently searching a new operator to register to
    1 registered, home network
    2 not registered, but ME is currently searching a new operator to register to
    3 registration denied
    4 unknown
    5 registered, roaming
    */
    *netstat = atoi(tmp);
    
    return AT_RET_SUCCESS;
}

// +CCLK: "16/03/16,14:36:51"
int at_get_time_from_cclk(const char* cmd, char* time_str)
{
    char *tr;
    char token_0[ ] = "\"";
    //char token_1[ ] = "\r\n";
    char *temp_bp = NULL;
    
    char *p_cmd = NULL;
    char buffer[AT_MAX_BUFF_SIZE] = {0,};
    //char tmp[AT_MAX_BUFF_SIZE] = {0,};
    
    memset(buffer, 0x00, sizeof(buffer));
    strcpy(buffer, cmd);
    
    p_cmd = strstr(buffer, "+CCLK:");
    
    if ( p_cmd == NULL)
        return AT_RET_FAIL;
    
    tr = strtok_r(p_cmd, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
    
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
    
    //at_get_number(tmp, tr);

    strcpy(time_str, tr);
    
    
    return AT_RET_SUCCESS;
}

int at_get_mode_from_cfun(const char* cmd, int* mode)
{
    char *tr;
    char token_0[ ] = ":";
    char token_1[ ] = "\r\n";
    char *temp_bp = NULL;
    
    char *p_cmd = NULL;
    char buffer[AT_MAX_BUFF_SIZE] = {0,};
    char tmp[AT_MAX_BUFF_SIZE] = {0,};
    
    memset(buffer, 0x00, sizeof(buffer));
    strcpy(buffer, cmd);
    
    p_cmd = strstr(buffer, "+CFUN:");
    
    if ( p_cmd == NULL)
        return AT_RET_FAIL;
        
    tr = strtok_r(p_cmd, token_0, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;
    
    tr = strtok_r(NULL, token_1, &temp_bp);
    if(tr == NULL) return AT_RET_FAIL;

    at_get_number(tmp, tr);
    
    // result chnage into mode.
    *mode = atoi(tmp);
    
    return AT_RET_SUCCESS;
}

// +CGEQREQ: 1,2,8,3648,0,0,0,1500,"1E4","1E5",3,0,0
int at_get_mode_from_cgeqreq(const char* cmd, network_qos_info_t* qos_info)
{
    char *tr;
    char token_0[ ] = ":,";
    char token_1[ ] = "\r\n";
    char *temp_bp = NULL;
    
    char *p_cmd = NULL;
    char buffer[AT_MAX_BUFF_SIZE] = {0,};
    //char tmp[AT_MAX_BUFF_SIZE] = {0,};
    
    memset(buffer, 0x00, sizeof(buffer));
    strcpy(buffer, cmd);
    
    p_cmd = strstr(buffer, "+CGEQREQ:");
    
    if ( p_cmd == NULL)
        return AT_RET_FAIL;
    
    tr = strtok_r(p_cmd, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    
    // ------------------- cid ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("1. cid = [%s]\n", tr);
    qos_info->cid = atoi(tr);
    
    // -------------------  traffic class ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("2. traffic class = [%s]\n", tr);
    qos_info->traffic_class = atoi(tr);
    
    // -------------------  max bitrate up link ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("3. max bitrate up link = [%s]\n", tr);
    qos_info->max_bitrate_uplink = atoi(tr);
    
    // -------------------  max bitrate down link ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("4. max bitrate down link = [%s]\n", tr);
    qos_info->max_bitrate_downlink = atoi(tr);
    
    // -------------------  guaranteed bitrate up link ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("5. guaranteed bitrate up link = [%s]\n", tr);
    qos_info->granteed_bitrate_uplink = atoi(tr);
    
    // -------------------  guaranteed bitrate down link ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("6. guaranteed bitrate down link = [%s]\n", tr);
    qos_info->granteed_bitrate_downlink = atoi(tr);
    
    // -------------------  delivery order ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("7. delivery order = [%s]\n", tr);
    qos_info->delivery_order = atoi(tr);
    
    // -------------------  max sdu size ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("8. max sdu size = [%s]\n", tr);
    qos_info->max_sdu_size = atoi(tr);
    
    // -------------------  sdu error ratio ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("9. sdu error ratio = [%s]\n", tr);
    memset(qos_info->sdu_err_ratio, 0x00, 8);
    strcpy((char *)qos_info->sdu_err_ratio, tr);
    
    // -------------------  residual bit error ratio  ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("10. residual bit error ratio = [%s]\n", tr);
    memset(qos_info->residual_bit_err_ratio, 0x00, 8);
    strcpy((char *)qos_info->residual_bit_err_ratio, tr);
    
    // ------------------- delivery of errorneous sdu ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("11. delivery of errorneous sdu = [%s]\n", tr);
    qos_info->delivery_of_err_sdu = atoi(tr);
    
    // ------------------- transfer delay ------------------------------
    tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("12. transfer delay = [%s]\n", tr);
    qos_info->transfer_delay = atoi(tr);
    
    // ------------------- transfer delay ------------------------------
    tr = strtok_r(NULL, token_1, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
    //printf("13. traffic handing priority = [%s]\n", tr);
    qos_info->traffic_handing_priority = atoi(tr); 
    
    return AT_RET_SUCCESS;
}

static int _cmgl_to_sms_msg_stat(const char* cmd, SMS_MSG_CONTENT_T* content)
{
	// +CMGL: 11,"REC UNREAD","01086687577",,"16/11/08,10:29:39+36"
	// msg..
	// ?
	char *tr;
    char token_0[ ] = ":,";
    char token_1[ ] = "\r\n";
    char *temp_bp = NULL;
    
    char *p_cmd = NULL;
    char buffer[AT_MAX_BUFF_SIZE] = {0,};
    char tmp[AT_MAX_BUFF_SIZE] = {0,};
    
    memset(buffer, 0x00, sizeof(buffer));
    strcpy(buffer, cmd);
    
    p_cmd = strstr(buffer, "+CMGL:");
    
    if ( p_cmd == NULL)
        return AT_RET_FAIL;
    
	// cmgl cmd..
    tr = strtok_r(p_cmd, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
	
	// msg index.
	tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
	
	// msg mark
	tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
	
	// msg from number
	tr = strtok_r(NULL, token_0, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }

	memset(tmp, 0x00, sizeof(tmp));
	at_get_number(tmp, tr);
	strcpy(content->msg_from, tmp);
	
	// time stamp
	tr = strtok_r(NULL, token_1, &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }
	
	memset(tmp, 0x00, sizeof(tmp));
	at_get_number(tmp, tr);
	strncpy(content->time_stamp, tmp, strlen(tmp)-2);
	
	// msg
	tr = strtok_r(NULL, "", &temp_bp);
    if(tr == NULL) {
        return AT_RET_FAIL;
    }

	if(tr[0] == 0x0a)
		strcpy(content->msg_buff, &tr[1]);
	else
		strcpy(content->msg_buff, tr);

	 return AT_RET_SUCCESS;
	
}

int at_get_unread_sms_from_cmgl(const char* cmd, SMS_MSG_STAT_T* p_sms_msg_stat)
{
	int i = 0;
    char *p_cmd = NULL;
	char *tmp_cmd = NULL;
	
    char buffer[AT_MAX_BUFF_SIZE*2] = {0,};
	char buffer2[AT_MAX_BUFF_SIZE] = {0,};
    //char tmp[AT_MAX_BUFF_SIZE] = {0,};
    
	int total_unread_cnt = 0 ;
	
	int total_cmd_len = 0;
	int msg_info[MAX_SMS_SAVE][1] = { 0, };
	
	//first, data of 'CMGL' cut. next to copy again.
	tmp_cmd = strstr(cmd, "+CMGL: ");
	if ( tmp_cmd == NULL )
		return AT_RET_FAIL;
	total_cmd_len = strlen(tmp_cmd) - strlen("\r\n\r\nOK");
    memset(buffer, 0x00, sizeof(buffer));
    strncpy(buffer, tmp_cmd, total_cmd_len);
	
	p_cmd = buffer;
	
	//get information to parse all contents of CMGL.
    while ( (tmp_cmd = strstr(p_cmd, "+CMGL: ")) != NULL )
	{
		msg_info[total_unread_cnt][0] = total_cmd_len - strlen(tmp_cmd);
		p_cmd = tmp_cmd;
		p_cmd += strlen("+CMGL: ");
		total_unread_cnt++;
	}
	
	for ( i = 0 ; i < total_unread_cnt ; i++ )
	{
		int todo_copy_cnt = 0;
		SMS_MSG_CONTENT_T sms_content;
		memset(&sms_content, 0x00, sizeof(SMS_MSG_CONTENT_T));
		
		memset(buffer2, 0x00, sizeof(buffer2));
		
		if ( msg_info[i+1][0] > 0 )
			todo_copy_cnt = strlen(buffer + msg_info[i][0]) - strlen(buffer + msg_info[i+1][0]);
		else
			todo_copy_cnt = strlen(buffer + msg_info[i][0]);
		
		strncpy(buffer2, buffer + msg_info[i][0], todo_copy_cnt);
		_cmgl_to_sms_msg_stat(buffer2, &sms_content);
		
		p_sms_msg_stat->total_unread_cnt = total_unread_cnt;
		memcpy(&p_sms_msg_stat->msg[i], &sms_content, sizeof(SMS_MSG_CONTENT_T));
		
	}
	
	//printf(" total_unread_cnt is [%d]\r\n", total_unread_cnt);
	
	return AT_RET_SUCCESS;
}