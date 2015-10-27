/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Sobey.
 *       Filename:  patch.cpp
 *
 *    Description:  升级工具2.0 增加patch的部分功能实现文件
 *         Others:
 *
 *        Version:  1.0
 *        Created:  2015/10/21 15:26:36
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sean. Hou (hwt), houwentaoff@gmail.com
 *   Organization:  Sobey
 *
 * =====================================================================================
 */

#include "patch.h"
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include <errno.h>

#define LEN_SHORT            128/*  */

/**
 * @brief 从补丁号中获取全版本号
 *        如 补丁号为259 则其全版本号为200,59 为补丁号
 *
 * @param patchNum
 *
 * @return 
 */
int getCurBaseVer(int patch)
{
    int baseNum = 0;

    if (patch < 0)
    {
        return -1;
    }

    return (baseNum = patch / BASEINTERVAL);
}
/**
 * @brief 获取当前的补丁包号
 *
 * @param patch
 *
 * @return 
 */
int getCurPatchVer(int patch)
{
    int patchNum = 0;

    if (patch < 0)
    {
        return -1;
    }

    return (patchNum = patch % BASEINTERVAL);
}
int genHashValueFromDir(vector<string> & verList, string & hashValue)
{
//暂时不用
    return 0;
}
int genHashFromHis(const char *path, string & hashValue)
{
    vector<string> opList;
    FILE *fp = NULL;
    char buf[LEN_BUF] = {0};
    int r;
    string keySum;
    unsigned  char  md[16];
    int  i;
    char  tmp[3]={ '\0' },md5[33]={ '\0' };

    fp = fopen(path, "r");
    if (!fp)
    {
        ut_err("open fail\n");
        goto err;
    }
    while ((r = fscanf(fp, "%s", buf)) == 1)
    {
        if (buf[0] == '#')
        {
            if (fscanf(fp, "%*[^\n]\n") < 0)
            {
                ut_err("skip # fail\n");
                goto err;
            }
            continue;
        }
        r = fscanf(fp, "%*[ ]%[^\n]s", buf);//skip space huiche     
        opList.push_back(buf);
        keySum += buf;
    }
    //利用opList生成MD5值，作为hash值 供客户端查询
//    MD5(keySum.c_str(), strlen (keySum.c_str()), md);//mark
    for  (i = 0; i < 16; i++)
    {
        sprintf(tmp, "%2.2x" , md[i]);
        strcat(md5, tmp);
    }
    hashValue = md5;

    return 0;
err:
    return -1;
}
int addVer2His(version_t *newVer, const char * path)
{
    FILE *fp = NULL;
    char curOpVer[LEN_SHORT];

    if (!newVer || !path)
    {
        ut_err("newVer or path is NULL\n");
        goto err;
    }
    sprintf(curOpVer, "%s\n", newVer->reserved.hash);
    fp = fopen(path, "a+");
    if (!fp)
    {
        ut_err("file is not exist\n");
        goto err;
    }
    fputs(curOpVer, fp);
    fclose(fp);

    return 0;
err:    
    return -1;
}
int scanAllPatch(const char *path, vector<string> & verList)
{
    return 0;
}
bool shouldInsBase(int localBase, int remoteBase)
{
    return 0;
}
bool shouldInsPatch(int patchNum)
{
    return 0;
}
bool installPatch(int patchNum)
{
    return 0;
}
bool installBase(int baseNum)
{
    return 0;
}
//bool checkAllpatchIsIn(const int patchSet[BASEINTERVAL][2], patchSet need2down[BASEINTERVAL][2])
//{
//    return 0;
//}
int mkVer(int baseVer, int patchVer)
{
    return (baseVer*BASEINTERVAL + patchVer);
}
int itoa(int inum, string &str)
{
    char asicBuf[36] = {0};
    long num = 0;
    int i=0;

    while (inum)
    {
        num = inum % 10;
        asicBuf[i++] = '0' + num;
        inum /= 10;
    }
    asicBuf[i] = '\0';//01504102
    while (i--)
    {
        str += asicBuf[i];
    }
    return 0;    
}
/**
 * @brief 下载loss的包 version -> netVer ;date -> lossPatchs[i][1] fics_v1.0.0_2015.10.10_4005
 *
 * @param netVer
 * @param lossPatchs
 *
 * @return 
 */
