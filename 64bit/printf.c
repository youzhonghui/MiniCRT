#include "minicrt.h"

static char __fputc_tmp_val__ = 0;
long fputc(char c,FILE* stream)
{
	__fputc_tmp_val__ = c;
	if (fwrite(&__fputc_tmp_val__,1,1,stream) != 1)
	{
		return EOF;
	} 
	else
	{
		return c;
	}
}

static char __fputs_tmp_array__[256] = {0};
static int __fputs_tmp_size__ = 256;
long fputs(const char* str,FILE *stream)
{
	long len		= strlen(str);
	if( len >= __fputs_tmp_size__ )
		return EOF;
	strcpy( __fputs_tmp_array__,str );
	if (fwrite(__fputs_tmp_array__,1,len,stream) != len)
	{
		return EOF;
	}
	else
	{
		return len;
	}
}

#ifndef WIN32
#define va_list char*
#define va_start(ap,arg) (ap=(va_list)&arg+sizeof(arg))
#define va_arg(ap,t)	(*(t*)((ap+=sizeof(t)) - sizeof(t)))
#define va_end(ap) (ap=(va_list)0)
#else
#include <Windows.h>
#endif

static long vfprintf(FILE* stream, const char *format, va_list arglist)
{
	long translating	= 0;
	long ret	= 0;
	const	char* p	=0;
	for (p = format;*p != '\0';++p)
	{
		switch (*p)
		{
		case '%':
			if (!translating)
			{
				translating	= 1;
			} 
			else
			{
				if (fputc('%',stream) < 0)
				{
					return EOF;
				}
				++ret;
				translating = 0;
			}
			break;

		case 'd':
			if (translating)	//%d
			{
				char buf[16];
				translating	= 0;
				itoa(va_arg(arglist,long),buf,10);
				if (fputs(buf,stream) < 0)
				{
					return EOF;
				}
				ret+= strlen(buf);
			} 
			else if (fputc('d',stream) < 0)
			{
				return EOF;
			} 
			else
			{
				++ret;
			}
			break;

		case	's':
			if (translating)
			{
				const char *str	= va_arg(arglist,const char*);

				translating	= 0;
				if (fputs(str,stream) < 0)
				{
					return EOF;
				}
				ret +=strlen(str);
			}
			else if (fputc('s',stream) < 0)
			{
				return EOF;
			} 
			else
			{
				++ret;
			}

			break;

		default:
			if (translating)
			{
				translating	= 0;
			}
			if (fputc(*p,stream) <0)
			{
				return EOF;
			} 
			else
			{
				++ret;
			}

			break;
		}
	}
	return ret;
}

long printf(const char *format,...)
{
	char* arglist;
	asm( "movq %%rbp,%0":"=r"(arglist) );
	arglist -= 0xa8;
	return vfprintf(stdout,format,arglist);
}

long fprintf(FILE* stream,const char *format,...)
{
	char* arglist;
	asm( "movq %%rbp,%0":"=r"(arglist) );
	arglist -= 0xa8;
	return vfprintf(stream,format,arglist);
}

