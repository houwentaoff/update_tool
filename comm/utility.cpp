#include "utility.h"
#ifdef WIN32
#include "Markup_win32.h"
#else
#include "Markup_linux.h"
#endif
#ifdef WIN32
#include<windows.h>
#include<direct.h>
#include<assert.h>
#else
#include<pthread.h>
#include<unistd.h>
#include<sys/time.h>
#include<fcntl.h>
#include   <sys/types.h> 
#include   <dirent.h> 
#include   <sys/stat.h>
#include <stdio.h>
#include <sys/utsname.h>
#include<errno.h>
#endif
#include<stdlib.h>
#include <assert.h>
#include<string.h>
#include "Md5.h"

#ifdef FI_MAC
#include <mach-o/dyld.h>
#endif
#pragma  warning(disable:4819)
#pragma  warning(disable:4996)

#ifndef WIN32
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>


#include <sys/types.h> 
#include <sys/param.h> 
#include <net/if_arp.h> 
#endif
#include <algorithm>
#include <time.h>
#ifdef BUFSIZ
#undef BUFSIZ
#endif
#define BUFSIZ          512            /*  */
#ifndef WIN32
#define TIMEZONE_OFFSET(foo) foo->tm_gmtoff
#else
#define TIMEZONE_OFFSET(foo) (8*60*60)
#endif
using namespace std;
int use_localtime = 1 ;
const char month_tab[49] =
    "Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";
const char day_tab[] = "Sun,Mon,Tue,Wed,Thu,Fri,Sat,";


int getCurLocalIp(std::string& ip)
{
    int ret = -1;
    int i,j;
    vector<unsigned long> localIp;
    vector<nodeIp_t> vecIpAddrXml;

    getLocalIpAll(localIp);//get ip addr from `ifconfig`
    getIPfromXml(vecIpAddrXml);

    for (i = 0; i < vecIpAddrXml.size(); i++)
    {
        for (j = 0; j < localIp.size(); j++)
        {
            if (localIp[j] == vecIpAddrXml[i].ip)
            {
                unsigned long tmp = localIp[j];
                struct in_addr *test;
                test = (struct in_addr *)&tmp;
                ip   = inet_ntoa(*test);
//                ip   = inet_ntoa((struct in_addr)localIp[j]);
                ret  = 0;
                break;
            }
        }
    }
    if (-1 == ret)
    {
		if (localIp.empty())
		{
			return ret;
		}
        ip = inet_ntoa(*((struct in_addr *)&localIp[0]));//ip="0.0.0.0"?
        ret = 0;
    }
    return ret;
}


