#include<stdlib.h>
#include<stdio.h>

#include <string>
#include <vector>

#include "../comm/utility.h"
#include <algorithm>
#ifdef WIN32
#include "../comm/Markup_win32.h"
#include <windows.h>
#else
#include <unistd.h>
#include   <sys/types.h> 
#include   <dirent.h> 
#include   <sys/stat.h>
#include "../comm/Markup_linux.h"
#endif
#include<assert.h>
#include <string.h>
#ifndef WIN32
#define  stricmp   strcmp              /*  */
#define  _stricmp  strcmp               /*  */
#endif
int load_layoutxml(std::string& xmlfname,std::string& nodename,std::vector<std::string>&layouts)
{
	char buff[256];
	FiGetCurDir(256,buff);
	strcat(buff,xmlfname.c_str());
	CMarkup xmlreader;
	xmlreader.Load(buff);
	if(xmlreader.FindChildElem(nodename.c_str()))
	{
		xmlreader.IntoElem();
		while (xmlreader.FindChildElem("location"))
		{
			xmlreader.IntoElem();
#ifdef WIN32
			CString temp = xmlreader.GetData();
			std::string data = temp.GetBuffer(temp.GetLength());
			temp.ReleaseBuffer();
#else
			std::string data= xmlreader.GetData();
#endif
			layouts.push_back(data);
			//data.ReleaseBuffer();
			xmlreader.OutOfElem();
		}
		xmlreader.OutOfElem();
	}
    if(layouts.empty())
	{
		layouts.push_back(std::string("%root/"));
	}
	return 1;
}
bool genxml_defaultM(std::string& xmlname,char*path,std::vector<std::string>&allincfiles)
{
	if(xmlname.empty() )
	{
		return false;
	}
	std::string xmlfile = path;
#ifdef WIN32
	xmlfile+="\\update.xml";
#else
	xmlfile+="/update.xml";
#endif
	FILE* fp = fopen(xmlfile.c_str(),"wb");
	if(fp ==NULL)
	{
		return false;
	}
	fputs("<fiupdate>",fp);
	fputs("\n",fp);
	std::vector<std::string>::iterator iter = allincfiles.begin();
	for (;iter!=allincfiles.end(); ++iter)
	{
		
		std::vector<std::string> layouts;
		std::string filenamecpy= (*iter).c_str();
		if(filenamecpy=="update.xml"||filenamecpy.find("FiUpdateLoader")!=std::string::npos 
			||filenamecpy.find("FiUpdateMgr")!=std::string::npos ||filenamecpy.find("update_client")!=std::string::npos
			||filenamecpy.find("update")!=std::string::npos)
		{
			continue;
		}
		std::string filename = (*iter).c_str();
		char *p=strstr((char*)filename.c_str(),"/*");
		if (p != NULL)
		{
			*p=0;
		}
		load_layoutxml(xmlname,filename,layouts);
        //assert(layouts.size()>0);
		bool isreg = false;
		//<file name ="libLog.so" action="D" location="..."/>
		//<reg name="reg.reg" action="A" location="..."/>
		if(strstr(filename.c_str(),".reg") != NULL )
		{
			isreg =true;
		}
		std::string record =isreg?"<reg name=\"":"<file name=\"";
		record+= filenamecpy+"\" action=\"M\" location=\"";
		for(int i=0;i<layouts.size();++i)
		{
			std::string temp = layouts[i]+"\"/>";
			fputs((record+temp).c_str(),fp);
			fputs("\n",fp);
		}
	}
	 if(xmlname=="win_layout.xml")
	 {
		 fputs("<reboot>1</reboot>\n",fp);
	 }
	fputs("</fiupdate>",fp);
	fputs("\n",fp);

	fflush(fp);
	fclose(fp);
	return true;
}
int change_update_xml(std::string& xmlname,std::string& path,std::string &name,std::string& act,bool isfile)
{
	if(xmlname.empty() )
	{
		return false;
	}
	std::string xmlfile = path;
#ifdef WIN32
	xmlfile+="\\update.xml";
#else
	xmlfile+="/update.xml";
#endif

	CMarkup xmlwriter;

	xmlwriter.Load(xmlfile.c_str());
	xmlwriter.FindElem("fiupdate");
	xmlwriter.IntoElem();
	bool flag = true;
	std::vector<std::string> layout;
	load_layoutxml(xmlname,name,layout);
	int j=0;
	while( xmlwriter.FindElem("file") )
	{
		//xmlwriter.IntoElem();
		
		std::string str;
		#ifdef WIN32
		CString temp = xmlwriter.GetAttrib("name");
		str = std::string(temp.GetBuffer(temp.GetLength()));
		#else
		str = xmlwriter.GetAttrib("name");
		#endif
		if (str ==name )
		{
			//xmlwriter.IntoElem();
			bool ret = xmlwriter.SetAttrib("action",act.c_str());
			bool ret1= xmlwriter.SetAttrib("location",layout[j].c_str());
			j++;
			flag = false;
			//xmlwriter.OutOfElem();
		
		}
		//xmlwriter.OutOfElem();
		
	}
	if(flag)
	{
		for (int i=0;i<layout.size();++i)
		{
			if(isfile)
			{
				xmlwriter.AddElem("file");
			}
			else
			{

				xmlwriter.AddElem("reg");
			}
			//xmlwriter.IntoElem();
			xmlwriter.AddAttrib("name",name.c_str());
			xmlwriter.AddAttrib("action",act.c_str());
			xmlwriter.AddAttrib("location",layout[i].c_str());
			//xmlwriter.OutOfElem();
		}
		
		
	}
	xmlwriter.OutOfElem();
	xmlwriter.Save(xmlfile.c_str());
	return 1;

}
int main(int argc,char**argv)
{
	printf("usage:patch -genxml -win|-linux|-mac c:\\windows\n");
	printf("usage:patch -file|-reg -A -win|-linux|-mac c:\\windows\\lib.so\n");
	printf("usage:patch -zip x.zip c:\\windows\n");
	printf("usage:patch -patch x.zip -win|-linux|-mac c:\\windows\n");
	printf("usage:patch -reboot 0/1 c:\\windows\n");
	if(argc<2)
	{
		printf("more args are expected!\n");
		return 1;
	}

	std::string cmd = argv[1];
	transform(cmd.begin(), cmd.end(),cmd.begin(), tolower );
	if(cmd =="-reboot")
	{
		if(argc<4)
		{
			printf("usage:patch -reboot 0/1 c:\\update.xml\n");
			return 4;
		}

		std::string xmlfile = argv[3];
#ifdef WIN32
		xmlfile+="\\update.xml";
#else
		xmlfile+="/update.xml";
#endif
		CMarkup xmlwriter;

		xmlwriter.Load(xmlfile.c_str());
		xmlwriter.FindElem("fiupdate");
		xmlwriter.IntoElem();
		xmlwriter.AddElem("reboot",argv[2]);
		xmlwriter.OutOfElem();
		xmlwriter.Save(xmlfile.c_str());


	}
	if (cmd =="-genxml")
	{
		if (argc<4)
		{
			printf("more args are expected,example:patch -genxml -win|-linux|-mac c:\\dir\n");
			return 2;
		}
		std::string xmlfile;
		if(_stricmp(argv[2],"-win")==0)
		{
			xmlfile = "win_layout.xml";
		}
		if(stricmp(argv[2],"-linux")==0)
		{
			xmlfile="linux_layout.xml";
		}
		if(stricmp(argv[2],"-mac")==0)
		{
			xmlfile="mac_layout.xml";
		}
		if(xmlfile.empty())
		{
			assert(0);
		}
		//if (argc==3)
		{
			std::vector<std::string> allincfiles;
			if( travel_dir(argv[3],allincfiles) )
			{
				if(allincfiles.size() <=0 )
				{
					return 3;
				}
				std::vector<std::string>::iterator iter = allincfiles.begin();
				for (;iter!=allincfiles.end(); ++iter)
				{
					std::vector<std::string> layouts;
					//std::string filenamecpy= (*iter).c_str();
					std::string filename = (*iter).c_str();
					char *p=strstr((char*)filename.c_str(),"/*");
					if (p != NULL)
					{
						*p=0;
						std::string dirname =argv[3];
#ifdef WIN32
						dirname+="\\";
#else
						dirname +="/";
#endif
						dirname+=filename.c_str();
						std::vector<std::string> names;
						travel_dir((char*)dirname.c_str(),names);
						std::vector<std::string>::iterator diter;
						for(diter=names.begin();diter!=names.end();++diter)
						{
							std::string &file=*diter;
							if(file.find(".sys") !=std::string::npos || file.find(".reg") != std::string::npos)
							{
								char temp[256];
#ifdef WIN32
								sprintf(temp,"copy /Y %s\\%s  %s\\..\\ \r\n",dirname.c_str(),file.c_str(),dirname.c_str());
#else
								sprintf(temp,"cp -f %s/%s %s/../ \n",dirname.c_str(),file.c_str(),dirname.c_str());
#endif
								FiExecuteShell(temp);
							}
							if( file.find("omniORB") !=std::string::npos || file.find("omnithread")!=std::string::npos ||
								file.find("omniDynamic") != std::string::npos)
							{
								char temp[256];
#ifdef WIN32
								sprintf(temp,"del /s/q %s\\%s \n",dirname.c_str(),file.c_str());
#else
								sprintf(temp,"rm -f %s/%s \n",dirname.c_str(),file.c_str());
#endif
								FiExecuteShell(temp);
							}
						}

					}
				}
				
			}
			
			allincfiles.clear();
			if(travel_dir(argv[3],allincfiles))
			{
				genxml_defaultM(xmlfile,argv[3],allincfiles);
			}
			
		}
	}
	if( (cmd =="-file") || (cmd=="-reg"))
	{
		if( argc <5 )
		{
			printf("more args are expected,example:patch -file -A -win|-linux|-mac c:\\windows\\lib.so\n");
			return 2;
		}
		std::string xmlfile;
		if(stricmp(argv[3],"-win")==0)
		{
			xmlfile = "win_layout.xml";
		}
		if(stricmp(argv[3],"-linux")==0)
		{
			xmlfile="linux_layout.xml";
		}
		if(stricmp(argv[3],"-mac")==0)
		{
			xmlfile="mac_layout.xml";
		}
		if(xmlfile.empty())
		{
			assert(0);
		}
		std::string subcmd = &argv[2][1];
		std::string fullname = argv[4];
#ifdef WIN32
		std::string::size_type ps = fullname.find_last_of("\\");
#else
		std::string::size_type ps = fullname.find_last_of("/");
#endif
		std::string path(&fullname[0],ps);
		std::string name(&fullname[ps+1]);
		
		change_update_xml(xmlfile,path,name,subcmd,(cmd=="-file")?true:false);

	}
	if(cmd =="-zip")
	{
		if(argc<4)
		{
			printf("more args are expected,example:patch -zip x.zip c:\\windows\n");
			return 2;
		}
		char buff[256];
		FiGetCurDir(256,buff);
#ifdef WIN32
		
		std::string str=buff;
		str+="7z.exe a -tzip ";
		str+=buff;
		str+=argv[2];
		str+="  \"";
		str+=argv[3];
         if(argv[3][strlen(argv[3])-1]!='\\')
         {
             str+="\\\"";
         }
		str+=" -r";
#else
        if(argv[3][strlen(argv[3])-1]=='/')
        {
            argv[3][strlen(argv[3])-1]='\0';
        }
        std::string temp(argv[3]);
        std::string::size_type pos=temp.rfind("/");
        if( pos == std::string::npos)
        {
            return -4;
        }
        std::string root(&temp[0],pos);
        std::string foldername(&temp[pos+1]);
        chdir(root.c_str());
		std::string str;
		str = "zip -r ";
		str +=buff;
		str+=argv[2];
		str +=" ";
		str+=foldername;
        
#endif
		FiExecuteShell(str.c_str());
	}
	if(cmd =="-patch")
	{
		if(argc<5)
		{
			printf("more args are expected,example:patch -patch x.zip -win|-linux|-mac c:\\windows\n");
			return 2;
		}
		std::string xmlfile;
		if(stricmp(argv[3],"-win")==0)
		{
			xmlfile = "win_layout.xml";
		}
		if(stricmp(argv[3],"-linux")==0)
		{
			xmlfile="linux_layout.xml";
		}
		if(stricmp(argv[3],"-mac")==0)
		{
			xmlfile="mac_layout.xml";
		}
		if(xmlfile.empty())
		{
			assert(0);
		}
		{
			std::vector<std::string> allincfiles;
			if( travel_dir(argv[4],allincfiles) )
			{
				if(allincfiles.size() <=0 )
				{
					return 3;
				}
				std::vector<std::string>::iterator iter = allincfiles.begin();
				for (;iter!=allincfiles.end(); ++iter)
				{
					std::string pfile = *iter;
					if((pfile.find("omniDynamic") != std::string::npos) || (pfile.find("omniORB4") != std::string::npos) ||
						(pfile.find("omnithread") != std::string::npos) || (pfile.find("stdc++") != std::string::npos) )
					{
						char temp[256];
#ifdef WIN32
						sprintf(temp,"del /s/q %s\\%s  \r\n",argv[4],pfile.c_str());
#else
						sprintf(temp,"rm -rf %s/%s  \n",argv[4],pfile.c_str());
#endif
						FiExecuteShell(temp);
						continue;
					}
					std::vector<std::string> layouts;
					//std::string filenamecpy= (*iter).c_str();
					std::string filename = (*iter).c_str();
					char *p=strstr((char*)filename.c_str(),"/*");
					if (p != NULL)
					{
						*p=0;
						std::string dirname =argv[4];
#ifdef WIN32
						dirname+="\\";
#else
						dirname +="/";
#endif
						dirname+=filename.c_str();
						std::vector<std::string> names;
						travel_dir((char*)dirname.c_str(),names);
						std::vector<std::string>::iterator diter;
						for(diter=names.begin();diter!=names.end();++diter)
						{
							std::string &file=*diter;
							if(file.find(".sys") !=std::string::npos || file.find(".reg") != std::string::npos)
							{
								char temp[256];
#ifdef WIN32
								sprintf(temp,"copy /Y %s\\%s  %s\\..\\ \r\n",dirname.c_str(),file.c_str(),dirname.c_str());
#else
								sprintf(temp,"cp -f %s/%s %s/../ \n",dirname.c_str(),file.c_str(),dirname.c_str());
#endif
								FiExecuteShell(temp);
							}
							if( file.find("omniORB") !=std::string::npos || file.find("omnithread")!=std::string::npos ||
								file.find("omniDynamic") != std::string::npos)
							{
								char temp[256];
#ifdef WIN32
								sprintf(temp,"del /s/q %s\\%s \n",dirname.c_str(),file.c_str());
#else
								sprintf(temp,"rm -f %s/%s \n",dirname.c_str(),file.c_str());
#endif
								FiExecuteShell(temp);
							}
						}

					}
				}
				
			}
			
			allincfiles.clear();
			if(travel_dir(argv[4],allincfiles))
			{
				genxml_defaultM(xmlfile,argv[4],allincfiles);
			}
		}
		char buff[256];
		FiGetCurDir(256,buff);
#ifdef WIN32
		
		std::string str=buff;
		str+="7z.exe a -tzip ";
		str+=buff;
		str+=argv[2];
		str+="  \"";
		str+=argv[4];
         if(argv[4][strlen(argv[4])-1]!='\\')
         {
             str+="\\\"";
         }
		str+=" -r";
#else
        if(argv[4][strlen(argv[4])-1]=='/')
        {
            argv[4][strlen(argv[4])-1]='\0';
        }
        std::string temp(argv[4]);
        std::string::size_type pos=temp.rfind("/");
        if( pos == std::string::npos)
        {
            return -4;
        }
        std::string root(&temp[0],pos);
        std::string foldername(&temp[pos+1]);
        chdir(root.c_str());
		std::string str;
		str = "zip -r ";
		str +=buff;
		str+=argv[2];
		str +=" ";
		str+=foldername;
        
#endif
		FiExecuteShell(str.c_str());
	}
	printf("finish\n");
	return 0;
}
