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
 * @param ret
 *
 * @return 
 */
int _set_difference(const Pkg_t &pkg, set<pkg_ele_t> &rSet, vector<string> &ret)
{
    return 0;
}
bool Pkg_t::empty()
{
    return list.empty();
}
void Pkg_t::clear()
{
    return list.clear();
}
