#include "FiUpdateLoadImpl.h"
#include "FiUpdateAssistant.h"
FiUpdateLoaderImpl::FiUpdateLoaderImpl()
{
}

FiUpdateLoaderImpl::~FiUpdateLoaderImpl()
{
}

::CORBA::Long FiUpdateLoaderImpl::OnFileStreamArrive(const char* filename, 
                                                     ::CORBA::Long packid, 
                                                     ::CORBA::Long flag, 
                                                     const ::bfstream& bfs, 
                                                     ::CORBA::Long checknum, 
                                                     ::CORBA::Long& retrans)
{
   return  FiUpdateAssistant::getinstance()->handl_input(filename,packid,flag,bfs,checknum,retrans);
    
}

::CORBA::Long FiUpdateLoaderImpl::RollBack(const char* version,const char* date)
{
    return FiUpdateAssistant::getinstance()->RollBack(version,date);
}