int downLossPatch(version_t *netVer, patchSet_t lossPatchs)
{
    int i=0;
    int ret = 0;
//    char srcFile[512]={0};
    char dstFile[512]={0};
    string tmpDate;
    string curType;

//    sprintf(srcFile, "/sobey/fics/download/fics_%s_", netVer->version);//正则 re
//    sprintf(dstFile, "/sobey/fics/update/fics_%s_", netVer->version);//正则 re
    for (i =0; i<=BASEINTERVAL; i++)
    {
        if (lossPatchs[i][0]!=0)
        {
#if 1
            ret = FiUpdateAssistant::getinstance()->downLossPkg(netVer, lossPatchs[i][1], mkVer(getCurBaseVer(netVer->patchNo), i));//down over
#else
            ret = FiUpdateAssistant::getinstance()->downloadFile(ip, srcFile, dstFile);
#endif
            long2Date(lossPatchs[i][1], tmpDate);//set to str date  
            sprintf(dstFile, "fics_%s_%s_%d", netVer->version, tmpDate.c_str(), mkVer(getCurBaseVer(netVer->patchNo), i));//正则 re
            if (ret == 0)
            {
                ut_dbg("download file %s %s success\n", dstFile);
                lossPatchs[i][0] = 0;
            }
            else
            {
                ut_err("download file %s fail\n", dstFile);
                ret = -1;
                //                            i--;//一直下载不下来则退出 重新查询版本信息
            }
        }
    }
    i = 0;
    //check patch
    while (lossPatchs[i++][0] && i<=BASEINTERVAL){};
    if (i != BASEINTERVAL)
    {
        ret = -2;
        ut_err("patch:%d is not be downed.\n", i);
    }
    else
    {
        ret = 0;
        ut_dbg("down all patchs complete\n");
    }
    return ret;
}
int patchs2str(patchSet_t lossPatchs, string &set)
{
    char buf[8];
    int i = 0;

    for (i = 0; i<BASEINTERVAL; i++)
    {
        if (lossPatchs[i][0] != 0)
        {
            sprintf(buf, "%d", lossPatchs[i][0]);
            set += buf;
            set += ",";
        }
    }
    return 0;
}
/**
 * @brief      比较字符串和正则表达式是否匹配
 *          如: fics_v1.0.0_2015.10.10_4005 用 fics_v1.0.0_*_4005 匹配成功 
 *          如: server_v1.0.0_2015.10.10_4001_32_Linux2.6.tar.gz 用server_v1.0.0_*_4001_32_Linux2.6.tar.gz 匹配成功

 * @param src   原始字符串
 * @param re  对应的正则表达式
 *
 * @return 
 */
bool matchRE(const char *src, const char *re)
{
    char *str = src;
    char *find = NULL;
    bool ret = false;
    char *last_str = NULL;
    char *re_tmp = re;

    while (*str && *re_tmp)
    {
        if (*re_tmp == '*')
        {
            str++;
            if (NULL != strstr(str, re_tmp+1))//find it
            {
                ret = true;
                break;
            }
        }
        else
        {
            if (*str != *re_tmp)
            {
                ret = false;
                break;
            }
        }
        str++;
        re_tmp++;
    }
    if (*str == '\0')
    {
        ret = true;
    }
    return ret;
}
int getPkgList(char const * path, char const * prefix, char const *suffix, const char *version, vector<string> & pkgList)
{
    char cmdBuf[256] = {0};
    char tmppath[256]={0};
    char *fileName = NULL;
    FILE *fp = NULL;

    sprintf(cmdBuf, "find %s -name %s_%s_*.%s", path, prefix, version, suffix);
    if (NULL == (fp = popen(cmdBuf, "r")))
    {
        ut_err("popen fail\n");
        goto err;
    }
    while (fscanf(fp, "%[^\n]", tmppath) == 1)
    {
        fileName = basename(tmppath);
        pkgList.push_back(fileName);
    }
    pclose(fp);
    //find ./ -name 'server_v1.0.0_*.tar.gz'
    return 0;
err:
    return -1;
}
int getDirList(char const * path, vector<string> & dirList)
{
    DIR*   dir=NULL; 
    struct   dirent*   dirlist; 
    struct   stat   filestat;

    if( path == NULL)
    {

        ut_err("The path is empty!\n");
        return -1;
    }
    dir = opendir(path);
    if( dir == NULL)
    {
        ut_err("Open directory : %s failed errno:%d!\n",path,errno);
        return -2;
    }
    while( (dirlist=readdir(dir))!= NULL )
    {

        if((dirlist->d_name[0]=='.'))
        {
            continue;
        }
        std::string filename =dirlist->d_name; 
        std::string fullName=path;
        fullName += "/";
        fullName += dirlist->d_name;
        stat(fullName.c_str(),&filestat);

        if(S_ISDIR(filestat.st_mode) )
        {
//            filename +="/*";
            dirList.push_back(filename);
        }
    }
    closedir(dir);    
    return 0;
}
/**
 * @brief 将字符串 2014.05.10 转化为整数20140510
 *
 * @param date
 *
 * @return 
 */
