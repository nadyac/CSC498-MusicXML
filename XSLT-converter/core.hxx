/*
 * @(#)Core.hxx 1.0 9/28/2000
 * 
 * Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
 *
 * Precompiled header.
 * 
 */

#include "windows.h"
#include "wtypes.h"
#include "limits.h"
#include "oleauto.h"
#include "wininet.h"
#include "msxml2.h"
#include "resource\messages.h"

#ifdef _DEBUG
void __cdecl _assert(const char * psz, const char * pszFile, unsigned iLine);
#define Assert(pred) (void)((pred) || (_assert(#pred, __FILE__, __LINE__), 0))
#else
#define Assert(pred)
#endif

#define ChkSucc(ret) {hr = (ret); if (FAILED(hr)) goto Cleanup;}
#define ChkOk(ret) {hr = (ret); if (S_OK != hr) goto Cleanup;}
#define ChkSuccCOM(ret) {hr = (ret); if (FAILED(hr)) {Exception::CreateFromCOMError(hr); goto Cleanup;}}
#define ChkMem(ret) {if (!(ret)) {hr = Exception::CreateFromSystemHR(E_OUTOFMEMORY); goto Cleanup;}}
#define ChkWin(ret) {if (!(ret)) {hr = Exception::CreateFromWinError(::GetLastError()); goto Cleanup;}}
#define ChkReg(ret) {long retReg = (ret); if (retReg != NO_ERROR) {hr = Exception::CreateFromWinError(retReg); goto Cleanup;}}
#define SafeRelease(iface) {if (iface) iface->Release();}
#define SafeClose(hndl) {if (hndl) ::CloseHandle(hndl);}
#define SafeRegClose(hkey) {if (hkey) ::RegCloseKey(hkey);}

extern HANDLE g_hStdIn;
extern HANDLE g_hStdOut;
extern HANDLE g_hStdErr;

#define EOF     (-1)

#define NOVTABLE __declspec(novtable)

#define _wcslen(s) lstrlenW(s)

int __cdecl _wcsnicmp(const WCHAR * pwsz1, const WCHAR * pwsz2, size_t cwch);

char * __cdecl _strndup(const char * psSrc, int cchSrc);
inline char * __cdecl strdup(const char * pszSrc) {return _strndup(pszSrc, strlen(pszSrc));}

WCHAR * __cdecl _wcsndup(const WCHAR * pwsSrc, int cwchSrc);
inline WCHAR * __cdecl _wcsdup(const WCHAR * pwszSrc) {return _wcsndup(pwszSrc, _wcslen(pwszSrc));}

extern DWORD g_dwPlatformId;


#include "util.hxx"


/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

