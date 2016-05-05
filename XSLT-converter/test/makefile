#
# makefile 10/12/2000
# 
# Copyright (c) 2000 Microsoft Corporation. All rights reserved.
#

# --------------------------------------------
# Macro Definitions
# --------------------------------------------

OUTDIR=.
INTDIR=.\obj

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

LINK=link.exe

CXXFLAGS= \
    /nologo /Fo"$(INTDIR)\\" \
    /Fd"test.pdb" /MLd \
    /DWIN32 /D_CONSOLE /DWIN32_LEAN_AND_MEAN=1 /D_DEBUG /DMBCS \
    /Ze /Zp8 /Gy /W3 /WX /Gd /QIfdiv- /QIf /QI0f /GB /Gi /Gm /GX- /GR- /GF /Od /Zi /Oi

LINKFLAGS= \
    kernel32.lib \
    /machine:I386 /out:"$(OUTDIR)\test.exe" \
    /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\test.pdb" /fixed /nologo \
    /debug /debugtype:cv

LINKOBJS= \
	"$(INTDIR)\test.obj"


# --------------------------------------------
# Description Blocks
# --------------------------------------------

ALL : "$(OUTDIR)\test.exe"

CLEAN :
	-@erase $(OUTDIR)\test.exe
	-@erase $(INTDIR)\test.obj
        @nmake /nologo ALL

"$(INTDIR)" :
    @if not exist "$(INTDIR)\$(NULL)" mkdir "$(INTDIR)"
    @if not exist ".\temp\$(NULL)" mkdir temp

"$(OUTDIR)\test.exe" : "$(INTDIR)" $(LINKOBJS)
    @echo LINK: linking $*
    @$(LINK) $(LINKFLAGS) $(LINKOBJS)

"$(INTDIR)\test.obj" : test.cxx


# --------------------------------------------
# Inference Rules
# --------------------------------------------

.cxx{$(INTDIR)}.obj :
    @$(CXX) $(CXXFLAGS) $<


# end makefile