int getIPfromXml(std::vector<nodeIp_t>& vecIpAddr)
{
    CMarkup xml;
    const char *cfgConfig = "../config/FicsConfig.xml";
    std::string strTmp;
    int ret = -1;
    char cmdBuf[256];
    FILE *fp = NULL;
    char tmppath[256]={0};
    int r=0;
    std::vector<string> pathList;
    vector<string>::iterator iter;
    vector<nodeIp_t>::iterator it;
    int nodeId;
//#ifndef WIN32    
    if (!FiIsExistFile(cfgConfig))
    {
//        if ()//服务端不允许不存在
        ut_err("file [%s] is not exist, try to auto generate\n", cfgConfig);
        //scan all FicsConfig.xml then connect all ip who is node[0-1].
#ifdef WIN32
		sprintf(cmdBuf, "dir /s /b ..\\*FicsConfig.xml");
#else
		sprintf(cmdBuf, "find ../client/Config -name \'FicsConfig.xml\'");
#endif

#ifdef WIN32
		if (NULL ==(fp = _popen(cmdBuf, "r")))
		{
            ut_err("_popen error \n");
			ret = -2;
			return ret;
		}
        do
        {
            r = fscanf(fp, "%[^\n]", tmppath);
            fgetc(fp);
            if (r != EOF)
            {
                pathList.push_back(tmppath);
            }
        }while (r != EOF);
        _pclose(fp);
#else
        if (NULL == (fp = popen(cmdBuf, "r")))
        {
            ut_err("popen error errno[%d]\n", errno);
            ret = -2;
            return ret;
        }
        do
        {
            r = fscanf(fp, "%[^\n]", tmppath);
            fgetc(fp);
            if (r != EOF)
            {
                pathList.push_back(tmppath);
            }
        }while (r != EOF);
        pclose(fp);
#endif
    }
    else
    {
        pathList.push_back(cfgConfig);
    }
    for (iter = pathList.begin(); iter != pathList.end(); iter++)
    {

        if (!xml.Load(iter->c_str())) 
        {
            ut_err("load ficsconfig xml[%s] failed\n", cfgConfig);
            return ret;
        }    

        /*-----------------------------------------------------------------------------
         *  parse extern IP from FicsConfig.xml
         *-----------------------------------------------------------------------------*/
        if (xml.FindElem("fics"))
        {
            xml.IntoElem();
            if (xml.FindElem("NodeServersInfo"))
            {
                xml.IntoElem();
                while (xml.FindElem("OneNodeServer"))
                {
                    xml.IntoElem();
                    if (xml.FindElem("NodeSeq"))
                    {
#ifdef WIN32
						std::string nodeSeq;
						nodeSeq = xml.GetData();
						nodeId = atoi(nodeSeq.c_str());
#else
                        nodeId = atoi(xml.GetData().c_str());
#endif
                    }
                    xml.FindElem("ExternalIps");
                    xml.IntoElem();
                    while (xml.FindElem("IP"))
                    {
                        strTmp = xml.GetData();
                        unsigned long ip = inet_addr(strTmp.c_str());
                        bool exist = false;
                        for (it = vecIpAddr.begin(); it != vecIpAddr.end(); it++)
                        {
                            if (it->ip == ip && it->nodeId == nodeId)
                            {
                                exist = true;
                                break;
                            }
                        }
                        if (exist == false)
                        {
                            nodeIp_t tmp;
                            tmp.ip = ip;
                            tmp.nodeId = nodeId;
                            vecIpAddr.push_back(tmp);
                        }
                    }
                    xml.OutOfElem();            
                    xml.OutOfElem();            
                }
            }
        }
    }
    if (!vecIpAddr.empty())
    {
        ret = 0;
    }

    return ret;
}
int getLocalIpAll(std::vector<unsigned long>& vecIpAddr)
{
#if defined(_WIN32) || defined(_WIN64)

    WSADATA wsaData;
    int err = WSAStartup(MAKEWORD(2,2),&wsaData);
    if (err != 0)
    { 
        assert(0);
        return -1;
    }

	char szHostName[128]={0}; 

    err = gethostname(szHostName, 128);
    if( err != 0 ) 
    {    
        assert(0);
        return -1;
    }

    struct hostent * pHost; 
    pHost = gethostbyname(szHostName); 
	int errnoa = 1;
	errnoa = WSAGetLastError();
    for (int i=0; (pHost != NULL) && (pHost->h_addr_list[i] != NULL); i++) 
    { 
        vecIpAddr.push_back(((struct in_addr *)pHost->h_addr_list[i])->s_addr);
    }

    WSACleanup();
#else    
    int s;
    struct ifconf conf;
    struct ifreq *ifr;
    char buff[BUFSIZ];
    int num;
    int i;

    s = socket(PF_INET, SOCK_DGRAM, 0);

    conf.ifc_len = BUFSIZ;
    conf.ifc_buf = buff;

    ioctl(s, SIOCGIFCONF, &conf);
    num = conf.ifc_len / sizeof(struct ifreq);
    ifr = conf.ifc_req;

    for (i = 0; i < num; i++)
    {
        struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_addr);

        ioctl(s, SIOCGIFFLAGS, ifr);
        if (((ifr->ifr_flags & IFF_LOOPBACK) == 0) && (ifr->ifr_flags & IFF_UP))
        {
            //printf("%s %s \n",ifr->ifr_name,inet_ntoa(sin->sin_addr));

            if (0 != sin->sin_addr.s_addr)
            {
                vecIpAddr.push_back(sin->sin_addr.s_addr);
            }
        }
        ifr++;
    }

    close(s);    
