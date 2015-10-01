                                                升级程序说明手册
1.打包

    全版本包的名字为fics_v1.0.0_2015.09.14.zip 格式为:
    fics_+版本号+日期+.zip
    单独版本的名字格式为:
    client_/server_/Massvr_ + 版本号 + 日期 + 系统位数(32/64) + 平台(Linux/Win) + (内核版本(2.6/3.2...)) + .tar.gz
-fics_v1.0.0_2015.09.14                                     #压缩包内容
    ├── client_v1.0.0_2015.09.14_32_Win.tar.gz
    ├── client_v1.0.0_2015.09.14_64_Linux2.6.tar.gz      #注意名字不能错，2.6为当前客户机内核版本
    ├── client_v1.0.0_2015.09.14_64_Mac.tar.gz           
    ├── client_v1.0.0_2015.09.14_64_Win.tar.gz           #linux客户端包名
    ├── Massvr_v1.0.0_2015.09.14_64_Linux2.6.tar.gz      #下载机服务器包名
    ├── readme.txt
    └── server_v1.0.0_2015.09.14_64_Linux2.6.tar.gz      #linux服务端包名
    安装包的内容直接替换示例中目录的文件:
    ps:
    linux下需要omniORB库支持，在你的linux机器上安装https://192.168.252.32/svn/fishare/trunk/fics_v1.1/code/fics_update/omniORB-4.1.4.tar.gz
    note: 现在你不必安装omniORB-4.1.4.tar.gz,已经将动态编译打包成静态编译。
    
2.目录结构

WIN cli
-Sobey
    ├── Config
    │   └── network.xml             #填写本地Ip和下载服务器Ip(需手动填写)需确保此文件存在并填写正确                                                
    ├── Fics
    │   ├── fitool.exe
    │   ├── fiversion
    │   ├── FiWatchDog.exe
    │   ├── scm.exe
    │   └── ....                    #Fics win客户安装文件
    └── update
        ├── 7z.dll                   #解压缩外部程序   
        ├── 7z.exe
        ├── FiUpdateLoader.exe       #手动升级服务器程序
        ├── FiUpdateLoader.exe.old   #旧版本服务器程序
        └── ....

    
LINUX Server
-/sobey/fics
    ├── config
    │   └── network.xml                             #本地Ip和下载服务器Ip                                                
    ├── ......
    ├── patch_version                                #版本信息
    ├── web
    │   └── ....                                    #Fics linux服务端安装文件
    └── update
        ├── FiUpdateLoader                           #自动升级服务器程序
        └── monitor_update.py                        #监控watchdog:监控FiUpdateLoader程序

LINUX Massvr
-/sobey/fics
    ├── config
    │   └── network.xml                             #本地Ip和下载服务器Ip                                                
    ├── ......                                       #Fics linux服务端安装文件
    ├── patch_version                                #版本信息
    ├── web
    │   └── ....                                    #Fics linux服务端安装文件
    ├── download                                     #Fics安装包下载目录
    │   ├── fics_v1.0.0_2015.09.14.zip              #fics安装包
    │   ├── fics_v1.0.0_2015.09.14                  #fics安装包解压目录
    │   ├── fics_v2.0.0_2015.09.18.zip              #fics安装包
    │   ├── fics_v2.0.0_2015.09.18                  #fics安装包解压目录
    │   └── ....                                    
    └── update
        ├── FiUpdateMgr                              #手动升级服务器程序
        └── monitor_update_svr_master.py                    #监控watchdog:监控FiupdateMgr程序



LINUX Cli
-/sobey/fics
    ├── config
    │   └── network.xml                             #本地Ip和下载服务器Ip                                                
    ├── ......
    ├── patch_version                                #版本信息
    ├── client
    │   └── ....                                    #Fics linux客户端安装文件
    └── update
        ├── FiUpdateLoader                           #自动升级客户端程序
        └── monitor_update.py                        #监控watchdog:监控FiUpdateLoader程序
3. 使用
一、
    WIN

        1) 将config, update目录放在fics安装目录Sobey中添加network.xml即可。已配好的win样例机:172.16.134.242
        1.1) windows下 network.xml配置文件见 https://192.168.252.32/svn/fishare/trunk/fics_v1.1/code/fics_update/config/network.example.xml
    Linux
        1) 安装omniORB 参考 1打包
        2) 在/sobey/fics/update目录中运行FiUpdateMgr(0,1号节点)/FiUpdateLoader(其他节点)
        3) 在/sobey/fics/update目录中运行monitor_update_svr.py(0,1号节点)/monitor_update.py(其他节点)
二、
    在网页上点击 高级管理-版本管理-升级-选择安装包如：(fics_v2.0.0_2015.09.18.zip)
    在网页上点击 高级管理-版本管理-回退-选择安装包如：(fics_v1.0.0_2015.09.14.zip):需要曾经上传成功的版本



