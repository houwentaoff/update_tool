#include "FiUpdateMgrImpl.h"
#include "../comm/utility.h"
#include "../comm/include.h"
#include<stdio.h>
#include<stdlib.h>
#include <assert.h>
#define LENPERPACK 400
FiUpdateMgrImpl::FiUpdateMgrImpl()
{
	
}
FiUpdateMgrImpl::~FiUpdateMgrImpl()
{
}
::CORBA::Long FiUpdateMgrImpl::QueryCurVersion(const char* inversion, const char* indate, 
											   const char* inpatchno, ::CORBA::String_out version,
											   ::CORBA::String_out date, ::CORBA::String_out patchno
											   )
{
    int ret = 0;
    version_t localVer, cliVer;
	version = CORBA::string_dup("");
	date = CORBA::string_dup("");
	patchno = CORBA::string_dup("");
 
    memset(&localVer, 0, sizeof (version_t));
    memset(&cliVer, 0, sizeof (version_t));

    strcpy(cliVer.version, inversion);
    strcpy(cliVer.date, indate);
    strcpy(cliVer.patchNo, inpatchno);
    
	ut_dbg("the client version:%s date:%s  patch:%s\n",inversion,indate,inpatchno);
	fflush(stdout);


	std::string strpatchver("");
	std::string strpatchdate("");
	std::string strpatchno("");
    getLocalVersion(&localVer);
	version = CORBA::string_dup(localVer.version);
	date    =  CORBA::string_dup(localVer.date);
	patchno =  CORBA::string_dup(localVer.patchNo);    
	bool rollbackflag=false;
	bool updateflag = false;
    
	ret = compareVersion(&localVer, &cliVer);
    if (ret > 0 && ret != 1000000)
    {
        //update localVer > cliVer
        updateflag = true;
    }
    else if (ret < 0)
    {
        //roll
        //rollbackflag = true;
        updateflag = true;
        ut_dbg("roolback to version:%s date:%s,patch:%s\n",strpatchver.c_str(),strpatchdate.c_str(),strpatchno.c_str());
		fflush(stdout);        
    }
	if (rollbackflag)
	{
		version = CORBA::string_dup(strpatchver.c_str());
		date =  CORBA::string_dup(strpatchdate.c_str());
		patchno =  CORBA::string_dup(strpatchno.c_str());
		ut_dbg("roolback to version:%s date:%s,patch:%s\n",strpatchver.c_str(),strpatchdate.c_str(),strpatchno.c_str());
		fflush(stdout);
        return 1;
	}
	if (updateflag)
	{
		std::string folder;

		std::string patchprix("fics_");
        patchprix += localVer.version;
        patchprix += "_";
        patchprix += localVer.date;
        if (strlen(localVer.patchNo) > 0)
        {
            patchprix += "_";
            patchprix += localVer.patchNo;
        }

#if defined WIN32|| defined WIN64
		char szFind[MAX_PATH];
		char buff1[200];
		int ret = FiGetCurDir(200,buff1);
		WIN32_FIND_DATA FindFileData;
		strcpy(szFind,buff1);
		strcat(szFind,"\\*.*");

		HANDLE hFind=::FindFirstFile(szFind,&FindFileData);
		if(INVALID_HANDLE_VALUE == hFind)
		{
			version = CORBA::string_dup("");
			date = CORBA::string_dup("");
			patchno = CORBA::string_dup("");
			printf("no package is available!****!! line:%d\n",__LINE__);
			fflush(stdout);
			return -3;
		}

		while(TRUE)
		{
			if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(FindFileData.cFileName[0]!='.')
				{
					if (strcmp(FindFileData.cFileName,patchprix.c_str())>0)
					{
						if (folder.length()>0)
						{
							if (strcmp(folder.c_str(),FindFileData.cFileName)>0)
							{
								folder = FindFileData.cFileName;
							}

						}
						else
						{
							folder = FindFileData.cFileName;
						}

					}
				}
			}

			if(!FindNextFile(hFind,&FindFileData))
				break;
		}
		FindClose(hFind);
#else
		DIR *dp;
		struct dirent *entry;
		
		char buff1[200];
		int ret = getPkgDlDir(200,buff1);
		if((dp = opendir(buff1)) == NULL)
		{
			version = CORBA::string_dup("");
			date = CORBA::string_dup("");
			patchno = CORBA::string_dup("");
			ut_dbg("no package is available!****!!\n");
			fflush(stdout);
			return -5;
		}
		while((entry = readdir(dp)) != NULL)
		{  
			struct stat statbuf;
			std::string fullname=buff1;
			fullname+=entry->d_name;
			lstat(fullname.c_str(), &statbuf);
			if(S_ISDIR(statbuf.st_mode))
			{  
				if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)  
					continue;
				if (strcmp(entry->d_name, patchprix.c_str()) == 0)//本地版本较 客户端版本 ==
				{
                    folder =entry->d_name;
                    break;
#if 0
					if (folder.length()>0)
					{
						if (strcmp(folder.c_str(),entry->d_name)<0)//sean >0 --> <0
						{
							folder = entry->d_name;
						}

					}
					else
					{
						folder =entry->d_name;
					}
#endif
				}
			}  

		}  

		closedir(dp);                  
