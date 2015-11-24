                                                升级程序说明手册
1.打包

    全版本包的名字为fics_v1.0.0_2015.09.14_1000.zip 格式为:
    fics_+版本号+日期+.zip
    单独版本的名字格式为:
    client_/server_/Massvr_ + 版本号 + 日期 + 版本补丁号 + 系统位数(32/64) + 平台(Linux/Win) + (内核版本(2.6/3.2...)) + .tar.gz
-fics_v1.0.0_2015.09.14                                     #压缩包内容
    ├── client_v1.0.0_2015.09.14_1000_32_Win.tar.gz
    ├── client_v1.0.0_2015.09.14_1000_64_Linux2.6.tar.gz      #注意名字不能错，2.6为当前客户机内核版本
    ├── client_v1.0.0_2015.09.14_1000_64_Win.tar.gz           #linux客户端包名
    ├── Massvr_v1.0.0_2015.09.14_1000_64_Linux2.6.tar.gz      #下载机服务器包名
    ├── readme.txt
    └── server_v1.0.0_2015.09.14_1000_64_Linux2.6.tar.gz      #linux服务端包名
    安装包的内容直接替换示例中目录的文件或者用脚本打包:直接运行./fipatch就能看见打包的用法示例。  
    fipatch打包需要配置文件如 *.conf
    支持正则表达弱匹配如:
    win_client/x32/*.sys          A        C:\Windows\System32\drivers\ 
    win_client/*.exe              A        %root\..\Fics\      
    ...
    libs/*                        M       %root/libs/
    update/FiUpdateMgr            D       %root/update/    
    升级工具配置文件*.conf以#开头的为注释，如文件*.conf的注释    
    #this file is generate by auto tools.dont modify.
    #version v1.0.0 2015.09.14 4005
    #action  add:A  modify:M    delete:D  
    #filename      action   install dst dir    

2.目录结构

WIN cli
-Sobey
    └── Fics
           ├── Config
           │       ├── update_network.xml       #本地Ip和下载服务器Ip(运行过程中自动生成)  
           │       └── ....                     #fics安装包附带的其它配置文件
           ├── update
           │       ├── 7z.dll                   #解压缩外部程序   
           │       ├── 7z.exe
           │       ├── md5sums.exe              #windows下产生md5的工具
           │       ├── FiUpdateLoader.exe       #自动升级服务器程序
           │       └── ....
           ├── .fics_history                     #升级历史信息
           ├── fitool.exe
           ├── fiversion
           ├── FiWatchDog.exe
           ├── scm.exe
           └── ....                    #Fics win客户安装文件


    
LINUX Server
-/sobey/fics
    ├── config
    │   └── update_network.xml                      #本地Ip和下载服务器Ip               
    ├── ......
    ├── patch_version                                #版本信息+补丁库md5值
    ├── .fics_history                                #升级历史信息
    ├── web
    │   └── ....                                    #Fics linux服务端安装文件
    └── update
        └── FiUpdateLoader                           #自动升级服务器程序

LINUX Massvr
-/sobey/fics
    ├── config
    │   └── update_network.xml                      #本地Ip和下载服务器Ip 
    ├── ......                                       #Fics linux服务端安装文件
    ├── patch_version                                #版本信息+补丁库md5值
    ├── .fics_history                                #升级历史信息
    ├── web
    │   └── ....                                    #Fics linux服务端安装文件
    ├── download                                     #Fics安装包下载目录
    │   ├── fics_v1.0.0_2015.09.14_1000.zip         #fics安装包
    │   ├── fics_v1.0.0_2015.09.14_1000             #fics安装包解压目录
    │   ├── fics_v2.0.0_2015.09.18_1003.zip         #fics安装包
    │   ├── fics_v2.0.0_2015.09.18_1003             #fics安装包解压目录
    │   └── ....                                    
    └── update
        └── FiUpdateMgr                              #升级服务器程序



LINUX Cli
-/sobey/fics
    ├── config
    │   └── update_network.xml                     #本地Ip和下载服务器Ip                                         
    ├── ......
    ├── patch_version                                #版本信息+补丁库md5值
    ├── .fics_history                                #升级历史信息
    ├── client
    │   └── ....                                    #Fics linux客户端安装文件
    └── update
        └── FiUpdateLoader                           #自动升级客户端程序
3. 使用
一、
    WIN

        1) 将config, update目录放在fics安装目录fics中用fitool.exe成功添加卷后即可。
    Linux
        2) 在机器上运行fiwatchdog。
二、
    在网页上点击高级管理-版本管理-升级-选择安装包如：(fics_v2.0.0_2015.09.18_1003.zip)
    在网页上点击高级管理-版本管理-回退-选择安装包如：(fics_v1.0.0_2015.09.14_1000.zip):需要曾经上传成功的版本



