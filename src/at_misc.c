#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "at/at_util.h"
#include "at/at_log.h"

int at_strlen_without_cr(const char *s)
{
	int cnt = 0;

	while (*s)
	{
		//printf("strlen [%c]\r\n" ,*s);
		if ( ( *s != '\r' ) && ( *s != '\n' ) )
			cnt++;
		s++;
	}
	//printf("strlen count [%d]\r\n" ,cnt);
	return cnt;
}


int at_strlen_with_cr(const char *s)
{
	int cnt = 0;

	while (*s)
	{
		//printf("strlen [%c]\r\n" ,*s);
		s++;
		cnt++;
	}
	//printf("strlen count [%d]\r\n" ,cnt);
	return cnt;
}

int at_remove_cr(const char *s, char* target, int target_len)
{
	int cnt = 0;

	while (*s)
	{
		//printf("strlen [%c]\r\n" ,*s);
		if ( ( *s != '\r' ) && ( *s != '\n' ) )
		{
			target[cnt] = *s;
			cnt++;
			
			if (cnt > target_len)
				return -1;
		}
		s++;
	}
	//printf("strlen count [%d]\r\n" ,cnt);
	return cnt;
}


int at_get_number(char *target, char *src)
{
	int cnt = 0;

	while (*src)
	{
		//printf("strlen [%c]\r\n" ,*s);
		if ( !(*src < '0' || *src > '9') )
		{
			target[cnt] = *src;
			cnt++;
		}
		src++;
	}
	//printf("strlen count [%d]\r\n" ,cnt);
	return cnt;
}


int at_remove_char(const char *s, char* target, int target_len, char targ_char)
{
	int cnt = 0;

	while (*s)
	{
		//printf("strlen [%c]\r\n" ,*s);
		if ( ( *s != targ_char ) )
		{
			target[cnt] = *s;
			cnt++;
			
			if (cnt > target_len)
				return -1;
		}
		s++;
	}
	//printf("strlen count [%d]\r\n" ,cnt);
	return cnt;
}


