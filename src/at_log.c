


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <logd_rpc.h>

#include <at/at_log.h>

extern FILE *g_debug_file;
void atlogd(AT_LOG_TYPE type, const char *format, ...)
{
	char tmp[1024] = {0};
	
	va_list va;
	va_start(va, format);
	vsprintf(tmp, format, va);
	va_end(va);
	
	if(g_debug_file != NULL) {
		fprintf(g_debug_file, tmp);
		fflush(g_debug_file);
	}

	switch (type)
	{
		case e_AT_LOG_LEVEL_D :
		{
			LOGD(eSVC_AT, tmp);
			break;
		}
		case e_AT_LOG_LEVEL_W :
		{
			LOGW(eSVC_AT, tmp);
			break;
		}
		case e_AT_LOG_LEVEL_I :
		{
			LOGI(eSVC_AT, tmp);
			break;
		}
		case e_AT_LOG_LEVEL_E :
		{
			LOGE(eSVC_AT, tmp);
			break;
		}
		case e_AT_LOG_LEVEL_T :
		{
			LOGT(eSVC_AT, tmp);
			break;
		}
		default : 
		{
			LOGD(eSVC_AT, tmp);
			break;
		}
	}

	
}