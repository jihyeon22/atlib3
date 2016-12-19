#ifndef _AT_MISC_H_
#define _AT_MISC_H_

int at_strlen_without_cr(const char *s);
int at_strlen_with_cr(const char *s);
int at_get_number(char *targ, char *src);
int at_remove_cr(const char *s, char* target, int target_len);
int at_remove_char(const char *s, char* target, int target_len, char targ_char);

// at cmd parsing util
int at_get_sms_idx_from_cmti(const char* cmd, int* idx);
int at_parse_sms_from_cmgr(char* buff, char* phone_num, char* time, char* msg);
int at_get_phonenum_cnum(const char* cmd, char* phone_num);

#endif