#endif
    return 0;
}
int getServerIP(std::vector<unsigned long>& vecIpAddr)
{
    int ret = -1;
    vector<nodeIp_t> vecIpAddrXml;
    vector<nodeIp_t>::iterator it;

    if (0 == getIPfromXml(vecIpAddrXml))
    {
        if (vecIpAddrXml.size() >= 2)
        {
            ret = 0;
        }
//        find (vecIpAddrXml.begin(), vecIpAddrXml.end(), ())
        for (it = vecIpAddrXml.begin();it != vecIpAddrXml.end();it++)
        {
            if ((*it).nodeId == 0 || (*it).nodeId == 1)
            {
                vecIpAddr.push_back((*it).ip);
            }
        }

    }
    return ret;
}

int writeLocalVer(version_t *ver)
{
    char patchVerBuf[200];
    FILE *fp;
    char *patchNo;
    char cmdBuf[250];

    if (!ver)
    {
        ut_err("params is null\n");
        goto err;
    }
    sprintf(patchVerBuf, 
            "version:%s\n"
            "date:%s\n"
            "patch:%s\n"
            "file_name:fics_%s_%s",
            ver->version, ver->date, ver->patchNo, ver->version, ver->date);
    patchNo = ver->patchNo;
    if (*patchNo)
    {
        sprintf(patchVerBuf+strlen(patchVerBuf),
                "_%s", ver->patchNo);
    }        

    fp = fopen("/sobey/fics/patch_version.tmp", "wb+");
    if (!fp)
    {
        ut_err("fp is null\n");
        goto err;
    }
    fputs(patchVerBuf, fp);
    fclose(fp);
    sprintf(cmdBuf, "mv /sobey/fics/patch_version.tmp %s -f", _PATH_VERSION);
    system(cmdBuf);
    return 0;
err:
    return -1;
}
int compareVersion(version_t *local, version_t *net)
{
    int ret;
    int newFirstVer     = 0;
    int newSecondVer    = 0;
    int newLastVer      = 0;    
    int localFirstVer   = 0;
    int localSecondVer  = 0;
    int localLastVer    = 0;
    int localDateYear   = 0;
    int localDateMoth   = 0;
    int localDateDay    = 0;
    int newDateYear     = 0;
    int newDateMoth     = 0;
    int newDateDay      = 0;
    int localpatchno    = 0;
    int newerpatchno    = 0;
    string localHash    = "";
    string newerHash    = "";

    if (!local || !net)
    {
        goto err;
    }
    
    sscanf(local->version, "v%d.%d.%d", &localFirstVer, &localSecondVer, &localLastVer);
    sscanf(local->patchNo, "%d", &localpatchno);
    sscanf(local->date, "%d.%02d.%02d", &localDateYear, &localDateMoth, &localDateDay);
    localHash = local->reserved.hash;
    ut_dbg("the local version:%s date:%s local %d.%d.%d date:%d.%d%d patch:%d\n hash:%s\n",
     local->version, local->date ,localFirstVer,localSecondVer,localLastVer,
        localDateYear,localDateMoth,localDateDay,localpatchno, localHash.c_str());
    fflush(stdout);
    if (0 == localFirstVer)
    {
        ut_err("is null\n");
    }
    sscanf(net->patchNo, "%d", &newerpatchno);
    sscanf(net->version,"v%d.%d.%d",&newFirstVer,&newSecondVer,&newLastVer);
    sscanf(net->date,"%d.%02d.%02d",&newDateYear,&newDateMoth,&newDateDay);
    newerHash = net->reserved.hash;

//    newerver = version;
//    newerdate = date;
//    std::string newerno = patchno;

    ut_dbg("from the net(svr/cli) version:%s date:%s net %d.%d.%d date:%d.%d%d patch:%d hash:%s\n"
        , net->version, net->date,newFirstVer,newSecondVer,newLastVer,
        newDateYear,newDateMoth,newDateDay,newerpatchno,newerHash.c_str());
    fflush(stdout);
    ret = (localFirstVer*10000+localSecondVer*100+localLastVer) - (newFirstVer*10000+newSecondVer*100+newLastVer);
 /* :TODO:2015/10/21 15:58:00:hwt:  忽略时间只比较版本号和补丁号,已经hash值*/
#if 0
    if (ret == 0)
    {
        ret = (localDateYear*10000+localDateMoth*100+localDateDay) - (newDateYear*10000+newDateMoth*100+newDateDay);
    }
#else
    if (ret == 0)
    {
        ret = localpatchno - newerpatchno;
    }
    if (ret == 0)
    {
        if (localHash != newerHash)
        {
            ret = -1;//上传过去的补丁包，当前版本没有更新,
        }
    }
#endif
 /* :TODO:End---  */
    
    return ret;
err:
    return 1000000;//need to modify
}
int getLocalVersion(version_t *version)
{
    FILE *fp;
    int r;
    char buf[LEN_BUF] = {0};
    char *p           = buf;
    
    if (!version)
    {
        goto err;
    }
    
    fp = fopen(_PATH_VERSION, "r");
    if (fp == NULL)
    {
        ut_err("open fail\n");
        goto err;
    }
    while ((r = fscanf(fp, "%s", buf)) == 1)
    {
        if (buf[0] == '#')
        {
            if (fscanf(fp, "%*[^\n]\n") < 0)
            {
                ut_err("skip # fail\n");
                goto err;
            }
            continue;
        }
        if (0 == strncmp(buf, "version:", strlen ("version:")))
        {
            r = fscanf(fp, "%*[ ]%[^\n]s", buf);//skip space huiche
            p = r == 0 ? strchr(buf, ':')+1 : buf;
            strcpy(version->version, p);

        }
        else if (0 == strncmp(buf, "patch:", strlen ("patch:")))
        {
            r = fscanf(fp, "%*[ ]%[^\n]s", buf);//skip space huiche
            p = r == 0 ? strchr(buf, ':')+1 : buf;
            strcpy(version->patchNo, p);
        }
        else if (0 == strncmp(buf, "date:", strlen ("date:")))
        {
            r = fscanf(fp, "%*[ ]%[^\n]s", buf);//skip space huiche
            p = r == 0 ? strchr(buf, ':')+1 : buf;
            strcpy(version->date, p);
        }
        else if (0 == strncmp(buf, "hash:", strlen ("hash:")))
        {
            r = fscanf(fp, "%*[ ]%[^\n]s", buf);//skip space huiche
            p = r == 0 ? strchr(buf, ':')+1 : buf;
            strcpy(version->reserved.hash, p);
        }
        
    }
    fclose(fp);
  
    return 0;
err:
    if (fp)fclose(fp);
    return 1000000;
}
int getPkgDlDir(int size, char* buff)
{
    int ret =0;

#ifdef WIN32
    ret = FiGetCurDir(size, buff);
#else
    strncpy(buff, _PATH_PKG_DL, size);
#endif
    return ret;
}
int FiGetCurDir(int size,char* buff)
{
#ifdef WIN32
      char szPath[MAX_PATH];
      ::GetModuleFileName(GetModuleHandle(NULL), szPath, MAX_PATH);
      int len = strlen(szPath);
      int cnt=len-1;
      while(szPath[cnt]!='\\')
      {
          cnt--;
      }
      szPath[cnt]='\0';
      strcpy(buff,szPath);
      strcat(buff,"\\");
      return 1;
#else
    int icount = 0,icountTem = 0;
    //char cPath[1024] = {0};
    if (buff == NULL)
    return 0;
    #ifndef FI_MAC
    icount  = readlink("/proc/self/exe",buff,size );
#else
    uint32_t usize=size;
    icount = _NSGetExecutablePath(buff,&usize);
    if(icount <0 )
    {
        buff[0] = '\0';
        return 0;
    }
    icount  = strlen(buff);
#endif
    if(icount < 0 || icount >= size)
    {
        buff[0] = '\0';
        return 0;
    }
    buff[icount] = '\0';
    icountTem = icount;
    do
    {
        if(buff[icountTem] == '/')
        {
            icountTem++;
            buff[icountTem] = '\0';
            break;                
        }
        icountTem--;
    }while(icountTem >= 0);
    return icountTem;
    
#endif

}


