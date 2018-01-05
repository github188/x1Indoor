#!/bin/bash

make clean

CUR_PATH=$(pwd)
IMAGE_PATH=$(pwd)/image
LIB_MINIGUI=$(pwd)/lib/minigui/lib

if [ "$#" != 1 ]; then
	echo "please select target;
	example: 	
	./build.sh ml8_v6s
	./build.sh ml8_v7s
	./build.sh ah8_e81s"
	exit 0
fi

if [ "$1" == "ml8_v6s" ] || [ "$1" == "ml8_v7s" ]; then
	echo "Start Build "$1""
	cp -rf $IMAGE_PATH/res_vxs/* $IMAGE_PATH/

elif [ "$1" == "ah8_e81s" ]; then
	echo "Start Build "$1""
	cp -rf $IMAGE_PATH/res_e81s/* $IMAGE_PATH/

elif [ "$1" == "clean" ]; then
        echo "Start clean ln file"
	rm $LIB_MINIGUI/libminigui_ths.so
	rm $LIB_MINIGUI/libminigui_ths-3.0.so
	rm $LIB_MINIGUI/libminigui_ths-3.0.so.12
	rm $LIB_MINIGUI/libpng.so
	rm $LIB_MINIGUI/libpng12.so
	rm $LIB_MINIGUI/libpng12.so.0
	rm $LIB_MINIGUI/libpng.so.3
	rm $LIB_MINIGUI/libz.so
	rm $LIB_MINIGUI/libz.so.1
	exit
else
	echo "Target is error!!!"
        echo "please select target;
	example: 	
	./build.sh ml8_v6s
	./build.sh ml8_v7s
	./build.sh ah8_e81s"
	exit 0
fi
echo `date` > $IMAGE_PATH/etc/firmVer

cd $LIB_MINIGUI
ln -s libminigui_ths-3.0.so.12.0.0 libminigui_ths.so
ln -s libminigui_ths-3.0.so.12.0.0 libminigui_ths-3.0.so
ln -s libminigui_ths-3.0.so.12.0.0 libminigui_ths-3.0.so.12
ln -s libpng12.so.0.37.0 libpng.so
ln -s libpng12.so.0.37.0 libpng12.so
ln -s libpng12.so.0.37.0 libpng12.so.0
ln -s libpng.so.3.37.0 libpng.so.3
ln -s libz.so.1.2.11 libz.so
ln -s libz.so.1.2.11 libz.so.1
cd $CUR_PATH

make


