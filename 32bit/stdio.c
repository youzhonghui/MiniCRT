//stdio.c
#include "minicrt.h"

int mini_crt_init_io()
{
	return	1;
}

#ifdef WIN32
#include <Windows.h>

FILE* fopen(const char *filename,const char* mode)
{
	HANDLE hFile	= 0;
	int access		= 0;
	int creation	= 0;
	if (strcmp(mode,"w") == 0)
	{
		access		|= GENERIC_WRITE;
		creation	|= CREATE_ALWAYS;
	}

	if (strcmp(mode,"w+") == 0)
	{
		access		|= GENERIC_WRITE |GENERIC_READ;
		creation	|= CREATE_ALWAYS;
	}

	if (strcmp(mode,"r") == 0)
	{
		access	|= GENERIC_READ;
		creation	= CREATE_ALWAYS;
	}

	if (strcmp(mode,"r+") == 0)
	{
		access	|= GENERIC_WRITE | GENERIC_READ;
		creation	|= TRUNCATE_EXISTING;
	}

	hFile	= CreateFileA(filename,access,0,0,creation,0,0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	return (FILE*)hFile;
}

int fread(void* buffer,int size,int count,FILE* stream)
{
	int read	= 0;
	if (!ReadFile((HANDLE)stream,buffer,size*count,&read,0))
	{
		return 0;
	}

	return read;
}

int fwrite(const void* buffer,int size,int count,FILE* stream)
{
	int written	= 0;
	if (!WriteFile((HANDLE)stream,buffer,size*count,&written,0))
	{
		return 0;
	}
	return written;
}

int fclose(FILE *fp)
{
	return CloseHandle((HANDLE)fp);
}

int fseek(FILE* fp,int offset,int set)
{
	return SetFilePointer((HANDLE)fp,offset,0,set);
}

#else //define linux

static int open(const char* filename,int flags,int mode)
{
	int fd	= 0;
	asm("movl $5,%%eax		\n\t"
		"movl %1,%%ebx	\n\t"
		"movl %2,%%ecx \n\t"
		"movl %3,%%edx \n\t"
		"int $0x80  \n\t"
		"movl %%eax,%0 \n\t":
		"=m"(fd):"m"(filename),"m"(flags),"m"(mode));

		return fd;
}

static int read(int fd,void* buffer,unsigned size)
{
	int ret	= 0;
	asm("movl $3,%%eax \n\t"
		"movl %1,%%ebx \n\t"
		"movl %2,%%ecx \n\t"
		"movl %3, %%edx \n\t"
		"int $0x80  \n\t"
		"movl %%eax,%0  \n\t":
	"=m"(ret):"m"(fd),"m"(buffer),"m"(size));
	return ret;
}

static int write(int fd,const void *buffer,unsigned size)
{
	int ret	= 0;
	asm("movl $4,%%eax  \n\t"
		"movl %1,%%ebx \n\t"
		"movl %2,%%ecx  \n\t"
		"movl %3,%%edx \n\t"
		"int $0x80  \n\t"
		"movl %%eax,%0 \n\t":
	"=m"(ret):"m"(fd),"m"(buffer),"m"(size));

	return ret;
}

static int close(int fd)
{
	int ret	= 0;
	asm("movl $6,%%eax  \n\t"
		"movl %1,%%ebx  \n\t"
		"int $0x80  \n\t"
		"movl %%eax,%0 \n\t":
	"=m"(ret):"m"(fd));

	return	ret;
}

static int seek(int fd,int offset,int mode)
{
	int ret	= 0;
	asm("movl $19,%%eax  \n\t"
		"movl %1,%%ebx \n\t"
		"movl %2,%%ecx  \n\t"
		"movl %3,%%edx \n\t"
		"int $0x80  \n\t"
		"movl %%eax,%0 \n\t":
	"=m"(ret):"m"(fd),"m"(offset),"m"(mode));

	return ret;
}

FILE* fopen(const char *filename,const char* mode)
{
	int fd	= -1;
	int flags	= 0;
	int access	= 00700; //创建文件的权限

	//来自于/usr/include/bits/fcntl.h
	//注意：以0开始的数字的八进制的
#define O_RDONLY	00
#define O_WRONLY	01
#define O_RDWR		02
#define O_CREAT		0100
#define O_TRUNC		01000
#define O_APPEND	02000

	if (strcmp(mode,"w") == 0)
	{
		flags	|= O_WRONLY | O_CREAT |O_TRUNC;
	}

	if (strcmp(mode,"w+") == 0)
	{
		flags	|= O_RDWR |O_CREAT|O_TRUNC;
	}

	if (strcmp(mode,"r") == 0)
	{
		flags	|= O_RDONLY;
	}

	if (strcmp(mode,"r+") == 0)
	{
		flags	|= O_RDWR | O_CREAT;
	}

	fd =open(filename,flags,access);
	return (FILE*)fd;
}

int fread(void* buffer,int size,int count,FILE* stream)
{
	return read((int)stream,buffer,size*count);
}
int fwrite(const void* buffer,int size,int count,FILE* stream)
{
	return write((int)stream,buffer,size*count);
}

int fclose(FILE *fp)
{
	return close((int)fp);
}

int fseek(FILE* fp,int offset,int set)
{
	return seek((int)fp,offset,set);
}

#endif

