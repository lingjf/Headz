# Microsoft Developer Studio Project File - Name="utest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=utest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "utest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "utest.mak" CFG="utest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "utest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "utest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "utest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../obj/msvc60/utest/release"
# PROP Intermediate_Dir "../obj/msvc60/utest/release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../src" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D malloc=stub_malloc /D free=stub_free /D calloc=stub_calloc /D realloc=stub_realloc /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib Ws2_32.lib libCppUTest_MSVC60.a /nologo /subsystem:console /machine:I386 /libpath:"../tst/CppUTest"

!ELSEIF  "$(CFG)" == "utest - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../obj/msvc60/utest/debug"
# PROP Intermediate_Dir "../obj/msvc60/utest/debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../src" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D malloc=stub_malloc /D free=stub_free /D calloc=stub_calloc /D realloc=stub_realloc /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib Ws2_32.lib libCppUTest_MSVC60.a /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"../tst/CppUTest"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "utest - Win32 Release"
# Name "utest - Win32 Debug"
# Begin Group "tst"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\tst\avltreez_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\bintreez_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\bitmapz_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\bitopz_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\dbugz_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\hashz_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\jeffz_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\listz_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\multreez_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\priheapz_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\rankz_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\rbtreez_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\rxstreez_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\rxtreez_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\sbtreez_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\slabz_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\slakz_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\sptreez_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\treapz_ut.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\ut_main.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\ut_stub.cpp
# End Source File
# Begin Source File

SOURCE=..\tst\utilz_ut.cpp
# End Source File
# End Group
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\atomz.c
# End Source File
# Begin Source File

SOURCE=..\src\avltreez.c
# End Source File
# Begin Source File

SOURCE=..\src\bintreez.c
# End Source File
# Begin Source File

SOURCE=..\src\bitmapz.c
# End Source File
# Begin Source File

SOURCE=..\src\bitopz.c
# End Source File
# Begin Source File

SOURCE=..\src\dbugz.c
# End Source File
# Begin Source File

SOURCE=..\src\hashz.c
# End Source File
# Begin Source File

SOURCE=..\src\jeffz.c
# End Source File
# Begin Source File

SOURCE=..\src\listz.c
# End Source File
# Begin Source File

SOURCE=..\src\multreez.c
# End Source File
# Begin Source File

SOURCE=..\src\octreez.c
# End Source File
# Begin Source File

SOURCE=..\src\octreez.h
# End Source File
# Begin Source File

SOURCE=..\src\priheapz.c
# End Source File
# Begin Source File

SOURCE=..\src\rankz.c
# End Source File
# Begin Source File

SOURCE=..\src\rbtreez.c
# End Source File
# Begin Source File

SOURCE=..\src\rxstreez.c
# End Source File
# Begin Source File

SOURCE=..\src\rxtreez.c
# End Source File
# Begin Source File

SOURCE=..\src\sbtreez.c
# End Source File
# Begin Source File

SOURCE=..\src\slabz.c
# End Source File
# Begin Source File

SOURCE=..\src\slakz.c
# End Source File
# Begin Source File

SOURCE=..\src\sptreez.c
# End Source File
# Begin Source File

SOURCE=..\src\timez.c
# End Source File
# Begin Source File

SOURCE=..\src\timez.h
# End Source File
# Begin Source File

SOURCE=..\src\treapz.c
# End Source File
# End Group
# End Target
# End Project
