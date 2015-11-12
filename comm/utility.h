#ifndef _FI_UPDATE_UITILITY_H__
#define _FI_UPDATE_UITILITY_H__

#ifdef WIN32
#include<windows.h>
#else
#include<pthread.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#endif
#include<vector>
#include<string>

#include "include.h"

typedef struct 
{
    unsigned long ip;
    int       nodeId;
//    nodeIp_t(unsigned long ip, int nodeId):ip(ip),nodeId(nodeId){};
}nodeIp_t;
#ifdef WIN32
int replace(char *path, int c, int d);
char *do_dirname(char *path);
char *do_basename(char *path);

#else
#endif
/**
 * @brief 去掉字符串结尾的空格 
 *
 * @param str
 *
 * @return 
 */
int stripSpace(char *str);
/**
 *  @brief: Returns the current time in common log format in a static
 * char buffer.
 *
 * commonlog time is exactly 25 characters long
 * because this is only used in logging, we add " [" before and "] " after
 * making 29 characters
 * "[27/Feb/1998:20:20:04 +0000] "
 *
 * Constrast with rfc822 time:
 * "Sun, 06 Nov 1994 08:49:37 GMT"
 * @return 
 */
char *get_commonlog_time(void);
/**
 * @brief 从FicsConfig.xml解析所有的外部ip+nodeId放入vecIpAddr
 *
 * @param vecIpAddr
 *
 * @return 
 */
int getIPfromXml(std::vector<nodeIp_t>& vecIpAddr);
/**
 * @brief 获取下载服务器的IP
 *
 * @param vecIpAddr
 *
 * @return 
 */
int getServerIP(std::vector<unsigned long>& vecIpAddr);
/**
 * @brief 获取和fics-config.xml匹配的外部IP
 *
 * @param ip
 *
 * @return 
 */
int getCurLocalIp(std::string &ip);
/**
 * @brief 获取本地所有网卡的Ip
 *
 * @param vecIpAddr
 *
 * @return 
 */
int getLocalIpAll(std::vector<unsigned long>& vecIpAddr);
/**
 * @brief 比较2个版本号的大小
 *
 * @param local
 * @param net
 *
 * @return  若local > net，返回>0 local < net 返回<0 版本一样则返回0
 */
int compareVersion(version_t *local, version_t *net);
int getLocalVersion(version_t *version);
int FiGetCurDir(int size,char* buff);
int getPkgDlDir(int size,char* buff);
int writeLocalVer(version_t *ver);

class FiLock
{
public:
	FiLock();
	~FiLock();
public:
	int lock();
	int unlock();
private:
#ifdef WIN32
	CRITICAL_SECTION mutex;
#else
	pthread_mutex_t  mutex;
#endif
};
class FiEvent
{
public:
	FiEvent();
	~FiEvent();
public:
	int signal();
	int wait(unsigned int timeout=0xffffffff);//msec
private:
#ifdef WIN32
	HANDLE hEvent;
#else
	pthread_cond_t  cond;
	pthread_mutex_t mutex;
#endif
};

#ifdef WIN32
typedef HANDLE FILEPTR;
#else
typedef int FILEPTR;
#endif

#ifdef WIN32
#define FILE_FROM_BEGIN FILE_BEGIN
#else
#define FILE_FROM_BEGIN SEEK_SET
#endif

int  FiCreateFile(const char* fullname);
FILEPTR FiOpenExistFile(const char* fullname);
int FiReadFile(FILEPTR fp,void*buff,int size);
int FiWriteFile(FILEPTR fp,void*buff,int size);
int FiSeekFile(FILEPTR fp,long dis,int dir);
int FiCloseFile(FILEPTR fp);
int FiExecuteShell(const char* cmd);
int FiExeWinUncompress(const char*cmd);
int FiUncompressFile2CurPath(const char* fullname);
int FiCreateDir(char* path);
bool FiIsExistFile(const char* fullname);
int FiGetAllFolder(const char *path,std::vector<std::string>& allfolders);

bool CheckFile_Md5(std::string& file1,std::string& file2);



struct File_Layout_t
{
    std::string strAction;
    std::string strName;
    std::string strLocation;
};
struct Reg_Layout_t
{
    std::string strAction;
    std::string strName;
	std::string strLocation;
};

struct FiUpdat_Pack_t
{
    bool bReboot;
    std::vector<File_Layout_t> allFiles;
    std::vector<Reg_Layout_t> allRegs;
    FiUpdat_Pack_t()
    {
        bReboot = false;
    }
};

bool travel_dir(char* path,std::vector<std::string>& allincfiles);
#endif
