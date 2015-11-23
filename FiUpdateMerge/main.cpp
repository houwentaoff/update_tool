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
#include <algorithm> 
#include <utility.h>
#include <patch.h>
#include <include.h>
#include <libgen.h>
#include <getopt.h>
#include <unistd.h>
#include "pkg.h"

using namespace std;

#define DEFAULT_PREFIX              "fics"            /*  */
#define DEFAULT_PATH            "./"/*  */
#define FICS_TMP_PATH           "./fics_tmp/"
static struct
{
    char version[128];//no default
    char prefix[128];//default:fics
    int patch_begin;//从版本号begin合并到end
    int patch_end;
    char patch_path[256];//补丁包位置
    char dst[256];//产生全版本的位置
    char date[128];
    int gen_patch;
    bool debug;
    bool nozip;
}merge_params;
static const char* program_name;
static struct option long_options[] = {
    {"help", 0, NULL, 'h'}, 
    {"version", 1, NULL, 'v'}, 
    {"begin", 1, NULL, 'b'}, 
    {"end", 1, NULL, 'e'}, 
    {"path", 1, NULL, 'p'}, 
    {"dest", 1, NULL, 'd'}, 
    {"prefix", 1, NULL, '~'}, 
    {"date", 1, NULL, '!'}, 
    {"patch", 1, NULL, '@'}, 
    {"debug", 0, NULL, '#'}, 
    {"nozip", 0, NULL, '$'}, 
    {0, 0, 0, 0}    
};
static const char *short_options = "hv:b:e:p:d:~:@:#$";

