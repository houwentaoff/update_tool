/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Sobey.
 *       Filename:  patch.h
 *
 *    Description:  升级工具2.0的功能，可以补丁回退和升级
 *         Others:
 *
 *        Version:  1.0
 *        Created:  2015/10/21 15:24:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sean. Hou (hwt), houwentaoff@gmail.com
 *   Organization:  Sobey
 *
 * =====================================================================================
 */
#ifndef __PATCH_H__
#define __PATCH_H__
#include "include.h"
#include "utility.h"
#include <vector>
#include <string>
#include "FiUpdateAssistant.h"

#define      BASEINTERVAL       100/* 补丁号间隔为100 则为base全版本 */
#ifndef WIN32
#define HISTORY        "/sobey/fics/history"            /*  */
#else
#define HISTORY        "../history"            /*  */
#endif

using namespace std;
//typedef long patchSet_t[100][2];
/**
 * @brief 从补丁号中获取全版本号
 *        如 补丁号为259 则其全版本号为200,59 为补丁号
 *
 * @param patchNum
 *
 * @return 
 */
int getCurBaseVer(int patch);
/**
 * @brief 获取当前的补丁包号
 *
 * @param patch
 *
 * @return 
 */
int getCurPatchVer(int patch);
/**
 * @brief 根据目录列表(补丁库所在目录)产生hash值， hash值将放入patch_version文件，用于上传旧补丁更新版本库，而版本号并不发生改变 时通知客户端更新
 *
 * @param verList
 * @param hashValue
 *
 * @return 
 */
int genHashValueFromDir(vector<string> & verList, string & hashValue);//not use
/**
 * @brief  根据history文件产生hash值， hash值将放入patch_version文件，用于上传旧补丁更新版本库，而版本号并不发生改变 时通知客户端更新。
 *
 * @param path      操作升级/回退，记录信息的history文件
 * @param hashValue
 *
 * @return 
 */
int genHashFromHis(const char *path, string & hashValue);
/**
 * @brief 将当前上传的包名字 放入历史记录文件中，方便查看历史记录
 *
 * @param newVer 版本名（不包含hash的版本名）
 * @param path   history文件路径
 *
 * @return 
 */
int addVer2His(version_t *newVer, const char * path);
/**
 * @brief 从指定的目录(/sobey/fics/down /sobey/fics/update/) 获取当前补丁包的所有路径列表
 *
 * @param path
 * @param verList
 *
 * @return 
 */
int scanAllPatch(const char *path, vector<string> & verList);//not use?

/**
 * @brief 是否安装base版本
 * 1. 若base不一样则需要安装新base
 * 2. 一样则不需要安装base
 *
 * @return true: 应该安装。 false:不应该安装
 */
bool shouldInsBase(int localBase, int remoteBase);
/**
 * @brief 是否应该安装补丁包 如果该补丁包已经被高版本覆盖则不需要安装：更多的情况是安装补丁包中的一部分
 *
 * @return 
 */
bool shouldInsPatch(int patchNum);

/**
 * @brief 安装补丁包 : 只安装比当前版本新的
 *
 * @param patchNum
 *
 * @return 
 */
bool installPatch(int patchNum);

/**
 * @brief 安装指定全版本，前提是该全版本文件已经被down下来了，并且已被解压?
 *
 * @param baseNum
 *
 * @return 
 */
bool installBase(int baseNum);

/**
 * @brief 检查所有服务器所有补丁包，客户端是否都存在，不存在则返回不存在的补丁包号码
 *
 * @param patchSet[BASEINTERVAL]     扫描服务端所有的补丁包列表[0-100]
 * @param need2down[BASEINTERVAL]    返回需要下载的补丁包列表
 *
 * @return true: 不用下载 need2down为空 false:需要下载，并返回需要下载补丁号列表
 */
