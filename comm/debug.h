/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Sobey.
 *       Filename:  debug.h
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  2015/9/14 10:02:09
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sean. Hou (hwt), houwentaoff@gmail.com
 *   Organization:  Sobey
 *
 * =====================================================================================
 */
#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifndef WIN32
#include <time.h>
#endif

#ifdef     DEBUG
#undef     DEBUG
#endif

#ifdef     ERROR
#undef     ERROR
#endif

#define    DEBUG                   1/*debug switch*/
#define    ERROR                   1/*debug switch*/

#if     DEBUG
#ifndef WIN32		
#define ut_dbg(fmt, args...)                                                                \
do                                                                                          \
{                                                                                           \
    printf("%s:", get_commonlog_time());                                                    \
    printf("[update dbg]:%s:%d: "fmt, __FILE__, __LINE__, ##args);                          \
}while(0)    /* */  

#else
#define ut_dbg(...)                                                                         \
do                                                                                          \
{                                                                                           \
	printf("%s:", get_commonlog_time());                                                    \
	printf("[update dbg]: "##__VA_ARGS__);                                                  \
}while(0)
#endif
#else
#define ut_dbg(...)
#endif

#if     ERROR
#ifndef WIN32	
#define ut_err(fmt, args...)                                                                \
do                                                                                          \
{                                                                                           \
    printf("%s:", get_commonlog_time());                                                    \
    printf("[update err]:%s:%d: "fmt, __FILE__, __LINE__, ##args);                          \
}while(0)
#else
#define ut_err(...)   \
do\
{\
	printf("%s:", get_commonlog_time());\
	printf("[update err]: "##__VA_ARGS__);\
}while(0)
#endif
#else
#define ut_err(...)
#endif

#endif
