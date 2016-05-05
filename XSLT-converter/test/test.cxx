/*
 * @(#)Test.cxx 1.0 10/17/2000
 * 
 * Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
 *
 * Quick and dirty tests for msxsl.exe.
 * 
 */

#include "windows.h"
#include "stdio.h"
#include "assert.h"

static const char * s_pszProg;
static const char s_pszProgDebug[] = "..\\debug\\msxsl.exe";
static const char s_pszProgRetail[] = "..\\retail\\msxsl.exe";

static char s_pszBuffer[32000];
static char s_pszBuffer2[32000];

bool fFoundError = false;
bool fIsNT;

#define ChkWin(ret) {if (!(ret)) {printf("Test Failed, test.cxx (%d), error #%d\n", __LINE__, ::GetLastError()); fFoundError = true; return;}}

enum {REDIRECT_NONE = 0, REDIRECT_STDIN = 1, REDIRECT_STDOUT = 2, REDIRECT_STDERR = 4};


void
ExecuteCommand(const char * pszCmdLine, HANDLE hStdIn, HANDLE hStdOut, HANDLE hStdErr)
{
    SECURITY_ATTRIBUTES secAttrs;
    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;

    secAttrs.nLength = sizeof(SECURITY_ATTRIBUTES);
    secAttrs.lpSecurityDescriptor = NULL;
    secAttrs.bInheritHandle = TRUE;

    memset(&startupInfo, 0, sizeof(STARTUPINFO));
    startupInfo.cb = sizeof(STARTUPINFO);
    startupInfo.dwFlags = STARTF_USESTDHANDLES;
    startupInfo.hStdInput = hStdIn;
    startupInfo.hStdOutput = hStdOut;
    startupInfo.hStdError = hStdErr;

    memset(&processInfo, 0, sizeof(PROCESS_INFORMATION));

    if (fIsNT)
    {
        sprintf(s_pszBuffer2, "%s /C \"%s\"", "cmd.exe", pszCmdLine);
    }
    else
    {
        sprintf(s_pszBuffer2, "%s /C %s", "command.com", pszCmdLine);
    }

    ChkWin(::CreateProcess(
                NULL,
                s_pszBuffer2,
                &secAttrs,
                &secAttrs,
                TRUE,
                0,
                NULL,
                NULL,
                &startupInfo,
                &processInfo));

    ::WaitForSingleObject(processInfo.hProcess, INFINITE);

    ::CloseHandle(processInfo.hThread);
    ::CloseHandle(processInfo.hProcess);
}


