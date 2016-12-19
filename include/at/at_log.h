
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
void atlogd(const char *format, ...);

#define ATLOGD(msg...)	atlogd(msg)
#define ATLOGW(msg...)	atlogd(msg)	// warning  : brown
#define ATLOGI(msg...)	atlogd(msg)	// info  	: green
#define ATLOGE(msg...)	atlogd(msg)	// error 	: red
#define ATLOGT(msg...)	atlogd(msg)	// trace 	: brightmagenta


#endif
