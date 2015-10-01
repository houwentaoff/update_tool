#ifndef _CONCURRENT_H_DEF_
#define _CONCURRENT_H_DEF_

#ifdef _WIN32
#include <windows.h>
#else 
#include<pthread.h>
#endif
#ifdef _WIN32
#include <process.h>

#endif


#ifdef _WIN32
typedef unsigned int TID;
#else
typedef pthread_t TID;
#endif

#ifdef _WIN32
typedef unsigned int EXITNO;
#else
typedef void*        EXITNO;
#endif


class IRunnable
{
public:
	virtual int svc()=0;
	virtual int stop()=0;
protected:
	IRunnable()
	{

	}
	virtual ~IRunnable()
	{

	}
};


// a java-like thread
class Thread :public IRunnable
{
public:
	Thread( IRunnable* delegater = NULL )
		:Impl( NULL )
		,tid(0)
	{
		if ( delegater == NULL )
		{
			delegater = this;
		}

		Impl = delegater;
#ifdef _WIN32
		thread_handle = INVALID_HANDLE_VALUE;
#endif
	}
	virtual ~Thread()
	{

	}
public:
#ifdef _WIN32
	static EXITNO _stdcall  ThreadFunc(void*thread_this)
	{
		Thread* pt = reinterpret_cast<Thread*>( thread_this );
		if ( pt == NULL )
		{
			return EXITNO(0);
		}

		pt->run_svc();

		::_endthreadex( 0 );


		return (EXITNO)(0);
	}
#else
    static EXITNO ThreadFunc(void*thread_this)
	{
		Thread* pt = reinterpret_cast<Thread*>( thread_this );
		if ( pt == NULL )
		{
			return EXITNO(0);
		}

		pt->run_svc();


		return (EXITNO)(0);
	}
#endif

private:
	int run_svc()
	{
		return Impl->svc();
	}
public:
	int close(unsigned int timeout=2000)
	{
		Impl->stop();

#ifdef _WIN32
		DWORD ret =::WaitForSingleObject( thread_handle,timeout );
		if ( ret != WAIT_OBJECT_0 )
		{
			::TerminateThread( thread_handle,0);
		}
#else
	//	int ret = 0;
	//	void * val = &ret; 
	//	pthread_join(get(),NULL);
#endif
		return 0;
	}
	virtual int svc()
	{
		return 0;
	}
	virtual int stop()
	{
#ifdef WIN32
		TerminateThread(thread_handle,0);
#else
        pthread_cancel(get());
#endif
		return 0;
	}

	int begin()
	{
#ifdef _WIN32
		thread_handle = (HANDLE)::_beginthreadex(
			NULL,
			0,
			&ThreadFunc,
			this,
			0,
			&tid );

		if ( thread_handle  == 0 )
		{
			return -1;
		}
#else
		int err = pthread_create(&tid,NULL,&ThreadFunc,this);
		if( err != 0 ) {
			return -1;
		}
		//pthread_detach(tid);
#endif

		return 0;
	}

	TID get()
	{
		return tid;
	}
	//DWORD  get_addr();
private:
	IRunnable*   Impl;
	TID          tid;
#ifdef _WIN32
	HANDLE thread_handle;
#endif
};


#endif
