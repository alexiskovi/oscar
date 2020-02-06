#!/usr/bin/env bash

OSCAR_SCRIPTS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

PATH="$PATH:$OSCAR_SCRIPTS_DIR/oscar_tools/bin"
source $OSCAR_SCRIPTS_DIR/oscar_tools/etc/oscar-completion.sh

OSCAR_ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/../../.." && pwd )"
