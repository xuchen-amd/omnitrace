#!/bin/bash -e

if [ ! -f CMakeLists.txt ]; then
    if [ ! -f ../CMakeLists.txt ]; then
        echo "Error! Execute script from source directory"
        exit 1
    else
        cd ..
    fi
fi

set -e

tolower()
{
    echo "$@" | awk -F '\|~\|' '{print tolower($1)}';
}

toupper()
{
    echo "$@" | awk -F '\|~\|' '{print toupper($1)}';
}

usage()
{
    print_option() { printf "    --%-20s %-24s     %s\n" "${1}" "${2}" "${3}"; }
    echo "Options:"
    print_option "help -h" "" "This message"

    echo ""
    print_default_option() { printf "    --%-20s %-24s     %s (default: %s)\n" "${1}" "${2}" "${3}" "$(tolower ${4})"; }
    print_default_option distro "[ubuntu|opensuse]" "OS distribution" "${DISTRO}"
    print_default_option versions "[VERSION] [VERSION...]" "Ubuntu or OpenSUSE release" "${VERSIONS}"
    print_default_option rocm-versions "[VERSION] [VERSION...]" "ROCm versions" "${ROCM_VERSIONS}"
    print_default_option python-versions "[VERSION] [VERSION...]" "Python 3 minor releases" "${PYTHON_VERSIONS}"
    print_default_option user "[USERNAME]" "DockerHub username" "${USER}"
    #print_default_option lto "[on|off]" "Enable LTO" "${LTO}"

    echo ""
    echo "Usage: ${BASH_SOURCE[0]} <OPTIONS> -- <build-release.sh OPTIONS>"
    echo "  e.g:"
    echo "       ${BASH_SOURCE[0]} --distro ubuntu --versions 20.04 --rocm-versions 5.0 5.1     -- --core +nopython --rocm-mpi +nopython"
    echo "       ${BASH_SOURCE[0]} --distro ubuntu --versions 20.04 --python-version 6 7 8 9 10 -- --rocm +python   --rocm-mpi +nopython"
}

send-error()
{
    usage
    echo -e "\nError: ${@}"
    exit 1
}

verbose-run()
{
    echo -e "\n### Executing \"${@}\"... ###\n"
    exec "${@}"
}

build-release()
{
    CONTAINER=$1
    OS=$2
    ROCM_VERSION=$3
    CODE_VERSION=$4
    shift
    shift
    shift
    shift
    local DOCKER_ARGS=""
    tty -s && DOCKER_ARGS="-it" || DOCKER_ARGS=""
    verbose-run docker run ${DOCKER_ARGS} --rm -v ${PWD}:/home/omnitrace --stop-signal "SIGINT" --env DISTRO=${OS} --env ROCM_VERSION=${ROCM_VERSION} --env VERSION=${CODE_VERSION} --env PYTHON_VERSIONS="${PYTHON_VERSIONS}" ${CONTAINER} /home/omnitrace/scripts/build-release.sh ${@}
}

reset-last()
{
    last() { send-error "Unsupported argument :: ${1}"; }
}

reset-last

: ${USER:=$(whoami)}
: ${DISTRO:=ubuntu}
: ${VERSIONS:=20.04 18.04}
: ${ROCM_VERSIONS:=5.0 4.5 4.3}
: ${MPI:=0}
: ${PYTHON_VERSIONS:="6 7 8 9 10"}

n=0
while [[ $# -gt 0 ]]
do
    case "${1}" in
        -h|--help)
            usage
            exit 0
            ;;
        "--distro")
            shift
            DISTRO=${1}
            last() { DISTRO="${DISTRO} ${1}"; }
            ;;
        "--versions")
            shift
            VERSIONS=${1}
            last() { VERSIONS="${VERSIONS} ${1}"; }
            ;;
        "--rocm-versions")
            shift
            ROCM_VERSIONS=${1}
            last() { ROCM_VERSIONS="${ROCM_VERSIONS} ${1}"; }
            ;;
        "--python-versions")
            shift
            PYTHON_VERSIONS=${1}
            last() { PYTHON_VERSIONS="${PYTHON_VERSIONS} ${1}"; }
            ;;
        --user|-u)
            shift
            USER=${1}
            reset-last
            ;;
        "--")
            shift
            SCRIPT_ARGS=${@}
            break
            ;;
        *)
            last ${1}
            ;;
    esac
    n=$((${n} + 1))
    shift
done

CODE_VERSION=$(cat VERSION)

for VERSION in ${VERSIONS}
do
    TAG=${DISTRO}-${VERSION}
    for ROCM_VERSION in ${ROCM_VERSIONS}
    do
        build-release ${USER}/omnitrace:release-base-${TAG}-rocm-${ROCM_VERSION} ${DISTRO}-${VERSION} ${ROCM_VERSION} ${CODE_VERSION} ${SCRIPT_ARGS}
    done
done
