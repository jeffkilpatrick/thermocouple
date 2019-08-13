#!/bin/sh
make -C /Users/kilpatjr/src/thermocouple/xcode -f /Users/kilpatjr/src/thermocouple/xcode/CMakeScripts/ZERO_CHECK_cmakeRulesBuildPhase.make$CONFIGURATION OBJDIR=$(basename "$OBJECT_FILE_DIR_normal") all
