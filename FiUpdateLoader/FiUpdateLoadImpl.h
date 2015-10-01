#ifndef _FI_UPDATE_LOADER_IMPL_H__
#define _FI_UPDATE_LOADER_IMPL_H__
#include "../idl/QueryUpdate.h"
class FiUpdateLoaderImpl:public POA_FiUpdateLoader
{
public:
    FiUpdateLoaderImpl();
    virtual ~FiUpdateLoaderImpl();
public:
    virtual ::CORBA::Long OnFileStreamArrive(const char* filename, 
                                             ::CORBA::Long packid, 
                                             ::CORBA::Long flag, 
                                             const ::bfstream& bfs, 
                                             ::CORBA::Long checknum, 
                                             ::CORBA::Long& retrans);
    virtual ::CORBA::Long RollBack(const char* version,const char* date);
};

#endif