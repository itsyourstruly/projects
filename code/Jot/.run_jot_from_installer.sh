#!/bin/bash
# Internal script used by installer to run jot and return to installer

# Get the directory where this script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Run jot
"${SCRIPT_DIR}/jot" "$@"

# After jot exits, rerun the installer
exec "${SCRIPT_DIR}/install.sh"

