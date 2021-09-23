#!/bin/bash
###############################################################################
# Name: junk.sh
# Author: Connor Haaf
# Date: 5/27/2021
# Pledge: I pledge my honor that I have abided by the Stevens Honor System.
# Description: Bash script to move files and directories to a junk bin
###############################################################################

help_flag=0
list_flag=0
purge_flag=0
num_flags=0
num_files=0
readonly recycleDir=~/.junk
helper(){
cat << ENDOFTEXT
Usage: junk.sh [-hlp] [list of files]
   -h: Display help.
   -l: List junked files.
   -p: Purge all files.
   [list of files] with no other arguments to junk those files.
ENDOFTEXT
}
while getopts ":hlp" option; do
    case "$option" in
        h) help_flag=1
        (( ++num_flags ))
           ;;
        l) list_flag=1
        (( ++num_flags ))
           ;;
        p) purge_flag=1
        (( ++num_flags ))
           ;;
        ?) echo "Error: Unknown option '-$OPTARG'." >&2
           helper >&2
           exit 1
           ;;
    esac
done
if [ $num_flags -ne 1 ] ; then
    if [ $num_flags -ne 0 ] ; then
    echo "Error: Too many options enabled." >&2
    helper >&2
    exit 1
    fi
fi

declare -a filesToDelete
shift "$(( OPTIND-1 ))"
for f in "$@"; do
    filesToDelete[$num_files]="$f"
    (( ++num_files ))
done
if [ $num_flags -ne 0 ] ; then
    if [ $num_files -ne 0 ] ; then
        echo "Error: Too many options enabled." >&2
        helper >&2
        exit 1
    fi
fi

if [ $num_flags -eq 0 ] ; then
    if [ $num_files -eq 0 ] ; then
        helper >&2
        exit 1
    fi
fi

if [ ! -d $recycleDir ] ; then
    mkdir $recycleDir
fi
for j in "${filesToDelete[@]}"; do
    if [ -a $j ] ; then
        mv $j $recycleDir
    else
        echo "Warning: '$j' not found."

    fi
done

if [ $list_flag -eq 1 ] ; then
    ls -lAF $recycleDir
    exit 0
fi

if [ $purge_flag -eq 1 ] ; then
    rm -rf ~/.junk/*
    rm -rf ~/.junk/.* 2> /dev/null
    exit 0
fi

if [ $help_flag -eq 1 ] ; then
    helper
    exit 0
fi
exit 0


# for i in filesToDelete 
#     if ! [ -f "~/.junk"] ; then
#         cd ~
#         mkdir junk
#         chmod 777 junk
#     fi

#     if [ -f "filesToDelete[$num_files]"]
#     mv "filesToDelete[$num_files]" s