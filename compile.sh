#!/bin/bash

. ./scripts.inc

set -e

if [ -z "$1" ]; then
  echo "Usage: $0 source_file"
  exit 1
fi

if [ -z "$USE_STD" ]; then
  export USE_STD="c++11"
fi

if [ -z "$USE_CC" ]; then
  export USE_CC="$default_compiler"
fi

file_name="$1"
shift
base_dir="/tmp/.build/$USE_CC/$USE_STD"
out_dir="$base_dir/`dirname "$file_name"`"
link_dir="/tmp/.bin"

if [ ! -d "$out_dir" ]; then
  mkdir -p "$out_dir"
fi

if [ -e "$link_dir" ]; then
  rm -rf "$link_dir"
fi
ln -s "$base_dir" "$link_dir"

out_binary="$out_dir/`basename "$file_name" .cpp`"

lclear

if [ -z "$CC_OPT" ]; then
  CC_OPT="-O0"
fi

CC_ARGS="-o $out_binary $CC_ARGS $CC_OPT -g -pthread -ftemplate-depth-1024 -ftemplate-backtrace-limit=0"
if [ "${USE_CC:0:5}" = "clang" ]; then
  CC_ARGS="$CC_ARGS -ferror-limit=3"
fi
if [ "$PRE_PROC" = "true" ]; then
  CC_ARGS="-E"
fi

if [ "$USE_CCACHE" = "true" ] && [ -e "$out_binary" ]; then
  echo "using cached version of $file_name: $out_binary"
else
  echo -n "started: "; date
  set -x
  time "$USE_CC" $CC_ARGS -Wall -Werror "-std=$USE_STD" -I . "$file_name" 2>&1
  set +x
  echo -n "finished: "; date
fi

if [ ! -e "$out_binary" ]; then
  echo "binary for $file_name was not properly built"
  exit 1
fi

if [ "$DO_RUN" = "true" ]; then
  "$out_binary" "$@"
fi
