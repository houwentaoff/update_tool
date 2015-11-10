#include "../comm/FiRpcSrv.h"
#include "FiUpdateMgrImpl.h"
#include "../comm/utility.h"
#include "../comm/include.h"
#ifdef WIN32
#include "../comm/Markup_win32.h"
#else
#include <sys/time.h>
#include <sys/resource.h>
#include "../comm/Markup_linux.h"
#endif
#include <string>
#include "../comm/utility.h"

#include <sys/utsname.h>
#include <getopt.h>
#include "FiUpdateAssistant.h"
#include "SAXParserHandler.h"
#ifndef WIN32
#include <errno.h>

#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MCAST_PORT 3702
#define MCAST_ADDR "239.255.255.250"     /**/
#define MCAST_DATA "Sean: BROADCAST TEST DATA"             /*多播发送的数据*/
#define MCAST_INTERVAL 1                            /*发送间隔时间*/

#endif

#define  FiEnableCoreDumps()\
{\
    struct rlimit   limit;\
    limit.rlim_cur = RLIM_INFINITY;\
    limit.rlim_max = RLIM_INFINITY;\
    setrlimit(RLIMIT_CORE, &limit);\
}
#define updatePkg(ver)   updateLocal(ver)          /*  */
/* The name of this program. */
static FiRpcSrv<FiUpdateMgrImpl> *UpdateSrv = NULL;   
static const char* program_name;
static struct option long_options[] = {
    {"master", 0, NULL, 'm'}, 
    {"help", 0, NULL, 'h'}, 
    {0, 0, 0, 0}    
};
static const char *short_options = "hm";
static struct {
    dev_e type; 
    bool master;
}umparams;

typedef enum
{
    LEADER=0,/*fics update leader:fics下载机的leader*/
    FOLLOWER,  /*fics update fllower:fics下载机的follower*/ 
}idType_e;

typedef struct
{
    char ip[40];//server ip
    char port[30];//open port RPC_PORT
    char nodeId[30];//master:0 other:-1
    idType_e type;
}endPoint_t;
typedef struct
{
    char action[30];//action:hello
    char version[50];//fics_v2.1.0_2015.12.19_[1-100]
    endPoint_t endPoint;
    char reserved[40];//reserved data
}helloType_t;

extern int gen_optional_pack_name(const ::PlatformInfoEx& PInfo,std::vector<std::string>& names);

static void print_usage(FILE *f, int exit_code)
{
    fprintf(f, "\nUsage: %s [options] ...\n", 
        program_name);
    fprintf(f,
            "\t -h  --help         \n"
            "\t -m  --master(leader)       \n"
            "\t example:%s  --master \n",
            program_name
            );
    exit(exit_code);
}

