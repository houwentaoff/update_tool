#include "FiUpdateMgrImpl.h"
#include "../comm/utility.h"
#include "../comm/include.h"
#include "../comm/patch.h"
#include<stdio.h>
#include<stdlib.h>
#include <assert.h>
#include <errno.h>
#define LENPERPACK 400
FiUpdateMgrImpl::FiUpdateMgrImpl()
{
	
}
FiUpdateMgrImpl::~FiUpdateMgrImpl()
{
}
/**
 * @brief   根据客户端的版本信息，返回客户端需要的版本 集合(链表)
 *
 * @param version
 * @param patchs
 *
 * @return 
 */
::CORBA::Long FiUpdateMgrImpl::queryPatchs(const char* version, ::patchSet_t patchs)
{
    //version: cur ver   fics_v1.0.0_2015.10.16_4005
    //paths cur pat num set
    //need to set value  patshs
//    long patchs[100];
    int patch = 0;//读取patch_version文件 4005
    int baseVer = 0;
    int patchVer = 0;
    int i=0;
    char fileNameRE[512]={0};//Regular Expression
    version_t curVer;
    int tmpVer = 0;
    char cmdBuf[512];
    char tmppath[256]={0};
    char *pos;
    char *date;
    char *fileName;
    FILE *fp = NULL;
    int r=0;
    /*-----------------------------------------------------------------------------
     *  0. scan fics_v1.0.0_2015.09.12_4000.zip -- fics_v1.0.0_2015.10.16_4005.zip
     *  1. get base num, get patchNum
     *  2. scan a
     *-----------------------------------------------------------------------------*/
    if (getLocalVersion(&curVer) != 0)
    {
        ut_err("get local ver fail\n");
        return 1;
    }
    patch = atoi(curVer.patchNo);
    baseVer  = getCurBaseVer(patch);//40
    patchVer = getCurPatchVer(patch);//5
    tmpVer = baseVer * BASEINTERVAL;
    for (i = 0; i<=patchVer; i++)
    {
        sprintf(fileNameRE, "fics_%s_*_%d.zip", curVer.version, tmpVer+i);//reg skip date eg:fics_v1.0.0_*_4005.zip
        sprintf(cmdBuf, "find %s -name \'%s\'", _PATH_PKG_DL, fileNameRE);
        if (NULL == (fp = popen(cmdBuf, "r")))
        {
            ut_err("popen error[%d]\n", errno);
            return 2;
        }
        r = fscanf(fp, "%[^\n]s", tmppath);
        fgetc(fp);
        if (r == 1)
        {
            if (!(fileName = basename(tmppath)))
            {
                pclose(fp);
                continue;
            }
            if (!(pos = strstr(fileName, curVer.version)))
            {
                pclose(fp);
                continue;
            }
            if (!(pos = strchr(pos, '_')))
            {
                pclose(fp);
                continue;
            }
            date = pos + 1;
            if (!(pos = strchr(date, '_')))
            {
                pclose(fp);
                continue;
            }
            *pos = 0;
            patchs[i][0] = 1;
            patchs[i][1] = date2Long(date);
        }
        pclose(fp);
    }

    return 0;
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
        char *posTmp = NULL;
        char *sdate  = NULL ;
        char *sversion = NULL;
        char *spatchNo = NULL;

        char strBuf[100] = {0};

        strcpy(strBuf, folder.c_str());
        posTmp = strchr(strBuf, '_');//_v1.0.0
        sversion = posTmp+1;
        posTmp = strchr(posTmp+1, '_');//_2015
        *posTmp = '\0';
        sdate = posTmp+1;
        posTmp = strchr(posTmp+1, '_');//_4005
        *posTmp = '\0';
        spatchNo = posTmp+1;
        //pos = strchr(pos+1, '_');//_4005
        selectversion = sversion;
        selectdate = sdate;
        strpatchno = spatchNo;
		version = CORBA::string_dup(selectversion.c_str());
		date =  CORBA::string_dup(selectdate.c_str());
		patchno =  CORBA::string_dup(strpatchno.c_str());
		ut_dbg("client ip[11.11.11.11]port[1111] update to version:%s date:%s, patchno:%s\n",selectversion.c_str(),selectdate.c_str(),strpatchno.c_str());
		fflush(stdout);
		return 1;
	}
	
	return 1;
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
		std::string name = prix+"_"+version+"_"+date+"_"+patchno+"_"+optinalname[i];
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
	
	std::string name = prix+"_"+version+"_"+date+"_"+patchno+"_"+fname;
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
	
