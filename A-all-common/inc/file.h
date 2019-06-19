#ifndef __FILE_H__
#define __FILE_H__

#include "sys_inc.h"

#define FILE_FD(f)	(f ? f->fd : -1)
#define FILE_READ_STATUS(f) (f ? f->status : E_FILE_STATUS_IO_READ_ERROR)
#define FILE_WRITE_STATUS(f) (f ? f->status : E_FILE_STATUS_IO_WRITE_ERROR)

#define IS_EXIST_FILE(path)	(access(path, F_OK) == 0)

#define LSEEK_TO_HEAD(fd) 	lseek_from_head(fd, 0L)
#define LSEEK_TO_END(fd) 	lseek_from_end(fd, 0L)

typedef enum __file_io_status__
{
	E_FILE_STATUS_IO_READ_ERROR = -1,
	E_FILE_STATUS_IO_WRITE_ERROR = -2,
	E_FILE_STATUS_IO_NORMAL = 0
} file_io_status_t;

typedef struct __file__
{
	int fd;
	char *file;
	file_io_status_t status;
} file_t;

bool create_dir(char *dir);

bool create_file(char *file);
bool clear_file(char *file);
file_t *open_file(char *file);
void file_close(file_t *f);
void file_fd_close(int fd);

off_t get_file_size(char *file);
off_t lseek_from_head(file_t *f, off_t offset);
off_t lseek_from_current(file_t *f, off_t offset);
off_t lseek_from_end(file_t *f, off_t offset);

int read_file(file_t *f, char *buf, int read_len);
int write_file(file_t *f, char *buf, int write_len);

#endif




