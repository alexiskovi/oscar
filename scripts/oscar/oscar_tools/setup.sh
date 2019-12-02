#!/usr/bin/env bash

OSCAR_SCRIPTS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

PATH="$PATH:$OSCAR_SCRIPTS_DIR/bin"
source $OSCAR_SCRIPTS_DIR/etc/oscar-completion.sh

OSCAR_ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../.." && pwd )"

function to_oscar_root() {
    cd $OSCAR_ROOT_DIR
}
