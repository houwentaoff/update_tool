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

#include <vector>
#include <string>

#define      BASEINTERVAL       100/* 补丁号间隔为100 则为base全版本 */

#define HISTORY        "/sobey/fics/history"            /*  */

using namespace std;
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
int genHashValueFromDir(vector<string> & verList, string & hashValue);
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
int scanAllPatch(const char *path, vector<string> & verList);

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
bool checkAllpatchIsIn(const int patchSet[BASEINTERVAL], int (*need2down)[]);

#endif
