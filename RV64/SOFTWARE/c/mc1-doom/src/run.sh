#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
pushd "${SCRIPT_DIR}" > /dev/null

EXE_FILE=out/mc1doom
PROFILE_FILE=""
SIM_ARGS=""

# Collect script arguments.
while true ; do
    if [ "$1" == "--profile" ] ; then
        shift 1
        PROFILE_FILE=/tmp/mr32sim-sym-$$
        mrisc32-elf-readelf -sW "${EXE_FILE}" | grep FUNC | awk '{print $2,$8}' > "${PROFILE_FILE}"
        SIM_ARGS="${SIM_ARGS} -P ${PROFILE_FILE}"
    elif [ "$1" == "-f" ] ; then
        shift 1
        SIM_ARGS="${SIM_ARGS} -f"
    elif [ "$1" == "-v" ] ; then
        shift 1
        SIM_ARGS="${SIM_ARGS} -v"
    else
        break
    fi
done

# Run the executable in the simulator.
mr32sim -g -ga 0x400089e0 -gp 0x400085c4 -gd 8 -gw 320 -gh 180 ${SIM_ARGS} "${EXE_FILE}" "$@"

# Delete the temporary profiling data.
if [ -n "${PROFILE_FILE}" ] ; then
    rm "${PROFILE_FILE}"
fi

popd > /dev/null

