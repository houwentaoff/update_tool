#ifndef _FI_UPDATE_ASSISTAN_H__
#define _FI_UPDATE_ASSISTAN_H__

#include "../comm/concurrent.h"
#include "../comm/FiRpcCli.h"
#include "../comm/FiRpcSrv.h"
#include "../comm/utility.h"
#include "../comm/include.h"
#include "../idl/QueryUpdate.hh"
typedef RPC_PROXY_HANDLE<FiUpdateMgr>* RPCHandle;
class FiUpdateAssistant
{
public:
    typedef enum
    {
        EWINDOWS,
        ELINUX,
        EMAC
    }os_e;
public:
    //int update();
    int setFile(std::string name);
    int downloadFile(const char *ip, const char *srcName, const char *dstName);
    virtual int svc();
private:
    int QueryPlatformInfo(PlatformInfoEx& info);
protected:
    FiUpdateAssistant();
    FiUpdateAssistant(FiEvent* hEvent);
    virtual ~FiUpdateAssistant();
    virtual int stop()
    {
        bReplaceEnd = true;
        evtReplaceFile.signal();
		return 0;
    }
public:
    int set(FiEvent* hEvent)
    {
        evnt = hEvent;
		return 0;
    }
    int startup();
    bool ConnectUpMgr();
	int DestroyConnectUpMgr();
    bool IsUpMgrConnected()
    {
		if( rHandle !=NULL)
		{
			return rHandle->isValidate;
		}
		else
		{
			return false;
		}
    }
    bool IsUpFinished()
    {
        return bUpFinished;
    }
    int QueryCurrentVersion(version_t *version);
    int OnUpdateFinish()
	{
		char buff[200];
		FiGetCurDir(sizeof(buff),buff);
		std::string rootpath(buff);
		std::string strcmd("cd ");
		strcmd += rootpath.c_str();
		FiExecuteShell(strcmd.c_str());
		evnt->signal();
		return 0;
	}
	//int UpdateVersionFile();
    //int handl_input(const char* filename);
    int RollBack(version_t *dstVer);
	bool CheckReg(std::vector<Reg_Layout_t>& regs);
public:
    static FiUpdateAssistant* getinstance()
    {
        static FiUpdateAssistant singleton;
        return &singleton;
    }

	bool checkfolder(const char* folder1_full,const char* folder2_full);
    int downLossPkg(version_t *netVer, long ldate, int patchNo);
    int beginDownloadPkg(const char* filename);
    int installAllPatch(version_t *ver);
    int installSinglePatch(const char *fileName);
    int queryPatchs(patchSet_t patchs);
    int comparePatchs(version_t *netVer, patchSet_t serPatchs, patchSet_t  lossPatchs);
    int getMD5FromRemote(const char *fileName, std::string &remoteMD5);
    bool restartAPP(const char *elf);
    int installOldPkg(const char *fileName);
    int setPlatform(long which);
private:
    FiRpcCli<FiUpdateMgr> *cli;
    RPCHandle rHandle;
    FiEvent* evnt;
    long   which;
    FILEPTR   fpdownload;

    FiEvent  evtReplaceFile;
    std::string dirfilename;
    bool    bReplaceEnd;
    std::string filename;
    //std::string layout;
    std::string newerver;
    std::string newerdate;
	int         newerpatchno;

    bool bUpFinished;

    std::string localIP;

	PlatformInfoEx    PInfo;
	CORBA::LongLong          _ref;
    bool installOver;
    bool ShutDown;
    int totalPkg;
    int curCountInstalled;
public:
    version_t localVer;
    version_t netVer;
};

#endif
