#!/usr/bin/env bash
python3 /apollo/modules/supervisor/gui/close_sv_gui.py &
cyber_launch stop /apollo/modules/supervisor/launch/sv_launch.launch