static void print_usage(FILE *f, int exit_code)
{
    fprintf(f, "\nUsage: %s [options] ...\n", 
        program_name);
    fprintf(f,
            "\t -h  --help         \n"
            "\t -v  --version       \n"
            "\t -b  --begin   begin patch num    \n"
            "\t -e  --end     end patch num  \n"
            "\t --prefix     default \"fics\".  \n"
            "\t --patch      patch num of zip default(the base verion of(end patch num)+%d).  \n"
            "\t --date      default (now) pkg time eg:2015.12.13.       \n"
            "\t -p  --path  (patch path)       default:\"./\"\n"
            "\t -d  --dest  (full version path) default:\"./\"      \n"
            "\t --debug    (debug mode)       \n"
            "\t --nozip    (dont unzip files)use orgin.       \n"
            "\t example:%s  --version 1.0.0 -b 4000 -e 4090 --date 2015.11.19 --patch 4100 -p ./ -d ./\n",
            BASEINTERVAL,
            program_name
            );
    exit(exit_code);
}
static int init_params(int argc, char **argv)
{
    int ch;
    int option_index = 0;
    time_t timeSt;
    struct tm *tp=NULL;

    opterr = 0;
    program_name = argv[0];

    if (argc < 2)
    {
        return -1;
    }
    while ((ch = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (ch) {
        case 'h':
            print_usage(stderr, 0);
            break;
        case 'v':
            strcpy(merge_params.version, "v");
            strcat(merge_params.version, optarg);
            break;
        case 'b':
            merge_params.patch_begin = atoi(optarg);
            break;
        case 'e':
            merge_params.patch_end = atoi(optarg);
            break;
        case 'p':
            strcpy(merge_params.patch_path, optarg);
            break;
        case 'd':
            strcpy(merge_params.dst, optarg);
            break;
        case '~':
            strcpy(merge_params.prefix, optarg);
            break;
        case '!':
            strcpy(merge_params.date, optarg);
            break;
        case '@':
            merge_params.gen_patch = atoi(optarg);
            break;
        case '#':
            merge_params.debug = true;
            break;
        case '$':
            merge_params.nozip = true;
            break;
        default:
            return -1;
            break;            
        }
    }
    if (strlen(merge_params.date) == 0)
    {
        time(&timeSt);
        tp = gmtime(&timeSt);
        if (!tp)
        {
            ut_err("get time err\n");
            return -1;
        }
        sprintf(merge_params.date, "%d.%.2d.%.2d", (1900+tp->tm_year), (1+tp->tm_mon), tp->tm_mday);
        ut_dbg("using default time %d.%.2d.%.2d\n", 1900+tp->tm_year, (1+tp->tm_mon), tp->tm_mday);
    }
    if (strlen(merge_params.dst) == 0)
    {
        strcpy(merge_params.dst, DEFAULT_PATH);
        ut_dbg("load default dest path: %s\n", DEFAULT_PATH);
    }
    if (strlen(merge_params.prefix) == 0)
    {
        strcpy(merge_params.prefix, DEFAULT_PREFIX);
        ut_dbg("load default prefix: %s\n", DEFAULT_PREFIX);
    }
    if (strlen(merge_params.patch_path) == 0)
    {
        strcpy(merge_params.patch_path, DEFAULT_PATH);
        ut_dbg("load default src path: %s\n", DEFAULT_PATH);
    }
    if (merge_params.patch_end <= 0 || merge_params.patch_begin <= 0)
    {
        return -1;
    }
    if (merge_params.gen_patch == 0)
    {
        merge_params.gen_patch = merge_params.patch_end+BASEINTERVAL - (merge_params.patch_end+BASEINTERVAL)%BASEINTERVAL;
        ut_dbg("load default dst patch num : %d\n", merge_params.gen_patch);
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
void printEleStr(const string &l)
{
    cout<<l<<endl;
}
#if 0
bool operator<(const Pkg_t& lhs, const Pkg_t& rhs)
{
        return lhs.pkgName<rhs.pkgName;
}
#endif
int setTarName(string &tarName, const char *date, const int patchNo)
{
    char buf[256] = {0};
    char tarNameBuf[256] = {0};
    char *prefix = NULL;
    char *oldDate = NULL;
    char *oldPatch = NULL;
    char *oldVersion = NULL;
    char *suffix = NULL;
    
    strcpy(buf, tarName.c_str());
    prefix = buf;
    oldVersion = strchr(buf, '_');
    if (!oldVersion)
    {
        ut_err("set tar version\n");
        goto err;
    }
    *oldVersion = '\0';
    oldDate = strchr(oldVersion+1, '_');
    if (!oldDate)
    {
        ut_err("set tar  date\n");
        goto err;
    }
    oldPatch = strchr(oldDate+1, '_');
    if (!oldPatch)
    {
        ut_err("set tar  patch\n");
        goto err;
    }
    *oldPatch = '\0';
    while (*(++oldPatch)!='_'){;}
    suffix = oldPatch+1;
    sprintf(tarNameBuf, "%s_%s_%d_%s",
        prefix, date, patchNo, suffix);
    tarName = tarNameBuf;
    return 0;
err:
    return -1;
}
/**
 * @brief //tar -zxvf ./fics_v1.0.0_2015.11.18_4040/client_v1.0.0_2015.11.16_4000_64_Linux2.6.tar.gz -C dst client_v1.0.0_2015.11.16_4000_64_Linux2.6/update/A --strip-components 1
 *          将ele->file 从./ele->pkgName/ele->tarName 解压到dst目录下的tarName中
 *
 * @param ele
 * @param dst
 * @param tarName
 *
 * @return 
 */
int cpFileFromTar2Dst(const pkg_ele_t &ele, const char *dst, const char *tarName)
{
    char cmdBuf[512]={0};
    char srcDirTmp[256]={0};
    char dstDirTmp[256]={0};
    const char *stripParam = "--strip-components 1";
    char *p = NULL;
    int ret = -1;

    strcpy(srcDirTmp, ele.tarName.c_str());
    strcpy(dstDirTmp, tarName);
    //strip suffix
    p = strstr(srcDirTmp, ".tar.gz");
    if(!p)
    {
        ut_err("cpFileFromTar2Dst err.\n");
        return -1;
    }
    *p = '\0';
    p = NULL;
    p = strstr(dstDirTmp, ".tar.gz");
    if(!p)
    {
        ut_err("cpFileFromTar2Dst err.\n");
        return -1;
    }
    *p = '\0';    

    sprintf(cmdBuf, "mkdir -p %s/%s ;tar -zxvf %s/%s -C %s/%s %s%s   %s",
            dst, dstDirTmp, ele.pkgName.c_str(), ele.tarName.c_str(),
            dst, dstDirTmp, srcDirTmp, ele.file.c_str(), stripParam);
    ret = system(cmdBuf);
    if (ret!=0)
    {
        ret = -1;
    }
    return ret;
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
    set<Pkg_t, myequal> masFileSet;
    set<Pkg_t, myequal> cliWinFileSet;
    set<Pkg_t, myequal> cliLinuxFileSet;
    set<Pkg_t, myequal> pkgFileSet;
    Pkg_t pkg;
    vector<string> pathList;
    set<pkg_ele_t> ret;
    set<pkg_ele_t>::iterator eleIt;
    set<Pkg_t, myequal>::iterator it;
    vector<string>::iterator itr;
    version_t ver;
    string dirNameTmp;
    char buf[256]={0};
    /*-----------------------------------------------------------------------------
     *  1.server_......tar.gz
     *-----------------------------------------------------------------------------*/
    const char *kinds[]={"server", "client", "Massvr", NULL};
    const char *bits[]={"32", "64", NULL};
    const char *plantform[]={"Win", "Linux", NULL};
    char suffix[256] = {0};
    int i,j,k=0;
    char cmdBuf[256] = {0};
    char zipDir[512]={0};
    char dirTmp[256];
    string zipFullPath;
    char *p = NULL;

    memset(&ver, 0, sizeof(version_t));
    memset(&merge_params, 0, sizeof(merge_params));
    INIT_G();
    //G.
    if (init_params(argc, argv)<0)
    {
        print_usage(stderr, 1);
        return -1;
    }
    if (getPkgList(merge_params.patch_path, merge_params.prefix, ".zip", merge_params.version, zipList) < 0)
    {
        ut_err("get pkg list fail\n");
    }
    if (!merge_params.nozip)
    {
        //unzip zip
        printf("---------begin unzip-------- \n");
        for (itr = zipList.begin(); itr<zipList.end();i++,itr++)
        {
            memset(&ver, 0, sizeof(version_t));
            getVerFromName(zipList[i].c_str(), &ver);
            if ((atoi(ver.patchNo) >= merge_params.patch_begin 
                        && atoi(ver.patchNo) <= merge_params.patch_end)
                    && (strcmp(ver.version, merge_params.version)==0)
                )
        {
            zipFullPath = merge_params.patch_path;
            zipFullPath +='/';
            zipFullPath += *itr;
            unzip(zipFullPath.c_str(), merge_params.dst);
        }
    }
    printf("---------unzip end---------- \n");
    }
    system("rm "FICS_TMP_PATH" -rf");
//    "server 64/32 linux2.6"
//    "client 64/32 linux2.6/Win"
//    "Massvr 64/32 linux2.6"
    for (k=0; kinds[k]!=NULL; k++)
    {
        for (i=0; bits[i]!=NULL; i++)
        {
            for (j=0; plantform[j]!=NULL; j++)
            {
                sprintf(suffix, "%s_%s*.tar.gz", bits[i], plantform[j]);
                pathList.clear();
                if (getPkgList(merge_params.patch_path, kinds[k], suffix, merge_params.version, pathList) < 0)
                {
                    ut_err("get %s %s tar.gz fail.\n", kinds[k], suffix);
                    continue;
                }
                if (pathList.empty())
                {
                    continue;
                }
                sort(pathList.begin(), pathList.end());
                ut_dbg("get %s %s tar.gz .\n", kinds[k], suffix);
                for_each(pathList.begin(), pathList.end(), printEleStr);
                for (int i=0; i<pathList.size(); i++)
                {
                    memset(&ver, 0, sizeof(version_t));
                    getVerFromName(pathList[i].c_str(), &ver);
                    //日期和 zip不匹配的 删除掉
                    if ((atoi(ver.patchNo) >= merge_params.patch_begin)&&
                                atoi(ver.patchNo) <= merge_params.patch_end)
                    {
                        sprintf(buf, "%s/%s_%s_%s_%s/", merge_params.patch_path, merge_params.prefix, ver.version, ver.date, ver.patchNo);
                        dirNameTmp = buf;
                        pathList[i].insert(0, dirNameTmp);
                    }
                    else
                    {
                        pathList.erase(pathList.begin()+i);
                        i--;
                    }
                }                
                pkgFileSet.clear();
                ret.clear();
                for (int i = 0; i<pathList.size(); i++)
                {
                    pkg.clear();
                    praseTargz(pathList[i].c_str(), pkg);//write -> pkg
                    if (!pkg.empty())
                    {
                        pkgFileSet.insert(pkg);
                    }
                }                
                //相邻集合求并集
                for(it = pkgFileSet.begin(); it!=pkgFileSet.end(); it++)
                {
                    _set_union(*it, ret);
                }
                printf("---------display need file list begin---------- \n");
                for_each(ret.begin(), ret.end(), printEle);
                printf("---------display need file list end  ---------- \n");
                string tarName;
                tarName = basename((char *)pathList[0].c_str());//full path
                setTarName(tarName, merge_params.date, merge_params.gen_patch);
                system("mkdir -p "FICS_TMP_PATH);
                //mkdir tar gz
                printf("---------get file from tar.gz begin ---------- \n");
                for(eleIt = ret.begin(); eleIt!=ret.end(); eleIt++)
                {
                    if (cpFileFromTar2Dst(*eleIt, FICS_TMP_PATH, tarName.c_str()) < 0)//tar -zvf *.tar.gz -C ./tmp etc/file
                    {
                        ut_err("cpFileFromTar2Dst fail.\n");
                        goto err;
                    }
                }                
                printf("---------get file from tar.gz end ------------ \n");
                //auto generate xml
                strcpy(dirTmp, tarName.c_str());
                p = strstr(dirTmp, ".tar.gz");
                if (p)
                {
                    *p = '\0';
                }
#if 1
                printf("---------merge update.xml from tar.gz begin ---------- \n");
                vector<patchEle_t> list,xmlRet;
                char xml_path[256]={0};
                for(it = pkgFileSet.begin(); it!=pkgFileSet.end(); it++)
                {
                    list.clear();
                    pkg_ele_t ele(it->pkgName, "/update.xml", it->tarName);
                    if (cpFileFromTar2Dst(ele, FICS_TMP_PATH, tarName.c_str()) < 0)
                    {
                        ut_err("cpFileFromTar2Dst update fail.\n");
                        goto err;
                    }
                    sprintf(xml_path, FICS_TMP_PATH"%s/update.xml", dirTmp);
                    load_xml(list, xml_path);
                    mergeUpdateXml(list, xmlRet);
                }
                mk_xml(xmlRet, xml_path);
                printf("---------merge update.xml from tar.gz end ------------ \n");
#endif
                //tar -zcvf ./tmp/tarName -> ./tmp/tarName.tar.gz

                printf("----------tar    file    begin --------------- \n");
                sprintf(cmdBuf, "tar -zcvf "FICS_TMP_PATH"%s  "FICS_TMP_PATH"%s --transform s="FICS_TMP_PATH"==", tarName.c_str(), dirTmp);
                if (0 != system(cmdBuf))
                {
                    ut_err("tar %s -> %s fail!!!!\n", dirTmp, tarName.c_str());
                    goto err;
                }
                printf("-----------tar    file    end ---------------- \n");
                //rm ./tmp/tarName
                sprintf(cmdBuf, "rm "FICS_TMP_PATH"%s -rf", dirTmp);
                system(cmdBuf);
            }
        }
    }
    //mv ./tmp -> fics_v1.0.0_12.12_4002
    sprintf(zipDir, "%s_%s_%s_%d", merge_params.prefix,
            merge_params.version, merge_params.date, merge_params.gen_patch);
    sprintf(cmdBuf, "rm %s -rf;mv "FICS_TMP_PATH" %s", zipDir, zipDir);//fics_v1.0.0_date_5000
    system(cmdBuf);
    //zip fics_v1.0.0_12.12_4002.zip
    printf("----------zip    file    begin --------------- \n");
    sprintf(cmdBuf, "zip -r %s.zip %s", zipDir, zipDir);
    system(cmdBuf);
    printf("-----------zip    file    end ---------------- \n");
    //rm fics_v1.0.0_12.12_4002
    //tar -zxvf .... -> .tar.gz

    return 0;
err:
    return -1;
}