FiLock::FiLock()
{
#ifdef WIN32
    InitializeCriticalSection(&mutex);
#else
    pthread_mutex_init(&mutex,NULL);
#endif
}
FiLock::~FiLock()
{
#ifdef WIN32
    DeleteCriticalSection(&mutex);
#else
    pthread_mutex_destroy(&mutex);
#endif
}

int FiLock::lock()
{
#ifdef WIN32
    EnterCriticalSection(&mutex);
#else
    pthread_mutex_lock(&mutex);
#endif
    return 0;
}
int FiLock::unlock()
{
#ifdef WIN32
    LeaveCriticalSection(&mutex);
#else
    pthread_mutex_unlock(&mutex);
#endif
    return 0;    
}

FiEvent::FiEvent()
{
#ifdef WIN32
    hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
#else
        pthread_mutex_init(&mutex,NULL);
    //mutex = PTHREAD_MUTEX_INITIALIZER;
        pthread_cond_init(&cond,NULL);
#endif  
}

FiEvent::~FiEvent()
{
    #ifdef WIN32
    CloseHandle(hEvent);
    #else
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    #endif
}

int FiEvent::signal()
{
    #ifdef WIN32
    SetEvent(hEvent);
    #else
    pthread_cond_broadcast(&cond);
    #endif
    return 0;
}
int FiEvent::wait(unsigned int timeout)//msec
{
    #ifdef WIN32
    DWORD ret =0;
    if(timeout ==0xffffffff)
        ret = WaitForSingleObject(hEvent,INFINITE);
    else
        ret = WaitForSingleObject(hEvent,timeout);
    if( ret ==WAIT_TIMEOUT)
    {
        return 0;
    }
    return 1;
    #else
    pthread_mutex_lock(&mutex);
    int ret = -1;
    if( timeout == 0xffffffff)
    {
        ret = pthread_cond_wait(&cond,&mutex);
    }
    else
    {
        struct timespec tsp;
        struct timeval now;
        gettimeofday(&now,NULL);
        tsp.tv_sec=now.tv_sec+timeout/1000;
        tsp.tv_nsec=now.tv_usec*1000;
        ret = pthread_cond_timedwait(&cond,&mutex,&tsp);
    }
    pthread_mutex_unlock(&mutex);
    if( ret == 0)
    {
        return 1;
    }
    return 0;
    #endif
    
}
#ifdef WIN32
int FiCreateFile(const char* fullname)
{
 HANDLE hFile = ::CreateFile(fullname,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ|FILE_SHARE_WRITE,
                                NULL,
                                CREATE_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL
                                );
    assert(hFile != INVALID_HANDLE_VALUE);

    CloseHandle(hFile);
    return 0;
}
#else
int FiCreateFile(const char* fullname)
{
    int ret = open(fullname,O_RDWR|O_CREAT|O_TRUNC,777);
    if( ret ==-1)
    {
        fclose(fopen(fullname,"w+"));
    }
    close(ret);
    return ret;
}
#endif



