#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#include "at/at_log.h"
#include "at/watchdog.h"

#include <mdsapi/mds_api.h>

#define MAX_WATCHDOG_COUNT		30
#define MAX_WATCHDOG_ID_LENGTH	128

struct sw_watchdong_data {
	int enable;
	char id[MAX_WATCHDOG_ID_LENGTH];
	time_t watchdog_alive_time;
	int watchdog_reset_time;
};

struct sw_watchdong_info {
	int watchdog_client_count;
	struct sw_watchdong_data data[MAX_WATCHDOG_COUNT];
};

static struct sw_watchdong_info g_watchdog_mng;
static pthread_t g_hwatchdog_thread = 0;
static int g_watchdog_thread_running = 0;

void _watchdog_loop(void* arg)
{
	int watchdog_enable_count = 0;
	time_t current_timestamp;
	int i;
	arg = NULL;
	while(g_watchdog_thread_running) {
		current_timestamp = time(NULL);
		watchdog_enable_count = 0;
		for(i = 0; i < MAX_WATCHDOG_COUNT; i++) {
			if(g_watchdog_mng.data[i].enable == 0)
				continue;

			if ((current_timestamp -  g_watchdog_mng.data[i].watchdog_alive_time ) > 864000) //864000 means 10 days
				g_watchdog_mng.data[i].watchdog_alive_time = current_timestamp;

			watchdog_enable_count += 1;
			if(current_timestamp - g_watchdog_mng.data[i].watchdog_alive_time > g_watchdog_mng.data[i].watchdog_reset_time) {
				system("sync &");
				//*
				int power_off_cnt = 0;
				while(1) {
					ATLOGD("[%s] ID watchdog occured!!! power_off_cnt[%d]\n", g_watchdog_mng.data[i].id, power_off_cnt);
					// board api
					mds_api_poweroff();
					sleep(1);
					/*
					system("poweroff &");
					sleep(1);
					if(power_off_cnt++ > 10) {
						system("echo c > /proc/sysrq-trigger");
					}
					*/
				}//*/

				ATLOGD("[%s] ID watchdog occured!!!\n", g_watchdog_mng.data[i].id);
			}
			else {
				ATLOGD("[%s] ID watchdog status [%d/%d]\n", g_watchdog_mng.data[i].id, 
															(current_timestamp - g_watchdog_mng.data[i].watchdog_alive_time), 
															g_watchdog_mng.data[i].watchdog_reset_time);
			}

			
		}
		if(watchdog_enable_count < 0) {
			ATLOGD("watchdog keepalive!!!!\n");
		}

		sleep(5);
		
	}
	return;
}

int _watchdog_serch_id(char *id) 
{
	int i;

	for(i = 0; i < MAX_WATCHDOG_COUNT; i++) {
		if(g_watchdog_mng.data[i].enable == 0) {
			continue;
		}
		
		if(!strcmp(g_watchdog_mng.data[i].id, id))
			return i;
	}
	return -1;
}

int _watchdog_serch_empty_index() 
{
	int i;

	for(i = 0; i < MAX_WATCHDOG_COUNT; i++) {
		if(g_watchdog_mng.data[i].enable == 0) {
			return i;
		}
	}
	return -1;
}


int init_watchdog()
{
	if(g_watchdog_thread_running == 1)
		return -1;

	g_watchdog_thread_running = 1;
	memset(&g_watchdog_mng, 0x00, sizeof(struct sw_watchdong_info));

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 300000);
	
	if(pthread_create(&g_hwatchdog_thread, &attr, (void *)_watchdog_loop, NULL) != 0)
		return -2;

	return 0;
}

int create_watchdog(char *id, int watchdog_reset_time)
{	
	int index;
	if(_watchdog_serch_id(id) >= 0) {
		ATLOGE("err : [%s] id is already registered\r\n", id);
		return -1;
	}

	index = _watchdog_serch_empty_index();
	if(index < 0) {
		ATLOGE("err : watchdog max is registered\r\n");
		return -1;
	}

	ATLOGD("create id = [%d]\n", index);
	g_watchdog_mng.watchdog_client_count += 1;

	g_watchdog_mng.data[index].enable = 1;
	strcpy(g_watchdog_mng.data[index].id, id);
	g_watchdog_mng.data[index].watchdog_reset_time = watchdog_reset_time;
	g_watchdog_mng.data[index].watchdog_alive_time = time(NULL);

	return index;
}

int watchdog_keepalive_id(char *id) 
{
	int index = _watchdog_serch_id(id);
	if(index < 0) {
		ATLOGE("err : [%s] ID can't find\n", id);
		return -1;
	}
	if(g_watchdog_mng.data[index].enable != 1) {
		ATLOGE("err : [%s] ID watchdog isn't Activation\n", id);
		return -1;
	}

	g_watchdog_mng.data[index].watchdog_alive_time = time(NULL);
	return 0;
}

int watchdog_delete_id(char *id)
{
	int index = _watchdog_serch_id(id);
	if(index < 0) {
		ATLOGE("err : [%s] ID can't find\n", id);
		return -1;
	}
	g_watchdog_mng.data[index].enable = 0;
	g_watchdog_mng.watchdog_client_count -= 1;

	return 0;
}

void watchdog_all_delete()
{
	memset(&g_watchdog_mng, 0x00, sizeof(struct sw_watchdong_info));
}

void watchdog_destroy()
{
	memset(&g_watchdog_mng, 0x00, sizeof(struct sw_watchdong_info));
	g_watchdog_thread_running = 0;
}

/* Test Code
int main()
{
	int i;
	int ret;
	int id_aa;
	int id_bb;
	int id_cc;
	int id_dd;
	ret = init_watchdog();
	printf("init_watchdog ret [%d]\n", ret);

	create_watchdog("aa", 10);
	create_watchdog("bb", 15);
	create_watchdog("cc", 18);
	create_watchdog("dd", 20);

	printf("aa delete [%d]\n", watchdog_delete_id("aa"));
	printf("bb delete [%d]\n", watchdog_delete_id("bb"));
	printf("bb delete [%d]\n", watchdog_delete_id("xx"));

	create_watchdog("aa", 10);
	i = 0;
	while(1) {
		i += 1;
		if(i % 3 == 0) {
			watchdog_keepalive_id("aa");
		}
		
		if(i % 5 == 0) {
			watchdog_keepalive_id("bb");
		}

		if(i % 6 == 0) {
			watchdog_keepalive_id("cc");
		}

		if(i % 7 == 0) {
			watchdog_keepalive_id("dd");
		}

		if(i % 8 == 0) {
			watchdog_keepalive_id("ee");
		}
		sleep(1);

		create_watchdog("xx", 100);
	}
}
*/
