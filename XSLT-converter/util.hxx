/*
 * @(#)Util.hxx 1.0 9/28/2000
 * 
 * Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
 *
 * Utility classes.
 * 
 */

#ifndef UTIL_HXX
#define UTIL_HXX


HRESULT WriteAsMBCS(HANDLE h, const char * psz);
HRESULT WriteAsMBCS(HANDLE h, const WCHAR * pwsz);


/*  ----------------------------------------------------------------------------
    NamespaceMgr()

    Helper class for managing namespace declarations specified on the command
    line.  These declarations are used to resolve QNames in parameters and
    in the start mode name.
*/
class NamespaceMgr
{
public:
    NamespaceMgr() {_pNSDecl = NULL; _cNSDecl = _cMaxNSDecl = 0;}
    ~NamespaceMgr() {free((void *) _pNSDecl);}

    // Allocates a new NSDecl structure and sets its prefix member
    HRESULT AddPrefix(const WCHAR * pwsPrefix, int cwchPrefix);

    // Sets the URI member of the last NSDecl structure that was added
    void SetURI(const WCHAR * pwsURI, int cwchURI);

    // Lookup the URI associated with a given prefix.  Raises error if not found.
    HRESULT LookupPrefix(const WCHAR * pwsPrefix, int cwchPrefix, BSTR * pbstrURI);

    // Parse a QName and return its local name and URI
    HRESULT ParseQName(const WCHAR * pwsName, int cwchName, int cwchPrefix, BSTR * pbstrLocalName, BSTR * pbstrURI);

    // Find the length of a name prefix (up to first colon character)
    static int FindPrefixLen(const WCHAR * pwsz, int cwch);

protected:

    struct NSDecl
    {
        const WCHAR * _pwsPrefix;
        int _cwchPrefix;
        const WCHAR * _pwsURI;
        int _cwchURI;
    };

private:
    NSDecl *    _pNSDecl;
    int         _cNSDecl;
    int         _cMaxNSDecl;
};


/*  ----------------------------------------------------------------------------
    ParameterMgr()

    Helper class for managing parameters passed via command line.
*/
class ParameterMgr
{
public:
    ParameterMgr();
    ~ParameterMgr();

    // Allocates a new Param structure and sets its name members
    HRESULT AddParameter(const WCHAR * pwsName, int cwchName, int cwchPrefix);

    // Sets the vValue member of the last Param structure that was added
    HRESULT SetParameterValue(const WCHAR * pwsValue, int cwchValue);

    // Resolve all prefixes using the passed NamespaceMgr
    HRESULT ResolvePrefixes(NamespaceMgr * pNSMgr);

    // Get pointer to the parameter QName
    BSTR GetQName(int iIndex) {Assert(iIndex < _cParams); return _pParams[iIndex]._bstrQName;}

    // Get pointer to the local part of a parameter's name
    BSTR GetLocalName(int iIndex) {Assert(iIndex < _cParams); return _pParams[iIndex]._bstrLocalName;}

    // Get pointer to the URI part of a parameter's name
    BSTR GetURI(int iIndex) {Assert(iIndex < _cParams); return _pParams[iIndex]._bstrURI;}

    // Return number of parameters currently managed
    int GetParameterCount() {return _cParams;}

    // Get pointer to the value of a parameter
    VARIANT * GetValue(int iIndex) {Assert(iIndex < _cParams); return &_pParams[iIndex]._vValue;}

protected:
    struct Param
    {
        int _cwchPrefix;
        BSTR _bstrQName;
        BSTR _bstrLocalName;
        BSTR _bstrURI;
        VARIANT _vValue;
    };

private:
    Param * _pParams;
    int     _cParams;
    int     _cMaxParams;
};


/*  ----------------------------------------------------------------------------
    StringConvert()

    Helper class for converting to and from Unicode.
*/
class StringConvert
{
public:
    StringConvert() {_pbDynamic = NULL;}
    ~StringConvert() {free(_pbDynamic);}

