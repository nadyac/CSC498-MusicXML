/*
 * @(#)Util.cxx 1.0 10/6/2000
 * 
 * Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
 *
 * Implementation of utility classes.
 * 
 */

#include "core.hxx"
#pragma hdrstop

#include "util.hxx"


const WCHAR * Exception::s_pwszContext = NULL;
const WCHAR * Exception::s_pwszText = NULL;


/*  ----------------------------------------------------------------------------
    ParameterMgr()
*/
ParameterMgr::ParameterMgr()
{
    _pParams = NULL;
    _cParams = _cMaxParams = 0;
}


/*  ----------------------------------------------------------------------------
    ~ParameterMgr()
*/
ParameterMgr::~ParameterMgr()
{
    while (_cParams--)
    {
        ::SysFreeString(_pParams[_cParams]._bstrQName);
        ::SysFreeString(_pParams[_cParams]._bstrLocalName);
        ::SysFreeString(_pParams[_cParams]._bstrURI);
        ::VariantClear(&_pParams[_cParams]._vValue);
    }
    free(_pParams);
}


/*  ----------------------------------------------------------------------------
    AddParameter()

    Allocates a new Param structure and sets its name members.
*/
HRESULT
ParameterMgr::AddParameter(const WCHAR * pwsName, int cwchName, int cwchPrefix)
{
    HRESULT hr = S_OK;
    Param * pNewParams;
    Param * pParam;

    if (_cParams >= _cMaxParams)
    {
        // Allocate new array
        Assert(_cParams == _cMaxParams);
        _cMaxParams = (_cParams + 1) * 2;
        pNewParams = (Param *) malloc(_cMaxParams * sizeof(Param));
        ChkMem(pNewParams);

        if (_pParams)
        {
            // Copy old array to new
            memcpy(pNewParams, _pParams, _cParams * sizeof(Param));
            free(_pParams);
        }

        _pParams = pNewParams;
    }

    pParam = &_pParams[_cParams];

    pParam->_cwchPrefix = cwchPrefix;
    pParam->_bstrLocalName = NULL;
    pParam->_bstrURI = NULL;
    ::VariantInit(&pParam->_vValue);

    pParam->_bstrQName = ::SysAllocStringLen(pwsName, cwchName);
    ChkMem(pParam->_bstrQName);

    _cParams++;

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    SetParameterValue()

    Sets the vValue member of the last Param structure that was added.
*/
HRESULT
ParameterMgr::SetParameterValue(const WCHAR * pwsValue, int cwchValue)
{
    HRESULT hr = S_OK;
    VARIANT * pVar;
    Assert(_cParams);

    // Allocate and save parameter value
    pVar = &_pParams[_cParams - 1]._vValue;
    V_VT(pVar) = VT_BSTR;
    V_BSTR(pVar) = ::SysAllocStringLen(pwsValue, cwchValue);
    ChkMem(pVar);

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    ResolvePrefixes()

    Resolve all prefixes using the passed NamespaceMgr.
*/
HRESULT
ParameterMgr::ResolvePrefixes(NamespaceMgr * pNSMgr)
{
    HRESULT hr = S_OK;
    Param * pParam;
    int iParam;
    Assert(pNSMgr);

    pParam = _pParams;
    for (iParam = _cParams; iParam--; pParam++)
    {
        // Allocate local name BSTR
        Assert(!pParam->_bstrLocalName);

        ChkSucc(pNSMgr->ParseQName(
                            pParam->_bstrQName,
                            ::SysStringLen(pParam->_bstrQName),
                            pParam->_cwchPrefix,
                            &pParam->_bstrLocalName,
                            &pParam->_bstrURI));
    }

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    AddPrefix()

    Allocates a new NSDecl structure and sets its prefix member.
*/
HRESULT
NamespaceMgr::AddPrefix(const WCHAR * pwsPrefix, int cwchPrefix)
{
    HRESULT hr = S_OK;
    NSDecl * pNewNSDecl;

    if (_cNSDecl >= _cMaxNSDecl)
    {
        // Allocate new array
        Assert(_cNSDecl == _cMaxNSDecl);
        _cMaxNSDecl = (_cNSDecl + 1) * 2;
        pNewNSDecl = (NSDecl *) malloc(_cMaxNSDecl * sizeof(NSDecl));
        ChkMem(pNewNSDecl);

        if (_pNSDecl)
        {
            // Copy old array to new
            memcpy(pNewNSDecl, _pNSDecl, _cNSDecl * sizeof(NSDecl));
            free(_pNSDecl);
        }

        _pNSDecl = pNewNSDecl;
    }

    _pNSDecl[_cNSDecl]._pwsPrefix = pwsPrefix;
    _pNSDecl[_cNSDecl]._cwchPrefix = cwchPrefix;

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    SetURI()

    Sets the URI member of the last NSDecl structure that was added.
    Increments the number of complete NSDecl structures.
*/
void
NamespaceMgr::SetURI(const WCHAR * pwsURI, int cwchURI)
{
    Assert(_pNSDecl && _pNSDecl[_cNSDecl]._pwsPrefix);

    _pNSDecl[_cNSDecl]._pwsURI = pwsURI;
    _pNSDecl[_cNSDecl++]._cwchURI = cwchURI;
}


/*  ----------------------------------------------------------------------------
    LookupPrefix()

    Lookup the URI associated with a given prefix.

    Returns S_OK if the prefix was found -- *pbstrURI is set to the URI.  If
    the prefix maps to the NULL URI, then *pbstrURI will be set to NULL.

    Raises MSXSL_E_PREFIX_UNDEFINED error if the prefix was not found.
*/
HRESULT
NamespaceMgr::LookupPrefix(const WCHAR * pwsPrefix, int cwchPrefix, BSTR * pbstrURI)
{
    HRESULT hr = S_OK;
    NSDecl * pNSDecl;

    *pbstrURI = NULL;

    if (_cNSDecl)
    {
        pNSDecl = _pNSDecl + _cNSDecl;

        // Traverse list of prefix->URI pairs from most recent to least, looking for match
        while (--pNSDecl >= _pNSDecl)
        {
            if (cwchPrefix == pNSDecl->_cwchPrefix &&
                !memcmp(pwsPrefix, pNSDecl->_pwsPrefix, cwchPrefix * sizeof(WCHAR)))
            {
                // We found a match
                *pbstrURI = ::SysAllocStringLen(pNSDecl->_pwsURI, pNSDecl->_cwchURI);
                ChkMem(*pbstrURI);
                return S_OK;
            }
        }
    }

    // Empty prefix resolves to null URI
    if (cwchPrefix)
    {
        // Prefix could not be resolved
        const WCHAR * pwszPrefix = _wcsndup(pwsPrefix, cwchPrefix);
        hr = Exception::CreateFromResource(MSXSL_E_PREFIX_UNDEFINED, pwszPrefix, NULL);
        free((void *) pwszPrefix);
    }

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    ParseQName()

    Parse a QName and return its local name and URI parts.
    If cwchPrefix is -1, the prefix will be calculated here.
*/
HRESULT
NamespaceMgr::ParseQName(const WCHAR * pwsName, int cwchName, int cwchPrefix, BSTR * pbstrLocalName, BSTR * pbstrURI)
{
    HRESULT hr = S_OK;

    if (cwchPrefix == -1)
    {
        cwchPrefix = NamespaceMgr::FindPrefixLen(pwsName, cwchName);
    }

    if (cwchPrefix)
    {
        // Account for colon after prefix
        *pbstrLocalName = ::SysAllocStringLen(pwsName + cwchPrefix + 1, cwchName - cwchPrefix - 1);
    }
    else
    {
        *pbstrLocalName = ::SysAllocStringLen(pwsName, cwchName);
    }
    ChkMem(*pbstrLocalName);

    // Attempt to resolve prefix using xmlns declarations maintained by this class
    hr = LookupPrefix(pwsName, cwchPrefix, pbstrURI);
    if (FAILED(hr))
    {
        ::SysFreeString(*pbstrLocalName);
    }

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    FindPrefixLen()

    Find the length of a QName prefix (extends to first ':' character).
*/
int
NamespaceMgr::FindPrefixLen(const WCHAR * pws, int cwch)
{
    const WCHAR * pwszBegin = pws;

    while (cwch--)
    {
        if (*pws == L':')
            return pws - pwszBegin;

        pws++;
    }

    return 0;
}


/*  ----------------------------------------------------------------------------
    WriteAsMBCS()

    Write the passed MBCS to a file.
*/
HRESULT
WriteAsMBCS(HANDLE h, const char * psz)
{
    HRESULT hr = S_OK;
    DWORD cbWritten;

    ChkWin(::WriteFile(
                h,
                psz,
                strlen(psz),
                &cbWritten,
                NULL));
    Assert(strlen(psz) == cbWritten);

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    WriteAsMBCS()

    Convert the passed Unicode string to MBCS and write it to a file.
*/
HRESULT
WriteAsMBCS(HANDLE h, const WCHAR * pwsz)
{
    HRESULT hr = S_OK;
    StringConvert strconv;
    DWORD cbWritten;

    ChkSucc(strconv.ConvertToMBCS(pwsz, _wcslen(pwsz)));

    ChkWin(::WriteFile(
                h,
                strconv.GetMBCS(),
                strconv.GetMBCSLen(),
                &cbWritten,
                NULL));
    Assert(strconv.GetMBCSLen() == (int) cbWritten);

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    ConvertToMBCS()

    Convert the Unicode string pwsz to characters from the current MBCS code
    page.  Caller should call GetMBCS() to get the resulting MBCS string.
*/
HRESULT
StringConvert::ConvertToMBCS(const WCHAR * pwsz, int cwch)
{
    HRESULT hr = S_OK;
    int iLen = 0;

    if (cwch)
    {
        // Get required buffer size
        iLen = ::WideCharToMultiByte(
                    CP_ACP,
                    0,
                    pwsz,
                    cwch,
                    NULL,
                    0,
                    NULL,
                    NULL);
        ChkWin(iLen);
    }

    if (iLen < BUFFER_SIZE)
    {
        _psz = (char *) _rgbStatic;
    }
    else
    {
        if (!_pbDynamic || iLen >= _iDynamicLen)
        {
            free(_pbDynamic);
            _pbDynamic = (BYTE *) malloc(iLen + 1);
            ChkMem(_pbDynamic);
            _iDynamicLen = iLen;
        }
        _psz = (char *) _pbDynamic;
    }

    if (cwch)
    {
        // Perform conversion
        iLen = ::WideCharToMultiByte(
                    CP_ACP,
                    0,
                    pwsz,
                    cwch,
                    _psz,
                    iLen,
                    NULL,
                    NULL);
        ChkWin(iLen);

        // Adjust length
        if (_psz[iLen - 1] == 0)
        {
            iLen--;
        }
    }

    // Always null-terminate buffer
    _psz[iLen] = 0;
    _iLen = iLen;

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    ConvertToUnicode()

    Convert the MBCS string psz to Unicode.  Caller should call GetUnicode() to
    get the resulting Unicode string.
*/
HRESULT
StringConvert::ConvertToUnicode(const char * psz, int cb)
{
    HRESULT hr = S_OK;
    int iLen;

    // Assume each byte in psz converts to one Unicode character
    iLen = cb * sizeof(WCHAR *) + 2;

    if (iLen <= BUFFER_SIZE)
    {
        _pwsz = (WCHAR *) _rgbStatic;
    }
    else
    {
        if (!_pbDynamic || iLen > _iDynamicLen)
        {
            free(_pbDynamic);
            _pbDynamic = (BYTE *) malloc(iLen);
            ChkMem(_pbDynamic);
            _iDynamicLen = iLen;
        }
        _pwsz = (WCHAR *) _pbDynamic;
    }

    if (cb)
    {
        // Perform conversion
        iLen = ::MultiByteToWideChar(
                    CP_ACP,
                    0,
                    psz,
                    cb,
                    _pwsz,
                    iLen);
        ChkWin(iLen);

        // Adjust length
        if (_pwsz[iLen - 1] == 0)
        {
            iLen--;
        }
    }

    // Always null-terminate buffer
    _pwsz[iLen] = 0;
    _iLen = iLen;

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    AttachMBCS()

    Give caller ownership of converted MBCS string.  Caller must free *ppszOut
    using free.
*/
HRESULT
StringConvert::AttachMBCS(const char ** ppszOut)
{
    HRESULT hr = S_OK;

    if (_psz == (char *) _rgbStatic)
    {
        // Return copy of MBCS string
        *ppszOut = (const char *) _strndup((const char *) _rgbStatic, _iLen);
        ChkMem(*ppszOut);
    }
    else
    {
        // Give caller ownership of dynamically allocated string
        *ppszOut = (const char *) _pbDynamic;
        _pbDynamic = NULL;
    }

    _psz = NULL;

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    AttachUnicode()

    Give caller ownership of converted Unicode string.  Caller must free *ppwszOut
    using free.
*/
HRESULT
StringConvert::AttachUnicode(const WCHAR ** ppwszOut)
{
    HRESULT hr = S_OK;

    if (_pwsz == (WCHAR *) _rgbStatic)
    {
        // Return copy of Unicode string
        *ppwszOut = (const WCHAR *) _wcsndup((const WCHAR *) _rgbStatic, _iLen);
        ChkMem(*ppwszOut);
    }
    else
    {
        // Give caller ownership of dynamically allocated string
        *ppwszOut = (const WCHAR *) _pbDynamic;
        _pbDynamic = NULL;
    }

    _pwsz = NULL;

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    FormatResourceMessage()

    Return a formatted string created from a message resource in *ppwszOut.
    *ppwszOut must be freed by the caller with a call to free().
*/
HRESULT
Resources::FormatResourceMessage(ID resid, const WCHAR ** ppwszOut, const WCHAR * pwszArg, ...)
{
    va_list va;
    va_start(va, pwszArg);

    return FormatMessage(resid, ppwszOut, FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE, pwszArg, &va);
}


/*  ----------------------------------------------------------------------------
    FormatResourceMessageList()

    As above, except that variable-length parameter list is passed as a va_list *.
*/
HRESULT
Resources::FormatResourceMessageList(ID resid, const WCHAR ** ppwszOut, const WCHAR * pwszArg, va_list * pva)
{
    return FormatMessage(resid, ppwszOut, FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_HMODULE, pwszArg, pva);
}


/*  ----------------------------------------------------------------------------
    FormatSystemMessage()

    Return a formatted system message in *ppwszOut.  *ppwszOut must be freed by
    the caller with a call to free().
*/
HRESULT
Resources::FormatSystemMessage(ID resid, const WCHAR ** ppwszOut)
{
    return FormatMessage(resid, ppwszOut, FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, NULL);
}


/*  ----------------------------------------------------------------------------
    FormatMessage()

    Return a formatted message in *ppwszOut.  *ppwszOut must be freed by the
    caller with a call to free().
*/
HRESULT
Resources::FormatMessage(ID resid, const WCHAR ** ppwszOut, DWORD dwFlags, const WCHAR * pwszArg, va_list * pva)
{
    HRESULT hr = S_OK;
    StringConvert strconv;
    va_list vaMarker;
    const WCHAR * pwsz;
    void ** ppvArgs = NULL;
    int cArgs = 0;
    char * pszOut = NULL;
    DWORD dwRet;
    bool fIsNT;

    fIsNT = (g_dwPlatformId != VER_PLATFORM_WIN32_WINDOWS);

    if (pwszArg)
    {
        // Count number of argument strings
        Assert(pva);
        vaMarker = *pva;
        pwsz = pwszArg;
        while (pwsz)
        {
            cArgs++;
            pwsz = va_arg(vaMarker, const WCHAR *);
        }

        // Build argument array
        ppvArgs = (void **) malloc(cArgs * sizeof(void *));
        ChkMem(ppvArgs);

        cArgs = 0;
        pwsz = pwszArg;
        while (pwsz)
        {
            if (fIsNT)
            {
                // Point directly at WCHAR strings
                ppvArgs[cArgs++] = (void *) pwsz;
            }
            else
            {
                // Convert arguments to MBCS if this is not NT
                ChkSucc(strconv.ConvertToMBCS(pwsz, _wcslen(pwsz)));
                ChkSucc(strconv.AttachMBCS((const char **) &ppvArgs[cArgs++]));
            }
            pwsz = va_arg(*pva, const WCHAR *);
        }
    }

    if (fIsNT)
    {
        dwRet = ::FormatMessageW(
                    dwFlags | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                    ::GetModuleHandle(NULL),
                    resid,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPWSTR) ppwszOut,
                    0,
                    (va_list *) ppvArgs);
    }
    else
    {
        // Windows 95/98 does not implement the ::FormatMessageW function
        dwRet = ::FormatMessageA(
                    dwFlags | FORMAT_MESSAGE_ARGUMENT_ARRAY,
                    ::GetModuleHandle(NULL),
                    resid,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPSTR) &pszOut,
                    0,
                    (va_list *) ppvArgs);

        if (!dwRet)
            goto Cleanup;

        // Convert ANSI string to Unicode
        ChkSucc(strconv.ConvertToUnicode(pszOut, strlen(pszOut)));
        ChkSucc(strconv.AttachUnicode(ppwszOut));
    }

Cleanup:
    if (!dwRet && resid != MSXSL_E_SYSTEM_ERROR)
    {
        // Message ID could not be resolved to text
        WCHAR rgwchID[16];
        hr = FormatResourceMessage(MSXSL_E_SYSTEM_ERROR, ppwszOut, _itow(resid, rgwchID, 16), NULL);
    }

    if (ppvArgs)
    {
        if (!fIsNT)
        {
            while (cArgs--)
            {
                free(ppvArgs[cArgs]);
            }
        }
        free(ppvArgs);
    }

    free(pszOut);

    return hr;
}


/*  ----------------------------------------------------------------------------
    SetContext()

    Set the context text of the currently pending exception with a formatted
    message resource.
*/
void
Exception::SetContext(Resources::ID resid, const WCHAR * pwszArg, ...)
{
    HRESULT hr;
    va_list va;
    va_start(va, pwszArg);

    // Only set context string if one hasn't been set already
    if (!s_pwszContext)
    {
        ChkSucc(Resources::FormatResourceMessageList(resid, &s_pwszContext, pwszArg, &va));
    }

Cleanup:
    return;
}


/*  ----------------------------------------------------------------------------
    Clear()

    Clear pending exception, if one exists.
*/
void
Exception::Clear()
{
    free((void *) s_pwszText);
    s_pwszText = NULL;
    free((void *) s_pwszContext);
    s_pwszContext = NULL;
}


/*  ----------------------------------------------------------------------------
    Create()

    Create an exception directly from text.
*/
HRESULT
Exception::CreateFromText(HRESULT hr, const WCHAR * pwszText)
{
    // Release previous exception
    Clear();

    // Create new exception text
    s_pwszText = _wcsdup(pwszText);

    return hr;
}


/*  ----------------------------------------------------------------------------
    CreateFromCOMError()

    Create an exception from the current thread's IErrorInfo.  It it does not
    exist, get the system error for hr.
*/
HRESULT
Exception::CreateFromCOMError(HRESULT hrCOM)
{
    HRESULT hr = S_OK;
    IErrorInfo * pei = NULL;
    BSTR bstrDesc = NULL;

    // Release previous exception
    Clear();

    // Get description from COM IErrorInfo
    ChkSucc(::GetErrorInfo(0, &pei));
    if (pei)
    {
        ChkSucc(pei->GetDescription(&bstrDesc));
        s_pwszText = _wcsndup(bstrDesc, ::SysStringLen(bstrDesc));
    }
    else
    {
        hr = Exception::CreateFromSystemHR(hrCOM);
    }

Cleanup:
    SafeRelease(pei);
    ::SysFreeString(bstrDesc);

    return SUCCEEDED(hr) ? hrCOM : hr;
}


/*  ----------------------------------------------------------------------------
    CreateFromSystemHR()

    Create an exception for a system HRESULT.
*/
HRESULT
Exception::CreateFromSystemHR(HRESULT hrSystem)
{
    HRESULT hr = S_OK;

    // Release previous exception
    Clear();

    // Get the text of the exception from the system message table resource
    ChkSucc(Resources::FormatSystemMessage(hrSystem, &s_pwszText));

Cleanup:
    return SUCCEEDED(hr) ? hrSystem : hr;
}


/*  ----------------------------------------------------------------------------
    CreateFromWinError()

    Create an exception for a Win32 error code.
*/
HRESULT
Exception::CreateFromWinError(DWORD dwError)
{
    HRESULT hr = S_OK;

    // Release previous exception
    Clear();

    // Get the text of the exception from the system message table resource
    ChkSucc(Resources::FormatSystemMessage(dwError, &s_pwszText));

Cleanup:
    return SUCCEEDED(hr) ? HRESULT_FROM_WIN32(dwError) : hr;
}


/*  ----------------------------------------------------------------------------
    CreateFromResource()

    Create an exception from a messages resource identified by resid.
*/
HRESULT
Exception::CreateFromResource(Resources::ID resid, const WCHAR * pwszArg, ...)
{
    HRESULT hr = S_OK;
    va_list va;
    va_start(va, pwszArg);

    // Release previous exception
    Clear();

    // Get the text of the exception from the message table resource
    ChkSucc(Resources::FormatResourceMessageList(resid, &s_pwszText, pwszArg, &va));

Cleanup:
    return SUCCEEDED(hr) ? resid : hr;
}


/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


