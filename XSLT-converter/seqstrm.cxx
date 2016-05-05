/*
 * @(#)InStrm.cxx 1.0 10/19/2000
 * 
 * Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
 *
 * Implementation of FileInputStream and FileOutputStream classes.
 * 
 */

#include "core.hxx"
#pragma hdrstop

#include "seqstrm.hxx"


/*  ----------------------------------------------------------------------------
    Init()

    Initialize class for reading the file input stream referenced by HANDLE h.
*/
HRESULT
FileInputStream::Init(HANDLE h)
{
    HRESULT hr = S_OK;
    Assert(h);

    _h = h;
    _cbAvail = 0;
    _fEOF = false;

    // If input is a character file (e.g. console)
    _fConsole = (FILE_TYPE_CHAR == ::GetFileType(h));

    if (!_pBuffer)
    {
        _pBuffer = (BYTE *) malloc(BUFFER_SIZE);
        ChkMem(_pBuffer);
    }

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    Read()

    Implement ISequentialStream::Read on top of input stream HANDLE.
*/
HRESULT STDMETHODCALLTYPE
FileInputStream::Read(void * pv, ULONG cb, ULONG * pcbRead)
{
    HRESULT hr = S_OK;
    ULONG cbNeed;
    DWORD dwError;
    Assert(pcbRead);

    cbNeed = cb;

    while (cbNeed)
    {
        if (!_cbAvail)
        {
            if (_fEOF)
            {
                // Input is exhausted
                break;
            }

            // Need to refill buffer from input stream
            _pAvail = _pBuffer;
            if (!::ReadFile(
                        _h,
                        _pBuffer,
                        BUFFER_SIZE,
                        &_cbAvail,
                        NULL))
            {
                dwError = ::GetLastError();
                if (dwError != ERROR_BROKEN_PIPE)
                {
                    // Fatal error
                    hr = Exception::CreateFromWinError(dwError);
                    goto Cleanup;
                }

                // According to documentation, ERROR_BROKEN_PIPE is how pipe
                // writer signals it's finished.
                Assert(_cbAvail == 0);
            }

            if (_fConsole && *((char *) _pBuffer) == 0x1a)
            {
                // Ctrl-z was typed at the console, so suppress it and end input
                _cbAvail = 0;
            }

            if (_cbAvail == 0)
            {
                // Input is exhausted
                _fEOF = true;
                break;
            }
        }

        if (cbNeed < _cbAvail)
        {
            memcpy(pv, _pAvail, cbNeed);
            _pAvail += cbNeed;
            _cbAvail -= cbNeed;
            cbNeed = 0;
        }
        else
        {
            memcpy(pv, _pAvail, _cbAvail);
            pv = (BYTE *) pv + _cbAvail;
            cbNeed -= _cbAvail;
            _cbAvail = 0;
        }
    }

    *pcbRead = cb - cbNeed;

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    Init()

    Open local file named pwszFileName for writing.
*/
HRESULT
FileOutputStream::Init(const WCHAR * pwszFileName)
{
    HRESULT hr = S_OK;

    if (g_dwPlatformId != VER_PLATFORM_WIN32_WINDOWS)
    {
        _h =::CreateFileW(
                pwszFileName,
                GENERIC_WRITE,
                0,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
    }
    else
    {
        // Windows 95/98 do not support ::CreateFileW
        StringConvert strconv;

        ChkSucc(strconv.ConvertToMBCS(pwszFileName, _wcslen(pwszFileName)));

        _h = ::CreateFileA(
                strconv.GetMBCS(),
                GENERIC_WRITE,
                0,
                NULL,
                CREATE_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
    }
    ChkWin(INVALID_HANDLE_VALUE != _h);

    _fClose = true;

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    Write()

    Implement ISequentialStream::Write by forwarding calls to WriteFile.
*/
HRESULT STDMETHODCALLTYPE
FileOutputStream::Write(void const * pv, ULONG cb, ULONG * pcbWritten)
{
    HRESULT hr = S_OK;
    Assert(_h);

    ChkWin(::WriteFile(
                _h,
                pv,
                cb,
                pcbWritten,
                NULL));
    Assert(cb == *pcbWritten);

Cleanup:
    return hr;
}


/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

