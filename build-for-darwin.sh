#!/bin/sh

PWD=$(pwd)
SRC_DIR=$PWD/src
BUILD_DIR=$PWD/build

prepare_dirs(){
	if [ ! -d $BUILD_DIR ]; then
		mkdir -p $BUILD_DIR || exit 1
	fi
	return 0
}

build_target(){
	make -C $SRC_DIR TARGET_OS=Darwin strip && mv $SRC_DIR/snander $BUILD_DIR
	make -C $SRC_DIR clean
	return 0
}

#clean_all(){
#	if [ -d $BUILD_DIR ]; then
#		rm -rf $BUILD_DIR || exit 1
#	fi
#	return 0
#}

prepare_dirs
build_target
#clean_all
