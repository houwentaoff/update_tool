/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Sobey.
 *       Filename:  main.cpp
 *
 *    Description:  合并补丁包为全版本
 *         Others:
 *
 *        Version:  1.0
 *        Created:  2015/11/12 18:17:51
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sean. Hou (hwt), houwentaoff@gmail.com
 *   Organization:  Sobey
 *
 * =====================================================================================
 */

#include <set>
#include <iostream>
#include <string>
#include<algorithm> 

using namespace std;

class Pkg_t
{
    public:
        Pkg_t();
        Pkg_t(string name, set<string> &path):pkgName(name), list(path){}
    public:
        string pkgName;
        set <string> list;
};

class pkg_ele_t
{
    public:
        pkg_ele_t(){};
        pkg_ele_t(string name, string f):pkgName(name), file(f){}
        bool operator==(const pkg_ele_t& ele) const
        {
            return (file==ele.file);
        }
        bool operator<(const pkg_ele_t& ele) const//只能小于不能大于否则集合编译不通过 find 函数需要这
        {
            return (file<ele.file);
        }        
        string pkgName;
        string file;
};
/**
 * @brief 集合的并集运算
 *
 * @param list1
 * @param list2
 * @param ret
 *
 * @return 
 */
static int set_union(const Pkg_t &pkg, set<pkg_ele_t> &ret)
{
    set<string>::iterator it;
    pkg_ele_t pkg_ele;

    for (it = pkg.list.begin(); it != pkg.list.end(); it++)
    {
        //exist: who is newer. newer -> ret
        pkg_ele = pkg_ele_t(pkg.pkgName, *it);
        if (ret.find(pkg_ele) != ret.end())
        {
//            cout<<"erase:"<<pkg.pkgName<<":"<<*it<<"!!!!!!!"<<endl;
            ret.erase(pkg_ele);
        }
        ret.insert(pkg_ele);
    }
    return 0;
}
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
void printEle(const pkg_ele_t &l)
{
    cout<<l.pkgName<<":"<<l.file<<endl;
}
#if 0
bool operator<(const Pkg_t& lhs, const Pkg_t& rhs)
{
        return lhs.pkgName<rhs.pkgName;
}
#endif
int command(const char *cmd, vector<string> &content)
{
    FILE *fp = NULL;
    char tmppath[256]={0};

    if (NULL ==(fp = popen(cmd, "r")))
    {
        ut_err("popen fail\n");
        goto err;
    }//加一个判断错误的 mark
    while (fscanf(fp, "%[^\n]s", tmppath) == 1)
    {
        fgetc(fp);
        pkgList.push_back(tmppath);
    }
    pclose(fp);
    return 0;
err:
    return -1;
}
/**
 * @brief 将包中的文件填入ｓｅｔ　ｐkg_t中
 *         tar -tf a.tar.gz
 * @param path
 * @param pkg
 *
 * @return 
 */
int praseTargz(const char *path, Pkg_t &pkg)
{
    char  cmdBuf[256]={0};
    char dirTmp[256]={0};
    set<string> fileList;//file -> fileList

    if (!path)
    {
        ut_err("path is null\n");
        goto err;
    }
    strcpy(dirTmp, path);
    dirname(dirTmp);
    basename(dirTmp);
    sprintf(cmdBuf, "tar -tf %s", path);
    command(cmdBuf, fileList);
    for_each(fileList.begin(), fileList.end(), );//将vector中的string全部插入集合set中。
    pkg.pkgName = dirTmp;
    return 0;
err:
    return -1;
}
int string2set(const string &s, )
{
    s
    return 0;
}
/*-----------------------------------------------------------------------------
 *  1. 重定义masFileSet集合的并操作
 *       pkgName大的则合并:以大的进行合并，合并的时候舍弃小的。
 *  
 *-----------------------------------------------------------------------------*/
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{

    /*-----------------------------------------------------------------------------
     *  1. list all tar.gz file  -> //uncompress all pkg
     *  2. set_union(List_A.begin(), List_A.end(), List_B.begin(), List_B.end(), List_C.begin());
     *-----------------------------------------------------------------------------*/
    vector<string> zipList;
    if (getPkgList("./", "fics", ".zip", "v1.0.0", zipList) < 0)
    {
        ut_err("get pkg list fail\n");
    }
    //unzip zip
    vector<string>::iterator it;
    for (it = zipList.begin(); it<zipList.size();it++)
    {
        unzip(it->c_str(), "./");
    }
    set<string> tmp1;
    set<string> tmp2;
    set<string> tmp3;
    set<Pkg_t, myequal> masFileSet;
    set<Pkg_t, myequal> cliWinFileSet;
    set<Pkg_t, myequal> cliLinuxFileSet;
    set<Pkg_t, myequal> serFileSet;
//    set<pkg_ele_t> retSerFileSet;

    tmp1.insert("libs/aee.so");//Add / Modify
    tmp1.insert("libs/b.so");
    tmp1.insert("libs/c.so");
    tmp1.insert("libs/d.so");
    tmp1.insert("libs/e.so");
    tmp1.insert("libs/f.so");

    tmp2.insert("libs/aee.so");//Add / Modify
    tmp2.insert("libs/b.so");
    tmp2.insert("libs/c.so");
    tmp2.insert("libs/d.so");
    tmp2.insert("libs/e.so");

    tmp3.insert("libs/aee.so");//Add / Modify
    tmp3.insert("libs/bee.so");
    tmp3.insert("libs/cee.so");
    tmp3.insert("libs/dee.so");

    /*-----------------------------------------------------------------------------
     *  1.server_......tar.gz
     *-----------------------------------------------------------------------------*/
//    vector<Pkg_t> pkgList;
    Pkg_t pkg;
    vector<string> pathList;
    if (getPkgList("./", "server_", ".tar.gz", pathList) < 0)
    {
        ut_err("get tar.gz fail.\n");
        goto err;
    }
    for (i = 0; i<pathList.size(); i++)
    {
        pkg.clear();
        praseTargz(pathList[i].c_str(), pkg);//write -> pkg
        if (!pkg.empty())
        {
            serFileSet.insert(pkg);
        }
//        pkgList.push_back(pkg);
    }

    set<pkg_ele_t> ret;
    set<Pkg_t, myequal>::iterator it;
    //相邻集合求并集
    serFileSet.insert(s1);
    serFileSet.insert(s2);
    serFileSet.insert(s3);
    for(it = serFileSet.begin(); it!=serFileSet.end(); it++)
    {
        set_union(*it, ret);
    }
    for_each(ret.begin(), ret.end(), printEle);
    return 0;
}

