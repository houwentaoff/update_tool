/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Sobey.
 *       Filename:  pkg.h
 *
 *    Description:  
 *         Others:
 *
 *        Version:  1.0
 *        Created:  2015/11/17 9:54:30
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sean. Hou (hwt), houwentaoff@gmail.com
 *   Organization:  Sobey
 *
 * =====================================================================================
 */
#ifndef __PKG_H__
#define __PKG_H__
#include <string>
#include <string.h>
#include <algorithm> 
#include <set>
#include <vector>
#define LEN_NAME            128/*  */

using namespace std;
class Pkg_t
{
    public:
        Pkg_t(){};
        Pkg_t(string p, string t, set<string> &path):tarName(t), pkgName(p), list(path){};
        bool empty();
        void clear();
    public:
        string pkgName;//所在的包名 如 fics_v1.0.0_2015.12.10_4000
        string tarName;//tar.gz包名 如 server_v1.0.0_2015.12.10_4000_64_Linux2.6.tar.gz
        set <string> list;//包包含的文件名集合  
};
class pkg_ele_t
{
    public:
        pkg_ele_t(){};
        pkg_ele_t(string name, string f, string t):pkgName(name), file(f), tarName(t){};
        bool operator==(const pkg_ele_t& ele) const
        {
            return (file==ele.file);
        }
        bool operator<(const pkg_ele_t& ele) const//只能小于不能大于否则集合编译不通过 find 函数需要这
        {
            return (file<ele.file);
        }        
        string tarName;//所在的包名 如 fics_v1.0.0_2015.12.10_4000
        string pkgName;//tar.gz包名 如 server_v1.0.0_2015.12.10_4000_64_Linux2.6.tar.gz
        string file;//包中的一个文件名
};
class myequal 
{
    public:
        bool operator() (const Pkg_t &a, const Pkg_t &b) const 
        {
            if(a.pkgName < b.pkgName)
                return true;
            else
                return false;
        }
};
typedef enum{
    ADD=0,
    MDF,
    DEL,
}action_e;
typedef enum{
    EXE=0,
    SVC,
    TXT,
    APP,
}file_type_e;
typedef struct patchEle
{
    char src_name [LEN_NAME];
    char dst_name [LEN_NAME];
    action_e action;
    file_type_e type;
    bool operator ==(const patchEle &r)
    {
        return (strcmp(r.src_name, this->src_name) == 0)&&(strcmp(r.dst_name, this->dst_name) == 0);
    }
}patchEle_t;
/**
 * @brief old升级包和，高版本升级包的差集
 *        差集的值表示要安装的文件
 *
 * @param pkg
 * @param ret
 *
 * @return 
 */
int _set_difference(const Pkg_t &pkg, const set<pkg_ele_t> &rSet, Pkg_t &pkgRet);
/**
 * @brief 集合的并集运算
 *
 * @param list1
 * @param list2
 * @param ret
 *
 * @return 
 */
int _set_union(const Pkg_t &pkg, set<pkg_ele_t> &ret);
/**
 * @brief 将包中的文件填入set　ｐkg_t中
 *         tar -tf a.tar.gz
 * @param path
 * @param pkg
 *
 * @return 
 * @sa      praseDirTargz
 */
int praseTargz(const char *path, Pkg_t &pkg);
/**
 * @brief  根据list中的配置 生成update.xml
 *
 * @param list
 * @param xml_name
 *
 * @return 
 */
int mk_xml(vector<patchEle_t> &list, const char *xml_name, bool reboot = false);
/**
 * @brief 加载update.xml中的信息到list中
 *
 * @param list
 * @param xml_name
 *
 * @return 
 * @ 合并补丁包中使用，用于合并update.xml文件
 */
int load_xml(vector<patchEle_t> &list, const char *xml_name, bool &reboot);
/**
 * @brief 合并list中的内容到ret中
 *
 * @param list
 * @param ret
 *
 * @return 
 * @sa   在合并补丁包中使用，用于合并同类型补丁包中的update.xml元素
 */
int mergeUpdateXml(vector<patchEle_t> &list, vector<patchEle_t> &ret);
/**
 * @brief 
 *
 * @param path
 * @param pkg
 *
 * @return 
 * @sa      praseTargz
 */
int praseDirTargz(const char *path, Pkg_t &pkg);
#endif
