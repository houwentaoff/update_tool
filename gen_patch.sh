#!/bin/bash - 
#===============================================================================
#
#          FILE: gen_patch.sh
# 
#         USAGE: ./gen_patch.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Dr. Sean (hwt),  houwentaoff@gmail.com
#  ORGANIZATION: Sobey Iserlohn,  China
#       CREATED: Friday, October 09, 2015 9:45
#      REVISION:  ---
#===============================================================================
set -o nounset                              # Treat unset variables as an error

name="fics"
version=""  #v1.1.0
fiver=""  #v1.1
bit=""      #32/64
platform="" #Linux/Win
date=""     #2015.09.14
fidate=""   #20150914
client=""   #client
server=""   #server
massvr=""   #Massvr
kernel=""   #2.6/3.2
path=""
cur_dir="`pwd`"

function ver2fiver()
{
    fiver=`echo ${version} |  awk '{split($0,b,".");print b[1]}'`
    fiver+="."
    fiver+=`echo ${version} |  awk '{split($0,b,".");print b[2]}'`
#    fiver="v1.1"
}
function date2fidate()
{
    fidate=`echo ${date} |  awk '{split($0,b,".");print b[1]}'`
    fidate+=`echo ${date} |  awk '{split($0,b,".");print b[2]}'`
    fidate+=`echo ${date} |  awk '{split($0,b,".");print b[3]}'`
#    fidate="20150914"
}
function tar_Massvr()
{
    mas_dir="${massvr}_${version}_${date}_${bit}_${platform}${kernel}"
    mas_tar="${cur_dir}/${mas_dir}.tar.gz"
    mas_dir_tmp="${mas_dir}"
    rm ${mas_dir_tmp} -rf
    mkdir -p ${mas_dir_tmp}
    cp -a ${path}/client ${mas_dir_tmp}/ -rf
    cp -a ${path}/format ${mas_dir_tmp}/ -rf
    cp -a ${path}/libs ${mas_dir_tmp}/   -rf
    cp -a ${path}/mds ${mas_dir_tmp}/    -rf
    cp -a ${path}/web ${mas_dir_tmp}/    -rf
    cp -a ${path}/update ${mas_dir_tmp}/ -rf
    cp -a ${path}/masupdate.xml ${mas_dir_tmp}/update.xml -rf
    echo "${name} ${fiver} ${fidate}" >${mas_dir_tmp}/fiversion
    tar --exclude '*.svn' -zcvf  ${mas_tar} ${mas_dir_tmp}
}

