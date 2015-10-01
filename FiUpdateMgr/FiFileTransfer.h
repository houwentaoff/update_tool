#ifndef _FI_FILE_TRANSFER_H__
#define _FI_FILE_TRANSFER_H__
#include "../comm/FiRpcCli.h"
#include "../idl/QueryUpdate.h"
#include "../comm/concurrent.h"
class FiUpdateMgrImpl;
typedef RPC_PROXY_HANDLE<FiUpdateLoader>* RPCHandle;
class FiFileTransfer:public Thread
{
public:
	FiFileTransfer(char* addr,const char* p,FiUpdateMgrImpl*_impl);
	~FiFileTransfer();
public:
	int TransferFile();
public:
	virtual int svc();
	virtual int stop();
private:
	std::string ip;
	std::string filename;
	FiRpcCli<FiUpdateLoader>    TransWorker;
	RPCHandle   hTrans;
    FiUpdateMgrImpl*  impl;
	
};
#endif


