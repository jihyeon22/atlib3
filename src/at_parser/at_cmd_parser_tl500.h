#ifndef __AT_CMD_PARSER_TL500__
#define __AT_CMD_PARSER_TL500__

int set_gps_on_tl500(AT_MODEM_MODE op);
int set_gps_off_tl500();
int get_gps_ant_tl500(char *ant, int buf_len);

int get_adc_main_pwr_tl500(int* main_pwr_volt);
#endif
