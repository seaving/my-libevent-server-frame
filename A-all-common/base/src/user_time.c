#include "user_time.h"
#include "sys_inc.h"
#include "log_trace.h"

static long _start_sec = 0;

void time_init()
{
	struct timespec start_time;
	clock_gettime(CLOCK_MONOTONIC, &start_time);
	_start_sec = (start_time.tv_sec + (start_time.tv_nsec / (1000 * 1000 * 1000)));
}

unsigned int get_system_sec(void)
{
	struct timespec now;
	long sec = 0;
	long current = 0;

	clock_gettime(CLOCK_MONOTONIC, &now);
	current = (now.tv_sec + (now.tv_nsec / (1000 * 1000 * 1000)));
	sec = current - _start_sec;
	return (unsigned int)sec;
}

unsigned long GetTimestrip_us()
{
	struct timeval tCurr;
	gettimeofday(&tCurr, NULL);
	return (1000000L * tCurr.tv_sec + tCurr.tv_usec);
}

void GetSystemTime(int *Year, int *Mon, int *Data, int *Hour, int *Min, int *Sec)
{
	time_t now;          //实例化time_t结构

	struct tm *timenow;  //实例化tm结构指针

	time(&now);

    //time函数读取现在的时间(国际标准时间非北京时间)，然后传值给now
    timenow = localtime(&now);	

	//分别获得 年 月 日 时  分
	if (Year != NULL)
	{
		*Year = timenow->tm_year - 100;
	}
	if (Mon != NULL)
	{
		*Mon = timenow->tm_mon + 1; // 0 --- 11
	}
	if (Data != NULL)
	{
		*Data = timenow->tm_mday;
	}
	if (Hour != NULL)
	{
		*Hour = timenow->tm_hour;
	}
	if (Min != NULL)
	{
		*Min = timenow->tm_min;
	}
	if (Sec != NULL)
	{
		*Sec = timenow->tm_sec;
	}

	//localtime函数把从time取得的时间now换算成你电脑中的时间(就是你设置的地区)
	//asctime函数把时间转换成字符，通过printf()函数输出
    //PRINTF("Local   time   is   %s/n",asctime(timenow));
	//Tue Jun 10 14:25:14 2014
}

int GetSystemUTCTime(int *Year, int *Mon, int *Data, int *Hour, int *Min, int *Sec)
{
	time_t cur_time = time(NULL);
	if (cur_time < 0)
	{
		LOG_TRACE_PERROR("time error!\n");
		return -1;
	}

	struct tm utc_tm;
	if (NULL == gmtime_r(&cur_time, &utc_tm))
	{
		LOG_TRACE_PERROR("gmtime error!\n");
		return -1;
	}

	struct tm *timenow = &utc_tm;

	//分别获得 年 月 日 时  分
	if (Year != NULL)
	{
		*Year = timenow->tm_year - 100;
	}
	if (Mon != NULL)
	{
		*Mon = timenow->tm_mon + 1; // 0 --- 11
	}
	if (Data != NULL)
	{
		*Data = timenow->tm_mday;
	}
	if (Hour != NULL)
	{
		*Hour = timenow->tm_hour;
	}
	if (Min != NULL)
	{
		*Min = timenow->tm_min;
	}
	if (Sec != NULL)
	{
		*Sec = timenow->tm_sec;
	}

	return 0;
}

void gmt_time(char *str, char *gmt, int bufsize)
{
	int y = 0, m = 0, d = 0, h = 0, min = 0, s = 0;
	GetSystemUTCTime(&y, &m, &d, &h, &min, &s);
    snprintf(gmt, bufsize - 1, "%s: 20%d%d-%d%d-%d%d %d%d:%d%d:%d%d %d", 
    							str ? str : "*", 
    							y / 10, y % 10, 
    							m / 10, m % 10, 
    							d / 10, d % 10, 
    							h / 10, h % 10, 
    						  min / 10, min % 10, 
    							s / 10, s % 10, 
    							SYSTEM_SEC);
}

void system_time(char *str, char *sys_time, int bufsize)
{
	int y = 0, m = 0, d = 0, h = 0, min = 0, s = 0;
	GetSystemTime(&y, &m, &d, &h, &min, &s);
    snprintf(sys_time, bufsize - 1, "%s: 20%d%d-%d%d-%d%d %d%d:%d%d:%d%d %u", 
    							str ? str : "*", 
    							y / 10, y % 10, 
    							m / 10, m % 10, 
    							d / 10, d % 10, 
    							h / 10, h % 10, 
    						  min / 10, min % 10, 
    							s / 10, s % 10, 
    							SYSTEM_SEC);
}

void gmt_time_field(int *y, int *m, int *d, int *h, int *min, int *s, unsigned int *system_sec)
{
	int year = 0, mon = 0, day = 0, hour = 0, minute = 0, sec = 0;
	GetSystemUTCTime(&year, &mon, &day, &hour, &minute, &sec);

	if (y) *y = year;
	if (m) *m = mon;
	if (d) *d = day;
	if (h) *h = hour;
	if (min) *min = minute;
	if (s) *s = sec;
	if (system_sec) *system_sec = SYSTEM_SEC;
}