long date2Long(const char *date)
{
    long ldate = 0;

    if (!date)
    {
        ut_err("date is null\n");
        return 0;
    }
    while (*date)
    {
        if (*date == '.')
        {
            continue;
        }
        ldate *= 10;
        ldate +=*date - '0';
        date++;
    }
    return ldate;
}
/**
 * @brief 将整数 20140510转化为 2014.05.10
 *
 * @param ldate
 * @param sdate
 *
 * @return 
 */
int long2Date(long ldate, string &sdate)
{
    char sdateBuf[36] = {0};
    long num = 0;
    int i=0;

    while (ldate)
    {
        num = ldate % 10;
        sdateBuf[i++] = '0' + num;
        ldate /= 10;
    }
    sdateBuf[i] = '\0';//01504102
    while (i--)
    {
        sdate += sdateBuf[i];
    }
    return 0;
}
int checkAndDownPkg()
{
    /*-----------------------------------------------------------------------------
     *  1. ask which pkg should be downed.
     *  2. compare local pkg
     *  3. down pkg that I dont have
     *-----------------------------------------------------------------------------*/
    patchSet_t patchs;//memset
    patchSet_t lossPatchs;//memset
    int i = 0;
    int ret = 0;

    memset(&patchs, 0, sizeof(patchSet_t));
    memset(&lossPatchs, 0, sizeof(patchSet_t));
    
    FiUpdateAssistant::getinstance()->queryPatchs(patchs);
    if (0 == FiUpdateAssistant::getinstance()->comparePatchs(&(FiUpdateAssistant::getinstance()->netVer), patchs, lossPatchs))
    {
        ut_dbg("my patchs is the same as server, dont need to down\n");
    }
    else
    {
        ut_dbg("I have lost some patchs waiting for downing\n");
        i = 4;
        while (i-- && ret!=0)
        {
            if ((ret = downLossPatch(&(FiUpdateAssistant::getinstance()->netVer), lossPatchs)) < 0)
            {
                //lossPatchs changed
                string set;
                patchs2str(lossPatchs, set);
                ut_err("down file fail !!!! lost patch list:%s \n", set.c_str());
            }
        }
    }
    return 0;
}
/**
 * @brief    server_v1.1.0_2015.09.29_4005_64_Linux2.6.tar.gz
 *
 * @param tarName
 *
 * @return 
 */
int getPatchNumFromName(char const *tarName)
{
    char *version;
    char *date;
    char *patchNo;
    char patchNoBuf[64];
    int ipatchNo = 0;
    int i =0;

    version = strchr(tarName, '_');
    if (!version)
    {
        goto err;
    }
    date = strchr(version+1, '_');
    if (!date)
    {
        goto err;
    }
    patchNo = strchr(date+1, '_');
    if (!patchNo)
    {
        goto err;
    }
    patchNo++;
    while (*patchNo != '_')
    {
        patchNoBuf[i++] = *patchNo;
    }
    patchNoBuf[i] = '\0';
    ipatchNo = atoi(patchNoBuf);
    return ipatchNo;
err:
    return -1;
}