void
Diff(const char * pszFile)
{
    HANDLE hTemp, hBaseline;
    DWORD cbTemp, cbBaseline;
    char pszPath[256];

    sprintf(pszPath, "temp\\%s", pszFile);

    hTemp = ::CreateFile(
                pszPath,
                GENERIC_READ,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
    ChkWin(hTemp != NULL);

    ChkWin(::ReadFile(hTemp, s_pszBuffer, sizeof(s_pszBuffer), &cbTemp, NULL));

    sprintf(pszPath, "baseline\\%s", pszFile);

    hBaseline = ::CreateFile(
                pszPath,
                GENERIC_READ,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
    ChkWin(hBaseline != NULL);

    ChkWin(::ReadFile(hBaseline, s_pszBuffer2, sizeof(s_pszBuffer), &cbBaseline, NULL));

    if (cbTemp != cbBaseline || memcmp(s_pszBuffer, s_pszBuffer2, cbTemp))
    {
        fFoundError = true; 
        printf("DIFF FAILED!\n");
    }
    
    ::CloseHandle(hTemp);
    ::CloseHandle(hBaseline);
}


void
RunTest(const char * pszCmdLine, const char * pszFile)
{
    if (pszFile)
        printf("----- %s\n", pszFile);

    sprintf(s_pszBuffer, pszCmdLine, s_pszProg, s_pszProg);

    ExecuteCommand(s_pszBuffer, ::GetStdHandle(STD_INPUT_HANDLE), ::GetStdHandle(STD_OUTPUT_HANDLE), ::GetStdHandle(STD_ERROR_HANDLE));

    if (pszFile)
        Diff(pszFile);
}


void
RunErrorTest(const char * pszCmdLine, const char * pszErrorFile, const char * pszDiffFile1 = NULL)
{
    SECURITY_ATTRIBUTES secAttrs;
    HANDLE h;

    if (pszErrorFile)
        printf("----- %s\n", pszErrorFile);

    sprintf(s_pszBuffer, "temp\\%s", pszErrorFile);

    secAttrs.nLength = sizeof(SECURITY_ATTRIBUTES);
    secAttrs.lpSecurityDescriptor = NULL;
    secAttrs.bInheritHandle = TRUE;

    h = ::CreateFile(
            s_pszBuffer,
            GENERIC_WRITE,
            0,
            &secAttrs,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    ChkWin(h != NULL);

    sprintf(s_pszBuffer, pszCmdLine, s_pszProg, s_pszProg);

    ExecuteCommand(s_pszBuffer, ::GetStdHandle(STD_INPUT_HANDLE), ::GetStdHandle(STD_OUTPUT_HANDLE), h);

    ::CloseHandle(h);

    Diff(pszErrorFile);

    if (pszDiffFile1)
        Diff(pszDiffFile1);
}


void main(int argc, char * argv[])
{
    OSVERSIONINFOA vi;

    if (argc >= 2 && !_stricmp(argv[1], "retail"))
    {
        s_pszProg = s_pszProgRetail;
    }
    else
    {
        s_pszProg = s_pszProgDebug;
    }

    // Get the OS version we're running
    vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    ChkWin(GetVersionExA(&vi));
    fIsNT = (vi.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS);

    puts("");
    puts("==========================================================");
    puts("Command Line Errors ...");
    puts("==========================================================");

    RunErrorTest("%s -invalid", "cmderr1.txt");
    RunErrorTest("%s -o -?", "cmderr2.txt");
    RunErrorTest("%s -unk", "cmderr3.txt");
    RunErrorTest("%s books.xml valid.xsl -o valid.out param -xw ", "cmderr4.txt");
    RunErrorTest("%s books.xml valid.xsl -o valid.out =val", "cmderr5.txt");
    RunErrorTest("%s books.xml valid.xsl -o valid.out param + val ", "cmderr6.txt");
    RunErrorTest("%s books.xml valid.xsl -o valid.out param == val", "cmderr7.txt");
    RunErrorTest("%s books.xml valid.xsl -o valid.out param ' = ' val ", "cmderr8.txt");
    RunErrorTest("%s", "cmderr9.txt");
    RunErrorTest("%s books.xml", "cmderr10.txt");
    RunErrorTest("%s books.xml valid.xsl -o ", "cmderr11.txt");
    RunErrorTest("%s books.xml valid.xsl param=", "cmderr12.txt");
    RunErrorTest("%s books.xml valid.xsl - ", "cmderr13.txt");
    RunErrorTest("%s books.xml valid.xsl -o valid.out 'param=val'", "cmderr14.txt");
    RunErrorTest("%s -t -t -? -xw -xw -? ", "cmderr15.txt");
    RunErrorTest("%s books.xml valid.xsl -m ", "cmderr16.txt");
    RunErrorTest("%s books.xml valid.xsl xmlns:foo", "cmderr17.txt");
    RunErrorTest("%s books.xml valid.xsl xmlns:foo=", "cmderr18.txt");
    RunErrorTest("%s books.xml valid.xsl foo:bar=val", "cmderr19.txt");
    RunErrorTest("%s -op", "cmderr20.txt");
    RunErrorTest("%s - - < valid.xsl", "cmderr21.txt");
    RunErrorTest("%s -u 2.5 books.xml valid.xsl", "cmderr22.txt");
    RunErrorTest("%s -pi books.xml -u", "cmderr23.txt");
    RunErrorTest("%s books.xml - -pi", "cmderr24.txt");
    RunErrorTest("%s books.xml valid.xsl -pi", "cmderr25.txt");

    puts("");
    puts("==========================================================");
    puts("Command Line Parsing ...");
    puts("==========================================================");

    RunTest("%s /O  	temp\\cmdln1.xml 	books.xml 	valid.xsl	p1= last-name  p2 = 'first-name'  p3 =price  -xw  ", "cmdln1.xml");
    RunTest("\"%s\" /XW /Xw   \"../test/books.xml	\"  /xW /xw \"valid.xsl  \"  p1=\"last-name\"p2 =first-name 'p3''=''price' /o  \"temp\\cmdln2.xml", "cmdln2.xml");
    RunTest("\"%s \"  \"books.xml\"\"valid.xsl\"-o \"temp\\cmdln3.xml\"\"p1\"\"=\"\"'1' = '1'\"", "cmdln3.xml");

    puts("");
    puts("==========================================================");
    puts("Transforms ...");
    puts("==========================================================");

    RunTest("%s books.xml ws.xsl -o temp\\tr1.xml unknown1=val unknown2=val unknown3=val unknown4=val", "tr1.xml");
    RunTest("%s -xW books.xml ws.xsl -o temp\\tr2.xml unknown1=val unknown2=val unknown3=val unknown4=val", "tr2.xml");
    RunTest("%s books.xml valid.xsl p1=first-name > temp\\tr3.xml", "tr3.xml");
    RunTest("%s /v /V valid.xml valid.xsl p1='elem' > temp\\tr4.xml", "tr4.xml");
    RunTest("%s invalid.xml copyof.xsl > temp\\tr5.xml", "tr5.xml");
    RunTest("%s -m foo valid.xml valid.xsl > temp\\tr6.xml", "tr6.xml");
    RunTest("%s valid.xml valid.xsl /M my-ns:foo xmlns:my-ns='http://my.com' my-ns:param='val' > temp\\tr7.xml", "tr7.xml");
    RunTest("%s valid.xml valid.xsl -m my-ns:foo my-ns:param='val' xmlns:my-ns=\"unknown\" xmlns:my-ns=http://my.com  > temp\\tr8.xml", "tr8.xml");
    RunTest("%s valid.xml valid.xsl xmlns='http://my.com' xmlns:my-ns2='http://my.com2' -m foo my-ns2:param2='val2' param=val > temp\\tr9.xml", "tr9.xml");
    RunTest("%s books.xml valid.xsl xmlns='http://my.com' xmlns='' p1=first-name p2=last-name p3=price p4=val4 p5=val5>temp\\tr10.xml", "tr10.xml");
    RunTest("%s books.xml valid.xsl xmlns='http://my.com' xmlns='' xmlns='http://my.com' xmlns:foo='unknown' param=first-name -m foo> temp\\tr11.xml", "tr11.xml");
    RunTest("%s books.xml valid.xsl p1=first-name | %s - copyof.xsl > temp\\tr12.xml", "tr12.xml");
    RunTest("%s / copyof.xsl < books.xml > temp\\tr13.xml", "tr13.xml");
    RunTest("%s books.xml / p1=last-name < valid.xsl > temp\\tr14.xml", "tr14.xml");
    RunTest("%s books.xml copyof10.xsl | more | %s - copyof.xsl | sort > temp\\tr15.xml ", "tr15.xml");
    RunTest("%s copyof.xsl copyof.xsl | %s books.xml - | sort > temp\\tr16.xml ", "tr16.xml");
    RunTest("%s -u 3.0 books.xml version.xsl > temp\\tr17.xml ", "tr17.xml");
    RunTest("%s books.xml version.xsl -u 4.0 > temp\\tr18.xml ", "tr18.xml");
    RunTest("%s -pi books.xml > temp\\tr19.xml ", "tr19.xml");
    RunTest("%s books.xml -pi p1=val > temp\\tr20.xml ", "tr20.xml");
    RunTest("%s baseline\\pi.xsl -pi > temp\\tr21.xml ", "tr21.xml");

    puts("");
    puts("----- Output to console (manually check since can't redirect to file)");
    RunTest("%s books.xml valid.xsl", NULL);

    puts("");
    puts("==========================================================");
    puts("Transform Errors ...");
    puts("==========================================================");

    RunErrorTest("%s unknown.xml valid.xsl", "trerr1.txt");
    RunErrorTest("%s http://unknown/unknown.xml valid.xsl", "trerr2.txt");
    RunErrorTest("%s books.xml invalid1.xsl", "trerr3.txt");
    RunErrorTest("%s books.xml invalid2.xsl > temp\\trerr4.xml", "trerr4.txt", "trerr4.xml");
    RunErrorTest("%s books.xml valid.xsl -o \\unknown\\unknown\\unknown\\unknown.txt", "trerr5.txt");
    RunErrorTest("%s books.xml valid.xsl -o valid.out  !p!=val", "trerr6.txt");
    RunErrorTest("%s books.xml valid.xsl xmlns:foo='http://my.com' foo:bar:baz='val'", "trerr7.txt");
    RunErrorTest("%s books.xml valid.xsl /m foo::=val xmlns:foo=a", "trerr8.txt");
    RunErrorTest("%s -XW valid.xml copyof.xsl | sort | %s - copyof.xsl", "trerr9.txt");
    RunErrorTest("%s -xw invalid1.xsl copyof.xsl | %s books.xml -", "trerr10.txt");
    RunErrorTest("%s -Xw invalid2.xsl copyof.xsl | %s books.xml - > temp\\trerr11.xml", "trerr11.txt", "trerr11.xml");
    RunErrorTest("%s -u 2.6 books.xml copyof.xsl", "trerr12.txt");
    RunErrorTest("%s -pi copyof.xsl", "trerr13.txt");
    RunErrorTest("%s -pi bad-pi.xml", "trerr14.txt");
    RunErrorTest("%s bad-pi2.xml -pi", "trerr15.txt");
    RunErrorTest("%s -pi bad-pi3.xml", "trerr16.txt");

    puts("");
    puts("----- Validation error (manually check since URL changes)");
    RunTest("%s -v invalid.xml valid.xsl p1='elem'", NULL);

    puts("");
    puts("----- Resolution error (manually check since URL changes)");
    RunTest("%s /xe -Xe -XE -xE invalid.xml copyof.xsl", NULL);

    puts("");
    puts("==========================================================");
    puts("Timings ...");
    puts("==========================================================");
    RunTest("%s -t /t books.xml valid.xsl -T -o temp\\timings.xml", "timings.xml");

    puts("");
    puts("==========================================================");
    printf("MSXSL Tests %s\n", fFoundError ? "FAILED" : "PASSED");
    puts("==========================================================");
}

