#!/bin/bash

do_command() {
    local cmd="${1}"
    if ! eval ${cmd}; then
        echo "Failed to run command \"${cmd}\""
    fi
}

pkg_list="make build-essential gcc graphviz doxygen doxygen-gui valgrind libgtest-dev"

cmd="sudo apt update -y"
do_command "${cmd}"


cmd="sudo apt install ${pkg_list} -y"
do_command "${cmd}"