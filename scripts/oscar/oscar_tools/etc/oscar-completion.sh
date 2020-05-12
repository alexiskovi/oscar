#!/usr/bin/env bash

###############################################################################
# Copyright 2019 Nikolay Dema. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###############################################################################

OSCAR_ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../../../.." && pwd )"

function oscarcd() {
    cd $OSCAR_ROOT_DIR
}

_oscar_completions()
{
    # if [ "${#COMP_WORDS[@]}" == "2" ]; then
    #     #printf "${COMP_WORDS[1]}"
    #     COMPREPLY=($(compgen -W "docker trajectory test help" -- "${COMP_WORDS[1]}"))
    #
    # elif [ "${#COMP_WORDS[@]}" == "3" ]; then
    #
    #     if [ "${COMP_WORDS[1]}" == "docker" ]; then
    #         COMPREPLY=($(compgen -W "start stop into" -- "${COMP_WORDS[2]}"))
    #
    #     elif [ "${COMP_WORDS[1]}" == "trajectory" ]; then
    #         COMPREPLY=($(compgen -W "generate play stop" -- "${COMP_WORDS[2]}"))
    #
    #     elif [ "${COMP_WORDS[1]}" == "test" ]; then
    #         COMPREPLY=($(compgen -W "val1 val2" -- "${COMP_WORDS[2]}"))
    #
    #     else
    #         return
    #     fi
    #
    # elif [ "${#COMP_WORDS[@]}" == "4" ]; then
    #
    #     if [ "${COMP_WORDS[1]}" == "trajectory" ]; then
    #
    #         if [ "${COMP_WORDS[2]}" == "generate" ]; then
    #             COMPREPLY=($(compgen -W "8type 0type" -- "${COMP_WORDS[3]}"))
    #
    #         else
    #             return
    #         fi
    #
    #     else
    #         return
    #     fi
    #
    # elif [ "${COMP_WORDS[3]}" == "8type" ] || [ "${COMP_WORDS[3]}" == "0type" ]; then
    #     COMP_COUNT=$((${#COMP_WORDS[@]}-1))
    #     COMPREPLY=($(compgen -W "-r -a -d -v -n --rtk-player -h" -- "${COMP_WORDS[${COMP_COUNT}]}"))
    #
    # elif [ "${COMP_WORDS[2]}" == "play" ]; then
    #
    # else
    #   return
    # fi

    if [ "${#COMP_WORDS[@]}" == "2" ]; then
        COMPREPLY=($(compgen -W "docker trajectory help" -- "${COMP_WORDS[1]}"))
    fi

    if [ "${COMP_WORDS[1]}" == "docker" ]; then

        if [ "${#COMP_WORDS[@]}" == "3" ]; then
            COMPREPLY=($(compgen -W "start stop into" -- "${COMP_WORDS[2]}"))
        fi

    elif [ "${COMP_WORDS[1]}" == "trajectory" ]; then

        if [ "${#COMP_WORDS[@]}" == "3" ]; then
            COMPREPLY=($(compgen -W "generate play stop" -- "${COMP_WORDS[2]}"))

        else

            if [ "${COMP_WORDS[2]}" == "generate" ]; then

                if [ "${#COMP_WORDS[@]}" == "4" ]; then
                    COMPREPLY=($(compgen -W "8type 0type" -- "${COMP_WORDS[3]}"))

                else
                    if [ "${COMP_WORDS[3]}" == "8type" ] || [ "${COMP_WORDS[3]}" == "0type" ]; then
                        COMP_COUNT=$((${#COMP_WORDS[@]}-1))
                        COMPREPLY=($(compgen -W "-r -a -d -v -n --rtk-player -h" -- "${COMP_WORDS[${COMP_COUNT}]}"))
                    fi
                fi

            elif [ "${COMP_WORDS[2]}" == "play" ]; then
                COMP_COUNT=$((${#COMP_WORDS[@]}-1))
                COMPREPLY=($(compgen -W "-r -n -l -h --loop --rtk-recorder" -- "${COMP_WORDS[${COMP_COUNT}]}"))

            elif [ "${COMP_WORDS[2]}" == "stop" ]; then
                return
            else
                return
            fi
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
