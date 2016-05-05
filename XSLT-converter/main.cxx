/*
 * @(#)Main.cxx 1.0 9/26/2000
 * 
 * Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
 *
 * Startup for executable.
 * 
 */

#include "core.hxx"
#pragma hdrstop

#include "transfrm.hxx"
#include "cmdline.hxx"
#include "msxmlinf.hxx"

DWORD g_dwPlatformId;

HANDLE g_hStdIn;
HANDLE g_hStdOut;
HANDLE g_hStdErr;


/*  ----------------------------------------------------------------------------
    PrintUsage()

    Prints the usage message to standard error.
*/
void
PrintUsage(MSXMLInfo * pMSXMLInfo)
{
    HRESULT hr;
    const WCHAR * pwszHeader = NULL;
    const WCHAR * pwszUsage = NULL;

    ChkSucc(Resources::FormatResourceMessage(MSXSL_MSG_HEADER, &pwszHeader, pMSXMLInfo->getVersionString(), NULL));
    WriteAsMBCS(g_hStdErr, pwszHeader);
    WriteAsMBCS(g_hStdErr, L"\r\n");

    ChkSucc(Resources::FormatResourceMessage(MSXSL_MSG_USAGE, &pwszUsage, NULL));
    WriteAsMBCS(g_hStdErr, pwszUsage);

Cleanup:
    free((void *) pwszHeader);
    free((void *) pwszUsage);
}


/*  ----------------------------------------------------------------------------
    FormatTiming()

    Formats a timing number (in microseconds) for output.
*/
WCHAR *
FormatTiming(DWORD dwTiming, WCHAR * pwszTiming)
{
    WCHAR * pwszDecimal, * pwsz;
    int cwch;

    if (dwTiming == 0)
    {
        // Special case 0
        pwszTiming[0] = L'0';
        pwszTiming[1] = 0;
        return pwszTiming;
    }

    // Guarantee that conversion results in at least 3 digits
    cwch = 0;
    if (dwTiming < 100)
    {
        pwszTiming[0] = L'0';
        cwch++;

        if (dwTiming < 10)
        {
            pwszTiming[1] = L'0';
            cwch++;
        }
    }

    _itow(dwTiming, pwszTiming + cwch, 10);

    cwch = _wcslen(pwszTiming);

    // Figure position of decimal point
    pwszDecimal = pwszTiming + cwch - 3;

    if (dwTiming >= 10000)
    {
        // If timing contains more than 4 decimal digits, truncate up to 3 digits
        pwszDecimal[3 - min(cwch - 4, 3)] = 0;
    }

    if (dwTiming < 1000000)
    {
        // Add decimal point
        for (pwsz = pwszDecimal + 4; pwsz > pwszDecimal; pwsz--)
        {
            pwsz[0] = pwsz[-1];
        }
        *pwszDecimal = L'.';
    }

    return pwszTiming;
}


/*  ----------------------------------------------------------------------------
    PrintTimings()

    Prints load and transformation times to stdout.
*/
HRESULT
PrintTimings(MSXMLInfo * pMSXMLInfo, TransformHelper * transHelper)
{
    HRESULT hr = S_OK;
    const WCHAR * pwszHeader = NULL;
    const WCHAR * pwszTimings = NULL;
    WCHAR rgwchSourceTime[16];
    WCHAR rgwchStylesheetTime[16];
    WCHAR rgwchCompileTime[16];
    WCHAR rgwchExecutionTime[16];

    ChkSucc(Resources::FormatResourceMessage(MSXSL_MSG_HEADER, &pwszHeader, pMSXMLInfo->getVersionString(), NULL));
    WriteAsMBCS(g_hStdErr, pwszHeader);
    WriteAsMBCS(g_hStdErr, L"\r\n");

    ChkSucc(Resources::FormatResourceMessage(
                MSXSL_MSG_TIMINGS,
                &pwszTimings,
                FormatTiming(transHelper->GetSourceTime(), rgwchSourceTime),
                FormatTiming(transHelper->GetStylesheetTime(), rgwchStylesheetTime),
                FormatTiming(transHelper->GetCompileTime(), rgwchCompileTime),
                FormatTiming(transHelper->GetExecutionTime(), rgwchExecutionTime),
                NULL));

    WriteAsMBCS(g_hStdErr, pwszTimings);

Cleanup:
    free((void *) pwszHeader);
    free((void *) pwszTimings);

    return hr;
}


/*  ----------------------------------------------------------------------------
    main()
*/
int __cdecl
main(void)
{
    HRESULT hr = S_OK;
    OSVERSIONINFOA vi;
    MSXMLCommandLine cmdLine;
    MSXMLInfo msxmlInfo;
    TransformHelper transHelper;
    const WCHAR * pwszText = NULL;
    WCHAR rgwchInt[16];

    ChkSuccCOM(::CoInitialize(NULL));

    // Get stdin, stdout, stderr handles
    g_hStdIn = ::GetStdHandle(STD_INPUT_HANDLE);
    ChkWin(INVALID_HANDLE_VALUE != g_hStdIn);

    g_hStdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
    ChkWin(INVALID_HANDLE_VALUE != g_hStdOut);

    g_hStdErr = ::GetStdHandle(STD_ERROR_HANDLE);
    ChkWin(INVALID_HANDLE_VALUE != g_hStdErr);

    // Get the OS version we're running
    vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
    ChkWin(GetVersionExA(&vi));
    g_dwPlatformId = vi.dwPlatformId;

    // Get information about the version of MSXML in use
    ChkSucc(msxmlInfo.FindLatestMSXMLInfo());

    // Parse command line.  S_FALSE will be returned if usage message should be printed.
    hr = cmdLine.Parse();
    if (S_OK != hr)
    {
        PrintUsage(&msxmlInfo);
    }
    else
    {
        // If specific version was specified, override now
        if (cmdLine.GetMSXMLVersion() != MSXMLInfo::VERSION_NONE)
        {
            ChkSucc(msxmlInfo.FindMSXMLInfo(cmdLine.GetMSXMLVersion()));
        }

        // Perform transformation
        ChkSucc(transHelper.Transform(&msxmlInfo, &cmdLine));

        // Output timing information
        if (cmdLine.DoTimings())
        {
            ChkSucc(PrintTimings(&msxmlInfo, &transHelper));
        }
    }

Cleanup:

    if (FAILED(hr))
    {
        // Print exception text
        if (SUCCEEDED(Resources::FormatResourceMessage(MSXSL_E_CMDLINE_ERROR, &pwszText, Exception::GetContext(), _itow(hr, rgwchInt, 16), Exception::GetText(), NULL)))
        {
            WriteAsMBCS(g_hStdErr, L"\r\n");
            WriteAsMBCS(g_hStdErr, pwszText);
        }
    }

    free((void *) pwszText);
    ::CoUninitialize();

    return (S_OK != hr);
}


/*  ----------------------------------------------------------------------------
    mainCRTStartup()

    Entry point into the EXE.
*/
void __cdecl
mainCRTStartup(void)
{
    ::ExitProcess(main());
}


/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

