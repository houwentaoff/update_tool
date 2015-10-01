#include"../comm/utility.h"
#include "FiUpdateAssistant.h"
#include "../comm/FiRpcCli.h"
#include "../comm/include.h"
#include<iostream>
#include "SAXParserHandler.h"
#ifdef WIN32
#include <Windows.h>
#include <assert.h>
// #ifdef  _WIN32_WINNT
// #undef _WIN32_WINNT
// #define _WIN32_WINNT 0x0501
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include <stdio.h>
#include <string>
//extern FILE *stdout;

#define  FiEnableCoreDumps()\
{\
    struct rlimit   limit;\
    limit.rlim_cur = RLIM_INFINITY;\
    limit.rlim_max = RLIM_INFINITY;\
    setrlimit(RLIMIT_CORE, &limit);\
}

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
	std::string str(_path);
	str+="FiUpdateLoader.log";
	freopen(str.c_str(),"w",stdout);
	freopen(str.c_str(),"w",stderr);

	FiEvent evnt;
    char pathConf[256];
    XmlParserEngine xmlParser;
    std::string serverIp[2]={"",""};
    std::string localIp;    
    int count = 2;
    int nodeSeq;
    CMarkup xmlmaker, xmlloader;
    
    FiGetCurDir(sizeof(pathConf),pathConf);
    strcat(pathConf, "../config/network.xml");
    if (!FiIsExistFile(pathConf))
    {
        ut_err("update config : network.xml is not exist.\n");
        goto err;
    }
    //linux 1. get local ip from bootip,2.get serverip from FicsConfig.xml 
#ifndef WIN32


    if (!xmlParser.load("../config/bootip.xml"))
    {
        ut_err("bootip.xml is not exist\n");
        goto err;
    }

    localIp= xmlParser.GetEle("localip");

    serverIp[0] = xmlParser.GetEle("ServerIp");
    serverIp[1] = xmlParser.GetEle("ServerIp");

    xmlmaker.SetDoc("xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");     
    if (!xmlloader.Load("../config/FicsConfig.xml"))
    {
        ut_err("FicsConfig.xml is not exist\n");
        goto err;
    }

    xmlloader.ResetMainPos();
    xmlloader.FindElem();
    xmlloader.IntoElem();
    xmlloader.FindElem();
    xmlloader.IntoElem();

    while (count--)
    {
        xmlloader.FindElem("OneNodeServer");
        xmlloader.IntoElem();
        if (xmlloader.FindElem("NodeSeq"))
        {
            nodeSeq = atoi(xmlloader.GetData().c_str());
            if (nodeSeq == 0 || nodeSeq == 1)//server 
            {
                if (!xmlloader.FindElem("InternalIPs"))
                {
                    ut_err("FicsConfig.xml InternalIPs is not exist\n");
                    goto err;
                }
                xmlloader.IntoElem();
                if (!xmlloader.FindElem("IP"))
                {
                    ut_err("xml IP is not exist\n");
                    goto err;
                }
                serverIp[nodeSeq] = xmlloader.GetData();
                xmlloader.OutOfElem();
            }
        }
        xmlloader.OutOfElem();
    }
    
    xmlmaker.AddElem("UpMgrConfig");
    xmlmaker.IntoElem();
    xmlmaker.AddElem("LocalIP", localIp.c_str());
    xmlmaker.AddElem("UpMgrIp", serverIp[0].c_str());
    xmlmaker.AddElem("UpMgrBackIp", serverIp[1].c_str());
    xmlmaker.Save("../config/network.xml");
#endif
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
		unsigned int sleeptime = 1000*60;
		if (ret == 20||ret == 30)
		{
			sleeptime = 1000*10;
		}
#ifdef WIN32
        Sleep(sleeptime);
#else
        sleep(sleeptime/1000);
#endif        
        //evnt.wait();
        if( FiUpdateAssistant::getinstance()->IsUpFinished() )
        {
            exit(2);//restart by monitor_update.py
        }
    }while(true);
    
    return 0;
err:
    return -1;
}
