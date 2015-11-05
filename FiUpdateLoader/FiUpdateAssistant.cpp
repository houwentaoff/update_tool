#include "FiUpdateAssistant.h"
#include "../comm/utility.h"
#include "../comm/include.h"
#include "../comm/patch.h"
#include "SAXParserHandler.h"
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <libgen.h>
#endif

#ifdef _WIN32
#define ACCESS _access
#define MKDIR(a) _mkdir((a))
#else
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#endif
#include<stdio.h>
#include<stdlib.h>
#ifndef WIN32
#include<unistd.h>
#include <errno.h>
#endif
#include<string>
#include <assert.h>
#include<vector>
#include<fcntl.h>
#include<algorithm>
#pragma  warning(disable:4819)
#pragma  warning(disable:4996)
#define NOTIFY_UPDATELAOAD_FINISH_FAIL() \
    do{                             \
    bUpFinished = false;       \
    OnUpdateFinish();          \
    }while(0)   
#define NOTIFY_UPDATELAOAD_FINISH_SUCCESS() \
    do{                             \
    bUpFinished = true;       \
    OnUpdateFinish();          \
    }while(0)   


#define CHECK_SYSTEM_STATUS_FAIL_RETURN(EXPRESSION) \
    do{                                         \
    int status = -1;                            \
    status =(EXPRESSION);                       \
    if(  (status == -1)||(status==127) )        \
        {                                           \
        ut_dbg("system execute fail,at line %d",__LINE__);\
        fflush(stdout);                                \
        OnUpdateFinish();                        \
        return -20;                             \
        }                                           \
    }while(0)



struct FilePair 
{
    std::string file1;
    std::string file2;
};

const char *prixMatrix[] = {"server", "client", "Massvr", NULL};

FiUpdateAssistant::FiUpdateAssistant(FiEvent* hEvent)
:evnt(hEvent)
{
    _ref =0;
    
    rHandle =NULL;
    rHandle= NULL;
    fpdownload= NULL;
    bReplaceEnd= false;
    
    bUpFinished = false;
    totalPkg = 0;
    curCountInstalled = 0;
    installOver = false;

    memset(&localVer, 0, sizeof(version_t));
    memset(&netVer, 0, sizeof(version_t));

    //char* addr = "127.0.0.1";
    // updateimpl = new FiRpcSrv<FiUpdateLoaderImpl>(5795,"FiLoader",addr);
    // updateimpl->run();
}
FiUpdateAssistant::FiUpdateAssistant()
{
    _ref =0;
    
    rHandle =NULL;
    rHandle= NULL;
    fpdownload= NULL;
    bReplaceEnd= false;
    evnt = NULL;

    bUpFinished = false;
    totalPkg = 0;
    curCountInstalled = 0;
    installOver = false;
    memset(&localVer, 0, sizeof(version_t));
    memset(&netVer, 0, sizeof(version_t));
}
int FiUpdateAssistant::QueryPlatformInfo(PlatformInfoEx& info)
{
#ifdef WIN32
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 
    LPFN_ISWOW64PROCESS fnIsWow64Process; 
    BOOL bIsWow64 =FALSE;
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle("kernel32"),"IsWow64Process"); 
    if (NULL != fnIsWow64Process) 
    { 
        
        fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
    } 
    info.OSRunMode = (bIsWow64)?64:32;
    SYSTEM_INFO Oinfo;                                   //用SYSTEM_INFO结构判断64位AMD处理器 
    GetSystemInfo(&Oinfo);                               //调用GetSystemInfo函数填充结构 
    OSVERSIONINFOEX os; 
    os.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);   
    if(GetVersionEx((OSVERSIONINFO *)&os))                  
    { 
        CString vname; 
        //下面根据版本信息判断操作系统名称 
        switch(os.dwMajorVersion){                        //判断主版本号 
            case 4: 
                switch(os.dwMinorVersion){                //判断次版本号 
            case 0: 
                if(os.dwPlatformId==VER_PLATFORM_WIN32_NT) 
                    info.OSName="WinNT";                //1996年7月发布 
                else if(os.dwPlatformId==VER_PLATFORM_WIN32_WINDOWS) 
                    info.OSName="Win95"; 
                break; 
            case 10: 
                info.OSName="Win98"; 
                break; 
            case 90: 
                info.OSName="WinMe"; 
                break; 
                } 
                break; 
            case 5: 
                switch(os.dwMinorVersion){               //再比较dwMinorVersion的值 
            case 0: 
                info.OSName="Win2000";                    //1999年12月发布 
                break; 
            case 1: 
                info.OSName=_T("WinXP");                    //2001年8月发布 
                break; 
            case 2: 
                if(os.wProductType==VER_NT_WORKSTATION && 
                    Oinfo.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
                    info.OSName="WinXPProfessional"; 
                else if(GetSystemMetrics(89)==0) 
                    info.OSName="WinServer2003";        //2003年3月发布 
                else if(GetSystemMetrics(89)!=0) 
                {
                    info.OSName="WinServer2003";
                    info.OSPackName = "R2";
                }

                break; 
                } 
                break; 
            case 6: 
                switch(os.dwMinorVersion){ 
            case 0: 
                if(os.wProductType==VER_NT_WORKSTATION)
                    info.OSName="WinVista"; 
                else 
                    info.OSName="WinServer2008";          //服务器版本 
                break; 
            case 1: 
                if(os.wProductType==VER_NT_WORKSTATION) 
                    info.OSName="Win7"; 
                else 
                {
                    info.OSName="WinServer2008";
                    info.OSPackName="R2";
                }
                break; 
                } 
                break; 
            default:
                info.OSName="Win";
                break; 
        } 

    } 
    if (info.OSName.in()==NULL)
    {
        info.OSName="Win";
    }
    info.platform = EWINDOWS;
    //info.OSRunMode = sizeof(long)*8;
    if(info.OSPackName.in()==NULL)
    {
        info.OSPackName="";
    }
    ut_dbg("fiupdate loader get config platform:windows %d bit\n",info.OSRunMode);
    fflush(stdout);
#else//linux mac
    struct utsname uts;
    uname(&uts);
    //info.OSRunMode = sizeof(long)*8;
    char* p =strstr(uts.release,".");//2.6.38-8-generic,we need only 2.6
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
    std::string osmode = uts.machine;
    if( osmode.find("x86_64") != std::string::npos)
    {
        info.OSRunMode = 64;
    }
    else
    {
        info.OSRunMode = 32;
    }
    //info.OSPackName = uts.release;
#ifdef __linux__
    info.platform =ELINUX;
    info.OSName ="Linux";
    ut_dbg("fiupdate loader get config platform:linux %d bit\n",info.OSRunMode);
    fflush(stdout);
#else
    info.platform = EMAC;
    info.OSName="Mac";
    ut_dbg("fiupdate loader get config platform:mac %d bit\n",info.OSRunMode);
    fflush(stdout);
#endif
#endif

    return 0;

}
int FiUpdateAssistant::startup()
{
    XmlParserEngine xmlParser;
    char buf[200];
    FiGetCurDir(sizeof(buf),buf);
    std::string strWebDir =buf;
    std::string strNetConfig =buf;    
    strNetConfig +="../config/network.xml";

    xmlParser.load(strNetConfig.c_str());
#ifdef WIN32
    strWebDir +="../web/";
#else
    strWebDir +="../web";
#endif
    ut_dbg("FiUpdateAssistant::startup\n");
    fflush(stdout);

    if( ACCESS(strWebDir.c_str(),0) == -1)//web folder not exist
    {
        which = CLIENT;//fics client
    }
    else
    {
        which = SERVER;//mds
        xmlParser.load(strNetConfig.c_str());
        std::string loaclIp = xmlParser.GetEle("LocalIP");
        std::string svrIp = xmlParser.GetEle("UpMgrIp");
        std::string svrBackIp = xmlParser.GetEle("UpMgrBackIp");
        if (loaclIp == svrIp || loaclIp == svrBackIp)//node no = 0,1
        {
            which = MAS_SRV;
        }
    }
    QueryPlatformInfo(PInfo);
    return 0;
}
FiUpdateAssistant::~FiUpdateAssistant()
{
    bReplaceEnd = true;
    evtReplaceFile.signal();
}

int FiUpdateAssistant::DestroyConnectUpMgr()
{
    delete cli;
    cli = NULL;
    return 0;
}

bool FiUpdateAssistant::ConnectUpMgr()
{
    bool ret = false;
    cli = new FiRpcCli<FiUpdateMgr>;
    if( cli == NULL )
    {
        ret = false;
        return ret;
    }
    XmlParserEngine xmlParser;
    char buf[100];
    FiGetCurDir(sizeof(buf),buf);
    std::string strNetConfig =buf;
#ifdef WIN32
    strNetConfig +="../Config/network.xml";//sean
#else
    if(which==CLIENT)
    {
        strNetConfig +="../config/network.xml";
    }
    else
    {
        strNetConfig +="../config/network.xml";
    }
#endif
    xmlParser.load(strNetConfig.c_str());
    CMarkup xml;
    if (!xml.Load(strNetConfig.c_str()))
    {
        ut_err("load ficsconfig xml[%s] failed\n", strNetConfig.c_str());
        return false;
    }
    //xmlParser.GetMgrIp();
    if (xml.FindElem("UpMgrConfig"))
    {
        xml.IntoElem();
        while (xml.FindElem("UpMgrIp"))//find a node can connecting;find the newest version?
        {
            std::string ip = xml.GetData();
            xml.FindElem("UpMgrBackIp");
            std::string backIp = xml.GetData();

            ut_dbg("load mgr ip from %s and ip is %s \n",strNetConfig.c_str(),ip.c_str());
            fflush(stdout);
            if( ip.empty())
            {
                ut_err("there is no <UpMgrIp> in the xml config file\n");
                fflush(stdout);
                return false;
            }
            rHandle = cli->create_rpc_proxy(RPC_SERVER_NAME,(char*)(ip.c_str()), RPC_PORT);
            if(rHandle == NULL || !rHandle->isValidate)
            {
                ut_dbg("can not connect server %s with service %s at port %d\n", ip.c_str(), RPC_SERVER_NAME, RPC_PORT);
                ut_dbg("try to connect back server  %s\n", backIp.c_str());
                rHandle = cli->create_rpc_proxy(RPC_SERVER_NAME,(char*)(backIp.c_str()), RPC_PORT);
                if (!rHandle || !rHandle->isValidate)
                {
                    ut_dbg("can not connect server %s with service %s at port %d\n", backIp.c_str(), RPC_SERVER_NAME, RPC_PORT);            
                    ut_dbg("try others\n");
                }
                else
                {
                    ret = true;
                    break;
                }
            }
            else
            {
                ret = true;
                break;
            }
        }
    }
    if (ret == false)
    {
        ut_dbg("check the config pls\n");
    }
    return  rHandle->isValidate;
err:
    fflush(stdout);
    delete cli;
    rHandle = NULL;
    return false;      
}

