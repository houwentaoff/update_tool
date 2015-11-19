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
#ifdef WIN32
#else
#include <dirent.h>
#endif
#include <sys/stat.h>
#ifndef WIN32
#include <unistd.h>
#include <libgen.h>
#endif
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <errno.h>
#include <map>

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
int record2History(const char *fileName)
{
    version_t curVer;
    if (!fileName)
    {
        ut_err("file name is null\n");
        return -1;
    }
    memset(&curVer, 0, sizeof(version_t));
    getVerFromName(fileName, &curVer);
    addVer2His(&curVer, HISTORY);
    return 0;
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
    sprintf(curOpVer, "fics %s %s %s\n", newVer->version, newVer->date, newVer->patchNo);
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
    char dstFile[512]={0};
    string tmpDate;
    string curType;

//    sprintf(srcFile, "/sobey/fics/download/fics_%s_", netVer->version);//正则 re
//    sprintf(dstFile, "/sobey/fics/update/fics_%s_", netVer->version);//正则 re
    for (i =0; i<BASEINTERVAL; i++)
    {
        if (lossPatchs[i][0]!=0)
        {
#if 1
            ret = FiUpdateAssistant::getinstance()->downLossPkg(netVer, lossPatchs[i][1], mkVer(getCurBaseVer(atoi(netVer->patchNo)), i));//down over
#else
            ret = FiUpdateAssistant::getinstance()->downloadFile(ip, srcFile, dstFile);
#endif
            long2Date(lossPatchs[i][1], tmpDate);//set to str date  
            sprintf(dstFile, "fics_%s_%s_%d", netVer->version, tmpDate.c_str(), mkVer(getCurBaseVer(atoi(netVer->patchNo)), i));//正则 re
            if (ret == 0)
            {
                ut_dbg("download file %s success\n", dstFile);
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
    while (!lossPatchs[i++][0] && i<BASEINTERVAL){};
    if (i < BASEINTERVAL)
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
            sprintf(buf, "%d", (int)lossPatchs[i][0]);
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
    char const *str = src;
    char *find = NULL;
    bool ret = false;
    char *last_str = NULL;
    char const *re_tmp = re;

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
#ifdef WIN32
    sprintf(cmdBuf, "dir /b  \"%s\\%s_%s_*%s\"", path, prefix, version, suffix);
#else
    sprintf(cmdBuf, "find %s -name \'%s_%s_*%s\'", path, prefix, version, suffix);
#endif
    if (NULL == (fp = popen(cmdBuf, "r")))
    {
        ut_err("popen fail\n");
        goto err;
    }
    while (fscanf(fp, "%[^\n]s", tmppath) == 1)
    {
        fgetc(fp);
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
#ifdef WIN32
    //此结构说明参MSDN;
    WIN32_FIND_DATA FindFileData;
    //查找文件的句柄;
    HANDLE hListFile;
    char szFullPath[MAX_PATH];
    //相对路径;
    char szFilePath[MAX_PATH];
    //构造相对路径;
    sprintf(szFilePath, "%s\\*", path);
    //查找第一个文件，获得查找句柄，如果FindFirstFile返回INVALID_HANDLE_VALUE则返回;
    if((hListFile = FindFirstFile(szFilePath, &FindFileData)) == INVALID_HANDLE_VALUE)
    {
        //查找文件错误;
        return false;
    }
    else
    {
        do 
        {
            //过滤.和..;
            //“.”代表本级目录“..”代表父级目录;
            if( FindFileData.cFileName[0]==(TCHAR)('.') )
            {
                continue;
            }
            std::string filename = FindFileData.cFileName;
            //构造全路径;
            sprintf(szFullPath, "%s\\%s", path, FindFileData.cFileName);


            //如果是文件夹，则递归调用EnmuDirectory函数;
            if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                dirList.push_back(filename);
            }
            
        }while(FindNextFile(hListFile, &FindFileData));
    }
    //关闭句柄;
    FindClose(hListFile);
#else
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
#endif
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
            date++;
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
    int year;
    int month;
    int day;
    
    day     = ldate%100;
    ldate  /= 100;
    month   = ldate%100;
    ldate  /= 100;
    year    = ldate;
    
    sprintf(sdateBuf, "%d.%.2d.%.2d", year, month, day);
    sdate = sdateBuf;

    return 0;
}
int syncPkg()
{
    /*-----------------------------------------------------------------------------
     *  1. ask which pkg should be downed.
     *  2. compare local pkg
     *  3. down pkg that I dont have
     *-----------------------------------------------------------------------------*/
    patchSet_t patchs;//memset
    patchSet_t lossPatchs;//memset
    int i = 0;
    int ret = 1;

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
    char const *version;
    char const *date;
    char const *patchNo;
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
        patchNoBuf[i++] = *(patchNo++);
    }
    patchNoBuf[i] = '\0';
    ipatchNo = atoi(patchNoBuf);
    return ipatchNo;
err:
    return -1;
}
int gen_optional_pack_name(const ::PlatformInfoEx& PInfo,std::vector<std::string>& names)/*根据客户端传来的平台参数判断客户端需要的安装包的后缀*/
{
	char lm[6];
	lm[0]='_';
	sprintf(lm+1, "%d", (int)PInfo.OSRunMode);
	std::string filename1 =lm;
	filename1+="_";
	filename1 +=PInfo.OSName;
	if(strlen((PInfo.OSPackName.in())) >0)
	{
		std::string str=(PInfo.OSPackName.in());
		names.push_back(filename1+str+".tar.gz");
	}
	names.push_back(filename1+".tar.gz");
	if (PInfo.platform == 0 )
	{
		std::string str =lm;
		str+="_Win.tar.gz";
		names.push_back(str);
	}
	if (PInfo.platform == 1 )
	{
		std::string str =lm;
		str+="_Linux.tar.gz";
		names.push_back(str);
	}
	if( PInfo.platform == 2)
	{
		std::string str =lm;
		str+="_Mac.tar.gz";
		names.push_back(str);
	}
	return 0;
}
int getVerFromName(const char *fileName, version_t *ver)
{
    char buff[512]  = {0};
    char *date      = NULL;
    char *version   = NULL;
    char *patchNo   = NULL;
    char *end       = NULL;

    if (!fileName || !ver)
    {
        ut_err("filename or ver is null \n");
        goto err;
    }
    
    strcpy(buff, fileName);
    version = strstr(buff, "_v");
    if (!version)
    {
        ut_err("get veresion fail\n");
        goto err;
    }
    version ++;
    date = strchr(version, '_');
    if (!date)
    {
        ut_err("get date fail\n");
        goto err;
    }
    *date  = '\0';
    date ++;
    patchNo = strchr(date, '_');
    if (!patchNo)
    {
        ut_err("get patch no fail\n");
        goto err;
    }
    *patchNo = '\0';
    patchNo ++;
    end = strchr(patchNo, '_');
    if (end)
    {
        *end = '\0';
    }
    sprintf(ver->version, "%s", version);
    sprintf(ver->date, "%s", date);
    sprintf(ver->patchNo, "%s", patchNo);

    return 0;
err:
    return -1;
}
int getMD5FromLocal(const char *fileName, string &localMd5)
{
    char cmdBuf[512] = {0};
    char localMd5Buf[256] = {0};
    FILE *fp = NULL;

    if (!fileName)
    {
        ut_err("file name is null\n");
        goto err;
    }
    if (getMD5FromCache(fileName, localMd5) == 0)
    {
        return 0;
    }
#ifdef WIN32
    sprintf(cmdBuf, "md5sums -u %s", fileName);
#else
    sprintf(cmdBuf, "md5sum %s", fileName);
#endif
    if (NULL ==(fp = popen(cmdBuf, "r")))
    {
        ut_err("popen fail\n");
        goto err;
    }
    if (fscanf(fp, "%[^ ]s", localMd5Buf) != 1)//6345b3f685ceefa3630bca9571b17534
    {
        pclose(fp);
        goto err;
    }
    pclose(fp);
    if (strlen(localMd5Buf) < 32)
    {
        ut_err("md5 fail\n");
        goto err;
    }
    localMd5 = localMd5Buf;
    if (addEleMD52Cache(fileName, localMd5) < 0)
    {
        ut_err("update md5 cache fail\n");
    }

    return 0;
err:
    return -1;
}
static map<string, string>MD5Cache; 

int getMD5FromCache(const char* filename, string &localMd5)
{
    map<string, string>::iterator it;
//file name -> basename(filename)?
    it = MD5Cache.find(filename);
    if (it==MD5Cache.end())
    {
        return -1;
    }
    localMd5 = it->second;

    return 0;
}
int addEleMD52Cache(const char* filename, string &localMd5)
{
    MD5Cache.insert(make_pair(filename, localMd5));
    ut_dbg("md5 cache: add ele file:%s val:%s\n", filename, localMd5.c_str());
    return 0;
}
int delEleMD52Cache(const char* filename, string &localMd5)
{
    map<string, string>::iterator it;

    it = MD5Cache.find(filename);    
    if (it==MD5Cache.end())
    {
        ut_dbg("md5 cache: del ele file:%s val:%s fail!!!\n", filename, localMd5.c_str());
        return -1;
    }
    MD5Cache.erase(it);
    ut_dbg("md5 cache: del ele file:%s val:%s\n", filename, localMd5.c_str());

    return 0;
}
bool unzip(const char* src, const char *dst)
{
    char cmdBuf[512];
    int suffixPos = strlen(".zip");
    int len = 0;

    if (!src || !dst)
    {
        ut_err("src dst is null\n");
        goto err;
    }
    len = strlen(src);
    if (strcmp(&src[len-suffixPos], ".zip")!=0)
    {
        ut_err("this is not a zip file.\n");
        goto err;
    }
    sprintf(cmdBuf, "unzip -o %s -d %s", src, dst);
    system(cmdBuf);
    return true;
err:
    return false;
}
