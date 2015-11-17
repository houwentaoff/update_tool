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
#include <algorithm> 
#include <set>
#include <vector>
using namespace std;
class Pkg_t
{
    public:
        Pkg_t(){};
        Pkg_t(string p, string t, set<string> &path):tarName(t), pkgName(p), list(path){};
        bool empty();
        void clear();
    public:
        string pkgName;
        string tarName;
        set <string> list;
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
        string tarName;
        string pkgName;
        string file;
};
/**
 * @brief old升级包和，高版本升级包的差集
 *        差集的值表示要安装的文件
 *
 * @param pkg
 * @param ret
 *
 * @return 
 */
int _set_difference(const Pkg_t &pkg, set<pkg_ele_t> &rSet, vector<string> &ret);
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
#endif
