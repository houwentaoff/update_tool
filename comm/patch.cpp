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
    MD5(keySum.c_str(), strlen (keySum.c_str()), md);
    for  (i = 0; i < 16; i++)
    {
        sprintf(tmp, "%2.2x" , md[i]);
        strcat(md5, tmp);
    }

    return (hashValue = md5);
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

}
bool shouldInsBase(int localBase, int remoteBase)
{

}
bool shouldInsPatch(int patchNum)
{

}
bool installPatch(int patchNum)
{

}
bool installBase(int baseNum)
{

}
bool checkAllpatchIsIn(const int patchSet[BASEINTERVAL], int (*need2down)[])
{

}
int downLossPatch(version_t *netVer, patchSet_t *lossPatchs)
{
    int i=0;
    int ret = 0;
    char srcFile[512]={0};
    char dstFile[512]={0};

    sprintf(srcFile, "/sobey/fics/download/fics_%s_*_", netVer->version);//正则 re
    sprintf(dstFile, "/sobey/fics/update/fics_%s_*_", netVer->version);//正则 re
    for (i =0; i<=BASEINTERVAL; i++)
    {
        if (lossPatchs[i]!=0)
        {
            sprintf(srcFile+strlen(srcFile), "%d", getCurBaseVer(netVer->patchNo)*BASEINTERVAL + i);
            sprintf
            ret = FiUpdateAssistant::getinstance()->downloadFile(ip, srcFile, dstFile);
            if (ret == 0)
            {
                ut_dbg("download file %s success\n", dstFile);
                lossPatchs[i] = 0;
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
    while (lossPatchs[i++] && i<=BASEINTERVAL){};
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
int patchs2str(patchSet_t *lossPatchs, string &set)
{
    char buf[8];
    int i = 0;

    for (i = 0; i<BASEINTERVAL; i++)
    {
        if (lossPatchs[i] != 0)
        {
            sprintf(buf, "%d", lossPatchs[i]);
            set += buf;
            set += ",";
        }
    }
    return 0;
}
/**
 * @brief      比较字符串和正则表达式是否匹配
 *          如: fics_v1.0.0_2015.10.10_4005 用 fics_v1.0.0_*_4005 匹配成功 
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
int getDirList(char const * path, vector<string> & dirList)
{
    return 0;
}
/**
 * @brief 比较服务器的Patchs和本地的包进行对比，并将缺少的包标记到 lossPatchs中
 *
 * @param serPatchs
 * @param lossPatchs
 *
 * @return 
 */
int comparePatchs(version_t *netVer, patchSet_t serPatchs, patchSet_t  lossPatchs)
{
    int i=0;
    bool exist = false;
    vector <string> pathDirList;
    vector <string> iterator::itr;
    char tmpPath[256]={0};
    string version = netVer->version;
    int patchNo = 0;
    int basePatch = getCurBaseVer(atoi(netVer->patchNo));

    getDirList("./", pathDirList);//get dir list

    for (i=0; i<BASEINTERVAL; i++)
    {
        exist = false;
        if (serPatchs[i] != 0)
        {
            patchNo = basePatch * BASEINTERVAL;
            //check local pkg /sobey/fics/update/fics_v1.0.0_2015.10.14(*)_4005 暂时只检查目录不检查tar.gz的压缩包 fics_v1.0.0_date(skip)_4005
            // i ==> patchNum
            sprintf(tmpPath, "fics_%s_*_%d", version, patchNo+i);//正则
            for (itr = pathDirList.begin(); itr!= pathDirList.end(); itr++)
            {
                if (matchRE(itr->c_str(), tmpPath))
                {
                    exist = true;
                    break;
                }
            }
            if (!exist)//目录不存在 1. get dir name. 2. cmp file name
            {
                lossPatchs[i] = 1;
            }
        }
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
    patchSet_t patchs;
    patchSet_t lossPatchs;
    

    FiUpdateAssistant::getinstance()->queryPatchs(patchs);
    if (0 == comparePatchs(FiUpdateAssistant::getinstance()->netVer, patchs, &lossPatchs))
    {
        ut_dbg("my patchs is the same as server, dont need to down\n");
    }
    else
    {
        ut_dbg("I have lost some patchs waiting for downing\n");
        i = 4;
        while (i-- && ret!=0)
        {
            if ((ret = downLossPatch(FiUpdateAssistant::getinstance()->netVer, lossPatchs)) < 0)
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
