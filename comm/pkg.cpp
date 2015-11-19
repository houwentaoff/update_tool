/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Sobey.
 *       Filename:  pkg.cpp
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  2015/11/17 9:54:53
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sean. Hou (hwt), houwentaoff@gmail.com
 *   Organization:  Sobey
 *
 * =====================================================================================
 */
#include "pkg.h"
#include "include.h"
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include "utility.h"

/**
 * @brief 集合的并集运算
 *
 * @param pkg
 * @param ret
 *
 * @return 
 */
int _set_union(const Pkg_t &pkg, set<pkg_ele_t> &ret)
{
    set<string>::const_iterator it;
    pkg_ele_t pkg_ele;

    for (it = pkg.list.begin(); it != pkg.list.end(); it++)
    {
        //exist: who is newer. newer -> ret
        pkg_ele = pkg_ele_t(pkg.pkgName, *it, pkg.tarName);
        if (ret.find(pkg_ele) != ret.end())
        {
//            cout<<"erase:"<<pkg.pkgName<<":"<<*it<<"!!!!!!!"<<endl;
            ret.erase(pkg_ele);
        }
        ret.insert(pkg_ele);
    }
    return 0;
}
/**
 * @brief old升级包和，高版本升级包的差集
 *        差集的值表示要安装的文件
 *
 * @param pkg
 * @param rSet
 * @param pkgRet
 * 
 * @sa    _set_union
 * @return 
 */
int _set_difference(const Pkg_t &pkg, const set<pkg_ele_t> &rSet, Pkg_t &pkgRet)
{
    set<string>::const_iterator it;
    pkg_ele_t pkg_ele;
    for (it = pkg.list.begin(); it!=pkg.list.end(); it++)
    {
        pkg_ele = pkg_ele_t(pkg.pkgName, *it, pkg.tarName);
        if (rSet.find(pkg_ele) == rSet.end())
        {
            pkgRet.list.insert(*it);
        }
    }
    pkgRet.tarName = pkg.tarName;
    return 0;
}
int command(const char *cmd, vector<string> &content)
{
    FILE *fp = NULL;
    char tmppath[256]={0};
    int ret = -1;

    if (NULL ==(fp = popen(cmd, "r")))
    {
        ut_err("popen fail\n");
        goto err;
    }//加一个判断错误的 mark
    while (fscanf(fp, "%[^\n]s", tmppath) == 1)
    {
        fgetc(fp);
        content.push_back(strchr(tmppath,'/'));//skip first dir
    }
    ret = pclose(fp);
    return ret;
err:
    return ret;
}

int delDir(vector<string> &fileList)
{
    vector<string>::iterator it;
    for (it = fileList.begin(); it!= fileList.end(); it++)
    {
        if ((*it)[strlen(it->c_str()) - 1] == '/')
        {
            fileList.erase(it); 
            it--;
        }
    }
    return 0;
}
/**
 * @brief 将包中的文件填入set　ｐkg_t中
 *         tar -tf a.tar.gz
 * @param path
 * @param pkg
 *
 * @return 
 */
int praseTargz(const char *path, Pkg_t &pkg)
{
    char  cmdBuf[256]={0};
    char tmp[256]={0};
    char fileName[256]={0};
    char dirName[256]={0};
    int  ret = -1;
    char orginPwd[256]={0};
    vector<string> fileList;//file -> fileList

    if (!path)
    {
        ut_err("path is null\n");
        ret = -1;
        goto err;
    }
    strcpy(tmp, path);
    sprintf(fileName, "%s", basename(tmp));
    sprintf(dirName, "%s", dirname(tmp));
    getcwd(orginPwd, sizeof(orginPwd));
    ret = chdir(dirName);
    if (0 == ret)
    {
        sprintf(cmdBuf, "tar -tf %s", fileName);
        if (command(cmdBuf, fileList)!=0);
        {
            ret = -1;
        }
        chdir(orginPwd);
        delDir(fileList);
        pkg.list.insert(fileList.begin(), fileList.end());//将vector中的string全部插入集合set中。
        pkg.pkgName = dirName;
        pkg.tarName = fileName;
    }
    return ret;
err:
    return ret;
}

bool Pkg_t::empty()
{
    return list.empty();
}
void Pkg_t::clear()
{
    return list.clear();
}
