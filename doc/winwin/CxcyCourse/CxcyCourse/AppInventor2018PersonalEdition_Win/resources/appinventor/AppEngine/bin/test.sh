#!/bin/bash

SDK_BIN="$(cd -P "$(dirname "$0")" && pwd)"
echo $SDK_BIN
SDK_LIB="$(dirname "${SDK_BIN}")/lib"
echo $SDK_LIB
#A="$(dirname "${SDK_LIB}")"
#B="$(dirname "${A}")"
GAE_BIN="$(cd -P "$(dirname "$0")" && pwd)"
GAE_DIR="$(dirname "${GAE_BIN}")"
JDK_BIN="$(dirname "${GAE_DIR}")/jdk/bin"
readonly VERSION=$("${JDK_BIN}/java" -version 2>&1 \
    | sed -n '1s/.*version.*1\.\([0-9]\).*/\1/p')

echo $VERSION
