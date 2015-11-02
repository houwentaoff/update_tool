/*
 * =====================================================================================
 *       Copyright (c), 2013-2020, Sobey.
 *       Filename:  main.cpp
 *
 *    Description:  补丁包打包程序
 *         Others:
 *
 *        Version:  1.0
 *        Created:  2015/11/2 11:40:40
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Sean. Hou (hwt), houwentaoff@gmail.com
 *   Organization:  Sobey
 *
 * =====================================================================================
 */


#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <getopt.h>
#include "string.h"
#include "include.h"
#include "utility.h"

using namespace std;

#define LEN_NAME            128/*  */

static const char* program_name;
static struct option long_options[] = {
    {"version", 1, NULL, 'v'}, 
    {"date", 1, NULL, 'd'}, 
    {"pn", 1, NULL, '!'}, 
    {"platform", 1, NULL, '@'}, 
    {"kernel", 1, NULL, 'k'}, 
    {"bit", 1, NULL, 'b'}, 
    {"conf", 1, NULL, 'f'}, 
    {"c", 0, NULL, 'c'}, 
    {"s", 0, NULL, 's'}, 
    {"m", 0, NULL, 'm'}, 
    {"format", 1, NULL, '#'}, 
    {"path", 1, NULL, 'P'}, 
    {"debug", 0, NULL, 'D'}, 
    {"help", 0, NULL, 'h'}, 
    {0, 0, 0, 0}    
};
static const char *short_options = "v:d:!:@:#:k:b:f:csmP:hD";
typedef enum{
    TAR_GZ,
    ZIP,
}format_e;
typedef enum{
    LINUX,
    WINDOWS,
}platform_e;
typedef enum{
    SERVER_,
    CLIENT_,
    MASSVER_,
}p_type_e;
typedef enum{
    _BIT64=0,
    _BIT32,
}bit_e;
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

static struct {
    version_t version;
    platform_e pf_type;
    bit_e bit; 
    p_type_e ptype;
    format_e format;
    bool debug;
    char path[256];
    char kernel[32];
    char conf_file[256];
}upparams;

typedef struct patchEle
{
    char src_name [LEN_NAME];
    char dst_name [LEN_NAME];
    action_e action;
    file_type_e type;
}patchEle_t;

