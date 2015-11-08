/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Sobey.
 *       Filename:  os.h
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  Saturday, November 07, 2015 10:41:21 HKT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sean. Hou (hwt), houwentaoff@gmail.com
 *   Organization:  Sobey
 *
 * =====================================================================================
 */
#ifndef __OS_H__
#define __OS_H__

#ifdef WIN32
/* ========================================================================
   WINDOWS
   ======================================================================== */
#define dirname(x)        do_dirname(x)
#define basename(x)       do_basename(x)
#define popen(x, y)       _popen(x, y)
#define pclose(x)         _pclose(x)
#define chdir(x)          _chdir(x)
//#define sleep(x)          Sleep(1000*x)
#define errno                 (-1)//
#define S_ISDIR(m) (((m) & 0170000) == (0040000))
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
//#define stat(x, y)           _stat(x, y)
#else
/* ========================================================================
   LINUX
   ======================================================================== */

#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
//#define dirname(x)       dirname(x)
//#define basename(x)      basname(x)
#endif
#endif