    HRESULT ConvertToUnicode(const char * psz, int cch);
    HRESULT ConvertToMBCS(const WCHAR * pwsz, int cwch);

    // Only valid after ConvertToMBCS has been called and until next Convert call
    const char * GetMBCS() {return (const char *) _psz;}
    int GetMBCSLen() {return _iLen;}

    // Only valid after ConvertToUnicode has been called and until next Convert call
    const WCHAR * GetUnicode() {return (const WCHAR *) _pwsz;}
    int GetUnicodeLen() {return _iLen;}

    // Give caller ownership of converted MBCS string.  Caller must free *ppszOut using free.
    HRESULT AttachMBCS(const char ** ppszOut);

    // Give caller ownership of converted Unicode string.  Caller must free *ppwszOut using free.
    HRESULT AttachUnicode(const WCHAR ** ppwszOut);

private:
#ifdef _DEBUG
    enum {BUFFER_SIZE = 16};
#else
    enum {BUFFER_SIZE = 256};
#endif

    char * _psz;
    WCHAR * _pwsz;
    int _iLen;

    BYTE _rgbStatic[BUFFER_SIZE];
    BYTE * _pbDynamic;
    int _iDynamicLen;
};


/*  ----------------------------------------------------------------------------
    Resources()

    Helper class for managing resources linked into this executable.
*/
class Resources
{
public:
    typedef DWORD ID;

    static HRESULT FormatResourceMessage(ID resid, const WCHAR ** ppwszOut, const WCHAR * pwszArg, ...);
    static HRESULT FormatResourceMessageList(ID resid, const WCHAR ** ppwszOut, const WCHAR * pwszArg, va_list * pva);
    static HRESULT FormatSystemMessage(ID resid, const WCHAR ** ppwszOut);

protected:
    static HRESULT FormatMessage(ID resid, const WCHAR ** ppwszOut, DWORD dwFlags, const WCHAR * pwszArg, va_list * pva);
};


/*  ----------------------------------------------------------------------------
    Exception()

    Helper class for creating exceptions.

    NOTE: This class is currently safe only for single-threaded applications.
          It could be extended to use TLS to handle multiple concurrent threads.
*/
class Exception
{
public:
    static HRESULT CreateFromText(HRESULT hr, const WCHAR * pwszText);
    static HRESULT CreateFromCOMError(HRESULT hrCOM);
    static HRESULT CreateFromSystemHR(HRESULT hrSystem);
    static HRESULT CreateFromWinError(DWORD dwError);
    static HRESULT CreateFromResource(Resources::ID resid, const WCHAR * pwszArg, ...);

    static void SetContext(Resources::ID resid, const WCHAR * pwszArg, ...);

    static void Clear();

    static const WCHAR * GetContext() {return s_pwszContext ? s_pwszContext : L"";}
    static const WCHAR * GetText() {return s_pwszText ? s_pwszText : L"";}

private:
    static const WCHAR * s_pwszContext;
    static const WCHAR * s_pwszText;
};


/*  ----------------------------------------------------------------------------
    StackUnknown()

    Implementation of IUnknown for objects that are meant to be created on the
    stack.  Because of this, all external references to this object must be
    released before this object is destructed.
*/
template <class Base>
class NOVTABLE StackUnknown : public Base
{
public:

    /////////////////////////////////////////////////////////////////////////////
    // IUnknown
    //
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppv);
    virtual ULONG STDMETHODCALLTYPE AddRef() {return 1;}
    virtual ULONG STDMETHODCALLTYPE Release() {return 1;}
};


/*  ----------------------------------------------------------------------------
    QueryInterface()
*/
template <class Base>
HRESULT STDMETHODCALLTYPE
StackUnknown<Base>::QueryInterface(REFIID riid, void ** ppv)
{
    if (riid == __uuidof(Base) || riid == __uuidof(IUnknown))
    {
        // No need to AddRef since this class will only be created on the stack
        *ppv = this;
        return S_OK;
    }

    *ppv = NULL;
    return E_NOINTERFACE;
}


#endif UTIL_HXX

/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


