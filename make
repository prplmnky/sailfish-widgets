#!/bin/bash


#######################
# MER BUILD VARIABLES #
#######################
export MER_SSH_PORT=2222
export MER_SSH_USERNAME="mersdk"
export MER_SSH_PRIVATE_KEY="$HOME/SailfishOS/vmshare/ssh/private_keys/engine/mersdk"
export MER_SSH_SHARED_HOME="$HOME"
export MER_SSH_SHARED_SRC="$HOME"
export MER_SSH_SHARED_TARGET="$HOME/SailfishOS/mersdk/targets"

ROOT=`pwd`
PROJECTS=("database" "applicationsettings" "filemanagement")

function usage() {
  echo "$0 [project name]"
  echo "  where [project name] is a valid Qt project"
}

function compile_i486() {
  PROJECT=$1
  TARGET="i486"
  BUILD_PATH="$ROOT/build/$PROJECT/$TARGET"

  export MER_SSH_PROJECT_PATH="$ROOT/$PROJECT"
  export MER_SSH_SDK_TOOLS="$HOME/.config/SailfishBeta2/mer-sdk-tools/MerSDK/SailfishOS-i486"
  export MER_SSH_TARGET_NAME="SailfishOS-i486"
  cd $BUILD_PATH
  "$MER_SSH_SDK_TOOLS/make" clean
  "$MER_SSH_SDK_TOOLS/qmake" "$MER_SSH_PROJECT_PATH/${PROJECT}.pro" "-r" "-spec" "linux-g++-32" "$DEBUG"
  "$MER_SSH_SDK_TOOLS/make"
}

function compile_armv() {
  PROJECT=$1
  TARGET="armv"
  BUILD_PATH="$ROOT/build/$PROJECT/$TARGET"

  export MER_SSH_PROJECT_PATH="$ROOT/$PROJECT"
  export MER_SSH_SDK_TOOLS="$HOME/.config/SailfishBeta2/mer-sdk-tools/MerSDK/SailfishOS-armv7hl"
  export MER_SSH_TARGET_NAME="SailfishOS-armv7hl"
  echo "$DEBUG"
  cd $BUILD_PATH
  "$MER_SSH_SDK_TOOLS/make" clean
  "$MER_SSH_SDK_TOOLS/qmake" "$MER_SSH_PROJECT_PATH/${PROJECT}.pro" "-r" "-spec" "linux-g++" "$DEBUG"
  "$MER_SSH_SDK_TOOLS/make"
}

function usageFn() {
  echo "Compile the Qt Qml plugins for the VM and phone environment"
  echo "usage: make [-D] [-h] [-p projects]"
  echo "-h: displays this program's usage" 
  echo "-D: adds DEBUG identifier to qmake"
  echo "-p: compiles the specified project(s)"
}

DEBUG="CONFIG-=debug"
while getopts :hDp: opt "$@"; do
  case $opt in
  h)
    usageFn
    exit 0
    ;;
  p)
    PROJECTS=("$OPTARG")
    ;;
  D)
    DEBUG="CONFIG+=debug"
    ;;
  *)    
    echo "Invalid argument given. Exiting." >&2
    usageFn
    exit 1
    ;;
  esac  
done

###############################
# Start the SDK VirtualBox VM #
###############################
echo "Starting SDK"
VBoxHeadless -s "MerSDK" &
sleep 10

###########
# Compile #
###########
echo "Starting Build"

for proj in ${PROJECTS[@]}; do
  echo "STARTING PROJECT: $proj i486"
  compile_i486 $proj
  echo "STARTING PROJECT: $proj armv"
  compile_armv $proj
done
