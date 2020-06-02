
#ifndef AT_LOG_H
#define AT_LOG_H 1


//#define LOGD(svc,msg...)	logd(svc, eDebug, msg)	// debug 	: gray
// workaround!! 
/*
#define ATLOGD(svc,msg...)	printf(msg)
#define ATLOGW(svc,msg...)	logd(svc,eWarning, msg)	// warning  : brown
#define ATLOGI(svc,msg...)	logd(svc,eInfo, msg)	// info  	: green
#define ATLOGE(svc,msg...)	logd(svc,eError, msg)	// error 	: red
#define ATLOGT(svc,msg...)	logd(svc,eTrace, msg)	// trace 	: brightmagenta
*/

typedef enum {
	e_AT_LOG_LEVEL_D = 0,
	e_AT_LOG_LEVEL_W,
    e_AT_LOG_LEVEL_I,
    e_AT_LOG_LEVEL_E,
    e_AT_LOG_LEVEL_T,
    e_AT_LOG_LEVEL_MAX,
} AT_LOG_TYPE;

void atlogd(AT_LOG_TYPE type, const char *format, ...);

#define ATLOGD(msg...)	atlogd(e_AT_LOG_LEVEL_D, msg)
#define ATLOGW(msg...)	atlogd(e_AT_LOG_LEVEL_W, msg)	// warning  : brown
#define ATLOGI(msg...)	atlogd(e_AT_LOG_LEVEL_I, msg)	// info  	: green
#define ATLOGE(msg...)	atlogd(e_AT_LOG_LEVEL_E, msg)	// error 	: red
#define ATLOGT(msg...)	atlogd(e_AT_LOG_LEVEL_T, msg)	// trace 	: brightmagenta

#endif