#endif

		assert(!folder.empty());
		
		std::string selectversion ;
		std::string selectdate;
		if (folder.at(folder.length()-2)=='_')//倒数第二个
		{
			strpatchno.resize(2);
			strpatchno[0]= folder.at(folder.length()-1);
			strpatchno[1]=0;
			folder.resize(folder.length()-2);
		}
		std::string::size_type pos = folder.find('v');
		std::string::size_type endpos = folder.rfind('_');
		assert(pos!=std::string::npos&&endpos!=std::string::npos);
		for (;pos!=endpos;pos++)
		{
			selectversion.push_back(folder[pos]);
		}
		++endpos;
		while (folder[endpos]!='\0')
		{
			selectdate.push_back(folder[endpos]);
			endpos++;
		}
		assert(!selectversion.empty()&&!selectdate.empty());
		version = CORBA::string_dup(selectversion.c_str());
		date =  CORBA::string_dup(selectdate.c_str());
		patchno =  CORBA::string_dup(strpatchno.c_str());
		ut_dbg("client ip[11.11.11.11]port[1111] update to version:%s date:%s, patchno:%s\n",selectversion.c_str(),selectdate.c_str(),strpatchno.c_str());
		fflush(stdout);
		return 1;
	}
	
	return 1;
}
int gen_optional_pack_name(const ::PlatformInfoEx& PInfo,std::vector<std::string>& names)/*根据客户端传来的平台参数判断客户端需要的安装包的后缀*/
{
	char lm[6];
	lm[0]='_';
	sprintf(lm+1,"%d",PInfo.OSRunMode);
	std::string filename1 =lm;
	filename1+="_";
	filename1 +=PInfo.OSName;
	if(strlen((PInfo.OSPackName.in())) >0)
	{
		std::string str=(PInfo.OSPackName.in());
		names.push_back(filename1+str+".tar.gz");
	}
	names.push_back(filename1+".tar.gz");
	if (PInfo.platform == 0 )
	{
		std::string str =lm;
		str+="_Win.tar.gz";
		names.push_back(str);
	}
	if (PInfo.platform == 1 )
	{
		std::string str =lm;
		str+="_Linux.tar.gz";
		names.push_back(str);
	}
	if( PInfo.platform == 2)
	{
		std::string str =lm;
		str+="_Mac.tar.gz";
		names.push_back(str);
	}
	return 0;
}