static void print_usage(FILE *f, int exit_code)
{
    fprintf(f, "\nUsage: %s [options] ...\n", 
        program_name);
    fprintf(f,
            "\t -v              --version         \n"
            "\t -d              --date         \n"
            "\t --pn            patch num         \n"
            "\t --platform      linux/win         \n"
            "\t -k              --kernel :kernel version 2.6/3.2        \n"
            "\t -b              --bit    :bit 32/64  default(64)       \n"
            "\t -f              config file:update.conf default(ser_linux_update.conf/cli_linux_update.conf/cli_win_update.conf)        \n"
            "\t -c/s/m          client/server/mas         \n"
            "\t --format        pkg format:tar.gz  default(tar.gz)       \n"
            "\t -P              path of release.        default(./) \n"
            "\t -D              debug         \n"
            "\t example:linux server:%s -v 1.0.0 -d 2015.09.14 --pn 4000 -f ./ser_linux_update.conf -P /fics_release/ -b 32 -k 2.6 --platform linux  -s \n"
            "\t example:linux master:%s -v 1.0.0 -d 2015.09.14 --pn 4000 -f ./mas_linux_update.conf -P /fics_release/ -b 32 -k 2.6 --platform linux  -m\n"
            "\t example:windows client:%s -v 1.0.0 -d 2015.09.14 --pn 4000 -f ./cli_win_update.conf -P /fics_release/ -b 32 --platform win \n"
            "\t example:linux client:%s -v 1.0.0 -d 2015.09.14 --pn 4000 -f ./cli_linux_update.conf -P /fics_release/ -b 64 -k 2.6 --platform linux  -c \n"
            "\t example:zip:%s --format zip -v 1.0.0 -d 2015.09.14 --pn 4000 -P ./ \n",
            program_name, program_name, program_name, program_name, program_name);
    exit(exit_code);
}
int load_conf(char const *conf, vector<patchEle_t> &list)
{
    FILE *fp = NULL;
    int r = 0;
    patchEle_t eleTmp;
    char buf[100] = {0};

    if (!conf)
    {
        ut_err("conf is null!!!\n");
        goto err;
    }
    fp = fopen(conf, "r");
    if (fp == NULL)
    {
        ut_err("open fail\n");
        goto err;
    }
    while ((r = fscanf(fp, "%s", buf)) == 1)
    {
        if (buf[0] == '#')
        {
            if (fscanf(fp, "%*[^\n]\n") < 0)//skip # comment
            {
                ut_err("skip # fail\n");
                goto err;
            }
            continue;
        }
        strcpy(eleTmp.src_name, buf);
        r = fscanf(fp, "%*[ ]%s", buf);
        if (r!=1)
        {
            ut_err("conf is correct?");
            goto conf_err;
        }
        switch (buf[0])
        {
            case 'M':
                eleTmp.action = MDF;
                break;
            case 'A':
                eleTmp.action = ADD;
                break;
            case 'D':
                eleTmp.action = DEL;
                break;
            default:
                ut_err("unknow id in %s\n", conf);
                break;
        }
        r = fscanf(fp, "%*[ ]%[^\n]s", buf);
        if (r!=1)
        {
            goto conf_err;
        }
        strcpy(eleTmp.dst_name, buf);
        //insert to xml
        fgetc(fp);
        list.push_back(eleTmp);
    }
    fclose(fp);
  
    return 0;
conf_err:
    ut_err("conf is correct?");
err:
    if (fp)fclose(fp);
    return -1;    
}
int mk_xml(vector<patchEle_t> &list)
{
    FILE *fp;
    const char *xml_name = "update.xml";
    char *p;
    char eleBuf[512] = {0};
    std::vector<patchEle_t>::iterator itr;
    patchEle_t *pele;
    
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

    return 0;
err:
    return -1;
}
int init_params(int argc, char *params[])
{
    int ch;
    int option_index = 0;

    opterr = 0;
    program_name = params[0];    
    while ((ch = getopt_long(argc, params, short_options, long_options, &option_index)) != -1) {
        switch (ch) {
        case 'D':
            upparams.debug = true;
            break;
        case 'h':
            print_usage(stderr, 0);
            break;
        case 'v':    
            strcpy(upparams.version.version, optarg);
            break;
        case 'd':
            strcpy(upparams.version.date, optarg);
            break;
        case '!'://pn
            strcpy(upparams.version.patchNo, optarg);
            break;
        case '@'://platform
            if (strcmp(optarg, "linux") == 0)
            {
                upparams.pf_type = LINUX;
            }
            else if (strcmp(optarg, "windows") == 0)
            {
                upparams.pf_type = WINDOWS;
            }else
            {
                ut_err("unknow command\n");
                goto err;
            }
            break;
        case 'k'://kernel
            strcpy(upparams.kernel, optarg);
            break;
        case 'b'://bit
            if (strcmp(optarg, "32") == 0)
            {
                upparams.bit = _BIT32;
            }
            else if (strcmp(optarg, "64"))
            {
                upparams.bit = _BIT64;
            }
            else
            {
                ut_err("unknow command\n");
                goto err;
            }
            break;
        case 'f'://config file
            strcpy(upparams.conf_file, optarg);
            break;
        case 'c'://client/server/mas 
            upparams.ptype = CLIENT_;
            break;
        case 's'://client/server/mas 
            upparams.ptype = SERVER_;
            break;
        case 'm'://client/server/mas 
            upparams.ptype = MASSVER_;
            break;
        case '#'://format
            if (strcmp(optarg, "zip") == 0)
            {
                upparams.format = ZIP;
            }
            else if (strcmp(optarg, "tar.gz"))
            {
                upparams.format = TAR_GZ;
            }
            else
            {
                ut_err("unknow command\n");
                goto err;
            }
            break;
        case 'P'://path
            strcpy(upparams.path, optarg);
            break;
        default:
            return -1;
            break;
        }
    }    
    if (strlen(upparams.path) == 0)
    {
        strcpy(upparams.path, "./");
    }
    if (upparams.pf_type == WINDOWS)
    {
        upparams.ptype = CLIENT_;
    }
    return 0;   
err:
    return -1;
}
int tar_pkg(vector<patchEle_t> &list)
{
    char cmd_buf[512] = {0};
    char const *prefix[]={"server", "client", "Massvr", NULL};
    char const *bits[]={"64","32", NULL};
    char const *platform[] = {"Linux", "WIN", NULL};
    char const *format[] = {"tar.gz", "zip", NULL};
    char tmp_path[512]={0};
    std::vector<patchEle_t>::iterator itr;

    sprintf(tmp_path, "/tmp/%s_%s_%s_%s_%s_%s",
            prefix[upparams.ptype], upparams.version.version,
            upparams.version.date, upparams.version.patchNo,
            bits[upparams.bit], platform[upparams.pf_type]
            );
    if (upparams.pf_type == LINUX)
    {
        sprintf(&tmp_path[strlen(tmp_path)], "%s", upparams.kernel);
    }
 
    sprintf(cmd_buf, "mkdir -p %s", tmp_path);
    system(cmd_buf);
    //mkdir -p /tmp/server_v1.0.0_2015.09.14_4000_64_Linux2.6.tar.gz
//    load_xml();
    for (itr=list.begin(); itr != list.end(); itr++)
    {
        //cp upparams.path/itr->src_name      tmp_path/itr->src_name      -rf
        sprintf(cmd_buf, 
                "cp %s/%s %s/  -rf", 
                upparams.path,
                itr->src_name,
                tmp_path);
        system(cmd_buf);
    }
    //tar pkg
    //1. tar -zcvf tmp_path.tar.gz tmp_path
    //2. rm  tmp_path
    sprintf(cmd_buf, "tar -zcvf %s.tar.gz %s",
            tmp_path, tmp_path);
    system(cmd_buf);
    if (!upparams.debug)
    {
        sprintf(cmd_buf, "rm %s -rf", tmp_path);
        system(cmd_buf);
    }
    return 0;
}
int zip_pkg()
{
    char dir_name[512] = {0};
    /*-----------------------------------------------------------------------------
     * rm dir_name -rf
     * mkdir -p dirname
     *  cp  -a server_version_*.tar.gz/ -rf
     *  cp  -a Massvr_version_*.tar.gz/ -rf
     *  cp  -a client_$version_*_4002.tar.gz  ./test/  -rf
     *-----------------------------------------------------------------------------*/
    char cmd_buf[512] = {0};
    char re_file[512] = {0};//file RE 

    sprintf(dir_name, "fics_%s_%s_%s",
            upparams.version.version,
            upparams.version.date,
            upparams.version.patchNo);
    sprintf(cmd_buf, "rm %s -rf", dir_name);
    system(cmd_buf);
    sprintf(cmd_buf, "mkdir -p %s", dir_name);
    system(cmd_buf);

    sprintf(re_file, "server_%s_*_%s*",
            upparams.version.version,
            upparams.version.patchNo);
    sprintf(cmd_buf, "cp -a %s %s", re_file, dir_name);
    system(cmd_buf);
    sprintf(re_file, "Massvr_%s_*_%s*",
            upparams.version.version,
            upparams.version.patchNo);
    sprintf(cmd_buf, "cp -a %s %s", re_file, dir_name);
    system(cmd_buf);
    sprintf(re_file, "client_%s_*_%s*",
            upparams.version.version,
            upparams.version.patchNo);
    sprintf(cmd_buf, "cp -a %s %s", re_file, dir_name);    
    system(cmd_buf);
    sprintf(cmd_buf, "zip -r \"%s.zip\" %s", dir_name, dir_name);
    system(cmd_buf);

    return 0;
}
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
    vector<patchEle_t> list;
    
    memset(&upparams, 0, sizeof(upparams));

    if (init_params(argc, argv)<0)
    {
        print_usage(stderr, 1);
    }

    if (upparams.format == TAR_GZ)
    {
        if (load_conf(upparams.conf_file, list)<0)
        {
            ut_err("load conf fail\n");
            goto err;
        }
        if (mk_xml(list)<0)
        {
            ut_err("mk xml fail\n");
            goto err;
        }        
        printf("mk xml success!!!\n");
        tar_pkg(list);
    }
    else if (upparams.format == ZIP)
    {
        zip_pkg();
        printf("mk zip success!!!\n");
    }
    return EXIT_SUCCESS;
err:
    return -1;
}