int FiUpdateAssistant::downLossPkg(version_t *netVer, long ldate, int patchNo)
{
    char* filename;
    int ret =0;
    char* layout;
    int trycnt=0;
    ::CORBA::String_var outfilename;
    ::CORBA::Long size;
    string strDate;
    string strPatchNo;
    
    if (ldate == 0)
    {
        ut_err("ldate is null\n");
        return -1;
    }
    long2Date(ldate, strDate);
    itoa(patchNo, strPatchNo);
    ut_dbg("prepare to downPkg\n");
    fflush(stdout);

    while( (ret ==0)&& trycnt<2 )
    {
        trycnt++;
        if (trycnt > 1)
        {
            ut_dbg("Attempt to connect server [%d]\n" , trycnt);
        }
        MAKE_RPC_INVOKE(rHandle,StartupUpdate(PInfo, which, netVer->version , strDate.c_str(), strPatchNo.c_str(), outfilename, size, _ref),ret);
    }
    if( ret == 0)
    {
        ut_dbg("network crash,stopping update line:%d\n",__LINE__);
        fflush(stdout);
//        NOTIFY_UPDATELAOAD_FINISH_FAIL();
        return -7;
    }
    filename = outfilename.inout();
    if(_ref==0)
    {
        ut_dbg("no package available line:%d\n",__LINE__);
        fflush(stdout);
//        NOTIFY_UPDATELAOAD_FINISH_SUCCESS();
//        UpdateVersionFile();
        return 0;
    }

    this->filename = filename;
    //this->layout = layout;
    ut_dbg("start download file %s\n",filename);
    fflush(stdout);
    //CORBA::string_free(filename);
    // CORBA::string_free(layout);
    beginDownloadPkg(this->filename.c_str());
    return 0;  
}
int FiUpdateAssistant::beginDownloadPkg(const char* filename)
{
    int pack_expect =1;
    char buff[256];
    int ret =-1;

    ut_dbg("begin translate file:%s\n",filename);
    fflush(stdout);
    if( pack_expect==1 )
    {
        FiGetCurDir(sizeof(buff),buff);
        std::string fullname(buff);
#ifdef WIN32 
        char *suffix =".tar.gz";
#else
        char *suffix = ".tar.gz";
#endif
        char* p = strstr((char*)filename, suffix);
        assert(p!=NULL);
        this->dirfilename= std::string(filename,p-filename);
        char cmd[300];
#ifdef WIN32
        std::string tmp = fullname+dirfilename;
        sprintf(cmd,"rd /s/q %s\n",tmp.c_str());
        FiExecuteShell(cmd);
        tmp.clear();
        tmp=fullname+filename;
        sprintf(cmd,"del /s/q %s\n",tmp.c_str());
        FiExecuteShell(cmd);
#else
        std::string tmp=fullname+dirfilename;
        sprintf(cmd,"rm -rf %s\n",tmp.c_str());
        FiExecuteShell(cmd);
        FiExecuteShell(cmd);

        tmp = fullname + filename;
        sprintf(cmd,"rm -f %s\n",tmp.c_str());
        FiExecuteShell(cmd);
#endif
        fullname += filename;
        //fullname +=".tmp";
        ret = FiCreateFile(fullname.c_str());
        if( ret==-1 )
        {
            ut_dbg("can not create file named %s locally, make sure run as a root \n",fullname.c_str());
            fflush(stdout);
//            NOTIFY_UPDATELAOAD_FINISH_FAIL();
            return -1;
        }
        fpdownload = FiOpenExistFile(fullname.c_str());
    }

    do 
    {
        ::CORBA::Long flag;
        int trycnt=0;
        ret =0;
        bfstream_var bfs;
        ::CORBA::Long checknum;
 /* :TODO:2015/10/15 15:21:32:hwt:  trycnt = 5 改为 trycnt = 1，受系统TCP keeplive影响，一次默认为75s，5次时间太长,会导致迟迟没有更新 10 分钟*/
        while( (ret == 0)&& trycnt<2 )
        {
            trycnt++;
            if (trycnt > 1)
            {
                ut_dbg("Attempt to connect server pack_expect[%d][%d]\n" ,pack_expect, trycnt);
            }
            MAKE_RPC_INVOKE(rHandle,TranslateFile(_ref,pack_expect,bfs,checknum,flag),ret);
        }
 /* :TODO:End---  */
        if( ret ==0 )
        {
            ut_dbg("network crash,stopping update\n");
            fflush(stdout);
//            NOTIFY_UPDATELAOAD_FINISH_FAIL();
            FiCloseFile(fpdownload);
            return -34;
        }
        unsigned int chknum =0;
        unsigned int size = bfs->length();
        for(unsigned int i=0;i<size;++i)
        {
            chknum +=(unsigned int)(bfs[i]);
        }
        if(chknum ==checknum)
        {
            int wsize =FiWriteFile(fpdownload,(char*)(&bfs[0]),size);
            assert(wsize==size);//need to modify hwt
            pack_expect++;
            if( flag )
            {
                FiCloseFile(fpdownload);
                ret = 0;
                //down file over
//                ret=svc();
                break;
            }
        }
        else
        {
            ut_dbg("check num is  incorrect,redownload package[%d]!\n",pack_expect);
            fflush(stdout);
            ret = -1;
            //return 0;
        }

    } while (true);

    return ret;
}
static bool comp(string a, string b)
{
    return a<b;
}
/**
 * @brief 安装大于当前版本的所有patch
 *
 * @param version
 * @param lossPatchs
 *
 * @return 已完成更新包的个数
 */
int FiUpdateAssistant::installAllPatch(version_t *ver)
{

    /*-----------------------------------------------------------------------------
     *  1. scan all tar.gz/dir
     *  2. sort 
     *  3. install tar.gz/dir
     *-----------------------------------------------------------------------------*/
    string fileName;
    vector<string> pkgList;
    char *suffix        = ".tar.gz";
    const char *prefix  = prixMatrix[which];
    char *verTest       = netVer.version;//"v1.0.0"; 不能为空""
    int i               = 0;
    int ret             = 0;
    char *path          = "./";
    string pkgName   = "server_v1.0.0_2015.10.10_4005_32/64_Linux2.6.tar.gz";
    int localPatchNo = atoi(ver->patchNo);
    int size         = 0;
    int ipatchNo     = 0;
    //scan all tar.gz
    if (getPkgList(path, prefix, suffix, verTest, pkgList) < 0)
    {
        ut_err("get pkg list fail\n");
    }
    if (!pkgList.empty())
    {
        //从小到大  删除不合法的 //need to modify
        sort(pkgList.begin(), pkgList.end(), comp);
        //list : if dir * exist, esare *.tar.gz 
        size = pkgList.size();
        for (i=size-1; i>0; i--)
        {
            if(strstr(pkgList[i].c_str(), pkgList[i-1].c_str()))
            {
                //del pkgList[i];
                pkgList.erase(pkgList.begin()+i);
            }
        }
    }
    else
    {
        ut_err("there is no pkg?\n");
    }
    size = pkgList.size();
    totalPkg = 0;
    for (i=0; i<size; i++)
    {
        ipatchNo = getPatchNumFromName(pkgList[i].c_str());
        if (localPatchNo < ipatchNo)//取版本号比当前大的然后安装, 小的跳过
        {
            totalPkg = size - i;//大于当前版本的包有多少个
            break;
        }
    }
    curCountInstalled = 0;
    for (i = 0; i<size; i++)
    {
        ipatchNo = getPatchNumFromName(pkgList[i].c_str());
        if (localPatchNo >= ipatchNo)//取版本号比当前大的然后安装, 小的跳过
            continue;
        curCountInstalled ++;
        if (installSinglePatch(pkgList[i].c_str()) < 0)
        {
            ret = -1;
            ut_err("install patch [%s] fail\n", pkgList[i].c_str());
        }
    }

    if (ret == 0)
    {
        UpdateVersionFile();
        installOver = true;
        ret = curCountInstalled;
    }
    return ret;
}
/**
 * @brief 安装单个补丁包，只有版本信息，补丁号 没有时间
 *
 * @param fileName : 文件则解压安装，目录则直接运行里面的install.sh
 * @param Unknown
 *
 * @return 
 */
