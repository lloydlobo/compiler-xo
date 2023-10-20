#!/bin/bash
set -eu

# Taken from https://github.com/odin-lang/Odin/blob/master/build_odin.sh

YEAR=$(date +"%Y")
MONTH=$(date +"%m")
VERSION="dev-$YEAR-$MONTH"


# Build Options

CC=clang
CFLAGS="-std=c99 -Wall -Werror"

# Append version as a pre-processor macro to CFLAGS
CFLAGS="$CFLAGS -DLUMINA_VERSION_RAW="
CFLAGS="$CFLAGS$VERSION"

DISABLED_WARNINGS="-Wno-switch -Wbuiltin-macro-redefined -Wno-unused-value -Wno-unused-function -Wno-unused-variable"
LDFLAGS="-pthread -lm -lstdc++"

OS_ARCH="$(uname -m)"
OS_NAME="$(uname -s)"

SOURCE_FILES="src/main.c" # SOURCE_FILES="compiler.c print.c codegen.c"
OUTPUT_BINARY="lumina"

error() {
    printf "ERROR: %s\n" "$1"
    exit 1
}

build_compiler() {
    local build_mode="$1"

    case "$build_mode" in
        debug)
            EXTRAFLAGS="-g"
            ;;
        release)
            EXTRAFLAGS="-O3"
            ;;
        release-native)
            if [ "$OS_ARCH" == "arm64" ]; then
                EXTRAFLAGS="-O3 -mcpu=native"
            else
                EXTRAFLAGS="-O3 -march=native"
            fi
            ;;
        nightly)
            EXTRAFLAGS="DNIGHTLY -O3"
            ;;
        *)
            error "Unsupported build mode: $build_mode"
            ;;
    esac

    set -x
    $CC $CFLAGS $DISABLED_WARNINGS $EXTRAFLAGS $LDFLAGS $SOURCE_FILES -o $OUTPUT_BINARY
    set +x
}

run_demo() {
    ./lumina examples/demo/demo.lum
}

run_wip() {
    ./lumina test.lum
}

# Main Script
if [ $# -eq 0 ]; then
    build_compiler debug
    run_wip
elif [ $# -eq 1 ]; then
    case $1 in
        clean)
            rm -f $OUTPUT_BINARY
            ;;
        report)
            error "Report functionality is unimplemented."
            # [ ! -f "./lumina" ] && build_lumina debug
            # ./lumina report # NOTE: unimplimented
            ;;
        *)
            build_compiler "$1"
            ;;
    esac
    run_demo
else
    error "Too many arguments!"
fi
