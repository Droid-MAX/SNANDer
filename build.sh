#!/bin/sh

PWD=$(pwd)
SRC_DIR=$PWD/src
BUILD_DIR=$PWD/build
DOWNLOAD_DIR=$PWD/dl
BINDIR_X86=$BUILD_DIR/x86
BINDIR_X64=$BUILD_DIR/x64
LIBS_DIR=$BUILD_DIR/libs
LIBSDIR_X86=$LIBS_DIR/x86
LIBSDIR_X64=$LIBS_DIR/x64

LIBUSB_URL="https://github.com/libusb/libusb/releases/download/v1.0.22/libusb-1.0.22.tar.bz2"
LIBUSB_DIR=""

check_mingw(){
	if [ ! -x "$(command -v i686-w64-mingw32-gcc)" ] || [ ! -x "$(command -v x86_64-w64-mingw32-gcc)" ]; then
		echo 'Error: mingw-w64 is not installed.' >&2
		exit 1
	fi
}

prepare_dirs(){
	if [ ! -d $BUILD_DIR ]; then
		mkdir -p $BUILD_DIR || exit 1
	fi
	if [ ! -d $DOWNLOAD_DIR ]; then
		mkdir -p $DOWNLOAD_DIR || exit 1
	fi
	if [ ! -d $BINDIR_X86 ]; then
		mkdir -p $BINDIR_X86 || exit 1
	fi
	if [ ! -d $BINDIR_X64 ]; then
		mkdir -p $BINDIR_X64 || exit 1
	fi
	if [ ! -d $LIBS_DIR ]; then
		mkdir -p $LIBS_DIR || exit 1
	fi
	if [ ! -d $LIBSDIR_X86 ]; then
		mkdir -p $LIBSDIR_X86 || exit 1
	fi
	if [ ! -d $LIBSDIR_X64 ]; then
		mkdir -p $LIBSDIR_X64 || exit 1
	fi
	return 0
}

download_files(){
	if [ -d $DOWNLOAD_DIR ] && [ ! -d $DOWNLOAD_DIR/libusb-1.0.22 ]; then
		cd $DOWNLOAD_DIR; \
		wget $LIBUSB_URL; \
		tar xf libusb-1.0.22.tar.bz2
	fi
	LIBUSB_DIR=$(cd $DOWNLOAD_DIR/libusb-1.0.22 && pwd)
	return 0
}

build_depends_x86(){
	if [ -d $LIBUSB_DIR ]; then
		cd $LIBUSB_DIR; \
		./configure --host=i686-w64-mingw32 --prefix=$LIBSDIR_X86; \
		make clean; \
		make; \
		make install
	fi
	return 0
}

build_depends_x64(){
	if [ -d $LIBUSB_DIR ]; then
		cd $LIBUSB_DIR; \
		./configure --host=x86_64-w64-mingw32 --prefix=$LIBSDIR_X64; \
		make clean; \
		make; \
		make install
	fi
	return 0
}

build_target_x86(){
	make -C $SRC_DIR CC=i686-w64-mingw32-gcc STRIP=i686-w64-mingw32-strip WINDRES=i686-w64-mingw32-windres CONFIG_STATIC=yes TARGET_OS=MinGW LIBS_BASE=$LIBSDIR_X86 strip && mv $SRC_DIR/*.exe $BINDIR_X86
	return 0
}

build_target_x64(){
	make -C $SRC_DIR CC=x86_64-w64-mingw32-gcc STRIP=x86_64-w64-mingw32-strip WINDRES=x86_64-w64-mingw32-windres CONFIG_STATIC=yes TARGET_OS=MinGW LIBS_BASE=$LIBSDIR_X64 strip && mv $SRC_DIR/*.exe $BINDIR_X64
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

check_mingw
prepare_dirs
download_files
build_depends_x86
build_depends_x64
build_target_x86
build_target_x64
#clean_all