static int init_param(int argc, char **argv)
{
    int ch;
    int option_index = 0;

    opterr = 0;
    program_name = argv[0];
    while ((ch = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (ch) {
        case 'h':
            print_usage(stderr, 0);
            break;
        case 'm':    
            umparams.type = MAS_SRV;
            umparams.master = true;
            break;
        default:
            return -1;
            break;
        }
    }
    return 0;
}
bool updateLocal(version_t *pver)
{
    PlatformInfoEx info;
    std::string osmode;
    struct utsname uts;
	std::vector<std::string> optinalname;
    int ret;
    char *p;
    char *ver = pver->version;
    char *date = pver->date;
    char *patchNo = pver->patchNo;
    int kind = 0;
    std::string name;
    std::string nameTmp;
    bool exist = false;
    char fullName[256]={0};
    char downPkgDir[256]={0};

    if (!pver)
    {
        ut_err("ver is null\n");
        goto err;
    }
    uname(&uts);
    //info.OSRunMode = sizeof(long)*8;
    p =strstr(uts.release,".");//2.6.38-8-generic,we need only 2.6
    if( p!= NULL ) {
        ++p;
        p =strstr(p,".");
        if(p != NULL) {
            *p=0;
            info.OSPackName = (const char*)(uts.release);
        }
    }
    if(info.OSPackName.in()==NULL)
    {
        info.OSPackName="";
    }
    osmode = uts.machine;
    if( osmode.find("x86_64") != std::string::npos)
    {
        info.OSRunMode = 64;
    }
    else
    {
        info.OSRunMode = 32;
    }
    info.platform = FiUpdateAssistant::ELINUX;
    info.OSName ="Linux";
    ut_dbg("fiupdate loader get config platform:linux %d bit\n",info.OSRunMode);
    fflush(stdout);    
    //find location.
    struct stat statBuf;
    strcpy(fullName, _PATH_PKG_DL);
    sprintf(fullName+strlen(fullName), "fics_%s_%s_%s",
            pver->version, pver->date, pver->patchNo);
    sprintf(downPkgDir, "%sfics_%s_%s_%s",
            _PATH_PKG_DL, pver->version, pver->date, pver->patchNo);
    if (-1 == lstat(fullName, &statBuf))
    {
        ut_err("does the pkg dir not exist?\n");
        goto err;
    }
    if (!S_ISDIR(statBuf.st_mode))
    {
        ut_err("pkg is not dir\n");
        goto err;
    }
	gen_optional_pack_name(info, optinalname);
    name = "Massvr_";
    name += ver;
    name += "_";
    name += date;
    name += "_";
    name += patchNo;
    nameTmp = name;
    name += optinalname[0];
    for (kind = 0; kind < optinalname.size();kind++)
    {
        name = nameTmp + optinalname[kind];
        sprintf(fullName, "%s/%s", downPkgDir, name.c_str());
        if (FiIsExistFile(fullName))
        {
            exist = true;
            break;
        }
        else
        {
            ut_err("tar is not exist[%s] try Compatible version!!\n", fullName);
            continue;
        }
    }
    if (!exist)
    {
        ut_err("all compatible tar is not exist\n");
        goto err;
    }
    //trans to curDir
    char cmdBuf[512];

    sprintf(cmdBuf, "cp %s " _PATH_PKG_DL "../update/", fullName);
    FiExecuteShell(cmdBuf);
    //begin to install
//    FiUpdateAssistant::getinstance()->setFile(name);
//    ret = FiUpdateAssistant::getinstance()->svc();
//    ut_dbg("install ret[%d]\n", ret);
    return true;
err:
    return false;
}
void *selfUpdate(void *params)
{
    version_t newVer;
    version_t localVer;
    int ret;
    
    memset(&newVer, 0, sizeof(version_t));
    memset(&localVer, 0, sizeof(version_t));
    getLocalVersion(&localVer);
    FiUpdateAssistant::getinstance()->setPlatform(MAS_SRV);
    /*-----------------------------------------------------------------------------
     *  watch patch_version if changed.
     *-----------------------------------------------------------------------------*/
    while (true)
    {
        sleep(1);
        getLocalVersion(&newVer);
        ret = compareVersion(&localVer, &newVer);        
        if (ret == 0 )
        {
            continue;
        }
        else if (ret < 0)
        {
            //update local
            if (!updatePkg(&newVer))
            {
                ut_dbg("update pkg fail patchNo:%s !!!! no pkg valid ?\n", newVer.patchNo);
            }
            memcpy((void *)&FiUpdateAssistant::getinstance()->netVer,
                    (void *)&newVer, sizeof(version_t));
            if (0 != FiUpdateAssistant::getinstance()->installAllPatch(&localVer))
            {
                FiUpdateAssistant::getinstance()->restartAPP("fics");
                goto exit;
            }
            else
            {
                memcpy((void *)&localVer, (void *)&newVer, sizeof(version_t));
                ut_dbg("update down\n");
            }
        }
        else if (ret > 0 && ret != 1000000)
        {
            //rollback
            memcpy((void *)&FiUpdateAssistant::getinstance()->netVer,
                    (void *)&newVer, sizeof(version_t));
            FiUpdateAssistant::getinstance()->RollBack(&newVer);
            FiUpdateAssistant::getinstance()->restartAPP("fics");
            goto exit;
        }
    }
    return NULL;
exit:
    UpdateSrv->shutdown();
    exit(0);
    return NULL;
}

int init_helloType(helloType_t *hello)
{
    if (!hello)
    {
        ut_err("hello is null\n");
        return -1;
    }
    memset(hello, 0, sizeof(hello));
    strcpy(hello->action, "action:hello");
    strcpy(hello->version, "version:");
    strcpy(hello->endPoint.ip, "ip:");
    strcpy(hello->endPoint.port, "port:");
    strcpy(hello->endPoint.nodeId, "nodeid:");
    
    return 0;
}
/**
 * @brief 接收master/其他server 的心跳包，更新download下的当前zip全版本安装包。master只发心跳包，不接收(master是所有的源)。
 *
 * @param params
 *
 * @return 
 */
void* recvHelloHandler(void *params)
{
    int s;                                       
    struct sockaddr_in local_addr; 
    version_t localVer;
    int err = -1;

    memset(&localVer, 0, sizeof(localVer));
    s = socket(AF_INET, SOCK_DGRAM, 0);     
    if (s == -1)
    {
        perror("socket()");
        return (void *)-1;
    }   

    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(MCAST_PORT);

    /*绑定socket*/
    err = bind(s,(struct sockaddr*)&local_addr, sizeof(local_addr)) ;
    if(err < 0)
    {
        ut_err("bind()fail errno[%d]\n", errno);
        return (void *)-2;
    }

    /*设置回环许可*/
    int loop = 0;//禁止
    err = setsockopt(s,IPPROTO_IP, IP_MULTICAST_LOOP,&loop, sizeof(loop));
    if(err < 0)
    {
        ut_err("setsockopt():IP_MULTICAST_LOOP\n");
        return (void *)-3;
    }

    struct ip_mreq mreq;                                     /*加入广播组*/
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);   /*广播地址*/
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);   /*网络接口为默认*/
    /*将本机加入广播组*/
    err = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof
            (mreq));
    if (err < 0)
    {
        ut_err("setsockopt():IP_ADD_MEMBERSHIP\n");
        return (void *)-4;
    }

    int addr_len = 0;
    helloType_t buff;
    int n = 0;
    while (1)
    {
        addr_len = sizeof(local_addr);
        memset(&buff, 0, sizeof(buff));                  /*清空接收缓冲区*/
        /*接收数据*/
        n = recvfrom(s, &buff, sizeof(buff), 0,(struct sockaddr*)&local_addr,
                &addr_len);
        if( n == -1)
        {
            ut_err("recvfrom()\n");
        }
        if (strcmp(buff.action, "action:hello") != 0 || buff.endPoint.type != LEADER)
        {
            continue;
        }
        /*打印信息*/
        ut_dbg("Recv hello message from server:%s\n", buff.version);
        getLocalVersion(&localVer);
        version_t net;
        int ret;
        memset(&net, 0, sizeof(net));
        char *version = strstr(buff.version, "_v");//version:fics_v2.1.0_2015.09.04
        if (version)version++;
        char *date = strchr(version, '_');
        if (date)*date++='\0';
        char *patchNo = strchr(date, '_');
        if (patchNo)patchNo++;
        if (version)strcpy(net.version, version);
        if (date)strcpy(net.date, date);
        if (patchNo)strcpy(net.patchNo, patchNo);
        ret = compareVersion(&localVer, &net);
        if ( ret!= 0 && ret!=1000000)
        {
            char *ip = strstr(buff.endPoint.ip, "ip:");
            ip = ip+strlen("ip:");
            char srcFile[256]={0};
            char cmdBuf[250]={0};
            sprintf(srcFile, "/sobey/fics/download/fics_%s_%s", net.version, net.date);
            if (patchNo)sprintf(srcFile+strlen(srcFile), "_%s", patchNo);
            strcat(srcFile, ".zip");
            //down file
            ret = FiUpdateAssistant::getinstance()->downloadFile(ip, srcFile, srcFile);
            if (ret == 0)
            {
                ut_dbg("download file success\n");
                //modify local patch_version
                char patchVerBuf[200];

                sprintf(cmdBuf, "unzip %s -d %s", srcFile, _PATH_PKG_DL);
                system(cmdBuf);//unzip pkg
                writeLocalVer(&net);
                //overwrite patch_version
            }
            else
            {
                ut_err("download file fail\n");
                sprintf(cmdBuf, "rm %s -rf", srcFile);
                system(cmdBuf);
            }
        }
        sleep(MCAST_INTERVAL);
    }

    /*退出广播组*/
    err = setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP,&mreq, sizeof
            (mreq));

    close(s);
    return NULL;
}
int sendHello()
{
    int s;
    int err=0;
    helloType_t hello;
    version_t localVer;
    struct sockaddr_in mcast_addr;      
    std::string localIp;

    init_helloType(&hello);
    memset(&localVer, 0, sizeof(version_t));

    if (getCurLocalIp(localIp) < 0)
    {
        ut_err("get local ip fail\n");
        return -1;
    }
    getLocalVersion(&localVer);

    s = socket(AF_INET, SOCK_DGRAM, 0);         
    if (s == -1)
    {
        ut_err("socket() fail\n");
        return -1;
    }
//fics
    sprintf(hello.version+strlen(hello.version), "fics_%s_%s",
        localVer.version, localVer.date);
    strcat(hello.endPoint.ip, localIp.c_str());
    sprintf(hello.endPoint.port+strlen(hello.endPoint.port), "%d",
        RPC_PORT);
    sprintf(hello.endPoint.nodeId+strlen(hello.endPoint.nodeId),"%d",
        umparams.master ? 0 : -1);//need to modify
    hello.endPoint.type = umparams.master == true ? LEADER : FOLLOWER;
    memset(&mcast_addr, 0, sizeof(mcast_addr));/*初始化IP多播地址为0*/
    mcast_addr.sin_family = AF_INET;                 /*设置协议族类行为AF*/
    mcast_addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);/*设置多播IP地址*/
    mcast_addr.sin_port = htons(MCAST_PORT);         /*设置多播端口*/

    
