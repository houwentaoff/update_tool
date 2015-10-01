#ifndef _FISHARE_RPC_SERVANT_HPP_
#define _FISHARE_RPC_SERVANT_HPP_

#ifndef __CORBA_H_EXTERNAL_GUARD__
#include <omniORB4/CORBA.h>
#endif
#ifndef __OMNIPOA_H__
#include <omniORB4/poa.h>
#endif



#include "concurrent.h"

#ifdef WIN32
#include "windows.h"
#else
#include<unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#pragma warning (disable:4996 )
#pragma warning (disable:4101)
template<typename T>
class FiRpcSrv:public IRunnable
{
public:
	FiRpcSrv(unsigned short _port,char* _name, char* _ip=NULL)
		:worker(this)
		,name(NULL)
	{
		int len = strlen(_name);
		name = new char[len+1];
		memset(name, 0, len+1);
		strcpy(name,_name);
		if( _ip !=NULL) {
			len = strlen(_ip);
			//ASSERT(len>=7&&len<=15);//IPv4
			ip = new char[len+1];
			memset(ip, 0, len+1);
			strcpy(ip,_ip);
		}
		else
		{
			ip =NULL;
		}
		port = _port;
		init(port);
	}
	~FiRpcSrv()
	{
		worker.close();
		delete []name;
	}
public:
	
	int run()
	{

		worker.begin();
		#ifdef WIN32
		Sleep(2000);
		#else
		sleep(2);
		#endif

		return 0;

	}
	int register_rpc_interface(char* name,::PortableServer::Servant srv)
	{
		if ( name == NULL || CORBA::is_nil(poa) )
		{
			return -1;
		}

// 		int len = strlen(name);
// 		PortableServer::ObjectId* pid = new PortableServer::ObjectId(len);
// 		PortableServer::ObjectId& id = *pid;
// 
// 		id.length(len);
// 
// 		for( int i = 0; i < len; i++ )  id[i] = *name++;

		PortableServer::ObjectId_var oid = PortableServer::string_to_ObjectId(name);
		poa->activate_object_with_id(oid, srv);



		return 0;
	}

protected:
	int init(unsigned short port=2815)
	{
		char buff[50]={0};
		char tmp[10]={0};

		sprintf(tmp,"%d",port);
		strcpy(buff,"giop:tcp:");
		if( ip != NULL) {
			strcat(buff,ip);
		}
		strcat(buff,":");
		strcat(buff,tmp);// giop:tcp:127.0.0.1(null):5781

		const char* options[][2] = { { "endPoint",buff },{ 0, 0 } };
		int    argc=0;
		char** argv=NULL;
		orb= CORBA::ORB_init(argc,argv,"omniORB4",options);//set servant listen at a port
		if (CORBA::is_nil(orb))
		{
			return -1;
		}
		try
		{
			CORBA::Object_var obj = orb->resolve_initial_references("omniINSPOA");
			poa = PortableServer::POA::_narrow(obj);//get a poa
		}
		catch (...)
		{
			return -6;
		}
		

		if ( CORBA::is_nil(poa) )
		{
			return -1;
		}

		return 0;
	}
	int finit()
	{
		try
		{
			orb->destroy();
		}
		catch (...)
		{
			return 0;
		}
		
		return 0;
	}
	virtual int svc()
	{
		T* srv = new T();
		register_rpc_interface(name,srv);
		srv->_remove_ref();

		PortableServer::POAManager_var mgr = poa->the_POAManager();

		mgr->activate();
		orb->run();//make the servant peer run.event loop.
		return 0;

	}
	virtual int stop()
	{
		return finit();
	}

private:
	CORBA::ORB_var orb;
	PortableServer::POA_var poa;
private:
	Thread         worker;
	char*          name;
	char*          ip;
	unsigned short port;
};

#endif
