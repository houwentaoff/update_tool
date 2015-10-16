#ifndef _FI_SAXPARASER_HANDLER_HPP__
#define _FI_SAXPARASER_HANDLER_HPP__

#ifdef WIN32
#include "../comm/Markup_win32.h"
#else
#include "../comm/Markup_linux.h"
#endif
#include <string>
#include "../comm/utility.h"
/*
class XMLParserHandler
{
public:
    XMLParserHandler()
    {
        bRebootFlag= false;
        bUpMgrIpEleStarted= false;
        bLocalIpflag= false;
    }
    virtual ~XMLParserHandler()
    {
    }

    void startElement(const XMLString& uri, const XMLString& localName, const XMLString& qname, const Attributes& attributes)
    {
        if( localName == "file")
        {
            File_Layout_t layout;
            int cnt =0;
            for(int i=0;i<attributes.getLength();++i)
            {
                if( attributes.getLocalName(i) == "name")
                {
                    cnt++;
                    layout.strName = attributes.getValue(i);
                }
                if( attributes.getLocalName(i) == "action")
                {
                    cnt++;
                    layout.strAction = attributes.getValue(i);
                }
                if( attributes.getLocalName(i) == "location")
                {
                    cnt++;
                    layout.strLocation = attributes.getValue(i);
                }
            }
            if(cnt >=3)
            {
                upack.allFiles.push_back(layout);
                }
        }
        if( localName == "reg")
        {
            Reg_Layout_t rlayout;
            int cnt =0;
            for(int i=0;i<attributes.getLength();++i)
            {
                if( attributes.getLocalName(i) == "name")
                {
                    cnt++;
                    rlayout.strName = attributes.getValue(i);
                }
                if( attributes.getLocalName(i) == "action")
                {
                    cnt++;
                    rlayout.strAction = attributes.getValue(i);
                }
            }
            if(cnt >=2)
            {
                upack.allRegs.push_back(rlayout);
                }
        }
        if(localName == "reboot")
        {
            bRebootFlag=true; 
        }
        if( localName == "UpMgrIp")
        {
            bUpMgrIpEleStarted = true;
        }
        if(localName == "LocalIP")
        {
            bLocalIpflag = true;
        }
    }
    
    void endElement(const XMLString& uri, const XMLString& localName, const XMLString& qname)
    {
        if( localName == "reboot")
        {
            upack.bReboot = (strReboot=="0")?false:true;
            bRebootFlag = false;
        }
        if( localName == "UpMgrIp")
        {
            bUpMgrIpEleStarted = false;
        }
        if(localName == "LocalIP")
        {
            bLocalIpflag = false;
        }
    }
    
    
public:
       FiUpdat_Pack_t& pack()
       {
              return upack;
       }
protected:
    
private:
    FiUpdat_Pack_t upack;
    bool bRebootFlag;
    std::string strReboot;
public:
    std::string strUpMgrIp;
    bool        bUpMgrIpEleStarted;
    std::string strLocalIp;
    bool bLocalIpflag;
};
*/
class XmlParserEngine
{
public:
    XmlParserEngine();
    virtual ~XmlParserEngine();
public:
    int load(std::string name);
    //int parse();
    FiUpdat_Pack_t pack()
    {
        FiUpdat_Pack_t upack;

        {
         CMarkup xmlLoader;
         
         xmlLoader.Load(filename.c_str());
         if(xmlLoader.FindElem("fiupdate") )
         {
             xmlLoader.IntoElem();
             while (xmlLoader.FindElem("file"))
             {
                // xmlLoader.IntoElem();
                 File_Layout_t layout;
                layout.strName = xmlLoader.GetAttrib("name");
                layout.strAction = xmlLoader.GetAttrib("action");
                layout.strLocation = xmlLoader.GetAttrib("location");
                //xmlLoader.OutOfElem();
                upack.allFiles.push_back(layout);
             }
            
             xmlLoader.OutOfElem();
         }
        }
        {
            CMarkup Loader;

            Loader.Load(filename.c_str());
            if(Loader.FindElem("fiupdate") )
            {
             Loader.IntoElem();

             while (Loader.FindElem("reg"))
             {
                //// Loader.IntoElem();
                 Reg_Layout_t rlayout;
                 rlayout.strName = Loader.GetAttrib("name");
                 rlayout.strAction = Loader.GetAttrib("action");
                 rlayout.strLocation = Loader.GetAttrib("location");
                // Loader.OutOfElem();
                 upack.allRegs.push_back(rlayout);
             }


             Loader.OutOfElem();
            }
        }
        {
            CMarkup xmlLoader;

            xmlLoader.Load(filename.c_str());
            if(xmlLoader.FindElem("fiupdate") )
         {
             xmlLoader.IntoElem();
             if (xmlLoader.FindElem("reboot"))
             {
                 //xmlLoader.IntoElem();
                 if( xmlLoader.GetData() =="0")
                 {
                     upack.bReboot = false;
                 }
                 else
                 {
                     upack.bReboot = true;
                 }

                 //xmlLoader.OutOfElem();
             }

             xmlLoader.OutOfElem();
         }
        }
         return upack;
// #else
//         FiUpdat_Pack_t upack;
//          CMarkup xmlLoader;
//          
//          xmlLoader.Load(filename.c_str());
//          //if(xmlLoader.FindElem("fiupdate") )
//          //{
//             // xmlLoader.IntoElem();
//              while (xmlLoader.FindChildElem("file"))
//              {
//                  xmlLoader.IntoElem();
//                  File_Layout_t layout;
//                 layout.strName = xmlLoader.GetAttrib("name");
//                 layout.strAction = xmlLoader.GetAttrib("action");
//                 layout.strLocation = xmlLoader.GetAttrib("location");
//                 xmlLoader.OutOfElem();
//                 upack.allFiles.push_back(layout);
//              }
//              
// 
//              while (xmlLoader.FindChildElem("reg"))
//              {
//                  xmlLoader.IntoElem();
//                  Reg_Layout_t rlayout;
//                  rlayout.strName = xmlLoader.GetAttrib("name");
//                  rlayout.strAction = xmlLoader.GetAttrib("action");
//                  rlayout.strLocation = xmlLoader.GetAttrib("location");
//                  xmlLoader.OutOfElem();
//                  upack.allRegs.push_back(rlayout);
//              }
// 
//              while (xmlLoader.FindChildElem("reboot"))
//              {
//                  xmlLoader.IntoElem();
//                  if( xmlLoader.GetData() =="0")
//                  {
//                      upack.bReboot = false;
//                  }
//                  else
//                  {
//                      upack.bReboot = true;
//                  }
//                  
//                  xmlLoader.OutOfElem();
//              }
//              
//             // xmlLoader.OutOfElem();
//         // }
//          return upack;
// 
// #endif

    }
    std::string GetMgrIp()
    {
        CMarkup xmlLoader;
        std::string ret;
        xmlLoader.Load(filename.c_str());
        if( xmlLoader.FindChildElem("UpMgrIp") )
        {
            xmlLoader.IntoElem();
            ret = xmlLoader.GetData();
            xmlLoader.OutOfElem();
        }
        return ret;
    }
    std::string GetEle(const char *eleName)
    {
        CMarkup xmlLoader;
        std::string ret;
        xmlLoader.Load(filename.c_str());
        if( xmlLoader.FindChildElem(eleName) )
        {
            xmlLoader.IntoElem();
            ret = xmlLoader.GetData();
            xmlLoader.OutOfElem();
        }
        return ret;
    }
    std::string LocalIP()
    {
        std::string ret;
        CMarkup xmlLoader;
        xmlLoader.Load(filename.c_str());

        if(xmlLoader.FindChildElem("LocalItem"))
        {
            xmlLoader.IntoElem();

            if(xmlLoader.FindChildElem("LocalIP"))
            {
                xmlLoader.IntoElem();
                ret = xmlLoader.GetData();
                xmlLoader.OutOfElem();
            }

            xmlLoader.OutOfElem();
            return ret;
        }


        if(xmlLoader.FindChildElem("NetConfig"))
        {
            xmlLoader.IntoElem();

            if(xmlLoader.FindChildElem("LocalIP"))
            {
                xmlLoader.IntoElem();
                ret = xmlLoader.GetData();
                xmlLoader.OutOfElem();
            }

            xmlLoader.OutOfElem();
        }
        return ret;
    }
protected:
    std::string filename;
    FiUpdat_Pack_t upack;
    
};

#endif
