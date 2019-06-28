#include "sys_inc.h"
#include "log_trace.h"
#include "file.h"

static void _file_node_free(file_t *f)
{
	if (f && f->file) free(f->file);
	if (f) free(f);
}

static file_t *_file_node_create(int fd, char *file)
{
	file_t *f = calloc(sizeof(file_t), 1);
	if (f)
	{
		f->file = calloc(1, strlen(file) + 1);
		if (f->file == NULL)
		{
			_file_node_free(f);
			return NULL;
		}

		memcpy(f->file, file, strlen(file));
		f->fd = fd;
	}

	return f;
}

void file_close(file_t *f)
{
	if (f && f->fd > 0)
	{
		file_fd_close(f->fd);
	}

	_file_node_free(f);
}

void file_fd_close(int fd)
{
	if (fd > 0)
	{
		close(fd);
	}
}

bool create_dir(char *dir)
{
    int i = 0;
    int len = 0;
    char str[512] = {0};
    strncpy(str, dir, sizeof(str) - 1);
    len = strlen(str);
    for (i = 0; i < len; i ++)
    {
        if (str[i] == '/')
        {
            str[i] = '\0';
            if (strlen(str) > 0 && ! IS_EXIST_FILE(str))
            {
                if (mkdir(str, 0777) != 0)
                {
                	LOG_TRACE_PERROR("mkdir %s error!", str);
					return false;
                }
            }
            
            str[i] = '/';
        }
    }
    
    if (len > 0 && ! IS_EXIST_FILE(str))
    {
        if (mkdir(str, 0777) != 0)
        {
        	LOG_TRACE_PERROR("mkdir %s error!", str);
			return false;
        }
    }

    return IS_EXIST_FILE(dir);
}

bool create_file(char *file)
{
	int fd = -1;
	if (file && ! IS_EXIST_FILE(file))
	{
		fd = open(file, O_CREAT, 0777);
		if (fd < 0)
		{
			LOG_TRACE_PERROR("open %s error!", file);
			return false;
		}

		file_fd_close(fd);
	}

	return IS_EXIST_FILE(file);
}

bool clear_file(char *file)
{
	int fd = -1;
	if (file && IS_EXIST_FILE(file))
	{
		fd = open(file, O_CREAT | O_RDWR | O_TRUNC, 0777);
		if (fd < 0)
		{
			LOG_TRACE_PERROR("open %s error!", file);
			return false;
		}

		file_fd_close(fd);
		return true;
	}

	return false;
}

file_t *open_file(char *file)
{
	int fd = -1;
	if (file && IS_EXIST_FILE(file))
	{
		fd = open(file, O_RDWR, 0777);
		if (fd < 0)
		{
			LOG_TRACE_PERROR("open %s error!", file);
			return NULL;
		}
		
		return _file_node_create(fd, file);
	}

	return NULL;
}

off_t get_file_size(char *file)
{
	off_t size = -1;
	file_t *f = open_file(file);
	size = lseek_from_end(f, 0L);
	lseek_from_head(f, 0L);
	file_close(f);
	return size;
}

off_t lseek_from_head(file_t *f, off_t offset)
{
	if (f && f->fd > 0)
	{
		return lseek(f->fd, offset, SEEK_SET);
	}

	return -1;
}

off_t lseek_from_current(file_t *f, off_t offset)
{
	if (f && f->fd > 0)
	{
		return lseek(f->fd, offset, SEEK_CUR);
	}

	return -1;
}

off_t lseek_from_end(file_t *f, off_t offset)
{
	if (f && f->fd > 0)
	{
		return lseek(f->fd, offset, SEEK_END);
	}

	return -1;
}

int read_file(file_t *f, char *buf, int read_len)
{
	int offset = 0;
	int n = 0;

	int read_min_len = 0;

	if (f && f->fd > 0 && buf && read_len > 0)
	{
		for ( ; ; )
		{
			read_min_len = read_len - offset > 1024 ? 1024 : read_len - offset;
			n = read(f->fd, buf + offset, read_min_len);
			if (n < 0)
			{
				LOG_TRACE_PERROR("read %s error!", f->file);
				f->status = E_FILE_STATUS_IO_READ_ERROR;
				break;
			}

			if (n == 0)
			{
				f->status = E_FILE_STATUS_IO_NORMAL;
				break;
			}
			
			offset += n;
			if (offset >= read_len)
			{
				f->status = E_FILE_STATUS_IO_NORMAL;
				break;
			}
		}
	}

	return offset;
}

int write_file(file_t *f, char *buf, int write_len)
{
	int offset = 0;
	int n = 0;

	int write_min_len = 0;

	if (f && f->fd > 0 && buf && write_len > 0)
	{
		for ( ; ; )
		{
			write_min_len = write_len - offset > 1024 ? 1024 : write_len - offset;
			n = write(f->fd, buf + offset, write_min_len);
			if (n != write_min_len)
			{
				LOG_TRACE_PERROR("write %s error! current write %d, "
							"already write %d, but need write %d", 
								f->file, n, offset, write_len);
				f->status = E_FILE_STATUS_IO_WRITE_ERROR;
				return -1;
			}
			
			offset += n;
			if (offset >= write_len)
			{
				f->status = E_FILE_STATUS_IO_WRITE_ERROR;
				break;
			}
		}
	}

	return offset;
}