bool checkAllpatchIsIn(const int patchSet[BASEINTERVAL], int (*need2down)[BASEINTERVAL]);//not use
/**
 * @brief 比较服务器的Patchs和本地的包进行对比，并将缺少的包标记到 lossPatchs中
 * 代替了checkAllpatchIsIn
 *
 * @param serPatchs
 * @param lossPatchs
 *
 * @return 
 */
int comparePatchs(version_t *netVer, patchSet_t serPatchs, patchSet_t  lossPatchs);
/**
 * @brief 对比服务器检查本地的包是否有缺少有则下载
 *
 * @return 
 */
int checkAndDownPkg();
/**
 * @brief 将整数 20140510转化为 2014.05.10
 *
 * @param ldate
 * @param sdate
 *
 * @return 
 */
int long2Date(long ldate, string &sdate);
/**
 * @brief 将字符串 2014.05.10 转化为整数20140510
 *
 * @param date
 *
 * @return 
 */
long date2Long(const char *date);
/**
 * @brief获取指定目录的目录列表
 *
 * @param path
 * @param dirList
 *
 * @return 
 */
int getDirList(char const * path, vector<string> & dirList);
/**
 * @brief      比较字符串和正则表达式是否匹配
 *          如: fics_v1.0.0_2015.10.10_4005 用 fics_v1.0.0_*_4005 匹配成功 
 * @param src   原始字符串
 * @param re  对应的正则表达式
 *
 * @return 
 */
bool matchRE(const char *src, const char *re);
/**
 * @brief 下载loss的包 version -> netVer ;date -> lossPatchs[i][1] fics_v1.0.0_2015.10.10_4005
 *
 * @param netVer
 * @param lossPatchs
 *
 * @return 
 */
int downLossPatch(version_t *netVer, patchSet_t lossPatchs);
/**
 * @brief 
 *
 * @param lossPatchs
 * @param set
 *
 * @return 
 */
int patchs2str(patchSet_t *lossPatchs, string &set);
/**
 * @brief int  to asic
 *
 * @param inum
 * @param str
 *
 * @return 
 */
int itoa(int inum, string &str);

/**
 * @brief 根据组版本号和patchnum生成版本号
 *
 * @param baseVer
 * @param patchVer
 *
 * @return 
 */
int mkVer(int baseVer, int patchVer);
/**
 * @brief   server_v1.1.0_2015.09.29_4005_64_Linux2.6.tar.gz
 *
 * @param tarName
 *
 * @return 
 */
int getPatchNumFromName(char const *tarName);
/**
 * @brief 根据传入的v1.0.0获取包列表
 *
 * @param path
 * @param prefix
 * @param suffix
 * @param version 不能为NULL?服务端version
 * @param pkgList
 *
 * @return 
 */
int getPkgList(char const * path, char const * prefix, char const *suffix, const char *version, vector<string> & pkgList);
int gen_optional_pack_name(const ::PlatformInfoEx& PInfo,std::vector<std::string>& names);/*根据客户端传来的平台参数判断客户端需要的安装包的后缀*/

/**
 * @brief  根据文件或者目录名（fileName） 获取当前文件对应的版本号(无hash)
 *
 * @param fileName
 * @param ver
 *
 * @return 
 */
int getVerFromName(const char *fileName, version_t *ver);
/**
 * @brief 从文件中获取md5值 先从cache中获取，失败则从文件中获取
 *
 * @param fileName
 * @param localMd5
 *
 * @return 0:success  -1:fail
 */
int getMD5FromLocal(const char *fileName, string &localMd5);
/**
 * @brief 从cache中获取文件的md5值
 *
 * @param filename
 * @param localMd5
 *
 * @return 
 */
int getMD5FromCache(const char* filename, string &localMd5);
/**
 * @brief 向cache中增加新MD5值
 *
 * @param filename
 * @param localMd5
 *
 * @return 
 */
int addEleMD52Cache(const char* filename, string &localMd5);
/**
 * @brief 删除map中文件对应的MD5值
 *
 * @param filename
 * @param localMd5
 *
 * @return 
 */
int delEleMD52Cache(const char* filename, string &localMd5);
#endif
