/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Sobey.
 *       Filename:  include.h
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  2015/9/14 10:13:17
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Sean. Hou (hwt), houwentaoff@gmail.com
 *   Organization:  Sobey
 *
 * =====================================================================================
 */
#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include "debug.h"
#include "os.h"
#include <time.h>
#include <fcntl.h>
#ifdef WIN32
#include <direct.h>
#include <windows.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#else
#include <stdarg.h>
#include <sys/utsname.h>
#include <libgen.h>
#include <unistd.h>
#include <errno.h>
#endif

#define RPC_PORT               5781 /*  */
#define RPC_SERVER_NAME        "FiUpdateMgr"
#define LEN_VERSION            30/*  */
#define LEN_BUF                 100/*  */
#define HASH_LEN                200
#define _M(x)                   (x*(1024*1024))/*  x M bytes*/
#ifdef WIN32
#define _PATH_VERSION        "..\\patch_version"
#define _PATH_CONF           "..\\config\\update_network.xml"
#else
#define _PATH_CONF           "../config/update_network.xml"
#define _PATH_VERSION        "/sobey/fics/patch_version" /*默认路径*/           /*  */
#endif
#define _PATH_PKG_DL         "/sobey/fics/download/"   /*默认路径*/

#define G (*ptr_to_globals)

#define INIT_G()  do { \
    ptr_to_globals = (struct globals *)&globals; \
    memset((void *)&globals, 0, sizeof(globals)); \
}while (0)

typedef struct 
{
    char version[LEN_VERSION];
    char date[LEN_VERSION];
    char patchNo[LEN_VERSION];
    union 
    {
        char hash[HASH_LEN];
        char reserved[HASH_LEN];
    }reserved;
}version_t;
typedef enum
{
    SERVER=0,/*fics 服务端*/
    CLIENT,  /*fics 客户端*/ 
    MAS_SRV  /*更新软件包服务机*/   
}dev_e;
struct globals;
extern struct globals *ptr_to_globals;
extern struct globals globals;
typedef struct logFile_t{
    const char *path;
    int fd;
    unsigned  size;
    unsigned int isRegular;
}logFile_t;
struct globals{
    /* log */
    logFile_t logFile;
    /* max size of file before rotation */ 
    unsigned logFileSize;
    /* number of rotated message files */  
    unsigned logFileRotate;
    time_t lastLogTime;
    /* exe path */
    const char *exe;
    /*  cwd path */
    const char *cwd;
    /* root dir /sobey/fics/ */
    const char *root;
};
#endif
