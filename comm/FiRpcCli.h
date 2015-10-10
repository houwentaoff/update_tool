#ifndef _FISHARE_RPC_CLIENT_HPP_
#define _FISHARE_RPC_CLIENT_HPP_

#ifndef __CORBA_H_EXTERNAL_GUARD__
#include <omniORB4/CORBA.h>
#endif

#ifdef WIN32
#include <windows.h>
#else
#include<unistd.h>
#endif

#include <stdlib.h>
#include <string>
#include <string.h>
#include "concurrent.h"
#pragma warning (disable:4996 )
#pragma warning (disable:4101)

#define MAKE_RPC_INVOKE(HAND,FUNC,ret)                                                        \
	do                                                                                        \
	{                                                                                         \
		try                                                                                   \
		{                                                                                     \
			if(HAND->isValidate)                                                              \
            {                                                                                 \
                  (HAND->proxy)->FUNC;                                                        \
				  ret = 1;                                                                    \
            }                                                                                 \
			else                                                                              \
            {                                                                                 \
				ret = 0;                                                                      \
            }                                                                                 \
		}                                                                                     \
		catch(CORBA::TRANSIENT&) {                                                            \
			ret=0;                                                                \
			HAND->isValidate=false;\
			break;\
		}                                                                                     \
		catch(CORBA::SystemException& ex) {                                                   \
			ret = 0;                                                                          \
			HAND->isValidate=false;\
            break;                                                                        \
		}                                                                                     \
		catch(CORBA::Exception& ex) {                                                         \
			HAND->isValidate=false;\
            ret =0;                                                                           \
			break;                                                                        \
		}                                                                                     \
		catch(...) {                                                                          \
			HAND->isValidate=false;\
            ret =0;                                                                           \
			break;                                                                        \
		}                                                                                     \
	} while (0)




template
<typename T>
struct RPC_PROXY_HANDLE
{
	bool         isValidate;
	typename T::_ptr_type proxy;
	RPC_PROXY_HANDLE()
	{
		isValidate = false;
		proxy = NULL;
	}
};


template
<typename T>
class FiRpcCli:public IRunnable
{
public:
	FiRpcCli()
	{
		//orbHandle =NULL;
		proxy = new RPC_PROXY_HANDLE<T>();
		exitInd = false;
		//tryNarrowThread = new Thread(this);
		//init();
	}
	~FiRpcCli()
	{
		//tryNarrowThread->close();
		//delete tryNarrowThread;
		{
			typename T::_var_type var = proxy->proxy;
		}
		
		//delete proxy;
		orb->destroy();//  modify by hwt  

		//finit();
	}
public:
	RPC_PROXY_HANDLE<T>* create_rpc_proxy(char* name,char* ip=NULL/*default: 127.0.0.1*/,unsigned short port=2815/*use port 2815 by default*/)
	{
		if( ip != NULL )
		{
			this->ip = ip;
		}
		
		this->port = port;
		servant = name;

		
		typename T::_ptr_type ret =NULL;
		orb = GenOrb();
		if (name == NULL || CORBA::is_nil(orb))
		{
			proxy->isValidate = false;
			proxy->proxy = NULL;
			return proxy;
		}
		char buff[256];
		strcpy(buff,"corbaloc:iiop:");
		if ( ip == NULL )
		{
			ip = "127.0.0.1";
		}
		strcat(buff,ip);
		char temp[10]={0};
		sprintf(temp,"%d",port);
		strcat(buff,":");
		strcat(buff,temp);
		strcat(buff,"/");
		strcat(buff,name);
		try{
			_orbHandle = orb->string_to_object(buff);//"corbaloc:iiop:ip:port/name" 协议规范 连接服务器 netstat  -nt 查看 
			ret= T::_narrow(_orbHandle);
		}
		catch(CORBA::TRANSIENT&) {
			
			
		}
		catch(CORBA::SystemException& ex) {
			
		}
		catch(CORBA::Exception& ex) {
			
		}
		catch(...) {
			
		}

		if( ret ==NULL )
		{
			proxy->isValidate = false;
		
			proxy->proxy = NULL;
			
		}
		else
		{
			proxy->isValidate = true;
			
			proxy->proxy = ret;
		}
		//tryNarrowThread->begin();
		
		return proxy;
	}
protected:
// 	int init()
// 	{
// // 		int    argc =0;
// // 		char** argv = NULL;
// // 
// // 		try
// // 		{
// // 			orb = CORBA::ORB_init(argc, argv);
// // 		}
// // 		catch(CORBA::TRANSIENT&) {
// // 			cerr << "Caught system exception TRANSIENT -- unable to contact the "
// // 				<< "server." << endl;
// // 		}
// // 		catch(CORBA::SystemException& ex) {
// // 			cerr << "Caught a CORBA::" << ex._name() << endl;
// // 		}
// // 		catch(CORBA::Exception& ex) {
// // 			cerr << "Caught CORBA::Exception: " << ex._name() << endl;
// // 		}
// // 		catch(...) {
// // 			cerr << "Caught omniORB::fatalException:" << endl;
// // 		}
// 
// 		if ( CORBA::is_nil(OrbInitializer::instance().orb) )
// 		{
// 			return -1;
// 		}
// 
// 		return 0;
// 	}
// 	int finit()
// 	{
// 		//orb->destroy();
// 		return 0;
// 	}