int FiUpdateAssistant::installSinglePatch(const char *fileName)
{
    int  ret = 0;
    char *suffix =".tar.gz";
    char *end = NULL;
    
    /*-----------------------------------------------------------------------------
     *  1. uncompress tar.gz
     *  2. begin install
     *-----------------------------------------------------------------------------*/
    string pkgName = "server_v1.0.0_2015.10.10_4005_32/64_Linux2.6.tar.gz";
    this->filename = fileName;
#ifndef WIN32
    struct stat st;
    char cmdBuf[512] = {0};
    if (stat(fileName, &st) == -1)
    {
        ut_err("stat error num:%d\n", errno);
    }
    else
    {
        if (S_ISDIR(st.st_mode))
        {
//            chdir("/sobey/fics/update");
            sprintf(cmdBuf, "./%s/install.sh", fileName);
            system(cmdBuf);
            return 0;
        }
    }
#endif
    end = strstr((char*)fileName, suffix);
    if (!end)
    {
        ut_err("pkg suffix is not tar.gz?\n");
        return -1;
    }
    this->dirfilename= std::string(fileName,end-fileName);
    
    ret = svc();
    ut_dbg("install ret[%d]\n", ret);

    return 0;
}
int FiUpdateAssistant::getMD5FromRemote(const char *fileName, string &remoteMD5)
{
    int rpcRet = 0 ;
    int trycnt = 0;
    string md5Val;
    ::CORBA::String_var outmd5Value;

    while( (rpcRet ==0)&& trycnt<2 )
    {
        trycnt++;
        if (trycnt > 1)
        {
            ut_dbg("Attempt to connect server [%d]\n" , trycnt);
        }
        MAKE_RPC_INVOKE(rHandle, getMD5FromFile(fileName, outmd5Value), rpcRet);
    }
    if( rpcRet == 0)
    {
        ut_dbg("network crash,stopping update line:%d\n", __LINE__);
        fflush(stdout);
        return -1;
    }
    else
    {
        remoteMD5 = outmd5Value.inout();
    }    
    return 0;
}
int FiUpdateAssistant::update()
{
    char* filename;
    int ret =0;
    char* layout;
    int trycnt=0;
    char buf[100];
    ::CORBA::String_var outfilename;
    ::CORBA::Long size;

    ut_dbg("prepare to update\n");
    fflush(stdout);
    FiGetCurDir(sizeof(buf),buf);
    std::string strNetConfig =buf;   

    while( (ret ==0)&& trycnt<2 )
    {
        trycnt++;
        if (trycnt > 1)
        {
            ut_dbg("Attempt to connect server [%d]\n" , trycnt);
        }
        MAKE_RPC_INVOKE(rHandle,StartupUpdate(PInfo, which, netVer.version , netVer.date, netVer.patchNo, outfilename,size,_ref),ret);
    }
    if( ret == 0)
    {
        ut_dbg("network crash,stopping update line:%d\n",__LINE__);
        fflush(stdout);
        NOTIFY_UPDATELAOAD_FINISH_FAIL();
        return -7;
    }
    filename = outfilename.inout();
    if(_ref==0)
    {
        ut_dbg("no package available line:%d\n",__LINE__);
        fflush(stdout);
        NOTIFY_UPDATELAOAD_FINISH_SUCCESS();
        UpdateVersionFile();
        return 0;
    }

    this->filename = filename;
    //this->layout = layout;
    ut_dbg("start download file %s\n",filename);
    fflush(stdout);
    //CORBA::string_free(filename);
    // CORBA::string_free(layout);
    handl_input(this->filename.c_str());
    return 20;  
}
int FiUpdateAssistant::queryPatchs(patchSet_t patchs)
{
    int ret =0;
    string version;
    version_t curVer;

    if (!patchs)
    {
        ut_err("patchs is null\n");
        ret = -1;
        goto err;
    }
    if (0 != getLocalVersion(&curVer))
    {
        ut_err("get local version fail\n");
        version = "";
    }
    else
    {
        version = curVer.version;
    }

    MAKE_RPC_INVOKE(rHandle, queryPatchs(version.c_str(), patchs), ret);
    if( ret ==0 )
    {
        ut_dbg("network crash,check the config pls\n");
        fflush(stdout);
        NOTIFY_UPDATELAOAD_FINISH_FAIL();
        return -2;
    }
    return ret;
err:
    return ret;
}
int FiUpdateAssistant::QueryCurrentVersion(version_t *version)
{
    char buf[100];

    FiGetCurDir(sizeof(buf),buf);

    int ret =0;
    ::CORBA::String_var outver;
    ::CORBA::String_var outdate;
    ::CORBA::String_var outpatchno;
    
    getLocalVersion(&localVer);    
    MAKE_RPC_INVOKE(rHandle,QueryCurVersion(localVer.version, localVer.date, localVer.patchNo,
     outver, outdate,outpatchno),ret);
    if( ret ==0 )
    {
        ut_dbg("network crash,check the config pls\n");
        fflush(stdout);
        NOTIFY_UPDATELAOAD_FINISH_FAIL();
        return -2;
    }
    char * outversion=outver.inout();
    char* date = outdate.inout();
    char* patchno = outpatchno.inout();
    
    if( outversion == NULL || date==NULL)
    {
        NOTIFY_UPDATELAOAD_FINISH_FAIL();
        return -9;
    }
    if( strlen(outversion)<=0 || strlen(date)<=0 )
    {
        ut_dbg("no package available\n");
        fflush(stdout);
        NOTIFY_UPDATELAOAD_FINISH_SUCCESS();
        return 34;
    }
    strcpy(version->version, outversion);
    strcpy(version->date, date);
    strcpy(version->patchNo, patchno);
    memcpy(&netVer, version, sizeof(version_t));

    return 0;
}
#if 1
int FiUpdateAssistant::downloadFile(const char *ip, const char *srcName, const char *dstName)
{
    int ret =0;
    char* layout;
    int trycnt=0;
    CORBA::LongLong fileRef=0;
    ::CORBA::String_var outfilename;
    ::CORBA::Long size;
    FILEPTR fpdownload;
    int pack_expect =1;

    if (!ip || !srcName || !srcName)
    {
        ut_err("params is null\n");
        goto err;
    }
    if (!cli)
    {
        cli = new FiRpcCli<FiUpdateMgr>;
    }

    ut_dbg("prepare to download %s\n", srcName);
    fflush(stdout);

    /*-----------------------------------------------------------------------------
     *  1. connect server
     *  2. get size, fileReference from server
     *  3. download file  by packet
     *-----------------------------------------------------------------------------*/
    if (!rHandle)
    {
        rHandle = cli->create_rpc_proxy(RPC_SERVER_NAME,(char*)ip, RPC_PORT);
        if (!rHandle)
        {
            ut_err("create_rpc_proxy fail\n");
            goto netcrash;
        }
    }
    while( (ret ==0)&& trycnt<2 )
    {
        trycnt++;
        if (trycnt > 1)
        {
            ut_dbg("Attempt to connect server [%d]\n" , trycnt);
        }
        MAKE_RPC_INVOKE(rHandle,starupTrans(srcName, size, fileRef),ret);
    }
    if( ret == 0)
    {
        ut_dbg("network crash,stopping update line:%d\n",__LINE__);
        fflush(stdout);
        NOTIFY_UPDATELAOAD_FINISH_FAIL();
        goto netcrash;
    }
    if(0 == fileRef)
    {
        ut_dbg("no file available line:%d\n",__LINE__);
        fflush(stdout);
        goto filenotexist;
    }

    ut_dbg("start download file %s\n", dstName);
    fflush(stdout);    
    
    ut_dbg("begin translate file:%s\n",srcName);
    ret = FiCreateFile(dstName);
    fpdownload = FiOpenExistFile(dstName);

    ret = -1;
    do 
    {
        ::CORBA::Long flag;
        int trycnt=0;
        int ret =0;
        bfstream_var bfs;
        ::CORBA::Long checknum;

        while( (ret == 0)&& trycnt<2 )
        {
            trycnt++;
            if (trycnt > 1)
            {
                ut_dbg("Attempt to connect server [%d]\n" , trycnt);
            }            
            MAKE_RPC_INVOKE(rHandle, transFile(fileRef, pack_expect, bfs, checknum, flag), ret);
        }
        if( ret ==0 )
        {
            ut_dbg("network crash,stopping update\n");
            fflush(stdout);
            NOTIFY_UPDATELAOAD_FINISH_FAIL();
            FiCloseFile(fpdownload);
            goto netcrash;
        }
        unsigned int chknum =0;
        unsigned int size = bfs->length();
        for(unsigned int i=0;i<size;++i)
        {
            chknum +=(unsigned int)(bfs[i]);
        }
        if(chknum ==checknum)
        {
            int wsize =FiWriteFile(fpdownload,(char*)(&bfs[0]),size);
            assert(wsize==size);//need to modify hwt
            pack_expect++;
            if( flag )
            {
                FiCloseFile(fpdownload);
                ut_dbg("download file [%s] success\n", dstName);
                break;
            }
        }
        else
        {
            ut_dbg("check num is  incorrect,redownload package[%d]!\n",pack_expect);
            fflush(stdout);
            //return 0;
        }

    } while (true);    

    return 0; 
err:
    if (cli)
    {
        delete cli;
        cli = NULL;
        rHandle = NULL;
    }
    return -1;
netcrash:
    if (cli)
    {
        delete cli;
        cli = NULL;
        rHandle = NULL;
    }    
    return -34;
filenotexist:
    return -8;
}

#endif
int FiUpdateAssistant::handl_input(const char* filename)
{
    int pack_expect =1;
    char buff[256];

    ut_dbg("begin translate file:%s\n",filename);
    fflush(stdout);
    if( pack_expect==1 )
    {
        FiGetCurDir(sizeof(buff),buff);
        std::string fullname(buff);
#ifdef WIN32 
        char *suffix =".tar.gz";
#else
        char *suffix = ".tar.gz";
#endif
        char* p = strstr((char*)filename, suffix);
        assert(p!=NULL);
        this->dirfilename= std::string(filename,p-filename);
        char cmd[300];
#ifdef WIN32
        std::string tmp = fullname+dirfilename;
        sprintf(cmd,"rd /s/q %s\n",tmp.c_str());
        FiExecuteShell(cmd);
        tmp.clear();
        tmp=fullname+filename;
        sprintf(cmd,"del /s/q %s\n",tmp.c_str());
        FiExecuteShell(cmd);
#else
        std::string tmp=fullname+dirfilename;
        sprintf(cmd,"rm -rf %s\n",tmp.c_str());
        FiExecuteShell(cmd);
        FiExecuteShell(cmd);

        tmp = fullname + filename;
        sprintf(cmd,"rm -f %s\n",tmp.c_str());
        FiExecuteShell(cmd);
#endif
        fullname += filename;
        //fullname +=".tmp";
        int ret = FiCreateFile(fullname.c_str());
        if( ret==-1 )
        {
            ut_dbg("can not create file named %s locally, make sure run as a root \n",fullname.c_str());
            fflush(stdout);
            NOTIFY_UPDATELAOAD_FINISH_FAIL();
            return -1;
        }
        fpdownload = FiOpenExistFile(fullname.c_str());
    }

    int ret =-1;
    do 
    {
        ::CORBA::Long flag;
        int trycnt=0;
        int ret =0;
        bfstream_var bfs;
        ::CORBA::Long checknum;
 /* :TODO:2015/10/15 15:21:32:hwt:  trycnt = 5 改为 trycnt = 1，受系统TCP keeplive影响，一次默认为75s，5次时间太长,会导致迟迟没有更新 10 分钟*/
        while( (ret == 0)&& trycnt<2 )
        {
            trycnt++;
            if (trycnt > 1)
            {
                ut_dbg("Attempt to connect server pack_expect[%d][%d]\n" ,pack_expect, trycnt);
            }
            MAKE_RPC_INVOKE(rHandle,TranslateFile(_ref,pack_expect,bfs,checknum,flag),ret);
        }
 /* :TODO:End---  */
        if( ret ==0 )
        {
            ut_dbg("network crash,stopping update\n");
            fflush(stdout);
            NOTIFY_UPDATELAOAD_FINISH_FAIL();
            FiCloseFile(fpdownload);
            return -34;
        }
        unsigned int chknum =0;
        unsigned int size = bfs->length();
        for(unsigned int i=0;i<size;++i)
        {
            chknum +=(unsigned int)(bfs[i]);
        }
        if(chknum ==checknum)
        {
            int wsize =FiWriteFile(fpdownload,(char*)(&bfs[0]),size);
            assert(wsize==size);//need to modify hwt
            pack_expect++;
            if( flag )
            {
                FiCloseFile(fpdownload);
                ret=svc();
                break;
            }
        }
        else
        {
            ut_dbg("check num is  incorrect,redownload package[%d]!\n",pack_expect);
            fflush(stdout);
            //return 0;
        }

    } while (true);

    return ret;
}

