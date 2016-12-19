#pragma once

int init_watchdog();
int create_watchdog(char *id, int watchdog_reset_time);
int watchdog_keepalive_id(char *id);
int watchdog_delete_id(char *id);
void watchdog_all_delete();
void watchdog_destroy();