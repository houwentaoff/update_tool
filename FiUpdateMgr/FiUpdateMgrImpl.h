#ifndef _FISHAR_UPDATEMGR_IMPL_H_
#define _FISHAR_UPDATEMGR_IMPL_H_
#include "../idl/QueryUpdate.hh"
#include "../comm/utility.h"

#include "../comm/concurrent.h"
#include<map>
#include<string>
#include<vector>
class FiFileTransfer;
class FiUpdateMgrImpl:public POA_FiUpdateMgr
{
public:
	FiUpdateMgrImpl();
	virtual ~FiUpdateMgrImpl();
public:
	virtual ::CORBA::Long QueryCurVersion(const char* inversion, const char* indate, const char* inpatchno,
		::CORBA::String_out version, ::CORBA::String_out date, ::CORBA::String_out patchno, ::CORBA::String_out hash);
	virtual ::CORBA::Long StartupUpdate(const ::PlatformInfoEx& PInfo, ::CORBA::Long which, 
		const char* version, const char* date, const char* patchno, ::CORBA::String_out filename,
		::CORBA::Long& size, ::CORBA::LongLong& ref) ;
	virtual ::CORBA::Long TranslateFile(::CORBA::LongLong ref, ::CORBA::Long packid,
		::bfstream_out bfs, ::CORBA::Long& checknum, ::CORBA::Long& flag) ;
    virtual ::CORBA::Long starupTrans(const char* fileName, ::CORBA::Long& size, ::CORBA::LongLong& fileRef);
    virtual ::CORBA::Long transFile(::CORBA::LongLong fileRef, ::CORBA::Long packid, ::bfstream_out bfs, ::CORBA::Long& checknum, ::CORBA::Long& flag);
    virtual ::CORBA::Long queryPatchs(const char* version, ::patchSet_t patchs);
    virtual ::CORBA::Long getMD5FromFile(const char* fileName, ::CORBA::String_out md5Value);    
    FiEvent                               evnt;
};


#endif
