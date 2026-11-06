#!/usr/bin/env bash

set +e

git status > /dev/null 2>&1
if [ $? -ne 0 ]; then
    ROOT_DIR=$(pwd)
else
    echo "Going back to the root of the project"
    ROOT_DIR="$(git rev-parse --show-toplevel)"
fi

set -e

SHOW_HELP=false
build_samples=OFF
build_python=OFF
build_tests=OFF
njobs=4
fresh=false

opt_should_configure=false
opt_should_build=false
opt_should_install=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help) SHOW_HELP=true; shift ;;
        # Script usage options ---------------------------
        --configure) opt_should_configure=true; shift ;;
        --build) opt_should_build=true; shift ;;
        --install) opt_should_install=true; shift ;;
        --all) opt_should_configure=true; opt_should_build=true; opt_should_install=true; shift ;;
        # ------------------------------------------------
        # Build options ----------------------------------
        --debug) build_type="Debug"; shift ;;
        --rel-with-deb) build_type="RelWithDebInfo"; shift ;;
        --samples) build_samples=ON; shift ;;
        --python) build_python=ON; shift ;;
        --njobs) njobs="$2"; shift 2 ;;
        --tests) build_tests=ON; shift ;;
        --fresh) fresh=true; shift ;;
        # ------------------------------------------------
        *) echo "Unkown option: $1"; exit 1 ;;
    esac
done

if [ "$SHOW_HELP" == "true" ]; then
    cat <<EOF

Build Box3D engine and other related modules
Usage: ./build.sh [OPTIONS]

Options:
    -h, --help          Display this help message
    --configure         Configure the build process (cmake -S . -B build ...)
    --build             Builds the engine and any other targets (cmake -b build ...)
    --install           Install the engine artifacts into an install directory (cmake --install build)
    --debug             Configure the build in Debug mode
    --rel-with-deb      Configure the build in Release with Debug Info (RelWithDebInfo)
    --samples           Build the samples that come with the engine
    --python            Build the Python bindings
    --njobs             Number of parallel jobs used for the build process
    --fresh             Use this to clean the Cmake cache and re-configure without rm -f the build dir
EOF
    exit 0
fi

USER_INSTALL_DIR="${ROOT_DIR}/install"

if [ ! -d "${USER_INSTALL_DIR}" ]; then
    mkdir -p $USER_INSTALL_DIR
fi

EXTERNAL_DEPS_DIR=${ROOT_DIR}/external_deps
if [ ! -d "${EXTERNAL_DEPS_DIR}" ]; then
    mkdir -p $EXTERNAL_DEPS_DIR
fi

[ "$fresh" == "true" ] && FRESH_CMD="--fresh" || FRESH_CMD=""

if [ "$opt_should_configure" == "true" ]; then

    echo "Configuring ..."

    cmake $FRESH_CMD -S . -B build \
        -DCMAKE_BUILD_TYPE:STRING=${build_type} \
        -DBOX3D_SAMPLES=${build_samples} \
        -DBOX3D_UNIT_TESTS=${build_tests} \
        -DBOX3D_DOUBLE_PRECISION=ON \
        -DCMAKE_INSTALL_PREFIX=${USER_INSTALL_DIR} \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

fi

if [ "$opt_should_build" == "true" ]; then
    echo "Building ..."

    cmake --build build --config=${build_type} --parallel ${njobs}

    echo "Copy compile_commands.json to the root of the project"
    cp build/compile_commands.json .
fi

if [ "$opt_should_install" == "true" ]; then
    echo "Installing to target dir ..."

    rm -rf ${USER_INSTALL_DIR}
    mkdir -p ${USER_INSTALL_DIR}

    cmake --install ${ROOT_DIR}/build --prefix ${USER_INSTALL_DIR}
fi
