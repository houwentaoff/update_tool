#!/bin/bash -
#===============================================================================
#
#          FILE: gen_zip.sh
# 
#         USAGE: ./gen_zip.sh 
# 
#   DESCRIPTION: 
# 
#       OPTIONS: ---
#  REQUIREMENTS: ---
#          BUGS: ---
#         NOTES: ---
#        AUTHOR: Dr. Sean (hwt),  houwentaoff@gmail.com
#  ORGANIZATION: Sobey chengdu,  China
#       CREATED: Monday, October 13, 2015 9:45
#      REVISION:  ---
#===============================================================================
set -o nounset                              # Treat unset variables as an error

dir_name="" #fics_v1.1.0_2015.09.14
version=""
date=""
path=""

function zip_tar()
{
    rm ${dir_name} -rf
    mkdir ${dir_name}
#server_v1.1.0_2015.10.12 client_v1.1.0_2015.10.12 Massvr_v1.1.0_2015.10.12
    cp -a  ${path}/server_${version}_${date}* ${dir_name}/ -rf
    cp -a  ${path}/Massvr_${version}_${date}* ${dir_name}/ -rf
    cp -a  ${path}/client_${version}_${date}* ${dir_name}/ -rf
    zip -r "${dir_name}.zip" ${dir_name}
}
function print_usage()
{
    echo "USAGE:`basename $0` -v value  -d value -P value"
    echo "-v:version"
    echo "-d:date"
    echo "-P:tar.gz path"
    echo "eg: fics_v1.1.0_2015.09.29.zip: `basename $0` -v 1.1.0  -d 2015.09.29 -P "/sobey/fics" "
    exit 1
}
if [[ $# -lt 1 ]];then
    print_usage 
fi
tmp=""
while getopts :v:d:P: tmp
do
    case ${tmp} in
        v) version="v"
           version+=$OPTARG
            ;;
        d) date=$OPTARG
            ;;
        P) path=$OPTARG
            ;;
        \?) echo "unknow cmd ${OPTARG}"
            exit;
            ;;
    esac
done
if [ "${path}" == "" ]
then
    path=`pwd`
fi

if [[ "${date}" == "" || "${version}" == "" || "${path}" == ""  ]]
then
    print_usage;
    exit 2
fi
dir_name="fics_"${version}"_"${date}

zip_tar;
