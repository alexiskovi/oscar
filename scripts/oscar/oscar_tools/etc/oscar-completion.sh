#!/usr/bin/env bash

OSCAR_ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../../.." && pwd )"

function oscarcd() {
    cd $OSCAR_ROOT_DIR
}

_oscar_completions()
{
    if [ "${#COMP_WORDS[@]}" == "2" ]; then
        #printf "${COMP_WORDS[1]}"
        COMPREPLY=($(compgen -W "docker test help" -- "${COMP_WORDS[1]}"))

    elif [ "${#COMP_WORDS[@]}" == "3" ]; then

        if [ "${COMP_WORDS[1]}" == "docker" ]; then
            COMPREPLY=($(compgen -W "start stop into" -- "${COMP_WORDS[2]}"))

        elif [ "${COMP_WORDS[1]}" == "test" ]; then
            COMPREPLY=($(compgen -W "val1 val2" -- "${COMP_WORDS[2]}"))

        else
            return
        fi

    else
      return
    fi
}

_oscarcd_completions()
{
    # if [ -z "$1" ]; then
    #   pwd
    # fi
    return
}

complete -F _oscar_completions oscar
complete -F _oscarcd_completions oscarcd
