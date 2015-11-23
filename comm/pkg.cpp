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
#include "utility.h"
#include "SAXParserHandler.h"
#ifdef WIN32
#include <direct.h>
#else
#include <libgen.h>
#endif

int mergeUpdateXml(vector<patchEle_t> &list, vector<patchEle_t> &ret)
{
    vector<patchEle_t>::iterator it;
    vector<patchEle_t>::iterator result;
    if (list.empty())
    {
        ut_dbg("merge src is null\n");
        return 0;
    }
    for (it = list.begin(); it!=list.end(); it++)
    {
        result = find(ret.begin(), ret.end(), *it);
        if (result==ret.end())//not find
        {
            if (it->action == ADD || it->action == MDF)
            {
                it->action = ADD;
            }
            ret.push_back(*it);
        }
        else//find it
        {
            if (result->action != it->action)
            {
                //update action
                if (it->action == DEL)
                {
                    result->action = it->action;
                }
            }
        }
    }
    return 0;
}
int load_xml(vector<patchEle_t> &list, const char *xml_name)
{
    XmlParserEngine xmlParser;
    FiUpdat_Pack_t pack;
    std::vector<File_Layout_t>::iterator it;
    patchEle_t tmp;

    if (!xml_name)
    {
        ut_err("xml is null\n");
        goto err;
    }
    xmlParser.load(xml_name);
    pack = xmlParser.pack();
    ut_dbg("try to load %d items\n",pack.allFiles.size());
    for(it=pack.allFiles.begin(); it!=pack.allFiles.end(); it++)
    {
        strcpy(tmp.src_name, it->strName.c_str());
        strcpy(tmp.dst_name, it->strLocation.c_str());
        switch (it->strAction[0])
        {
            case 'M':
                tmp.action = MDF;
                break;
            case 'A':
                tmp.action = ADD;
                break;
            case 'D':
                tmp.action = DEL;
                break;
            default:
                ut_err("unknow id in %s\n", xml_name);
                break;
        }
        tmp.type = EXE;
        list.push_back(tmp);
    }
    return 0;
err:
    return -1;
}
int mk_xml(vector<patchEle_t> &list, const char *xml_name)
{
    FILE *fp;
    char *p;
    char eleBuf[512] = {0};
    std::vector<patchEle_t>::iterator itr;
    patchEle_t *pele;

    if (!xml_name)
    {
        ut_err("xml name is null\n");
        goto err;
    }
    
    fp = fopen(xml_name, "wb");
    if(fp ==NULL)
    {
        ut_err("mk xml fail\n");
        goto err;
    }
    fputs("<fiupdate>", fp);
    fputs("\n", fp);
    for (itr=list.begin(); itr!=list.end(); itr++)
    {
        pele = &(*itr);
        memset(eleBuf, 0, sizeof(eleBuf));
//        std::string record = isreg?"<reg name=\"":"<file name=\"";
        sprintf(eleBuf, "<file name=\"%s\" action=\"%c\" location=\"%s\"/>",
                pele->src_name,
                (pele->action == ADD ? 'A' : pele->action == MDF ? 'M':'D'),
                pele->dst_name);
        fputs(eleBuf, fp);
        fputs("\n", fp);
    }
    fputs("</fiupdate>", fp);
    fputs("\n", fp);
    fclose(fp);

    return 0;
err:
    return -1;
}
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
 * @param pkg    当前补丁包的集合
 * @param rSet   并集结果(高版本的文件集合)
 * @param pkgRet 返回pkg - rSet的结果
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
int praseDirTargz(const char *path, Pkg_t &pkg)
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
        sprintf(cmdBuf, "find ./ -name *");
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
