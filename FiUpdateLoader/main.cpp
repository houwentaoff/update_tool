#include"../comm/utility.h"
#include "FiUpdateAssistant.h"
#include "../comm/FiRpcCli.h"
#include "../comm/include.h"
#include<iostream>
#include "SAXParserHandler.h"
#ifdef WIN32
#include <Windows.h>
#include <assert.h>
#include <stdlib.h>
#include <direct.h>
// #ifdef  _WIN32_WINNT
// #undef _WIN32_WINNT
// #define _WIN32_WINNT 0x0501
#else
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdio.h>
#include <string>


//extern FILE *stdout;
using namespace std;

#define  FiEnableCoreDumps()\
{\
    struct rlimit   limit;\
    limit.rlim_cur = RLIM_INFINITY;\
    limit.rlim_max = RLIM_INFINITY;\
    setrlimit(RLIMIT_CORE, &limit);\
}
//check if FicsConfig changed restart FiUpdateLoader
int main(int argc, char **argv)
{
	char _path[260];
    version_t netVer;
    version_t localVer;
    int ret = 0;

    memset(&netVer, 0, sizeof(version_t));
    memset(&localVer, 0, sizeof(version_t));
    
#ifdef WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	assert(0==iResult);

	TCHAR szWinSysDir[MAX_PATH+1] = "";
	GetSystemDirectory(szWinSysDir,sizeof(szWinSysDir)/sizeof(TCHAR));
	HWND hwnd;
	if(hwnd=::FindWindow("ConsoleWindowClass",NULL)) //找到控制台句柄
	{
		//sean 
		::ShowWindow(hwnd,SW_HIDE); //隐藏控制台窗口
	}

	{
		//解决bug:提前加载socket的库，避免执行下面的Wow64DisableWow64FsRedirection后，导致加载socket库不正常，omniorb连接不上
		int sock = socket(AF_INET,SOCK_STREAM,0);
		int xxx = WSAGetLastError();
		assert(sock != -1);
	}

	// Load kernel32 library
	typedef BOOL (WINAPI * LPWOW64DISABLEWOW64FSREDIRECTION)(PVOID *);
	LPWOW64DISABLEWOW64FSREDIRECTION lpWow64DisableWow64FsRedirection = NULL;
	HMODULE hK32lib = LoadLibrary(_T("kernel32.dll"));
	if (hK32lib != NULL)
	{
		lpWow64DisableWow64FsRedirection = (LPWOW64DISABLEWOW64FSREDIRECTION)GetProcAddress(hK32lib, "Wow64DisableWow64FsRedirection");
	}
	LPVOID oldValue;
	BOOL bResult = FALSE;
	if (lpWow64DisableWow64FsRedirection != NULL)
	{
		bResult = lpWow64DisableWow64FsRedirection(&oldValue);
	}

	//SHGetSpecialFolderPath(NULL, szTemp, CSIDL_PROGRAM_FILES, FALSE);

	FreeLibrary(hK32lib);

// 	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL); 
// 	LPFN_ISWOW64PROCESS fnIsWow64Process; 
// 	BOOL bIsWow64 =FALSE;
// 	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle("kernel32"),"IsWow64Process"); 
// 	if (NULL != fnIsWow64Process) 
// 	{ 
// 
// 		fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
// 	} 
// 	if( bIsWow64)
// 	{
// 		PVOID OldValue;
// 		try
// 		{
// 			BOOL bRet = Wow64DisableWow64FsRedirection (&OldValue);
// 		}
// 		catch (...)
// 		{
// 			
// 		}
// 
// 		
// 
// 	}
#else
    FiEnableCoreDumps();
#endif
	FiGetCurDir(sizeof(_path),_path);
#ifdef WIN32
    if (0 == _chdir(_path)) 
#else
    if (0 == chdir(_path))
#endif
        
    {
        ut_dbg("change cur dir success\n");
    }
    else
    {
        ut_err("change cur dir fail\n");
    }
       
	std::string str(_path);
	str+="FiUpdateLoader.log";
	freopen(str.c_str(),"a",stdout);
	freopen(str.c_str(),"a",stderr);
	ut_dbg ("path %s\n", str.c_str());
	FiEvent evnt;
    char pathConf[256];
    XmlParserEngine xmlParser;
    //std::string serverIp[2]={"",""};
    vector<string>serverIp;
    vector<unsigned long>::iterator it;
    std::string localIp;    
    int count = 2;
    int nodeSeq;
    int size=0;
    vector<unsigned long> vecIpAddr;
    CMarkup xmlmaker, xmlloader;
    
    FiGetCurDir(sizeof(pathConf),pathConf);
    strcat(pathConf, "../config/network.xml");

	ut_dbg ("path2 %s\n", pathConf);

    if (!FiIsExistFile(pathConf))
    {
        ut_err("update config : network.xml is not exist. try to auto generate.\n");
    }
    //linux 1. get local ip from ifconfig & ficsConfig.xml,2.get serverip from FicsConfig.xml 
//#ifndef WIN32

    if (getCurLocalIp(localIp) < 0)
    {
        ut_err("getCurLocalIp fail\n");
        goto err;
    }

    if (getServerIP(vecIpAddr) < 0)
    {
        ut_err("get server download ip fail\n");
        goto err;
    }
//    unsigned long tmp = vecIpAddr[0];
    for (it = vecIpAddr.begin(); it != vecIpAddr.end(); it++)
    {
        string  ip = inet_ntoa(*((struct in_addr*)&(*it)));
        serverIp.push_back(ip);
    }
//    serverIp[0] = inet_ntoa(*((struct in_addr*)&vecIpAddr[0]));
//    serverIp[1] = inet_ntoa(*((struct in_addr*)&vecIpAddr[1]));
    xmlmaker.SetDoc("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");     
    xmlmaker.AddElem("UpMgrConfig");
    xmlmaker.IntoElem();
    xmlmaker.AddElem("LocalIP", localIp.c_str());
    size = serverIp.size();
    for (int i=0; i<size; i+=2)
    {
        xmlmaker.AddElem("UpMgrIp", serverIp[i].c_str());
        xmlmaker.AddElem("UpMgrBackIp", serverIp[i+1].c_str());
    }
//    xmlmaker.AddElem("UpMgrIp", serverIp[0].c_str());
//    xmlmaker.AddElem("UpMgrBackIp", serverIp[1].c_str());
#ifdef WIN32
    if (!xmlmaker.Save(_T("../config/network.xml")))
#else
    if (!xmlmaker.Save("../config/network.xml"))
#endif
    {
        ut_err("generate network.xml fail\n");
    }
    else
    {
        ut_dbg("generate network.xml success\n");
    }
//#endif
    FiUpdateAssistant::getinstance()->set(&evnt);
    FiUpdateAssistant::getinstance()->startup();
    FiUpdateAssistant::getinstance()->ConnectUpMgr();
    do
    {
        while( !FiUpdateAssistant::getinstance()->IsUpMgrConnected() )
        {
			FiUpdateAssistant::getinstance()->DestroyConnectUpMgr();
#ifdef WIN32
            Sleep(6000);
#else
            sleep(6);
#endif
			ut_dbg("try reconnect up mgr\n");
			fflush(stdout);
            FiUpdateAssistant::getinstance()->ConnectUpMgr();
        }
        
        //FiUpdateAssistant::getinstance()->RollBack("v1.9.1","2012.1203");
        ret = FiUpdateAssistant::getinstance()->QueryCurrentVersion(&netVer);
        getLocalVersion(&localVer);
        ret = compareVersion(&localVer, &netVer);
        if (ret < 0)
        {
            //update
            FiUpdateAssistant::getinstance()->update();
        }
        else if (ret > 0 && ret != 1000000)
        {
            //rollback
            FiUpdateAssistant::getinstance()->update();
#if 0            
            FiUpdateAssistant::getinstance()->RollBack(
            netVer.version, netVer.date, netVer.patchNo);
#endif
        }
        else
        {
            ut_dbg("cur version is newest.\n");
            fflush(stdout);
        }
        // 1.query 2.compare 3.download 4.auth..  5.update
		unsigned int sleeptime = 1000*30;
  
        //evnt.wait();
        if( FiUpdateAssistant::getinstance()->IsUpFinished() )
        {
            exit(2);//restart by monitor_update.py
        }
        else
        {
#ifdef WIN32
            Sleep(sleeptime);
#else
            sleep(sleeptime/1000);
#endif
        }
    }while(true);
    
    return 0;
err:
    return -1;
}