#if 0
    err = bind(s,(struct sockaddr*)&mcast_addr, sizeof(mcast_addr)) ;
    if(err < 0)
    {
        perror("bind()");
        return -2;
    }
#endif
    int n;
    /*向多播地址发送数据*/
    do
    {
        n = sendto(s,                            /*套接字描述符*/
                &hello,      /*数据*/
                sizeof(hello),      /*长度*/
                0,
                (struct sockaddr*)&mcast_addr,
                sizeof(mcast_addr)) ;
        if( n < 0)
        {
            ut_err("sendto() fail errno:%d\n", errno);
        }       

        sleep(MCAST_INTERVAL);                           /*等待一段时间*/
    }while(-1 == n);
    close(s);
    
    return 0;
}
void *heartHandler(void *params)
{
    while(1)
    {
        sleep(60);
        if (sendHello()<0)
        {
            ut_err("send hello fail\n");
        }
    }
    return NULL;
}
int main(int argc,char** argv)
{
	char _path[260]; 
    FiEvent evnt;
    
 
// 	char buf[100];
// 	FiGetCurDir(sizeof(buf),buf);
// 	std::string strNetConfig =buf;
// 	strNetConfig +="../Config/NetConfig.xml";
// 	std::string ret("127.0.0.1");
// 	CMarkup xmlLoader;
// 	xmlLoader.Load(strNetConfig.c_str());
// 	while(xmlLoader.FindChildElem("LocalItem"))
// 	{
// 		xmlLoader.IntoElem();
// 
// 		if(xmlLoader.FindChildElem("LocalIP"))
// 		{
// 			xmlLoader.IntoElem();
// 			ret = xmlLoader.GetData();
// 			xmlLoader.OutOfElem();
// 		}
// 
// 		xmlLoader.OutOfElem();
// 	}
	FiGetCurDir(sizeof(_path),_path);
	std::string str(_path);
	str+="FiUpdateMgr.log";
	freopen(str.c_str(),"a",stdout);
#if 0
    if (0 == chdir(_path))
    {
        ut_dbg("change cur dir success\n");
    }
    else
    {
        ut_err("change cur dir fail\n");
    }    
#endif
#ifndef WIN32
    FiEnableCoreDumps();
    pthread_t tid[3];
	pthread_attr_t attr[3];
    struct stat st;
    umparams.type = SERVER;//default type
    umparams.master == false;

    FiUpdateAssistant::getinstance()->set(&evnt);
    
    if (init_param(argc, argv) < 0) 
    {
        print_usage(stderr, 1);
        return -1;
    }
    if (stat("../download", &st) == -1)
    {
        ut_err("stat error num :%d\n", errno);
    }
    else
    {
        if (!S_ISDIR(st.st_mode))
        {
            system("rm ../download -rf");
        }
    }
    system("mkdir -p ../download");
	if (pthread_attr_init(&attr[0]) < 0)
	{
		ut_err("set attr fail\n");
	}
	if (pthread_attr_init(&attr[1]) < 0)
	{
		ut_err("set attr fail\n");
	}
	if (pthread_attr_init(&attr[2]) < 0)
	{
		ut_err("set attr fail\n");
	}
	pthread_attr_setdetachstate(&attr[0], PTHREAD_CREATE_DETACHED);
	pthread_attr_setdetachstate(&attr[1], PTHREAD_CREATE_DETACHED);
	pthread_attr_setdetachstate(&attr[2], PTHREAD_CREATE_DETACHED);
    if (sendHello()<0)
    {
        ut_err("send hello fail\n");
    }

	pthread_create(&tid[0], &attr[0], selfUpdate, NULL);
    if (umparams.master == false)
    {
        pthread_create(&tid[1], &attr[1], recvHelloHandler, NULL);
    }
    else
    {
        pthread_create(&tid[2], &attr[2], heartHandler, NULL);
    }
#endif


    UpdateSrv = new FiRpcSrv<FiUpdateMgrImpl>(RPC_PORT, RPC_SERVER_NAME, (char*)(NULL));
	UpdateSrv->run();
#if 0	
	evnt.wait();
#else
    do
    {
#ifdef WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }while(true);
#endif
	
	return 0;	
}
