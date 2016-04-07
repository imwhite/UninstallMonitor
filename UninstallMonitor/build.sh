# /bash
# created by white
# 2016/2/22
#
# 输出可执行文件，分release debug版
#


ndk-build clean
ndk-build VAR_MODE="$1"

cp ./libs/armeabi/* ./out/
open ./out