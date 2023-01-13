#!/bin/sh

PWD=$(pwd)
SRC_DIR=$PWD/src
BUILD_DIR=$PWD/build
DOWNLOAD_DIR=$PWD/dl
LIBS_DIR=$BUILD_DIR/libs

LIBUSB_VER="1.0.26"
LIBUSB_URL="https://github.com/libusb/libusb/releases/download/v${LIBUSB_VER}/libusb-${LIBUSB_VER}.tar.bz2"
LIBUSB_DIR=""

prepare_dirs(){
	if [ ! -d $BUILD_DIR ]; then
		mkdir -p $BUILD_DIR || exit 1
	fi
	if [ ! -d $DOWNLOAD_DIR ]; then
		mkdir -p $DOWNLOAD_DIR || exit 1
	fi
	if [ ! -d $LIBS_DIR ]; then
		mkdir -p $LIBS_DIR || exit 1
	fi
	return 0
}

download_files(){
	if [ -d $DOWNLOAD_DIR ] && [ ! -d $DOWNLOAD_DIR/libusb-$LIBUSB_VER ]; then
		cd $DOWNLOAD_DIR; \
		wget $LIBUSB_URL; \
		tar xf libusb-$LIBUSB_VER.tar.bz2
	fi
	LIBUSB_DIR=$(cd $DOWNLOAD_DIR/libusb-$LIBUSB_VER && pwd)
	return 0
}

build_depends(){
	if [ -d $LIBUSB_DIR ]; then
		cd $LIBUSB_DIR; \
		./configure --prefix=$LIBS_DIR --disable-udev; \
		make clean; \
		make; \
		make install
	fi
	return 0
}

build_target(){
	make -C $SRC_DIR CONFIG_STATIC=yes LIBS_BASE=$LIBS_DIR strip && mv $SRC_DIR/snander $BUILD_DIR
	make -C $SRC_DIR clean
	return 0
}

#clean_all(){
#	if [ -d $BUILD_DIR ]; then
#		rm -rf $BUILD_DIR || exit 1
#	elif [ -d $DOWNLOAD_DIR ]; then
#		rm -rf $DOWNLOAD_DIR || exit 1
#	fi
#	return 0
#}

prepare_dirs
download_files
build_depends
build_target
#clean_all