#ifdef WIN32
int FiExecuteShell(const char* cmd)
{
    printf("%s\n",cmd);
    //WinExec(cmd,SW_SHOW);
    system(cmd);
    return 0;
}
int FiExeWinUncompress(const char*cmd)
{
    printf("%s\n",cmd);
    WinExec(cmd,SW_SHOW);
    //system(cmd);
    return 0;
}
#else
int FiExecuteShell(const char* cmd)
{ 
    printf(cmd);
    printf("\n");
    system(cmd);
    return 0;
}
#endif

FILEPTR FiOpenExistFile(const char* fullname)
{
#ifdef WIN32
    HANDLE hFile = CreateFile(fullname,
                                GENERIC_READ|GENERIC_WRITE,
                                FILE_SHARE_READ|FILE_SHARE_WRITE,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL
                                );
    assert(hFile != INVALID_HANDLE_VALUE);
    return hFile;
#else
    int fp = open(fullname,O_RDWR);
    assert( fp != -1 );
    return fp;
#endif
}

int FiReadFile(FILEPTR fp,void*buff,int size)
{
#ifdef WIN32
        DWORD cnt=0;
        BOOL ret = ReadFile(fp,buff,size,&cnt,NULL);
        if(!ret)
        {
            //CloseFile(hFile);
            return -9;
        }
        return cnt;
 #else
        ssize_t cnt = read(fp,buff,size);
        return cnt;
 #endif
}

int FiCloseFile(FILEPTR fp)
{
#ifdef WIN32
    CloseHandle(fp);
#else
    close(fp);
#endif
    return 0;
}

