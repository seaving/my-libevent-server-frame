#ifndef __USR_TIME_H__
#define __USR_TIME_H__


#define SYSTEM_SEC		get_system_sec()

void gmt_time(char *str, char *gmt, int bufsize);
void gmt_time_field(int *y, int *m, int *d, int *h, int *min, int *s, unsigned int *system_sec);

unsigned long GetTimestrip_us();

void time_init();

int GetSystemUTCTime(int *Year, int *Mon, int *Data, int *Hour, int *Min, int *Sec);
int gmt_sync(char *str, char *gmt, int bufsize, long base_time);
unsigned int get_system_sec(void);
void check_gmt_time(char *str, char *gmt_time, int bufsize, int base_time);
void GetSystemTime(int *Year, int *Mon, int *Data, int *Hour, int *Min, int *Sec);
void system_time(char *str, char *sys_time, int bufsize);

#endif



