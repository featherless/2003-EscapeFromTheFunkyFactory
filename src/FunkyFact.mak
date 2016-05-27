# Microsoft Developer Studio Generated NMAKE File, Based on FunkyFact.dsp
!IF "$(CFG)" == ""
CFG=FunkyFact - Win32 Debug
!MESSAGE No configuration specified. Defaulting to FunkyFact - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "FunkyFact - Win32 Release" && "$(CFG)" != "FunkyFact - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FunkyFact.mak" CFG="FunkyFact - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FunkyFact - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FunkyFact - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FunkyFact - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\FunkyFact.exe" "$(OUTDIR)\FunkyFact.bsc"


CLEAN :
	-@erase "$(INTDIR)\Characters.obj"
	-@erase "$(INTDIR)\Characters.sbr"
	-@erase "$(INTDIR)\Levels.obj"
	-@erase "$(INTDIR)\Levels.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\objects.obj"
	-@erase "$(INTDIR)\objects.sbr"
	-@erase "$(INTDIR)\obstacle.obj"
	-@erase "$(INTDIR)\obstacle.sbr"
	-@erase "$(INTDIR)\OpenGL.obj"
	-@erase "$(INTDIR)\OpenGL.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\FunkyFact.bsc"
	-@erase "$(OUTDIR)\FunkyFact.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\FunkyFact.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FunkyFact.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\Characters.sbr" \
	"$(INTDIR)\Levels.sbr" \
	"$(INTDIR)\objects.sbr" \
	"$(INTDIR)\OpenGL.sbr" \
	"$(INTDIR)\obstacle.sbr"

"$(OUTDIR)\FunkyFact.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=OpenGL32.lib glaux.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\FunkyFact.pdb" /machine:I386 /out:"$(OUTDIR)\FunkyFact.exe" 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\Characters.obj" \
	"$(INTDIR)\Levels.obj" \
	"$(INTDIR)\objects.obj" \
	"$(INTDIR)\OpenGL.obj" \
	"$(INTDIR)\obstacle.obj"

"$(OUTDIR)\FunkyFact.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "FunkyFact - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\FunkyFact.exe" "$(OUTDIR)\FunkyFact.bsc"


CLEAN :
	-@erase "$(INTDIR)\Characters.obj"
	-@erase "$(INTDIR)\Characters.sbr"
	-@erase "$(INTDIR)\Levels.obj"
	-@erase "$(INTDIR)\Levels.sbr"
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\objects.obj"
	-@erase "$(INTDIR)\objects.sbr"
	-@erase "$(INTDIR)\obstacle.obj"
	-@erase "$(INTDIR)\obstacle.sbr"
	-@erase "$(INTDIR)\OpenGL.obj"
	-@erase "$(INTDIR)\OpenGL.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\FunkyFact.bsc"
	-@erase "$(OUTDIR)\FunkyFact.exe"
	-@erase "$(OUTDIR)\FunkyFact.ilk"
	-@erase "$(OUTDIR)\FunkyFact.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\FunkyFact.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\FunkyFact.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\Characters.sbr" \
	"$(INTDIR)\Levels.sbr" \
	"$(INTDIR)\objects.sbr" \
	"$(INTDIR)\OpenGL.sbr" \
	"$(INTDIR)\obstacle.sbr"

"$(OUTDIR)\FunkyFact.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=OpenGL32.lib glaux.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\FunkyFact.pdb" /debug /machine:I386 /out:"$(OUTDIR)\FunkyFact.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\Characters.obj" \
	"$(INTDIR)\Levels.obj" \
	"$(INTDIR)\objects.obj" \
	"$(INTDIR)\OpenGL.obj" \
	"$(INTDIR)\obstacle.obj"

"$(OUTDIR)\FunkyFact.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("FunkyFact.dep")
!INCLUDE "FunkyFact.dep"
!ELSE 
!MESSAGE Warning: cannot find "FunkyFact.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "FunkyFact - Win32 Release" || "$(CFG)" == "FunkyFact - Win32 Debug"
SOURCE=.\Characters.cpp

"$(INTDIR)\Characters.obj"	"$(INTDIR)\Characters.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Levels.cpp

"$(INTDIR)\Levels.obj"	"$(INTDIR)\Levels.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\main.cpp

"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\objects.cpp

"$(INTDIR)\objects.obj"	"$(INTDIR)\objects.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\obstacle.cpp

"$(INTDIR)\obstacle.obj"	"$(INTDIR)\obstacle.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\OpenGL.cpp

"$(INTDIR)\OpenGL.obj"	"$(INTDIR)\OpenGL.sbr" : $(SOURCE) "$(INTDIR)"



!ENDIF 