int FiSeekFile(FILEPTR fp,long dis,int dir)
{
#ifdef WIN32
    SetFilePointer(fp,dis,NULL, dir);
#else
    lseek(fp,dis,dir);
#endif
    return 0;
}
int FiWriteFile(FILEPTR fp,void*buff,int size)
{
    #ifdef WIN32
    DWORD dwsize=0;
    BOOL ret = WriteFile(fp,buff,size,&dwsize,0);
    if( ret = TRUE)
    {
           return size;
    }
    assert(ret);
    return -1;
    #else
    ssize_t cnt = write(fp,buff,size);
    assert(cnt>0);
    return cnt;
    #endif
    
}

int FiCreateDir(char* path)
{
#ifdef _WIN32

 return _mkdir((path));
#else
    char cmdBuf[512];
    sprintf(cmdBuf, "mkdir -p %s; chmod 777 %s", path, path);
 return system(cmdBuf);//mkdir(path,777);
#endif
    return 0;
}

int FiGetAllFolder(const char *path,std::vector<std::string>& allfolders)
{
    #ifdef WIN32
    WIN32_FIND_DATA FindFileData;  
    HANDLE hFind=INVALID_HANDLE_VALUE;  
    std::string fullpath(path);
    fullpath+="*";
  
    hFind=FindFirstFile(fullpath.c_str(),&FindFileData);          //找到文件夹中的第一个文件  
  
    if(hFind==INVALID_HANDLE_VALUE)                               //如果hFind句柄创建失败，输出错误信息  
    {  
        FindClose(hFind);   
        return -1;    
    }  
    else   
    {  
        while(FindNextFile(hFind,&FindFileData)!=0)                            //当文件或者文件夹存在时  
        {  
            if((FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0&&strcmp(FindFileData.cFileName,".")==0||strcmp(FindFileData.cFileName,"..")==0)        //判断是文件夹&&表示为"."||表示为"."  
            {  
                 continue;  
            }  
            if((FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0)      //判断如果是文件夹  
            {  
                allfolders.push_back(FindFileData.cFileName);
            }  
        }  
        FindClose(hFind);  
    }  
    #else
    if( path == NULL)
    {
        return -1;
    }
    DIR*   dir=NULL; 
    struct   dirent*   dirlist; 
    struct   stat   filestat;

    dir = opendir(path);
    if( dir == NULL) {
        return -1;
    }
    while( (dirlist=readdir(dir))!= NULL ) 
    {

        if(
           (strcmp(dirlist->d_name, ".")==0) ||
           (strcmp(dirlist->d_name, "..")==0)
          )
        {
            continue;
        }
        std::string fullName=path;
        //fullName += "/";
        fullName += dirlist->d_name;
        //fullName.Format(_T("%s/%s"),W2A(path),dirlist->d_name);
        stat(fullName.c_str(),&filestat);
        if (S_ISDIR(filestat.st_mode) )
        {
            allfolders.push_back(dirlist->d_name);
        }

    }
    
    #endif
    return 1;
}

// unsigned int conv(unsigned int a)
// {
//     unsigned int b=0;
//     b|=(a<<24)&0xff000000;
//     b|=(a<<8)&0x00ff0000;
//     b|=(a>>8)&0x0000ff00;
//     b|=(a>>24)&0x000000ff;
//     return b;
//
bool CheckFile_Md5(std::string& file1,std::string& file2)
{
    if( file1 == file2)
        return true;
    FILE* fp1= fopen(file1.c_str(),"rb");
    FILE* fp2 = fopen(file2.c_str(),"rb");
    if( fp1 == NULL || fp2==NULL)
    {
        return false;
    }
    MD5VAL mval1 = md5File(fp1);
    MD5VAL mval2 = md5File(fp2);
    //printf("MD5值1 : %08x%08x%08x%08x\n",(mval1.a),(mval1.b),(mval1.c),(mval1.d));
    //printf("MD5值2 : %08x%08x%08x%08x\n",(mval2.a),(mval2.b),(mval2.c),(mval2.d));
    if(memcmp(&mval1,&mval2,sizeof(MD5VAL)) ==0)
    {
        return true;
    }
    fclose(fp2);
    fclose(fp1);

    return false;
}



bool FiIsExistFile(const char* fullname)
{
    FILE* fp = NULL;
#ifdef WIN32
	fp = fopen(_T(fullname),_T("rb"));
#else
	fp = fopen(fullname, "rb");
#endif
    if(fp ==NULL)
        return false;
    fclose(fp);
    return true;
}

#ifdef WIN32
bool travel_dir(char* path,std::vector<std::string>& allincfiles)
{
    //此结构说明参MSDN;
    WIN32_FIND_DATA FindFileData;
    //查找文件的句柄;
    HANDLE hListFile;
    char szFullPath[MAX_PATH];
    //相对路径;
    char szFilePath[MAX_PATH];
    //构造相对路径;
    sprintf(szFilePath, "%s\\*", path);
    //查找第一个文件，获得查找句柄，如果FindFirstFile返回INVALID_HANDLE_VALUE则返回;
    if((hListFile = FindFirstFile(szFilePath, &FindFileData)) == INVALID_HANDLE_VALUE)
    {
        //查找文件错误;
        return false;
    }
    else
    {
        do 
        {
            //过滤.和..;
            //“.”代表本级目录“..”代表父级目录;
            if( FindFileData.cFileName[0]==(TCHAR)('.') )
            {
                continue;
            }
            std::string filename = FindFileData.cFileName;
            //构造全路径;
            sprintf(szFullPath, "%s\\%s", path, FindFileData.cFileName);


            //如果是文件夹，则递归调用EnmuDirectory函数;
            if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                filename +="/*";
            }
            allincfiles.push_back(filename);
            
        }while(FindNextFile(hListFile, &FindFileData));
    }
    //关闭句柄;
    FindClose(hListFile);
    return true;
}
#else
bool travel_dir(char* path,std::vector<std::string>& allincfiles)
{
    if( path == NULL)
    {

        printf("The path is empty!\n");
        return false;
    }
    DIR*   dir=NULL; 
    struct   dirent*   dirlist; 
    struct   stat   filestat;

    dir = opendir(path);
    if( dir == NULL)
    {
        printf("Open directory : %s failed errno:%d!\n",path,errno);
        return false;
    }
    while( (dirlist=readdir(dir))!= NULL )
    {

        if((dirlist->d_name[0]=='.')
          )
        {
            continue;
        }
        std::string filename =dirlist->d_name; 
        std::string fullName=path;
        fullName += "/";
        fullName += dirlist->d_name;
        stat(fullName.c_str(),&filestat);

        if(S_ISDIR(filestat.st_mode) )
        {
            filename +="/*";

        }
        allincfiles.push_back(filename);
    }
    closedir(dir);
    return true;
}
#endif
char *get_commonlog_time(void)
{
    struct tm *t;
    char *p;
    unsigned int a;
    static char buf[30];
    int time_offset;
    time_t current_time;

    time(&current_time);
    
    if (use_localtime) {
        t = localtime(&current_time);
        time_offset = TIMEZONE_OFFSET(t);
    } else {
        t = gmtime(&current_time);
        time_offset = 0;
    }

    p = buf + 29;
    *p-- = '\0';
    *p-- = ' ';
    *p-- = ']';
    a = abs(time_offset / 60);
    *p-- = '0' + a % 10;
    a /= 10;
    *p-- = '0' + a % 6;
    a /= 6;
    *p-- = '0' + a % 10;
    *p-- = '0' + a / 10;
    *p-- = (time_offset >= 0) ? '+' : '-';
    *p-- = ' ';

    a = t->tm_sec;
    *p-- = '0' + a % 10;
    *p-- = '0' + a / 10;
    *p-- = ':';
    a = t->tm_min;
    *p-- = '0' + a % 10;
    *p-- = '0' + a / 10;
    *p-- = ':';
    a = t->tm_hour;
    *p-- = '0' + a % 10;
    *p-- = '0' + a / 10;
    *p-- = ':';
    a = 1900 + t->tm_year;
    while (a) {
        *p-- = '0' + a % 10;
        a /= 10;
    }
    /* p points to an unused spot */
    *p-- = '/';
    p -= 2;
    memcpy(p--, month_tab + 4 * (t->tm_mon), 3);
    *p-- = '/';
    a = t->tm_mday;
    *p-- = '0' + a % 10;
    *p-- = '0' + a / 10;
    *p = '[';
    return p;                   /* should be same as returning buf */
}