::CORBA::Long FiUpdateMgrImpl::StartupUpdate(const ::PlatformInfoEx& PInfo, ::CORBA::Long which,
											 const char* version, const char* date, const char* patchno,
											 ::CORBA::String_out filename, ::CORBA::Long& size, 
											 ::CORBA::LongLong& _ref
											 )
{
    char *prixMatrix[] = {"server", "client", "Massvr", NULL};
    
	char buff[200];
	int ret = 0;

    getPkgDlDir(200,buff);

	assert(strlen(version)>0&&strlen(date)>0);
	std::string prix;
    prix = prixMatrix[which];

	std::string folder("fics_");
	folder+=version;
	folder+="_";
	folder+=date;
	if (strlen(patchno)>0)
	{
		folder+="_";
		folder+=patchno;
	}
	std::vector<std::string> optinalname;
	gen_optional_pack_name(PInfo,optinalname);
	std::string fname;
	for (int i=0;i<optinalname.size();++i)
	{
		std::string name = prix+"_"+version+"_"+date+optinalname[i];
		std::string fullname(buff);
#ifdef WIN32
		fullname+=folder+"\\"+name;
#else
		fullname+=folder+"/"+name;
#endif
		ut_dbg("try to find package:%s\n",fullname.c_str());
		fflush(stdout);
		bool flag = FiIsExistFile(fullname.c_str());
		if (flag)
		{
			ut_dbg("package :%s is chosen\n",name.c_str());
			fflush(stdout);
			fname = optinalname[i];
			break;
		}
		
	}
	if (fname.empty())
	{
		filename = CORBA::string_dup("N");
		size =0;
		_ref = 0;
		ut_dbg("no package is available,no need to download patch files\n");
		fflush(stdout);
		return 1;
	}
	
	std::string name = prix+"_"+version+"_"+date+fname;
	std::string fullname(buff);
#ifdef WIN32
	fullname+=folder+"\\"+name;
#else
	fullname+=folder+"/"+name;
#endif

	FILE* fp = fopen(fullname.c_str(),"r");
	 //long  save_pos=ftell(fp);
	fseek(fp,0,SEEK_END);

	long fsize = ftell(fp);
	fseek(fp,0,SEEK_SET);
	
	size = fsize;
	fclose(fp);
	FILEPTR hf = FiOpenExistFile(fullname.c_str());
	_ref = (long long)(hf);
	filename = CORBA::string_dup(name.c_str());
	ut_dbg("begin translate file: %s\n",name.c_str());
    return 1;
	
}
::CORBA::Long FiUpdateMgrImpl::TranslateFile(::CORBA::LongLong _ref, ::CORBA::Long packid, 
							::bfstream_out bfs, ::CORBA::Long& checknum, ::CORBA::Long& flag)
{
	FILEPTR fp = (FILEPTR)(_ref);
	if( fp == NULL)
		return -34;
	bfstream* buff=new bfstream();
	buff->length(LENPERPACK);
	FiSeekFile(fp,(packid-1)*(LENPERPACK),FILE_FROM_BEGIN);
	int size = FiReadFile(fp,&((*buff)[0]),LENPERPACK);
	int status =1;
	flag = 0;
	if(size >= 0 )//last pack
	{
		if( size < LENPERPACK || size == 0 )
		{
			buff->length(size);
			flag =1;
			FiCloseFile(fp);
		}
	}
	else
	{
		status = -23;
		buff->length(0);
		FiCloseFile(fp);
		size =0;
	}
	checknum =0;
	for(unsigned int i=0;i<size;++i)
	{
		checknum +=(unsigned int)((*buff)[i]);
	}
	bfs = buff;
	return status;
}
::CORBA::Long FiUpdateMgrImpl::starupTrans(const char* fileName, ::CORBA::Long& size, ::CORBA::LongLong& fileRef)
{
    std::string name;
	FILE* fp = fopen(fileName,"r");
	 //long  save_pos=ftell(fp);
	fseek(fp,0,SEEK_END);

	long fsize = ftell(fp);
	fseek(fp,0,SEEK_SET);
	
	size = fsize;
	fclose(fp);
	FILEPTR hf = FiOpenExistFile(fileName);
	fileRef = (long long)(hf);
	ut_dbg("begin translate file: %s\n", fileName);
    return 1;    
}
::CORBA::Long FiUpdateMgrImpl::transFile(::CORBA::LongLong fileRef, ::CORBA::Long packid, ::bfstream_out bfs, ::CORBA::Long& checknum, ::CORBA::Long& flag)
{
    FILEPTR fp = (FILEPTR)(fileRef);
	if( fp == NULL)
		return -34;
	bfstream* buff=new bfstream();
	buff->length(LENPERPACK);
	FiSeekFile(fp,(packid-1)*(LENPERPACK),FILE_FROM_BEGIN);
	int size = FiReadFile(fp,&((*buff)[0]),LENPERPACK);
	int status =1;
	flag = 0;
	if(size >= 0 )//last pack
	{
		if( size < LENPERPACK || size == 0 )
		{
			buff->length(size);
			flag =1;
			FiCloseFile(fp);
		}
	}
	else
	{
		status = -23;
		buff->length(0);
		FiCloseFile(fp);
		size =0;
	}
	checknum =0;
	for(unsigned int i=0;i<size;++i)
	{
		checknum +=(unsigned int)((*buff)[i]);
	}
	bfs = buff;
	return status;
}
	
