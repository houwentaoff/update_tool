#include "FiFileTransfer.h"
#include "FiUpdateMgrImpl.h"
#include <stdio.h>
#include <fcntl.h>
#define LENPERPACK 400
FiFileTransfer::FiFileTransfer(char* addr,const char* p,FiUpdateMgrImpl*_impl)
:ip(addr)
,filename(p)
{
	hTrans = NULL;
    impl= _impl;
}

FiFileTransfer::~FiFileTransfer()
{
	
}


int FiFileTransfer::TransferFile()
{
	char *name = "FiLoader";
	hTrans= TransWorker.create_rpc_proxy(name,(char*)(ip.c_str()),5795);
    char buf[200];
	int ret = FiGetCurDir(200,buf);
    std::string path =buf;
    path+=filename;
	int status = 0;
    FILEPTR fp = FiOpenExistFile(path.c_str());
	int packid = 1;
	
    std::string transname=filename;

	bfstream buff;
	buff.length(LENPERPACK);
	int failtimes =0;
	do
	{

        int size = FiReadFile(fp,&(buff[0]),LENPERPACK);
		int flag = 0;
		if(size >= 0 )//last pack
		{
            if( size < LENPERPACK || size == 0 )
            {
                buff.length(size);
                flag =1;
            }
		}
        else
        {
           FiCloseFile(fp);
           impl->enqueuependingworker(this);
           return -9;
        }
		CORBA::Long checknum =0;
		for(unsigned int i=0;i<size;++i)
		{
			checknum +=(unsigned int)(buff[i]);
		}
		CORBA::Long expectedpack=0;
		do
		{
			MAKE_RPC_INVOKE(hTrans,OnFileStreamArrive(transname.c_str(),packid,flag,buff,checknum,expectedpack),status);
			if( status == 0)//transfer socket fail,
			{
				failtimes++;
				if( failtimes >= 5)
				{
                    FiCloseFile(fp);
                    impl->enqueuependingworker(this);
					return -2;
				}
			}
			else
			{
				break;
			}
		}while(true);
		
		packid++;
		if( (packid == expectedpack) & flag )
		{
			break;
		}
		packid = expectedpack;
        FiSeekFile(fp,(expectedpack-1)*(LENPERPACK),FILE_FROM_BEGIN);
		
	}while(true);
	
    FiCloseFile(fp);
    impl->enqueuependingworker(this);
	return 1;
}

int FiFileTransfer::svc()
{
	return TransferFile();
}

int FiFileTransfer::stop()
{
    Thread::stop();
    return 0;
	//failtimes = 6;
}