int splitstr(char*str,char* delm,std::vector<std::string>& allstrs)
{
    //char * s= (char*)(layout.c_str());
    char *next = strtok(str,delm);
    while( next != NULL)
    {
        allstrs.push_back(std::string(str));
        str = next;
    }
    if( str != NULL)
    {
        allstrs.push_back(std::string(str));
    }
    return 1;
}
bool wchar2char(wchar_t* wbuf,char*buf)
{
    while (*wbuf !=0)
    {
        *buf = (char)(*wbuf);
        buf++;
        wbuf++;
    }
    *buf=0;
    return true;
}
bool FiUpdateAssistant::CheckReg(std::vector<Reg_Layout_t>& regs)
{
#ifdef WIN32
    char buff[200];
    FiGetCurDir(sizeof(buff),buff);
    std::string rootpath(buff);

    std::string downfilefullname = rootpath+dirfilename+"\\";
    //std::string downfilefullname = rootpath;
    std::vector<Reg_Layout_t>::iterator iter;
    for( iter = regs.begin();iter != regs.end();++iter)
    {
        Reg_Layout_t &RegLayout = *iter;
        if((RegLayout.strAction != "D"))
        {
            std::string regfilename = downfilefullname;
            regfilename += RegLayout.strName;
            FILE* rfp = fopen(regfilename.c_str(),"rb");
            wchar_t wbuf[256];
            bool b=false;
            while ( fgetws(wbuf,256,rfp)> 0)
            {
                char buf[256];
                wchar2char(wbuf,buf);
REPEAT:
                char *tmp1=NULL;
                char* tmp2=NULL;
                if( (tmp1=strstr(buf,"["))!= NULL && (tmp2=strstr(buf,"]"))!=NULL )//[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\difsFsd\config]
                {
                    std::string strLocation;
        
                    //std::string strkey;
                    tmp1++;
                    //std::string loc;
                    while(tmp1 != tmp2)
                    {
                        strLocation.push_back(*tmp1);
                        tmp1++;
                    }
                    std::string::size_type ps;
                    ps = strLocation.find_first_of("\\");
                    if (ps == std::string::npos)
                    {
                        continue;
                    }
                    std::string strkey(&strLocation[0],ps);


                    std::string _regname(&strLocation[ps+1]);
                    HKEY hrootk=NULL;
                    HKEY  hk=NULL;


                    if( strkey.find("HKEY_LOCAL_MACHINE")!=std::string::npos)
                    {
                        hrootk = HKEY_LOCAL_MACHINE;
                    }
                    if( strkey.find("HKEY_CLASSES_ROOT")!=std::string::npos)
                    {
                        hrootk= HKEY_CLASSES_ROOT;
                    }
                    if( strkey.find("HKEY_CURRENT_USER")!=std::string::npos)
                    {
                        hrootk = HKEY_CURRENT_USER;
                    }
                    if( strkey.find("HKEY_USERS")!=std::string::npos)
                    {
                        hrootk = HKEY_USERS;
                    }
                    if( strkey.find("HKEY_CURRENT_CONFIG")!=std::string::npos)
                    {
                        hrootk = HKEY_CURRENT_CONFIG;
                    }
                    RegOpenKeyEx(hrootk,_regname.c_str(),NULL,KEY_ALL_ACCESS,&hk);
                    if((hk==NULL))
                    {
                        ut_dbg("update reg fail can not open reg [%s] at line [%d]\n",_regname.c_str(),__LINE__);
                        fflush(stdout);
                        return false;
                    }
                    while (fgetws(wbuf,256,rfp)> 0 )
                    {
                        wchar2char(wbuf,buf);
                        if( (tmp1=strstr(buf,"["))!= NULL && (tmp2=strstr(buf,"]"))!=NULL )
                        {
                            RegCloseKey(hk);
                            goto REPEAT;
                        }
                        char *tmp=NULL;
                        if ( ((tmp=strstr(buf,"=")) != NULL))//"WriteCcusable"=dword:00000000
                        {
                            DWORD m_dwCount;//记录字符串的长度+1（包括NULL字符）  
                            //std::string st;
                            tmp--;

                            if(buf[0]=='\"' && (*(tmp)) == '\"')
                            {
                                char keyname[100]={0};
                                strncpy(keyname,&buf[1],(tmp)-buf-1);
                                DWORD type;  
                                BYTE sreg[_MAX_PATH];  
                                DWORD len = _MAX_PATH;  
                                if (RegQueryValueEx(hk, keyname, 0, &type, sreg, &len)!=ERROR_SUCCESS) 
                                {
                                    if(RegLayout.strAction != "D")
                                    {
                                        ut_dbg("update reg fail,can not read reg key[%s] line[%d]\n",keyname,__LINE__);
                                        fflush(stdout);
                                        return false;
                                    }
                                }
                                char temp[10];
                                sprintf(temp,"%08x",*((DWORD*)sreg) );
                                if( strstr(buf,temp) != NULL)
                                {
                                }
                                else if(strstr(buf,(char*)(sreg)) !=NULL)
                                {
                                }
                                else
                                {
                                    ut_dbg("update reg fail ,value not the same key[%s],value[%s],new value[%s] ,line[%d]\n",keyname,sreg,buff);
                                    fflush(stdout);
                                    return false;
                                }

                            }
                            
                        }

                    }
                    RegCloseKey(hk);

                }
            }
            
            fclose(rfp);

        }

    }
    return true;
#endif
    return true;
}
bool FiUpdateAssistant::checkfolder(const char* folder1_full,const char* folder2_full)
{
    std::vector<std::string> allincfile1;
    std::vector<std::string> allincfile2;
    travel_dir((char*)(folder1_full),allincfile1);
    //travel_dir((char*)(folder2_full),allincfile2);

    for(int i=0;i<allincfile1.size();++i)
    {
        std::vector<std::string> incfile1;
        std::vector<std::string> incfile2;
        std::string f1=allincfile1[i];
        //std::string f2=allincfile1[i];
        //char*p1 = strstr((char*)f2.c_str(),"/*");
        char*p1=strstr((char*)f1.c_str(),"/*");
        if( (p1!=NULL))
        {
            p1++;
            
            *p1=0;
            std::string file1= std::string(folder1_full)+f1.c_str();
            std::string file2 = std::string(folder2_full)+f1.c_str();
            return checkfolder(file1.c_str(),file2.c_str());
        }
        if((p1==NULL))
        {
            std::string file1= std::string(folder1_full)+allincfile1[i];
            std::string file2 = std::string(folder2_full)+allincfile1[i];
            if( !CheckFile_Md5(file1,file2) )
            {
                ut_dbg("update file fail,fail to move file.check auth! file[%s]and file[%s]\n ",file1.c_str(),file2.c_str());
                fflush(stdout);
                //int *i =0;
                //*i =0;
                NOTIFY_UPDATELAOAD_FINISH_FAIL();
                return false;
            }
        }
        
        
    }
    return true;
}
static char* pause5="ping -n 5 localhost > nul \r\n";
int FiUpdateAssistant::setFile(std::string name)
{
    char* p = strstr((char*)name.c_str(), ".tar.gz");

    filename    = name;
    which       = MAS_SRV;
    dirfilename = std::string(name.c_str(), p-name.c_str());
    return 0;
}
int FiUpdateAssistant::svc()
{
    std::vector<FilePair> vecChangedFiles;
    char buff[200];
    std::string rootpath;
    string pkgDir;//rootDir + update + server_v1.0.0
    string rootDir;//->/sobey/fics
    string RelativePkgDir;//->  dirname: libs/ ,win_client/x32/
    string RelativePCDir;//->  dirname: libs , client, mds
    string pwdFullPath;//rootDir + RelativePCDir + [FileName] 
    
    FiGetCurDir(sizeof(buff),buff);
    rootpath = buff;
    rootDir  = dirname(buff);
    rootDir += "/";
    pkgDir   = rootDir;
    pkgDir  += "update/";
    ut_dbg("FiUpdateAssistant:: root path %s \n",rootpath.c_str());
    fflush(stdout);
    //std::string::size_type index =rootpath.rfind("/");
    //rootpath.erase(index,rootpath.size()-index);
    chdir(rootpath.c_str());//set current dir

    std::string strcmd("cd ");
    strcmd += rootpath.c_str();
    FiExecuteShell(strcmd.c_str());
#ifdef WIN32
    strcmd ="cd ";
    strcmd+= rootpath[0];
    strcmd+=rootpath[1];
    FiExecuteShell(strcmd.c_str());
    std::string unpress;
    unpress += "\"";
    unpress +="\"";
    unpress += buff;
    unpress +="7z.exe\"";
    unpress += " x ";
    unpress +="\"";
    unpress +=buff+filename;
    unpress +="\"  ";
    unpress +="-o\"";
    unpress +=rootpath+"\"";
    unpress += "\"";
    FiExecuteShell(unpress.c_str());
    std::string tmpfilename = filename;
    char *p = strstr((char*)tmpfilename.c_str(), ".gz");
    *p = '\0';
    unpress = "\"";
    unpress +="\"";
    unpress += buff;
    unpress +="7z.exe\"";
    unpress += " x ";
    unpress +="\"";
    unpress +=buff+tmpfilename;
    unpress +="\"  ";
    unpress +="-o\"";
    unpress +=rootpath+"\"";
    unpress += "\"";
    FiExecuteShell(unpress.c_str());
    //del tmpfilename *.tar
    std::string delStr = "del ";
    delStr  += "\"";
    delStr  += buff+tmpfilename;
    delStr  += "\"";
    FiExecuteShell(delStr.c_str());
    //FiExeWinUncompress(unpress.c_str());
    fflush(stdout);

#else
    std::string unpress("tar -zxvf ");
    unpress+= rootpath;
    unpress += filename.c_str();
    unpress +=" -C ";
    unpress += rootpath;
    FiExecuteShell(unpress.c_str());
#endif
    strcmd ="cd ";
    strcmd += dirfilename.c_str();
    pkgDir += dirfilename;
    pkgDir += "/";//mark need to modify
    FiExecuteShell(strcmd.c_str()); //enter pack dir

#ifdef WIN32
    std::string downfilefullname = rootpath+dirfilename+"\\";
#else
    std::string downfilefullname = rootpath+dirfilename+"/";
#endif
    std::string bupath = pkgDir +"backup";
    int ret =0;
    while ( (ret=FiCreateDir((char*)(bupath.c_str())))==-1)
    {
#ifdef WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    XmlParserEngine xmlParser;
    std::string xmlfile = downfilefullname+"update.xml";
    ut_dbg("load update xml from[%s]\n",xmlfile.c_str());
    fflush(stdout);
    //std::string xmlfile("./update.xml");
    xmlParser.load(xmlfile);
    //xmlParser.parse();
    FiUpdat_Pack_t pack = xmlParser.pack();

#ifdef WIN32
    std::string install_shell_name=pkgDir+"install.bat";
    std::string uninstall_shell_name=pkgDir+"uninstall.bat";
#else
    std::string install_shell_name=pkgDir+"install.sh";
    std::string uninstall_shell_name=pkgDir+"uninstall.sh";
#endif
    FiCreateFile(install_shell_name.c_str());
    FiCreateFile(uninstall_shell_name.c_str());

    FILEPTR fpinstall=FiOpenExistFile(install_shell_name.c_str());
    FILEPTR fpuninstall = FiOpenExistFile(uninstall_shell_name.c_str());

    //std::string tmp = rootpath;
#ifdef WIN32
    std::string installpath=rootpath+"..\\";
#else
    std::string installpath=rootpath+"../";
#endif
    // std::string::size_type index =installpath.rfind("/");
    //  installpath.erase(index,installpath.size()-index);


#ifdef WIN32
    std::string killself;
//    if(which != CLIENT)//server
//    {
//        killself = installpath +"difscmdmgr\\DifsMgr.exe -S __Child_Process__ \r\n";
//    }
//    else
//    {
//        killself = installpath +"difscmdmgr\\DifsMgr.exe -C __Child_Process__ \r\n";
//    }
//    FiWriteFile(fpinstall,(void*)(killself.c_str()),killself.length());
//    FiWriteFile(fpinstall,(void*)(killself.c_str()),killself.length());
//    FiWriteFile(fpinstall,(void*)(killself.c_str()),killself.length());
    FiWriteFile(fpinstall,(void*)("taskkill /f /im Fiwatchdog.exe \r\n"),strlen("taskkill /f /im Fiwatchdog.exe \r\n"));
    FiWriteFile(fpinstall,(void*)("taskkill /f /t /im scm.exe \r\n"),strlen("taskkill /f /t /im scm.exe \r\n"));
    FiWriteFile(fpinstall,(void*)("taskkill /f /t /im fitool.exe \r\n"),strlen("taskkill /f /t /im fitool.exe \r\n"));
//    FiWriteFile(fpuninstall,(void*)(killself.c_str()),killself.length());
//    FiWriteFile(fpuninstall,(void*)(killself.c_str()),killself.length());
    FiWriteFile(fpuninstall,(void*)("taskkill /f /im Fiwatchdog.exe \r\n"),strlen("taskkill /f /im Fiwatchdog.exe \r\n"));
    FiWriteFile(fpuninstall,(void*)("taskkill /f /t /im scm.exe \r\n"),strlen("taskkill /f /t /im scm.exe \r\n"));
    FiWriteFile(fpuninstall,(void*)("taskkill /f /t /im fitool.exe \r\n"),strlen("taskkill /f /t /im fitool.exe \r\n"));
    std::string delUpdate  = "del ";
    delUpdate         += rootpath;
    delUpdate         += "FiUpdateLoader.exe.old \r \n";
    FiWriteFile(fpinstall,(void*)(delUpdate.c_str()),strlen(delUpdate.c_str()));
    std::string renUpdate   = "ren ";
           renUpdate  += rootpath;
           renUpdate  += "FiUpdateLoader.exe ";
           renUpdate  += "FiUpdateLoader.exe.old \r \n";
    //FiExecuteShell(renUpdate.c_str());
    FiWriteFile(fpinstall,(void*)(renUpdate.c_str()),strlen(renUpdate.c_str()));

    //FiWriteFile(fpinstall,(void*)(delUpdate.c_str()),strlen(delUpdate.c_str()));

#else
    if(which != CLIENT )
    {
        FiWriteFile(fpinstall,(void*)("pkill -9 fiwatchdog \n"),strlen("pkill -9 fiwatchdog \n"));
        FiWriteFile(fpinstall,(void*)("pkill -9 WebClient \n"),strlen("pkill -9 WebClient \n"));
        FiWriteFile(fpinstall,(void*)("pkill -9 java \n"),strlen("pkill -9 java \n"));
        FiWriteFile(fpinstall,(void*)("pkill -9 fimds \n"),strlen("pkill -9 fimds \n"));
        FiWriteFile(fpinstall,(void*)("pkill -9 scm \n"),strlen("pkill -9 scm \n"));
        FiWriteFile(fpinstall,(void*)("pkill -9 fiioctlproxy \n"),strlen("pkill -9 fiioctlproxy \n"));
        FiWriteFile(fpinstall,(void*)("pkill -9 FicsFormatterEx.exe \n"),strlen("pkill -9 FicsFormatterEx.exe \n"));
        FiWriteFile(fpinstall,(void*)("ps aux | grep java | grep zoo | perl -lane 'print \"$F[1]\"' | xargs kill -9 \n"),strlen("ps aux | grep java | grep zoo | perl -lane 'print \"$F[1]\"' | xargs kill -9 \n"));
    }
    else//client: update less
    {
        FiWriteFile(fpinstall,(void*)("pkill -9 fiwatchdog \n"),strlen("pkill -9 fiwatchdog \n"));
        FiWriteFile(fpinstall,(void*)("pkill -9 fitool \n"),strlen("pkill -9 fitool \n"));
        FiWriteFile(fpinstall,(void*)("pkill -9 scm \n"),strlen("pkill -9 scm \n"));
        FiWriteFile(fpinstall,(void*)("pkill -9 fiioctlproxy \n"),strlen("pkill -9 fiioctlproxy \n"));
 
    }
    FiWriteFile(fpinstall,(void*)("unalias cp \n"),strlen("unalias cp \n"));
    std::string temp("chmod 777 ");
    temp+=pkgDir+"*\n";
    FiWriteFile(fpinstall,(void*)(temp.c_str()),temp.length());
    temp ="cd ";
    temp +=pkgDir+" \n";//
    FiWriteFile(fpinstall,(void*)(temp.c_str()),temp.length());
    FiWriteFile(fpuninstall,(void*)(temp.c_str()),temp.length());
#endif

    /*-----------------------------------------------------------------------------
     *  load info from update.xml
     *-----------------------------------------------------------------------------*/
    char backcmd[300];
    char cmd[300];
    char uncmd[300];
    string FileName;
    string srcPath;
    //pkgDir + layout.strName -> pkg file name
    std::vector<File_Layout_t>::iterator it;

    ut_dbg("try to update %d items\n",pack.allFiles.size());
    fflush(stdout);

    for(it=pack.allFiles.begin(); it!=pack.allFiles.end(); ++it)
    {
        File_Layout_t& layout = *it;
        char location[200]={0};
        char strTmpBuf[256] = {0};
        strcpy(strTmpBuf, layout.strName.c_str());
        strcpy(location, installpath.c_str());// %root -> /sobey/fics
        std::string &fileloc = layout.strLocation;//libs/*  
        RelativePkgDir = dirname(strTmpBuf);
        RelativePkgDir += "/";
        pwdFullPath = rootDir;  
        int len = fileloc.length();
#ifdef WIN32
        if( fileloc[len-1] == '/')
        {
            fileloc[len-1] = '\\';
        }
        if( fileloc[len-1] != '\\')
        {
            fileloc.push_back('\\');
        }
#else
        if( fileloc[len-1] != '/')
        {
            fileloc.push_back('/');
        }
#endif
        std::string strTemp = fileloc;
        transform(strTemp.begin(),strTemp.end(),strTemp.begin(), ::tolower);//to lower  need
        std::string::size_type pos = strTemp.find("%root");
        if( pos == std::string::npos)
        {
//            strcpy(location,fileloc.c_str());
            pwdFullPath = fileloc;
        }
        else
        {
            if(strTemp.length() > strlen("%root")+1)
            {
                strcat(location,&fileloc[strlen("%root")+1]);///sobey/fics/update/test_FiUpdateMgr
                char relativeBuf[128]={0};
                strcpy(relativeBuf, &fileloc[strlen("%root")+1]);
                RelativePCDir = relativeBuf;
                pwdFullPath  += relativeBuf;
            }
        }
        strcpy(strTmpBuf, layout.strName.c_str());
//        strTemp      = layout.strName;
        if (strTmpBuf[strlen(strTmpBuf)-1] != '*')
        {
            pwdFullPath += basename(strTmpBuf);//dir:lib/*, *.sys, FiUpdateMgr
        }
        std::string cfullname = pkgDir;//server_v1.0.0_date_patchNo
        char* p = strstr((char*)layout.strName.c_str(),"/*");//dir
        bool isdir=false;
        if(p)
        {
            isdir =true;
//            *p =0;
        }
//         if( (layout.strName.length()>=4) &&strcmp(&layout.strName[layout.strName.length()-4],".reg")==0 )//*.reg
//         {
//             std::string regfilename = rootpath+dirfilename+"\\";
//             regfilename += layout.strName;
//             FILE* rfp = fopen(regfilename.c_str(),"rb");
//             wchar_t wbuf[256];
//             bool b=false;
//             while ( fgetws(wbuf,256,rfp)> 0)
//             {
//                 char buf[256];
//                 wchar2char(wbuf,buf);
//                 char *tmp1=NULL;
//                 char* tmp2=NULL;
//                 if( (tmp1=strstr(buf,"["))!= NULL && (tmp2=strstr(buf,"]"))!=NULL )//[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\difsFsd\config]
//                 {
//                     Reg_Layout_t rlt;
//                     rlt.strName = layout.strName;
//                     rlt.strAction ="A";
//                     tmp1++;
//                     while(tmp1 != tmp2)
//                     {
//                         rlt.strLocation.push_back(*tmp1);
//                         tmp1++;
//                     }
//                     pack.allRegs.push_back(rlt);
//                     ut_dbg("find reg file[%s] location[%s] total %d\n",rlt.strName.c_str(),rlt.strLocation.c_str(),pack.allRegs.size());
//                     break;
//                 }
//             }
//             continue;
//         }
        //strcat(location,layout.strName.c_str());//+file name
        if(layout.strAction == "M")
        {
#ifdef WIN32
            cfullname +=layout.strName.c_str();
            if(isdir)
            {
                sprintf(backcmd,"MD %sbackup\\%s \r\n",downfilefullname.c_str(),layout.strName.c_str());
                FiWriteFile(fpinstall,backcmd,strlen(backcmd));
            }

            sprintf(backcmd,"%s \"%s%s\" %sbackup\\%s \r\n",(isdir)?"xcopy /E/H /Y ":"copy  /Y ",location,layout.strName.c_str(),
                downfilefullname.c_str(),isdir?std::string(std::string(layout.strName.c_str())).c_str():"");
            FiWriteFile(fpinstall,backcmd,strlen(backcmd));
            //FiWriteFile(fpinstall,backcmd,strlen(backcmd));
            //FiWriteFile(fpinstall," \r\n",strlen(" \r\n"));
            //FiWriteFile(fpinstall,pause5,strlen(pause5));

            sprintf(cmd,"%s \"%s%s\"  \"%s%s\" \r\n",(isdir)?"xcopy /E/H /Y ":"copy /Y ",downfilefullname.c_str(),layout.strName.c_str(),
                location,std::string(std::string(layout.strName.c_str())).c_str());
            FiWriteFile(fpinstall,cmd,strlen(cmd));
            //FiWriteFile(fpinstall,pause5,strlen(pause5));

            sprintf(uncmd,"%s %s%s \r\n",(isdir)?"rd /s/q ":"del /s/q  ",location,layout.strName.c_str());
            FiWriteFile(fpuninstall,uncmd,strlen(uncmd));
            sprintf(uncmd,"%s  %sbackup\\%s %s\r\n","move",downfilefullname.c_str(),layout.strName.c_str(),location);
            FiWriteFile(fpuninstall,uncmd,strlen(uncmd));
#else
            cfullname +=layout.strName.c_str();
            //install
            sprintf(cmd,"install -p -v -D -S .back %s/%s %s \n",
                    pkgDir.c_str(), layout.strName.c_str(), pwdFullPath.c_str());//pwdFullPath == *?
            FiWriteFile(fpinstall, cmd, strlen(cmd));
            //back up   attention : %s*.back
            sprintf(backcmd, "mkdir -p %sbackup/%s; for f in `ls %s*.back`; do fname=`basename $f`; newf=${fname%%.*}; mv $f %sbackup/%s/${newf}; done;\n",
                    pkgDir.c_str(), RelativePkgDir.c_str(), pwdFullPath.c_str(),
                    pkgDir.c_str(), RelativePkgDir.c_str());
            FiWriteFile(fpinstall, backcmd, strlen(backcmd));
            //recover
            sprintf(uncmd, "install -p -v -D %s/backup/%s %s\n",
                    pkgDir.c_str(), layout.strName.c_str(), pwdFullPath.c_str());
            FiWriteFile(fpuninstall, uncmd, strlen(uncmd));
//            sprintf(uncmd,"cp %s  %sbackup/%s %s\n",(isdir)?"-rf ":" -f ",downfilefullname.c_str(),layout.strName.c_str(),location);
//            FiWriteFile(fpuninstall,uncmd,strlen(uncmd));
#endif
            FilePair FP;
            FP.file2 = std::string(location)+layout.strName.c_str();
            FP.file1 = cfullname;
            if(isdir)
            {
                FP.file1+="/*";//自定义标记
                FP.file2+="/*";
            }
            vecChangedFiles.push_back( FP );//???
        }
        if(layout.strAction == "D")
        {
#ifdef WIN32
            cfullname += "backup\\";
            cfullname +=layout.strName.c_str();
            sprintf(cmd,"%s  %s%s %sbackup \r\n","move" ,location,layout.strName.c_str(),downfilefullname.c_str());
            FiWriteFile(fpinstall,cmd,strlen(cmd));
            FiWriteFile(fpinstall,cmd,strlen(cmd));
            if(isdir)
            {
                sprintf(backcmd,"MD %s%s \r\n",location,layout.strName.c_str());
                FiWriteFile(fpuninstall,backcmd,strlen(backcmd));
            }
            //sprintf(backcmd,"%s  %s%s %sbackup\\ \r\n",(isdir)?"xcopy /E/H /Y ":"copy /Y ",location,layout.strName.c_str(),downfilefullname.c_str());
            //sprintf(cmd,"%s %s%s\r\n",(isdir)?"rd /S /Q ":" del /S /Q",location,layout.strName.c_str());
            sprintf(uncmd,"%s  %sbackup\\%s %s%s\r\n",(isdir)?"xcopy /E/H /Y ":"copy /Y ",downfilefullname.c_str(),layout.strName.c_str(),location,isdir?std::string(std::string(layout.strName.c_str())).c_str():"");
            FiWriteFile(fpuninstall,uncmd,strlen(uncmd));
#else
            cfullname += "backup/";
            cfullname +=layout.strName.c_str();
            //back up
            sprintf(backcmd,"cp -a -rf %s %sbackup/%s\n",
                    pwdFullPath.c_str(), pkgDir.c_str(), RelativePkgDir.c_str());
            //rm -rf
            sprintf(cmd,"rm %s -rf \n",
                    pwdFullPath.c_str());
            //recover
            sprintf(uncmd,"install -p -v -D  %sbackup/%s %s\n",
                    pkgDir.c_str(), layout.strName.c_str(), pwdFullPath.c_str());
            FiWriteFile(fpinstall,backcmd,strlen(backcmd));
            FiWriteFile(fpinstall,cmd,strlen(cmd));
            FiWriteFile(fpuninstall,uncmd,strlen(uncmd));
#endif
            FilePair FP;
            FP.file2 = cfullname;
            if (isdir)
            {
                FP.file2+="/*";
            }
            
            vecChangedFiles.push_back( FP );
        }
        if( layout.strAction == "A")
        {
#ifdef WIN32
            //sprintf(backcmd,"move %s backup/\r\n",location);
            if(isdir)
            {
                sprintf(cmd,"MD %s%s \r\n",location,layout.strName.c_str());
                FiWriteFile(fpinstall,cmd,strlen(cmd));
            }
            sprintf(cmd,"%s %s%s %s%s\r\n",(isdir)?"xcopy /E/H /Y":"xcopy /Y ",downfilefullname.c_str(),layout.strName.c_str(),
                location,isdir?std::string(std::string(layout.strName.c_str())+"\\").c_str():"");
            sprintf(uncmd,"%s %s%s\r\n",(isdir)?"rd /S /Q":"del /S /Q ",location,layout.strName.c_str());
#else
            // sprintf(backcmd,"mv %s backup/\n",location);
            sprintf(cmd,"mkdir -p %s\n", RelativePCDir.c_str());
            //install
            sprintf(cmd+strlen(cmd),"install -p -v  %s/%s %s\n",
                    pkgDir.c_str(), layout.strName.c_str(), pwdFullPath.c_str());
            //recover
            if (isdir)//dir
            {
                sprintf(uncmd, "fileList=`ls %s%s`;", pkgDir.c_str(), RelativePkgDir.c_str());
                sprintf(uncmd+strlen(uncmd), "cd %s/%s && rm -rf ${fileList} && cd -\n", rootDir.c_str(), RelativePCDir.c_str());
            }
            else
            {
                sprintf(uncmd, "rm -rf %s\n", pwdFullPath.c_str());
            }
#endif
            FilePair FP;
            FP.file1 = downfilefullname+layout.strName.c_str();
            FP.file2 = std::string(location)+layout.strName.c_str();
            if (isdir)
            {
                FP.file1+="/*";
                FP.file2+="/*";
            }
            //FiWriteFile(fpinstall,backcmd,strlen(backcmd));
            FiWriteFile(fpinstall,cmd,strlen(cmd));
            FiWriteFile(fpuninstall,uncmd,strlen(uncmd));
            vecChangedFiles.push_back( FP );
        }
    }
    //rename 's/\.back$//' *.back
#ifdef WIN32
    std::vector<Reg_Layout_t> Regs;
    std::vector<Reg_Layout_t>::iterator iter;
    for( iter=pack.allRegs.begin();iter != pack.allRegs.end();++iter)
    {
        Reg_Layout_t &RegLayout = *iter;
        std::string strTemp = RegLayout.strLocation;
        transform(strTemp.begin(),strTemp.end(),strTemp.begin(),tolower);
        std::string::size_type pos = strTemp.find("%root");
        if( pos != std::string::npos)
        {
            std::string regfilename = rootpath+dirfilename+"\\";
            regfilename += RegLayout.strName;
            FILE* rfp = fopen(regfilename.c_str(),"rb");
            wchar_t wbuf[256];
            bool b=false;
            while ( fgetws(wbuf,256,rfp)> 0)
            {
                char buf[256];
                wchar2char(wbuf,buf);
                char *tmp1=NULL;
                char* tmp2=NULL;
                if( (tmp1=strstr(buf,"["))!= NULL && (tmp2=strstr(buf,"]"))!=NULL )//[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\difsFsd\config]
                {
                    Reg_Layout_t rlt;
                    rlt.strName = RegLayout.strName;
                    //RegLayout.strName = layout.strName;
                    rlt.strAction ="M";
                    tmp1++;
                    std::string loc;
                    while(tmp1 != tmp2)
                    {
                        loc.push_back(*tmp1);
                        tmp1++;
                    }
                    rlt.strLocation = loc;
                    Regs.push_back(rlt);
                    //pack.allRegs.push_back(rlt);
                    ut_dbg("find reg file[%s] location[%s] total %d\n",RegLayout.strName.c_str(),RegLayout.strLocation.c_str(),pack.allRegs.size());
                    fflush(stdout);
                    break;
                }
            }
        }
        else
        {
            Regs.push_back(RegLayout);
        }

    }
    pack.allRegs.swap(Regs);
    for( iter=pack.allRegs.begin();iter != pack.allRegs.end();++iter)
    {
        Reg_Layout_t &RegLayout = *iter;

        if( RegLayout.strAction == "A")
        {
            std::string regcmd(" regedit /s ");
            regcmd+=downfilefullname;
            regcmd+=RegLayout.strName+" \r\n";

            std::string unregcmd("reg delete ");

            unregcmd +=RegLayout.strLocation + " /va  /f \r\n ";
            //unregcmd += _regname+ " /f \r\n";

            FiWriteFile(fpinstall,(void*)(regcmd.c_str()),regcmd.length());
            FiWriteFile(fpuninstall,(void*)(unregcmd.c_str()),unregcmd.length());


        }
        if(RegLayout.strAction == "M")
        {
            std::string regcmd("  regedit /e ");
            regcmd +=downfilefullname + "backup/"+RegLayout.strName +"_bak.reg ";
            regcmd += RegLayout.strLocation +" \r\n";

            FiWriteFile(fpinstall,(void*)(regcmd.c_str()),regcmd.length());

            regcmd=(" regedit /s ");
            regcmd+=downfilefullname;
            regcmd+=RegLayout.strName+" \r\n";
            FiWriteFile(fpinstall,(void*)(regcmd.c_str()),regcmd.length());

            std::string unregcmd(" regedit /s ");
            unregcmd += downfilefullname+"backup/";
            unregcmd +=RegLayout.strName +"_bak.reg \r\n";
            FiWriteFile(fpuninstall,(void*)(unregcmd.c_str()),unregcmd.length());

        }
        if (RegLayout.strAction == "D")
        {
            std::string regcmd(" regedit /e ");
            regcmd +=downfilefullname + "backup/"+RegLayout.strName +"_bak.reg ";
            regcmd += RegLayout.strLocation +" \r\n";
            FiWriteFile(fpinstall,(void*)(regcmd.c_str()),regcmd.length());

            regcmd="reg delete ";
            regcmd +=RegLayout.strLocation + " /va  /f \r\n ";
            //regcmd += _regname +" /f \r\n";
            FiWriteFile(fpinstall,(void*)(regcmd.c_str()),regcmd.length());

            std::string unregcmd("regedit /s ");
            unregcmd += downfilefullname+"backup/";
            unregcmd +=RegLayout.strName +"_bak.reg \r\n";
            FiWriteFile(fpuninstall,(void*)(unregcmd.c_str()),unregcmd.length());

        }

    }

#endif

#ifdef WIN32
    sprintf(cmd,"copy /Y %s%s %sbackup\\ \r\n",installpath.c_str(),"patch_version",downfilefullname.c_str());
    sprintf(backcmd,"del /s/q %s%s\r\n",installpath.c_str(),"patch_version");
    sprintf(uncmd,"copy /Y %sbackup\\%s %s \r\n",downfilefullname.c_str(),"patch_version",installpath.c_str());
#else
    sprintf(cmd,"cp -f %s%s %sbackup/ \n",installpath.c_str(),"patch_version",downfilefullname.c_str());//back patch_version
    sprintf(backcmd,"rm -rf %s%s\n",installpath.c_str(),"patch_version");//recover patch_version
    sprintf(uncmd,"cp -f %sbackup/%s %s \n",downfilefullname.c_str(),"patch_version",installpath.c_str());
#endif
    FiWriteFile(fpinstall,cmd,strlen(cmd));
    FiWriteFile(fpuninstall,backcmd,strlen(backcmd));
    FiWriteFile(fpuninstall,uncmd,strlen(uncmd));
    
    FiCloseFile(fpinstall);

    //std::string exeinstall= downfilefullname;
    //exeinstall+=install_shell_name;
    FiExecuteShell(install_shell_name.c_str());
    //check updating

    ut_dbg("checking file-udpate\n");
    fflush(stdout);
#if 0//add by sean
    std::vector<FilePair>::iterator iterf;
    for (iterf=vecChangedFiles.begin();iterf!=vecChangedFiles.end();++iterf)//
    {
        FilePair& FP = *iterf;
        std::string filename = FP.file1.empty()?FP.file2:FP.file1;
        char* p = strstr((char*)filename.c_str(),"/*");
        bool isdir = false;
        if( p != NULL)
        {
            isdir = true;
            //*p=0;
        }
        if (FP.file1.empty() || FP.file2.empty())
        {
            ut_dbg("file1[%s]file2[%s]is null!\n", FP.file1.c_str(), FP.file2.c_str());
            
            if( isdir )
            {
                *p=0;
                if(ACCESS(filename.c_str(),0)==- 1)//不存在
                {
                    ut_err("update file fail,fail to move file.check auth! filename[%s]\n",filename.c_str());
                    fflush(stdout);
                    //int *i =0;
                    //*i =0;
                    NOTIFY_UPDATELAOAD_FINISH_FAIL();
                    return -1;
                }
            }
            else
            {
                FILEPTR tfp = FiOpenExistFile(filename.c_str());
                if(tfp ==NULL)
                {
                    ut_dbg("update file fail,fail to move file.check auth! filename[%s]\n ",filename.c_str());
                    fflush(stdout);
                    //int *i =0;
                    //*i =0;
                    NOTIFY_UPDATELAOAD_FINISH_FAIL();
                    return -1;
                }
                FiCloseFile(tfp);
            }
            
        }
        else
        {
            //ut_dbg("check file1 %s,file2%s \n",FP.file2.c_str(),FP.file1.c_str());
            if(isdir)
            {
                char*p1 = strstr((char*)FP.file2.c_str(),"/*");
                char*p2=strstr((char*)FP.file1.c_str(),"/*");
                p1++;
                p2++;
                *p1=0;
                *p2=0;
                bool ret =checkfolder(FP.file1.c_str(),FP.file2.c_str());
                if(!ret)
                {
                    NOTIFY_UPDATELAOAD_FINISH_FAIL();
                    return -45;
                }
            }
            else if(ACCESS(FP.file2.c_str(), 0) == 0 && !CheckFile_Md5(FP.file1,FP.file2) )
            {
                ut_dbg("update file fail,fail to move file.check auth!\n ");
                fflush(stdout);
                //int *i =0;
                //*i =0;
                NOTIFY_UPDATELAOAD_FINISH_FAIL();
                return -45;
            }
        }

    }
#endif
    if(!pack.allRegs.empty())
    {
        ut_dbg("checking reg-update\n");
        fflush(stdout);
        CheckReg(pack.allRegs);
    }

//    UpdateVersionFile();
    version_t curVer;
    memset(&curVer, 0, sizeof(version_t));
    getVerFromName(this->filename.c_str(), &curVer);
    addVer2His(&curVer, HISTORY);
    writeLocalVer(&curVer);
    chdir(rootDir.c_str());//set dir /sobey/fics
    
    char clean[200];
#ifdef WIN32
    sprintf(clean,"del %s%s\r\n",rootpath.c_str(),filename.c_str());
#else
    sprintf(clean,"rm -rf %s%s\n",rootpath.c_str(),filename.c_str());
#endif
    FiExecuteShell(clean);
    ut_dbg("update finish!\n");
    fflush(stdout);

    if(pack.bReboot)
    {
#ifdef WIN32
        std::string str("shutdown /r \n");
#else
        std::string str("reboot\n");
#endif
        //FiWriteFile(fpinstall,(void*)(str.c_str()),str.length());
//        FiWriteFile(fpuninstall,(void*)(str.c_str()),str.length());
        if (curCountInstalled == totalPkg)
        {
            FiExecuteShell(str.c_str());
        }
    }
    else
    {
#if 0
       if(totalPkg == curCountInstalled)
       {
           std::string curdir = "cd ";
           curdir += installpath +" \r \n";
           FiWriteFile(fpuninstall,(void*)(curdir.c_str()),curdir.length());
            //FiWriteFile(fpinstall,(void*)(startupself.c_str()),startupself.length());
//           FiWriteFile(fpuninstall,(void*)(startupself.c_str()),startupself.length());
           FiCloseFile(fpuninstall);
           FiExecuteShell(curdir.c_str());//need to modify
//           restartAPP("fics");
       }
#endif
    }
    
    NOTIFY_UPDATELAOAD_FINISH_SUCCESS();
    return 1;
}
bool FiUpdateAssistant::restartAPP(const char *app)
{
    void(app);
    const char *elf = NULL;
    string startupself;
#ifdef WIN32
    startupself = rootDir + "FiWatchDog.exe \r\n";
#else
    startupself = rootDir +"apache-tomcat-7.0.28/bin/startup.sh ;"
        startupself += rootDir + "client/fiwatchdog &\n";//"nohup"
#endif

    ut_dbg("restart %s \n", elf);
    fflush(stdout);
    //undo 1>log
    //do log>1
#ifdef WIN32
#else
    int devstdoutfd = 0;
    int devstderrfd = 0;
    if (-1 == (devstdoutfd = open("/dev/stdout", 0)))
    {
        ut_err("open stdout fail\n");
    }
    if (-1 == (devstderrfd = open("/dev/stderr", 0)))
    {
        ut_err("open stderr fail\n");
    }           
    fflush(stdout);
    if (-1 == dup2(devstdoutfd, STDOUT_FILENO))
    {
        ut_err("dup2 error\n");
    }
    if (-1 == dup2(devstderrfd, STDERR_FILENO))
    {
        ut_err("dup2 error\n");
    }
#endif
    FiExecuteShell(elf);//need to modify
#ifdef WIN32
#else
    char *path = "/sobey/fics/update/FiUpdateLoader.log";
    freopen(path,"a+",stdout);
    freopen(path,"a+",stderr);
    ut_dbg("fiwatchdog &\n");
    fflush(stdout);
#endif

    return true;
}
//server_v1.0.0_2015.10.10_4005_32/64_Linux2.6.tar.gz"
bool less_second(const std::string & m1, const std::string & m2) 
{
    typedef std::string::size_type SPOS;
    SPOS posVer1=m1.find("v");//version
    SPOS posVer2=m2.find("v");
    SPOS posDate1 = m1.find("_",posVer1);//date
    SPOS posDate2 = m2.find("_",posVer2);

    if( posVer1== std::string::npos || posVer2== std::string::npos||posDate1== std::string::npos||posDate2== std::string::npos)
    {
        int *i=0;
        *i =0;
    }
    std::string ver1(&m1[posVer1],posDate1-posVer1);
    std::string ver2(&m2[posVer2],posDate2-posVer2);
    std::string date1(&m1[posDate1]);
    std::string date2(&m2[posDate2]);
    if(ver1<ver2)
    {
        return true;
    }
    else if( ver1==ver2)
    {
        if(date1<date2)
        {
            return true;
        }
        return false;
    }
    else
    {
        return false;
    }

    return true;
}

int FiUpdateAssistant::RollBack(version_t *dstVer)
{
    char *version = dstVer->version;
    char *date    = dstVer->date;
    char *patchno = dstVer->patchNo;
    int ret = 0;

    std::string prix;
    prix = prixMatrix[which];
    newerver = version;
    newerdate = date;
    switch (which)
    {
        case SERVER:
            prix = "server";
            break;
        case CLIENT:
            prix = "client";
            break;
        case MAS_SRV:
            prix = "Massvr";
            break;
        default:
            break;
    }

    filename = prix+"_"+newerver+"_"+newerdate;
    if (patchno!=NULL&&strlen(patchno)>0)
    {
        filename+="_";
        filename+=patchno;
    }
    filename+=std::string("_")+((PInfo.OSRunMode==32)?"32":"64");
    if (PInfo.platform == EWINDOWS )
    {
        filename+="_Win";
    }
    if (PInfo.platform == ELINUX )
    {
        
        filename+="_Linux";
    }
    if( PInfo.platform == EMAC)
    {
        filename+="_Mac";
    }
    char buff[300]={0};
    FiGetCurDir(sizeof(buff),buff);
    std::string rootpath(buff);
    chdir(buff);
    std::string cmd("cd ");
    cmd += rootpath.c_str();
    FiExecuteShell(cmd.c_str());
#ifdef WIN32
    std::string installpath=rootpath+"..\\";
#else
    std::string installpath=rootpath+"../";
#endif
#ifdef WIN32
    cmd ="cd ";
    cmd+= rootpath[0];
    cmd+=rootpath[1];
    FiExecuteShell(cmd.c_str());
#endif

#ifdef WIN32
    std::string killself;
    system("taskkill /f /im Fiwatchdog.exe ");
    system("taskkill /f /t /im scm.exe ");
    system("taskkill /f /t /im fitool.exe ");
#else
    if(which != CLIENT)
    {
        system("pkill -9 fiwatchdog");
        system("pkill -9 WebClient");
        system("pkill -9 java");
        system("pkill -9 fimds");
        system("pkill -9 scm");
        system("pkill -9 fiioctlproxy");
        system("pkill -9 FicsFormatterEx.exe ");
        system("ps aux | grep java | grep zoo | perl -lane 'print \"$F[1]\"' | xargs kill -9 \n"),strlen("ps aux | grep java | grep zoo | perl -lane 'print \"$F[1]\"' | xargs kill -9 ");

    }
    else
    {
        system("pkill -9 fiwatchdog");
        system("pkill -9 fitool");
        system("pkill -9 scm");
        system("pkill -9 fiioctlproxy");
    }
 #endif
#ifndef WIN32
    system("unalias cp");
#endif
    std::vector<std::string> allfolders;
    FiGetAllFolder(rootpath.c_str(), allfolders);
    std::vector<std::string>::iterator iter=allfolders.begin();
    for(;iter!=allfolders.end();++iter)
    {
        std::string& str=*iter;
        if(str.find("_v")==std::string::npos)//清除 非安装包目录  _v 作为标记
        {
            allfolders.erase(iter);
            iter =allfolders.begin();
            continue;
        }
    }
    sort(allfolders.begin(),allfolders.end(),less_second);
    int base =-1;
    std::string str=version;
    str +="_";
    str+=date;
    if (patchno!=NULL&&strlen(patchno)>0)
    {
        str+="_";
        str+=patchno;
    }
    typedef std::string::size_type SPOS;
    for(int j=0;j<allfolders.size();++j)
    {
        ut_dbg("%s ",allfolders[j].c_str());
    }
    ut_dbg("\nscan folder over\n");
    fflush(stdout);
    for(int i=0;i<allfolders.size();++i)
    {
        SPOS pos = allfolders[i].find("_v");
        if( pos != std::string::npos && memcmp(&allfolders[i][pos+1],str.c_str(),str.length())>0 )
        {
            base = i;
            break;
        }
    }
    if( base == -1)
    {
        ut_dbg("no need to roll back,no package available\n");
        fflush(stdout);
        goto finish;
    }

    for(int j=allfolders.size()-1;j>=base;--j)
    {
        std::string fullname = rootpath;
        fullname+=allfolders[j];
#ifdef WIN32
        fullname +="\\uninstall.bat";
        
#else
        fullname +="/uninstall.sh";
        
#endif
        //fullname += " > rollback_"+allfolders[j]+".log";
        FiExecuteShell(fullname.c_str());
        //FiExecuteShell("cd ../");
    }

    //     FILE* fp = fopen("versionctl.txt","wb+");
    //     assert(fp!=NULL);
    //     fseek(fp,0,SEEK_SET);
    //     std::string nversion("version:");
    //     nversion+=version;
    //     std::string ndate("date:");
    //     ndate+=date;
    // 
    //     fputs(nversion.c_str(),fp);
    //     fputs("\n",fp);
    //     fputs(ndate.c_str(),fp);
    //     fputs("\n",fp);
    //     fclose(fp);

    ut_dbg("roll back finished!\n");
    fflush(stdout);
finish:
//    ret = installSinglePatch();//need to modify hwt
    if (ret == 0)
    {
        UpdateVersionFile();
    }
    NOTIFY_UPDATELAOAD_FINISH_SUCCESS();
    return 0;
}

int FiUpdateAssistant::UpdateVersionFile()
{
    newerdate = netVer.date;
    newerver  = netVer.version;
    if (which == MAS_SRV)
    {
        return 0;
    }
    if (newerdate == "" || newerver =="")
    {
        return 0;
    }
    
    sscanf(netVer.patchNo, "%d", &newerpatchno);
    
    FILE* curverfp = fopen(_PATH_VERSION,"wb+");
    fseek(curverfp,0,SEEK_SET);
    std::string nversion("version:");
    nversion+=newerver;
    std::string ndate("date:");
    ndate+=newerdate;
    fputs(nversion.c_str(),curverfp);
#ifndef WIN32
    fputs("\n",curverfp);
#else
    fputs("\r\n",curverfp);
#endif
    fputs(ndate.c_str(),curverfp);
#ifndef WIN32
    fputs("\n",curverfp);
#else
    fputs("\r\n",curverfp);
#endif
    char tmp[10];
    if( newerpatchno>0 )
    {
        std::string npno("patch:");
        
        sprintf(tmp,"%d",newerpatchno);
        npno +=tmp;
    
    fputs(npno.c_str(),curverfp);
#ifndef WIN32
    fputs("\n",curverfp);
#else
    fputs("\r\n",curverfp);
#endif
    }
    std::string name("file_name:");
    name+= "fics_";
    name+=newerver+"_"+newerdate;
    if(newerpatchno>0)
    {
        name+="_patch";
        name+=tmp;
    }
    
    fputs(name.c_str(),curverfp);
#ifndef WIN32
    fputs("\n",curverfp);
#else
    fputs("\r\n",curverfp);
#endif
    fclose(curverfp);
    return 0;
}
/**
 * @brief 比较服务器的Patchs和本地的包进行对比，并将缺少的包标记到 lossPatchs中
 *
 * @param serPatchs: 
 * @param lossPatchs
 *
 * @return 本地库和远程库一样则返回0，否则返回缺少包的个数
 */
int FiUpdateAssistant::comparePatchs(version_t *netVer, patchSet_t serPatchs, patchSet_t  lossPatchs)
{
    int i=0;
    int ret = 0;
    int rpcRet = 0;
    int trycnt=0;
    bool exist = false;
    const char *prefix = prixMatrix[which];
    vector <string> pkgList;
    vector <string> pathDirList;
    vector <string>::iterator itr;
    char tmpPath[256]={0};
    char tmpPathDir[256]={0};
    string version = netVer->version;
    string remoteMD5;
    string localMd5;
    int patchNo = 0;
    char *suffix = NULL;
    
    int basePatch = getCurBaseVer(atoi(netVer->patchNo));
    std::vector<std::string> optinalname;
    gen_optional_pack_name(PInfo,optinalname);

    getDirList("./", pathDirList);//get dir list

    if (getPkgList("./", prefix, ".tar.gz", version.c_str(), pkgList) < 0)
    {
        ut_err("get pkg list fail\n");
    }
    for (i=0; i<BASEINTERVAL; i++)
    {
        exist = false;
        if (serPatchs[i][0] != 0)//服务器有此包
        {
            patchNo = basePatch * BASEINTERVAL;
            //check local pkg /sobey/fics/update/fics_v1.0.0_2015.10.14(*)_4005 暂时只检查目录不检查tar.gz的压缩包 fics_v1.0.0_date(skip)_4005
            // i ==> patchNum
            sprintf(tmpPath, "%s_%s_*_%d%s", prixMatrix[which], version.c_str(), patchNo+i, optinalname[0].c_str());//正则 need to modify not fics_... client_... "server_v1.0.0_2015.10.10_4005_32/64_Linux2.6.tar.gz"; "server_v1.0.0_*_4001_32_Linux2.6.tar.gz
            //first : match dir
            strcpy(tmpPathDir, tmpPath);
            suffix = strstr(tmpPathDir, ".tar.gz");
            if (suffix)
            {
                *suffix = '\0'; 
            }
            for (itr = pathDirList.begin(); itr!= pathDirList.end(); itr++)
            {
                if (matchRE(itr->c_str(), tmpPathDir))
                {
                    exist = true;
                    break;
                }
            }
            //second : match tar.gz
            if (!exist && !pkgList.empty())
            {
                for (itr = pkgList.begin(); itr!=pkgList.end(); itr++)
                {
                    if (matchRE(itr->c_str(), tmpPath))
                    {
                        /*-----------------------------------------------------------------------------
                         *  1. get md5sum from server
                         *  2. compar = true; break; 
                         *-----------------------------------------------------------------------------*/
                        remoteMD5 = "";
                        localMd5  = "";
                        if (getMD5FromRemote(itr->c_str(), remoteMD5) < 0)
                        {
                            ut_err("get remote md5 fail\n");
                        }
                        getMD5FromLocal(itr->c_str(), localMd5);
                        if (remoteMD5 == localMd5)
                        {
                            exist = true;
                            break;
                        }
                    }
                }
            }
            if (!exist)//目录不存在 1. get dir name. 2. cmp file name
            {
                lossPatchs[i][0] = 1;
                lossPatchs[i][1] = serPatchs[i][1];
                ret++;
            }
        }
    }
    return ret;
}