function tar_cli_linux()
{
    cli_linux_dir="${client}_${version}_${date}_${bit}_${platform}${kernel}"
    cli_tar="${cur_dir}/${cli_linux_dir}.tar.gz"
    cli_dir_tmp="${cli_linux_dir}"
    rm ${cli_dir_tmp} -rf
    mkdir -p ${cli_dir_tmp}
    cp -a ${path}/client ${cli_dir_tmp}/ -rf
    cp -a ${path}/libs ${cli_dir_tmp}/   -rf
    cp -a ${path}/update ${cli_dir_tmp}/ -rf
    cp -a ${path}/cli_linux_update.xml ${cli_dir_tmp}/update.xml -rf
    echo "${name} ${fiver} ${fidate}" >${cli_dir_tmp}/fiversion
    tar --exclude '*.svn' -zcvf  ${cli_tar} ${cli_dir_tmp}

    echo ""
}
function tar_cli_win()
{
    cli_win_dir="${client}_${version}_${date}_${bit}_${platform}"
    cli_tar="${cur_dir}/${cli_win_dir}.tar.gz"
    cli_dir_tmp="${cli_win_dir}"
    rm ${cli_dir_tmp} -rf
    mkdir -p ${cli_dir_tmp}
    mkdir -p ${cli_dir_tmp}/Fics
    cp -a ${path}/win_client/*.exe ${cli_dir_tmp}/Fics/ -f
    cp -a ${path}/win_client/*.dll ${cli_dir_tmp}/Fics/ -f

    if [ "${bit}" -eq "32" ]
    then
        cp -a ${path}/win_client/x32/*.sys ${cli_dir_tmp}/ -f
    else
        cp -a ${path}/win_client/x64/*.sys ${cli_dir_tmp}/ -f
    fi
    cp -a ${path}/win_client/update ${cli_dir_tmp}/ -rf
    cp -a ${path}/cli_win_update.xml ${cli_dir_tmp}/update.xml -f
    echo "${name} ${fiver} ${fidate}" >${cli_dir_tmp}/fiversion
    tar --exclude '*.svn' -zcvf  ${cli_tar} ${cli_dir_tmp}
   
    echo ""
}
function tar_ser()
{
    ser_dir="${server}_${version}_${date}_${bit}_${platform}${kernel}"
    ser_tar="${cur_dir}/${ser_dir}.tar.gz"
    ser_dir_tmp="${ser_dir}"
    rm ${ser_dir_tmp} -rf
    mkdir -p ${ser_dir_tmp}
    cp -a ${path}/client ${ser_dir_tmp}/ -rf
    cp -a ${path}/format ${ser_dir_tmp}/ -rf
    cp -a ${path}/libs ${ser_dir_tmp}/   -rf
    cp -a ${path}/mds ${ser_dir_tmp}/    -rf
    cp -a ${path}/web ${ser_dir_tmp}/    -rf
    cp -a ${path}/update ${ser_dir_tmp}/ -rf
    cp -a ${path}/serupdate.xml ${ser_dir_tmp}/update.xml -rf
    echo "${name} ${fiver} ${fidate}" >${ser_dir_tmp}/fiversion
    tar --exclude '*.svn' -zcvf  ${ser_tar} ${ser_dir_tmp}
     
    echo ""
}

function main()
{
    echo ""
}
function print_usage()
{
    echo "USAGE:`basename $0` -v value -p value -d value [-k value] -P value [-c/-s/-m] -b value"
    echo "-v:version"
    echo "-p:platform"
    echo "-d:date"
    echo "-k:kernel version"
    echo "-P:release path"
    echo "-b:bit[32/64]"
    echo "eg:linux server : `basename $0` -v 1.1.0 -p Linux -d 2015.09.29 -k 2.6 -P "/sobey/fics" -s -b 64"
    echo "eg:linux mas server : `basename $0` -v 1.1.0 -p Linux -d 2015.09.29 -k 2.6 -P "/sobey/fics" -m -b 64"
    echo "eg:win client : `basename $0` -v 1.1.0 -p Win -d 2015.09.29 -P "/sobey/fics" -c -b 64"
    echo "eg:linux client : `basename $0` -v 1.1.0 -p Linux -d 2015.09.29 -P "/sobey/fics" -c -b 64 -k 2.6"
    exit 1
}
if [[ $# -lt 1 ]];then
    print_usage 
fi
tmp=""
while getopts :v:b:p:d:k:P:csm tmp
do
    case ${tmp} in
        b) bit=$OPTARG
            ;;
        v) version="v"
           version+=$OPTARG
            ;;
        p) platform=$OPTARG
            ;;
        d) date=$OPTARG
            ;;
        k) kernel=$OPTARG
            ;;
        P) path=$OPTARG
            ;;
        c) client="client"
            ;;
        s) server="server"
            ;;
        m) massvr="Massvr"
            ;;
        \?) echo "unknow cmd ${OPTARG}"
            exit;
            ;;
    esac
done
ver2fiver;
date2fidate;

echo ${version} ${platform} ${date} ${kernel} ${path} ${client} ${server} ${massvr} ${bit} ${fidate} ${fiver}

if [[ "${date}" == "" || "${version}" == "" || "${platform}" == "" || "${path}" == "" || "${bit}" == "" ]]
then
    print_usage;
    exit 2
fi

cd /tmp/

if [ "${client}" == "client" ]
then
    if [ "${platform}" == "Win" ]
    then
        tar_cli_win;
    elif [ "${platform}" == "Linux" ]
    then
        tar_cli_linux;
    fi
fi

if [ "${massvr}" == "Massvr" ]
then
    tar_Massvr;
fi

if [ "${server}" == "server" ]
then
    tar_ser;
fi