	CORBA::ORB_var GenOrb()
	{
		CORBA::ORB_var       orb;
		int    argc =0;
		char** argv = NULL;

		try
		{
			orb = CORBA::ORB_init(argc, argv);
		}
		catch(CORBA::TRANSIENT&) {

		}
		catch(CORBA::SystemException& ex) {

		}
		catch(CORBA::Exception& ex) {

		}
		catch(...) {

		}
		return orb;
	}
	int DestroyOrb(CORBA::ORB_var orb)
	{
		orb->destroy();
		return 0;
	}
	virtual int svc()
	{
		CORBA::Object_var handle;
		typename T::_ptr_type ret =NULL;
		if (servant.empty() )
		{
			return 0;
		}

		char buff[256];
		strcpy(buff,"corbaloc:iiop:");
		if ( ip.empty() )
		{
			ip = "127.0.0.1";
		}
		strcat(buff,ip.c_str());
		char temp[10]={0};
		sprintf(temp,"%d",port);
		strcat(buff,":");
		strcat(buff,temp);
		strcat(buff,"/");
		strcat(buff,servant.c_str() );
		
		do 
		{
			while((proxy->proxy !=NULL) && (proxy->isValidate))
			{
#ifdef WIN32
				::Sleep(5000);
#else
				sleep(5);
#endif
			}
			{
				DestroyOrb(orb);
				typename T::_var_type var = proxy->proxy;
				proxy->proxy = NULL;
			}
			try{
				orb=GenOrb();
				handle = orb->string_to_object(buff);//"corbaloc:iiop:ip:port/name"
				ret= T::_narrow(handle);
			}
			catch(CORBA::TRANSIENT&) {
				

			}
			catch(CORBA::SystemException& ex) {
				
			}
			catch(CORBA::Exception& ex) {
				
			}
			catch(...) {
				
			}
			if( ret != NULL )
			{
				
				proxy->isValidate = true;
				proxy->proxy = ret;
				//return 0;
			}
			else
			{
                #ifdef WIN32
				::Sleep(1000);
                #else
                sleep(1);
                #endif
			}
		} while ( !exitInd );
		return 1;
	}
	virtual int stop()
	{
		exitInd = true;
		return  0;
	}

private:
	CORBA::ORB_var       orb;
	Thread*              tryNarrowThread;
	RPC_PROXY_HANDLE<T>* proxy;
	std::string          ip;
	unsigned short       port;
	std::string          servant;
	bool                 exitInd;
	CORBA::Object_var _orbHandle;

	//CORBA::Object*       orbHandle;
protected:
	
private:
	
};

#endif
