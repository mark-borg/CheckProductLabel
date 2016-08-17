# Microsoft Developer Studio Project File - Name="checklabel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=checklabel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "checklabel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "checklabel.mak" CFG="checklabel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "checklabel - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "checklabel - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "checklabel - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 vfw32.lib d:/opencv/lib/cxcore.lib d:/opencv/lib/cv.lib d:/opencv/lib/highgui.lib htmlhelp.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "checklabel - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vfw32.lib d:/opencv/lib/cxcored.lib d:/opencv/lib/cvd.lib d:/opencv/lib/highguid.lib htmlhelp.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "checklabel - Win32 Release"
# Name "checklabel - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Acknowledgements.cpp
# ADD CPP /YX
# End Source File
# Begin Source File

SOURCE=.\checklabel.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\checklabel.rc
# End Source File
# Begin Source File

SOURCE=.\checklabelDlg.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Configuration.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\GUI\MessageWindow.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\GUI\ParamCtrl.cpp
# ADD CPP /I ".." /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\ParamDlg.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /YX"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Acknowledgements.h
# End Source File
# Begin Source File

SOURCE=.\checklabel.h
# End Source File
# Begin Source File

SOURCE=.\checklabelDlg.h
# End Source File
# Begin Source File

SOURCE=.\Configuration.h
# End Source File
# Begin Source File

SOURCE=.\CVWindow.h
# End Source File
# Begin Source File

SOURCE=.\GUI\MessageWindow.h
# End Source File
# Begin Source File

SOURCE=.\GUI\ParamCtrl.h
# End Source File
# Begin Source File

SOURCE=.\ParamDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\checklabel.ico
# End Source File
# Begin Source File

SOURCE=.\res\checklabel.rc2
# End Source File
# Begin Source File

SOURCE=.\res\img01.bmp
# End Source File
# Begin Source File

SOURCE=.\res\logo.v3.256.bmp
# End Source File
# End Group
# Begin Group "QV Core - All"

# PROP Default_Filter ""
# Begin Group "header files No. 1"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\AreaVarianceDetector.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\BarcodeDecoder.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\BarcodeReader.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\Camera.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\common.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\ContainerAreaFinder.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\ContainerOutlineFinder.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\coord_geom.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\EAN13BarcodeDecoder.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\framework.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\graphics.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\HighTexturedAreaFinder.h
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\NumberedFileCamera.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\AreaVarianceDetector.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\BarcodeDecoder.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\BarcodeReader.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\Camera.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\common.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\ContainerAreaFinder.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\ContainerOutlineFinder.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\coord_geom.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\EAN13BarcodeDecoder.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\graphics.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\HighTexturedAreaFinder.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\QV\src\QVTools\qvcore\NumberedFileCamera.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
