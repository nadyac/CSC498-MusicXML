#
# makefile 10/12/2000
# 
# Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
#

# --------------------------------------------
# Macro Definitions
# --------------------------------------------

!IF "$(CFG)" != "retail"
CFG=debug
!ENDIF

OUTDIR=.\$(CFG)
INTDIR=.\$(CFG)
RESDIR=.\resource

MC=mc

LINK=link.exe

BIND=bind.exe
BINDFLAGS=/u

CAB=cabarc
ZIP=wzzip
UNZIP=wzunzip

!IF "$(OS)" == "Windows_NT"
NULL=
RD=rd /s /q
!ELSE 
NULL=nul
RD=deltree /y
!ENDIF

!IF "$(CFG)" == "debug"

CXXFLAGS= \
    /nologo /Fo"$(INTDIR)\\" \
    /Fd"$(INTDIR)\msxsl.pdb" \
    /DWIN32 /D_CONSOLE /DWIN32_LEAN_AND_MEAN=1 /D_DEBUG /DUNICODE \
    /Zel /Zp8 /Gy /W3 /WX /Gz /QIfdiv- /QIf /QI0f /GB /Gi /Gm /GX- /GR- /GF /Od /Zi /Oi

LINKFLAGS= \
    kernel32.lib advapi32.lib ole32.lib oleaut32.lib msxml2.lib wininet.lib \
    /machine:I386 /out:"$(OUTDIR)\msxsl.exe" /nodefaultlib \
    /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\msxsl.pdb" /fixed /nologo \
    /debug /debugtype:cv /opt:nowin98

RFLAGS=/l 0x0409 /fo"$(RESDIR)\msxsl.res" /d "_DEBUG"

!ELSE

CXXFLAGS= \
    /nologo /Fo"$(INTDIR)\\" \
    /DWIN32 /D_CONSOLE /DWIN32_LEAN_AND_MEAN=1 /DNDEBUG /DUNICODE \
    /Zel /Zp8 /Gy /W3 /WX /Gz /QIfdiv- /QIf /QI0f /GB /Gi- /Gm- /GX- /GR- /GF /Oxs

LINKFLAGS= \
    kernel32.lib advapi32.lib ole32.lib oleaut32.lib msxml2.lib wininet.lib \
    /machine:I386 /out:"$(OUTDIR)\msxsl.exe" /nodefaultlib \
    /subsystem:console /incremental:no /fixed /nologo /opt:ref /opt:icf /opt:nowin98

RFLAGS=/l 0x0409 /fo"$(RESDIR)\msxsl.res"

!ENDIF

LINKOBJS= \
    "$(RESDIR)\msxsl.res" \
    "$(INTDIR)\main.obj" \
    "$(INTDIR)\msxmlinf.obj" \
    "$(INTDIR)\timer.obj" \
    "$(INTDIR)\cmdline.obj" \
    "$(INTDIR)\transfrm.obj" \
    "$(INTDIR)\crt.obj" \
    "$(INTDIR)\util.obj" \
    "$(INTDIR)\seqstrm.obj"

# --------------------------------------------
# Description Blocks
# --------------------------------------------

ALL : "$(OUTDIR)\msxsl.exe"

CLEAN :
        -@$(RD) $(OUTDIR)
        -@$(RD) $(INTDIR)
        -@erase "$(RESDIR)\messages.rc"
        @nmake /nologo ALL

CAB :
        @nmake /nologo CLEAN CFG=retail MCFLAGS=/A
        $(CAB) -p N retail\msxslsrc.cab @files.txt
        $(CAB) N retail\msxslexe.cab retail\msxsl.exe
        @cd test
        -@$(RD) msxsl
        @extract /E /L msxsl ..\retail\msxslsrc.cab
        @extract /E /L msxsl ..\retail\msxslexe.cab
        @cd msxsl
        msxsl test\valid.xml test\copyof.xsl
        @nmake
        @nmake CFG=retail
        @cd test
        @nmake
        @test
        @test retail
        @cd ..\..
        -@$(RD) msxsl
        @cd ..

ZIP :
        @nmake /nologo CLEAN CFG=retail MCFLAGS=/A
        $(ZIP) -P -k retail\msxslsrc.zip @files.txt
        $(ZIP) retail\msxslexe.zip retail\msxsl.exe
        @cd test
        -@$(RD) msxsl
	@md msxsl
	@cd msxsl
        $(UNZIP) -d ..\..\retail\msxslsrc.zip
        $(UNZIP) -d ..\..\retail\msxslexe.zip
        msxsl test\valid.xml test\copyof.xsl
        @nmake
        @nmake CFG=retail
        @cd test
        @nmake
        @test
        @test retail
        @cd ..\..
        -@$(RD) msxsl
        @cd ..

"$(OUTDIR)" :
    @if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(OUTDIR)\msxsl.exe" : "$(OUTDIR)" $(LINKOBJS)
    @echo LINK: linking $*
    @$(LINK) $(LINKFLAGS) $(LINKOBJS)
    @$(BIND) $(BINDFLAGS) "$(OUTDIR)\msxsl.exe"

"$(INTDIR)\main.obj" : PCH main.cxx transfrm.hxx cmdline.hxx
"$(INTDIR)\msxmlinf.obj" : PCH timer.cxx timer.hxx
"$(INTDIR)\timer.obj" : PCH timer.cxx timer.hxx
"$(INTDIR)\cmdline.obj" : PCH cmdline.cxx cmdline.hxx
"$(INTDIR)\transfrm.obj" : PCH transfrm.cxx transfrm.hxx cmdline.hxx seqstrm.hxx
"$(INTDIR)\crt.obj" : PCH crt.cxx
"$(INTDIR)\util.obj" : PCH util.cxx util.hxx
"$(INTDIR)\seqstrm.obj" : PCH seqstrm.cxx seqstrm.hxx

transfrm.hxx : timer.hxx seqstrm.hxx
cmdline.hxx : msxmlinf.hxx
core.hxx : util.hxx

PCH : "$(INTDIR)\core.pch"

"$(INTDIR)\core.pch" : "$(INTDIR)" "$(RESDIR)\messages.h" core.hxx core.cxx
    @$(CXX) $(CXXFLAGS) /Yc"core.hxx" /Fp"$(INTDIR)\core.pch" /c core.cxx

"$(RESDIR)\msxsl.res" : "$(RESDIR)\messages.rc" "$(RESDIR)\msxsl.rc"
    @echo RC: Compiling $(RESDIR)\msxsl.rc
    @$(RC) $(RFLAGS) /r "$(RESDIR)\msxsl.rc"

"$(RESDIR)\messages.h" : "$(RESDIR)\messages.rc"
"$(RESDIR)\messages.rc" : "$(RESDIR)\messages.mc"


# --------------------------------------------
# Inference Rules
# --------------------------------------------

.cxx{$(INTDIR)}.obj :
    @$(CXX) $(CXXFLAGS) /Yu"core.hxx" /Fp"$(INTDIR)\core.pch" /c $<

{$(RESDIR)}.mc{$(RESDIR)}.rc :
    @$(MC) $(MCFLAGS) $< /r "$(RESDIR)" /h "$(RESDIR)"

.SUFFIXES : .mc

# end makefile
