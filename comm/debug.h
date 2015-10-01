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
#ifdef     DEBUG
#undef  DEBUG
#endif

#ifdef     ERROR
#undef  ERROR
#endif

#define DEBUG                   1/*debug switch*/
#define ERROR                   1/*debug switch*/

#if     DEBUG
#ifndef WIN32		
#define ut_dbg(fmt, args...)   printf("[update dbg]:%s:%d: "fmt, __FILE__, __LINE__, ##args)
#else
#define ut_dbg(...)   printf("[update dbg]: "##__VA_ARGS__)
#endif
#else
#define ut_dbg(...)
#endif

#if     ERROR
#ifndef WIN32	
#define ut_err(fmt, args...)   printf("[update err]:%s:%d: "fmt, __FILE__, __LINE__, ##args)
#else
#define ut_err(...)   printf("[update err]:%s:%d: "__FILE__, __LINE__, ##__VA_ARGS__)
#endif
#else
#define ut_err(...)
#endif

#endif
