call mcc -W 'cpplib:MatlabHelpers,all' -T link:lib GetRotationAngle.m GetRotatedCoordinate.m
del *.log
del *.txt
del *.cpp
del *.exports
del *.def
del *.exp
rmdir /s /q v2

@REM Create Visual Studio folders if not exist
mkdir ..\MovementTracker_main\x64\Debug\bin;
mkdir ..\MovementTracker_main\x64\Debug\lib;
mkdir ..\MovementTracker_main\x64\Release\bin;
mkdir ..\MovementTracker_main\x64\Release\lib;
mkdir ..\MovementTracker_main\x86\Debug\bin;
mkdir ..\MovementTracker_main\x86\Debug\lib;
mkdir ..\MovementTracker_main\x86\Release\bin;
mkdir ..\MovementTracker_main\x86\Release\lib;

copy MatlabHelpers.dll ..\MovementTracker_main\x64\Debug\bin /Y
copy MatlabHelpers.dll ..\MovementTracker_main\x64\Release\bin /Y
copy MatlabHelpers.dll ..\MovementTracker_main\x86\Debug\bin /Y
copy MatlabHelpers.dll ..\MovementTracker_main\x86\Release\bin /Y

copy MatlabHelpers.lib ..\MovementTracker_main\x64\Debug\lib /Y
copy MatlabHelpers.lib ..\MovementTracker_main\x64\Release\lib /Y
copy MatlabHelpers.lib ..\MovementTracker_main\x86\Debug\lib /Y
copy MatlabHelpers.lib ..\MovementTracker_main\x86\Release\lib /Y