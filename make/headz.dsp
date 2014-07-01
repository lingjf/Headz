# Microsoft Developer Studio Project File - Name="headz" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=headz - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "headz.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "headz.mak" CFG="headz - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "headz - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "headz - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "headz - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../obj/msvc60/release"
# PROP Intermediate_Dir "../obj/msvc60/release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /GX /O2 /I "../include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /WX /YX
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../bin/headz_msvc60_r.lib"

!ELSEIF  "$(CFG)" == "headz - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../obj/msvc60/debug"
# PROP Intermediate_Dir "../obj/msvc60/debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /WX /YX /Yc /Yu
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../bin/headz_msvc60_d.lib"

!ENDIF 

# Begin Target

# Name "headz - Win32 Release"
# Name "headz - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\src\atomz.c
# End Source File
# Begin Source File

SOURCE=..\src\avltreez.c
# End Source File
# Begin Source File

SOURCE=..\src\avltreez.h
# End Source File
# Begin Source File

SOURCE=..\src\bintreez.c
# End Source File
# Begin Source File

SOURCE=..\src\bintreez.h
# End Source File
# Begin Source File

SOURCE=..\src\bitmapz.c
# End Source File
# Begin Source File

SOURCE=..\src\bitmapz.h
# End Source File
# Begin Source File

SOURCE=..\src\bitopz.c
# End Source File
# Begin Source File

SOURCE=..\src\bitopz.h
# End Source File
# Begin Source File

SOURCE=..\src\dbugz.c
# End Source File
# Begin Source File

SOURCE=..\src\dbugz.h
# End Source File
# Begin Source File

SOURCE=..\src\hashz.c
# End Source File
# Begin Source File

SOURCE=..\src\hashz.h
# End Source File
# Begin Source File

SOURCE=..\src\jeffz.c
# End Source File
# Begin Source File

SOURCE=..\src\jeffz.h
# End Source File
# Begin Source File

SOURCE=..\src\listz.c
# End Source File
# Begin Source File

SOURCE=..\src\listz.h
# End Source File
# Begin Source File

SOURCE=..\src\multreez.c
# End Source File
# Begin Source File

SOURCE=..\src\multreez.h
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

SOURCE=..\src\priheapz.h
# End Source File
# Begin Source File

SOURCE=..\src\rankz.c
# End Source File
# Begin Source File

SOURCE=..\src\rankz.h
# End Source File
# Begin Source File

SOURCE=..\src\rbtreez.c
# End Source File
# Begin Source File

SOURCE=..\src\rbtreez.h
# End Source File
# Begin Source File

SOURCE=..\src\rxstreez.c
# End Source File
# Begin Source File

SOURCE=..\src\rxstreez.h
# End Source File
# Begin Source File

SOURCE=..\src\rxtreez.c
# End Source File
# Begin Source File

SOURCE=..\src\rxtreez.h
# End Source File
# Begin Source File

SOURCE=..\src\sbtreez.c
# End Source File
# Begin Source File

SOURCE=..\src\sbtreez.h
# End Source File
# Begin Source File

SOURCE=..\src\slabz.c
# End Source File
# Begin Source File

SOURCE=..\src\slabz.h
# End Source File
# Begin Source File

SOURCE=..\src\slakz.c
# End Source File
# Begin Source File

SOURCE=..\src\slakz.h
# End Source File
# Begin Source File

SOURCE=..\src\sptreez.c
# End Source File
# Begin Source File

SOURCE=..\src\sptreez.h
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
# Begin Source File

SOURCE=..\src\treapz.h
# End Source File
# Begin Source File

SOURCE=..\src\utilz.h
# End Source File
# End Group
# End Target
# End Project
