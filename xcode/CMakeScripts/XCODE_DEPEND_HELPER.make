# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.Thermocouple.Debug:
/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/Debug/Thermocouple.framework/Thermocouple:
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/Thermocouple/Debug/Thermocouple.framework/Thermocouple


PostBuild.ThermocoupleMonitor.Debug:
PostBuild.Thermocouple.Debug: /Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/Debug/ThermocoupleMonitor
/Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/Debug/ThermocoupleMonitor:\
	/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/Debug/Thermocouple.framework/Versions/A/Thermocouple\
	/usr/lib/libncurses.dylib
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/Debug/ThermocoupleMonitor


PostBuild.gtest.Debug:
/Users/kilpatjr/src/thermocouple/xcode/gtest/Debug/libgtest.a:
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/gtest/Debug/libgtest.a


PostBuild.unittest.Debug:
PostBuild.Thermocouple.Debug: /Users/kilpatjr/src/thermocouple/xcode/unittest/Debug/unittest
PostBuild.gtest.Debug: /Users/kilpatjr/src/thermocouple/xcode/unittest/Debug/unittest
/Users/kilpatjr/src/thermocouple/xcode/unittest/Debug/unittest:\
	/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/Debug/Thermocouple.framework/Versions/A/Thermocouple\
	/Users/kilpatjr/src/thermocouple/xcode/gtest/Debug/libgtest.a
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/unittest/Debug/unittest


PostBuild.Thermocouple.Release:
/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/Release/Thermocouple.framework/Thermocouple:
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/Thermocouple/Release/Thermocouple.framework/Thermocouple


PostBuild.ThermocoupleMonitor.Release:
PostBuild.Thermocouple.Release: /Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/Release/ThermocoupleMonitor
/Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/Release/ThermocoupleMonitor:\
	/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/Release/Thermocouple.framework/Versions/A/Thermocouple\
	/usr/lib/libncurses.dylib
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/Release/ThermocoupleMonitor


PostBuild.gtest.Release:
/Users/kilpatjr/src/thermocouple/xcode/gtest/Release/libgtest.a:
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/gtest/Release/libgtest.a


PostBuild.unittest.Release:
PostBuild.Thermocouple.Release: /Users/kilpatjr/src/thermocouple/xcode/unittest/Release/unittest
PostBuild.gtest.Release: /Users/kilpatjr/src/thermocouple/xcode/unittest/Release/unittest
/Users/kilpatjr/src/thermocouple/xcode/unittest/Release/unittest:\
	/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/Release/Thermocouple.framework/Versions/A/Thermocouple\
	/Users/kilpatjr/src/thermocouple/xcode/gtest/Release/libgtest.a
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/unittest/Release/unittest


PostBuild.Thermocouple.MinSizeRel:
/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/MinSizeRel/Thermocouple.framework/Thermocouple:
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/Thermocouple/MinSizeRel/Thermocouple.framework/Thermocouple


PostBuild.ThermocoupleMonitor.MinSizeRel:
PostBuild.Thermocouple.MinSizeRel: /Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/MinSizeRel/ThermocoupleMonitor
/Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/MinSizeRel/ThermocoupleMonitor:\
	/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/MinSizeRel/Thermocouple.framework/Versions/A/Thermocouple\
	/usr/lib/libncurses.dylib
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/MinSizeRel/ThermocoupleMonitor


PostBuild.gtest.MinSizeRel:
/Users/kilpatjr/src/thermocouple/xcode/gtest/MinSizeRel/libgtest.a:
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/gtest/MinSizeRel/libgtest.a


PostBuild.unittest.MinSizeRel:
PostBuild.Thermocouple.MinSizeRel: /Users/kilpatjr/src/thermocouple/xcode/unittest/MinSizeRel/unittest
PostBuild.gtest.MinSizeRel: /Users/kilpatjr/src/thermocouple/xcode/unittest/MinSizeRel/unittest
/Users/kilpatjr/src/thermocouple/xcode/unittest/MinSizeRel/unittest:\
	/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/MinSizeRel/Thermocouple.framework/Versions/A/Thermocouple\
	/Users/kilpatjr/src/thermocouple/xcode/gtest/MinSizeRel/libgtest.a
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/unittest/MinSizeRel/unittest


PostBuild.Thermocouple.RelWithDebInfo:
/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/RelWithDebInfo/Thermocouple.framework/Thermocouple:
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/Thermocouple/RelWithDebInfo/Thermocouple.framework/Thermocouple


PostBuild.ThermocoupleMonitor.RelWithDebInfo:
PostBuild.Thermocouple.RelWithDebInfo: /Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/RelWithDebInfo/ThermocoupleMonitor
/Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/RelWithDebInfo/ThermocoupleMonitor:\
	/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/RelWithDebInfo/Thermocouple.framework/Versions/A/Thermocouple\
	/usr/lib/libncurses.dylib
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/ThermocoupleMonitor/RelWithDebInfo/ThermocoupleMonitor


PostBuild.gtest.RelWithDebInfo:
/Users/kilpatjr/src/thermocouple/xcode/gtest/RelWithDebInfo/libgtest.a:
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/gtest/RelWithDebInfo/libgtest.a


PostBuild.unittest.RelWithDebInfo:
PostBuild.Thermocouple.RelWithDebInfo: /Users/kilpatjr/src/thermocouple/xcode/unittest/RelWithDebInfo/unittest
PostBuild.gtest.RelWithDebInfo: /Users/kilpatjr/src/thermocouple/xcode/unittest/RelWithDebInfo/unittest
/Users/kilpatjr/src/thermocouple/xcode/unittest/RelWithDebInfo/unittest:\
	/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/RelWithDebInfo/Thermocouple.framework/Versions/A/Thermocouple\
	/Users/kilpatjr/src/thermocouple/xcode/gtest/RelWithDebInfo/libgtest.a
	/bin/rm -f /Users/kilpatjr/src/thermocouple/xcode/unittest/RelWithDebInfo/unittest




# For each target create a dummy ruleso the target does not have to exist
/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/Debug/Thermocouple.framework/Versions/A/Thermocouple:
/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/MinSizeRel/Thermocouple.framework/Versions/A/Thermocouple:
/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/RelWithDebInfo/Thermocouple.framework/Versions/A/Thermocouple:
/Users/kilpatjr/src/thermocouple/xcode/Thermocouple/Release/Thermocouple.framework/Versions/A/Thermocouple:
/Users/kilpatjr/src/thermocouple/xcode/gtest/Debug/libgtest.a:
/Users/kilpatjr/src/thermocouple/xcode/gtest/MinSizeRel/libgtest.a:
/Users/kilpatjr/src/thermocouple/xcode/gtest/RelWithDebInfo/libgtest.a:
/Users/kilpatjr/src/thermocouple/xcode/gtest/Release/libgtest.a:
/usr/lib/libncurses.dylib:
