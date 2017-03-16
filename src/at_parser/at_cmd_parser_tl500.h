#ifndef __AT_CMD_PARSER_TL500__
#define __AT_CMD_PARSER_TL500__

#include <at/at_util.h>
int set_gps_on_tl500(AT_GPS_ON_TYPE type, AT_GPS_BOOT_TYPE bootmode);
int set_gps_off_tl500();
int get_gps_ant_tl500(char *ant, int buf_len);

int get_adc_main_pwr_tl500(int* main_pwr_volt);
#endif
