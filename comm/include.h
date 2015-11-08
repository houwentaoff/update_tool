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

#define RPC_PORT               5781 /*  */
#define RPC_SERVER_NAME        "FiUpdateMgr"
#define LEN_VERSION            30/*  */
#define LEN_BUF                 100/*  */
#define HASH_LEN                200
#ifdef WIN32
#define _PATH_VERSION        "../patch_version"
#else
#define _PATH_VERSION        "/sobey/fics/patch_version" /*Ĭ��·��*/           /*  */
#endif
#define _PATH_PKG_DL         "/sobey/fics/download/"   /*Ĭ��·��*/

#include "debug.h"
#include "os.h"

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
    SERVER=0,/*fics �����*/
    CLIENT,  /*fics �ͻ���*/ 
    MAS_SRV  /*��������������*/   
}dev_e;

#endif
