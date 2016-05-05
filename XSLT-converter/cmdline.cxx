/*
 * @(#)CmdLine.cxx 1.0 9/28/2000
 * 
 * Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
 *
 * Implementation of ParseCommandLine and MSXMLCommandLine classes.
 * 
 */

#include "core.hxx"
#pragma hdrstop

#include "cmdline.hxx"


/*  ----------------------------------------------------------------------------
    SkipWhitespace()

    Return a pointer to the next non-whitespace character in pwsz.
*/
const WCHAR *
ParseCommandLine::SkipWhitespace(const WCHAR * pwsz)
{
    while (true)
    {
        switch (*pwsz)
        {
        case 0x9:
        case 0xA:
        case 0xD:
        case 0x20:
            pwsz++;
            continue;
        }
        break;
    }

    return pwsz;
}


/*  ----------------------------------------------------------------------------
    SkipNonWhitespace()

    Return a pointer to the next whitespace or EOS character in pwsz.
*/
const WCHAR *
ParseCommandLine::SkipNonWhitespace(const WCHAR * pwsz)
{
    while (true)
    {
        switch (*pwsz)
        {
        case 0:
        case 0x9:
        case 0xA:
        case 0xD:
        case 0x20:
            break;

        default:
            pwsz++;
            continue;
        }
        break;
    }

    return pwsz;
}


/*  ----------------------------------------------------------------------------
    IsWhitespaceChar()

    If the specified character is a whitespace character, return true.
*/
bool
ParseCommandLine::IsWhitespaceChar(WCHAR wch)
{
    switch (wch)
    {
    case 0x9:
    case 0xA:
    case 0xD:
    case 0x20:
        return true;
    }

    return false;
}


/*  ----------------------------------------------------------------------------
    Parse()

    Parses a command line string of the form:

    <filename> ([-switch] | [option] | ["option"])*
*/
HRESULT
ParseCommandLine::Parse(const WCHAR * pwszCmdLine)
{
    HRESULT hr = S_OK;
    const WCHAR * pwszStart;
    WCHAR wchQuote;
    TokenType toktype = TokFileName;

    ChkSucc(BeginParsing());

    while (true)
    {
        wchQuote = (WCHAR) -1;
        pwszStart = pwszCmdLine = SkipWhitespace(pwszCmdLine);

        if (!*pwszStart)
            break;

        switch (*pwszCmdLine)
        {
        case L'\'':
        case L'"':
            // Option is quoted, so find ending quote
            wchQuote = *pwszCmdLine;
            pwszStart = ++pwszCmdLine;
    
            while (*pwszCmdLine && (*pwszCmdLine != wchQuote))
                pwszCmdLine++;

            if (toktype == TokOption)
                toktype = TokQuotedOption;
            break;

        case L'-':
        case L'/':
            Assert(toktype != TokFileName);
            toktype = TokSwitch;
            pwszCmdLine++;

            // Fall through

        default:
            // A regular (non-quoted) option
            pwszStart = pwszCmdLine;
            pwszCmdLine = SkipNonWhitespace(pwszCmdLine);
            break;
        }

        // Notify derived class that a token was parsed.
        // If S_OK is not returned, quit parsing (even S_FALSE, etc. should cause this)
        ChkOk(ParsedToken(pwszStart, &pwszCmdLine, toktype));

        if (wchQuote == *pwszCmdLine)
            pwszCmdLine++;

        toktype = TokOption;
    }

    ChkSucc(EndParsing());

Cleanup:
    if (FAILED(hr))
    {
        // Add context information to the exception
        Exception::SetContext(MSXSL_E_CMDLINE_CTXT, NULL);
    }

    return hr;
}


/*  ----------------------------------------------------------------------------
    MakeErrorString()

    Creates a zero-terminated error string from a WCHAR * and a length in chars.
*/
const WCHAR *
MSXMLCommandLine::MakeErrorString(const WCHAR * pws, int cwch)
{
    // Free any previous error string
    free((void *) _pwszError);

    // Create error string
    _pwszError = _wcsndup(pws, cwch);

    return _pwszError ? _pwszError : L"";
}


/*  ----------------------------------------------------------------------------
    ParseName()

    Parse the name part of a name=value pair.  If the name begins with "xmlns",
    then the pair is a namespace declaration.  Otherwise, it is a named parameter.
*/
HRESULT
MSXMLCommandLine::ParseName(const WCHAR * pwsName, int cwchName)
{
    HRESULT hr = S_OK;
    int cwchPrefix;

    // Find length of prefix
    cwchPrefix = NamespaceMgr::FindPrefixLen(pwsName, cwchName);

    if ((cwchName == 5 || cwchPrefix == 5) && !memcmp(pwsName, L"xmlns", 5 * sizeof(WCHAR)))
    {
        // Namespace declaration
        if (cwchPrefix)
        {
            Assert(pwsName[5] == L':');
            ChkSucc(_nsMgr.AddPrefix(pwsName + 6, cwchName - 6));
        }
        else
        {
            // Default namespace declaration
            ChkSucc(_nsMgr.AddPrefix(L"", 0));
        }
        _stateParse = StateNSEquals;
    }
    else
    {
        // Named parameter
        ChkSucc(_paramMgr.AddParameter(pwsName, cwchName, cwchPrefix));
        _stateParse = StateParamEquals;
    }

    // Save name in case error occurs later
    _pwsName = pwsName;
    _cwchName = cwchName;

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    BeginParsing()

    Called by base class before parsing begins.
*/
HRESULT
MSXMLCommandLine::BeginParsing()
{
    _pwsInput = _pwsOutput = _pwsStylesheet = _pwsStartMode = NULL;
    _cwchInput = _cwchOutput = _cwchStylesheet = _cwchStartMode = 0;
    _fStripWS = _fValidate = _fTimings = _fStyleFromPI = false;
    _fResolveExt = true;
    _stateParse = StateOption;
    _stateOption = StateInput;
    _version = MSXMLInfo::VERSION_NONE;

    return S_OK;
}


/*  ----------------------------------------------------------------------------
    BeginParsing()

    Called by base class each time a command line token is parsed.

    Token begins with character *pws and includes all characters up to but not
    including **ppwsEnd.

    The base class tokens are coarse-grained.  This derived class may decide
    that the incoming token is actually composed of several sub-tokens (e.g.
    in case of parameters), and signals this by returning the actual end of the
    pws token in *ppwsEnd.
*/
HRESULT
MSXMLCommandLine::ParsedToken(const WCHAR * pws, const WCHAR ** ppwsEnd, TokenType type)
{
    HRESULT hr = S_OK;
    int cwch;
    const WCHAR * pwsName;
    Assert(pws && ppwsEnd && *ppwsEnd);

    cwch = *ppwsEnd - pws;

    switch (type)
    {
    case TokFileName:
        Assert(_stateParse == StateOption);
        break;

    case TokSwitch:
        if (_stateParse >= StateOutput)
        {
            return Exception::CreateFromResource(MSXSL_E_UNKNOWN_OPTION, MakeErrorString(pws, cwch), NULL);
        }

        switch (cwch)
        {
        case 0:
            switch (_stateOption)
            {
            case StateInput:
                // '-' or '/' character in place of input name means load from stdin
                _stateOption = _fStyleFromPI ? StateParam : StateStylesheet;
                break;

            case StateStylesheet:
                // '-' or '/' character in place of stylesheet name means load from stdin
                Assert(!_fStyleFromPI);
                _stateOption = StateParam;
                break;

            case StateParam:
                // '-' or '/' switch specified with no switch text
                return Exception::CreateFromResource(MSXSL_E_UNKNOWN_OPTION, MakeErrorString(pws, cwch), NULL);
            }
            break;

        case 1:
            switch (*pws)
            {
            case L'o':
            case L'O':
                _stateParse = StateOutput;
                break;

            case L'm':
            case L'M':
                _stateParse = StateMode;
                break;

            case L'v':
            case L'V':
                _fValidate = true;
                break;

            case L't':
            case L'T':
                _fTimings = true;
                break;

            case L'u':
            case L'U':
                _stateParse = StateVersion;
                break;

            case L'?':
                return MSXSL_MSG_USAGE;

            default:
                return Exception::CreateFromResource(MSXSL_E_UNKNOWN_OPTION, MakeErrorString(pws, cwch), NULL);
            }
            break;

        case 2:
            if (!_wcsnicmp(pws, L"xe", 2))
            {
                _fResolveExt = false;
                break;
            }
            else if (!_wcsnicmp(pws, L"xw", 2))
            {
                _fStripWS = true;
                break;
            }
            else if (!_wcsnicmp(pws, L"pi", 2))
            {
                switch (_stateOption)
                {
                case StateStylesheet:
                    _stateOption = StateParam;
                    break;

                case StateParam:
                    return Exception::CreateFromResource(MSXSL_E_PI_CONFLICT, NULL);
                }
                _fStyleFromPI = true;
                break;
            }

            // Fall through

        default:
            return Exception::CreateFromResource(MSXSL_E_UNKNOWN_OPTION, MakeErrorString(pws, cwch), NULL);
        }

        break;

    case TokOption:
    case TokQuotedOption:

        switch (_stateParse)
        {
        case StateOption:

            switch (_stateOption)
            {
            case StateInput:
                // This must be the input filename
                _pwsInput = pws;
                _cwchInput = cwch;
                _stateOption = _fStyleFromPI ? StateParam : StateStylesheet;
                break;

            case StateStylesheet:
                // This must be the stylesheet filename
                Assert(!_fStyleFromPI);
                _pwsStylesheet = pws;
                _cwchStylesheet = cwch;
                _stateOption = StateParam;
                break;

            default:
                // This must be a parameter or a namespace declaration
                if (type == TokOption)
                {
                    // Look for '=' character
                    for (pwsName = pws; pwsName < *ppwsEnd; pwsName++)
                    {
                        if (*pwsName == L'=')
                        {
                            break;
                        }
                    }

                    if (pwsName == pws)
                        return Exception::CreateFromResource(MSXSL_E_MISSING_NAME, NULL);
                }
                else
                {
                    // Quoted option, so treat as opaque name
                    pwsName = *ppwsEnd;
                }

                // Add a new parameter
                ChkSucc(ParseName(pws, pwsName - pws));

                // Name token ends at pwsName
                *ppwsEnd = pwsName;
            }
            break;

        case StateOutput:
            _pwsOutput = pws;
            _cwchOutput = cwch;
            _stateParse = StateOption;
            break;

        case StateMode:
            _pwsStartMode = pws;
            _cwchStartMode = cwch;
            _stateParse = StateOption;
            break;

        case StateVersion:
            // Version may be 2.6, 3.0, or 4.0
            if (!memcmp(pws, L"4.0", 3 * sizeof(WCHAR)))
            {
                _version = MSXMLInfo::VERSION_40;
            }
            else if (!memcmp(pws, L"3.0", 3 * sizeof(WCHAR)))
            {
                _version = MSXMLInfo::VERSION_30;
            }
            else if (!memcmp(pws, L"2.6", 3 * sizeof(WCHAR)))
            {
                _version = MSXMLInfo::VERSION_26;
            }
            else
            {
                return Exception::CreateFromResource(MSXSL_E_UNKNOWN_VERSION, NULL);
            }
            _stateParse = StateOption;
            break;

        case StateNSEquals:
        case StateParamEquals:
            if (*pws != L'=' || !cwch || (cwch > 1 && type == TokQuotedOption))
                return EndParsing();

            _stateParse = (_stateParse == StateNSEquals) ? StateURI : StateParamValue;

            // Equals token ends at pws + 1
            *ppwsEnd = pws + 1;
            break;

        case StateURI:
            _nsMgr.SetURI(pws, cwch);
            _stateParse = StateOption;
            break;

        case StateParamValue:
            _paramMgr.SetParameterValue(pws, cwch);
            _stateParse = StateOption;
            break;
        }

        break;
    }

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    EndParsing()

    Called by base class after parsing has finished.
*/
HRESULT
MSXMLCommandLine::EndParsing()
{
    HRESULT hr = S_OK;

    if (_stateOption == StateInput)
    {
        return Exception::CreateFromResource(MSXSL_E_MISSING_SOURCE, NULL);
    }
    else if (_stateOption == StateStylesheet)
    {
        Assert(!_fStyleFromPI);
        return Exception::CreateFromResource(MSXSL_E_MISSING_STYLESHEET, NULL);
    }
    else if (_stateParse == StateOutput)
    {
        return Exception::CreateFromResource(MSXSL_E_MISSING_OUTPUT, NULL);
    }
    else if (_stateParse == StateMode)
    {
        return Exception::CreateFromResource(MSXSL_E_MISSING_MODE, NULL);
    }
    else if (_stateParse == StateVersion)
    {
        return Exception::CreateFromResource(MSXSL_E_UNKNOWN_VERSION, NULL);
    }
    else if (_stateParse == StateParamEquals)
    {
        return Exception::CreateFromResource(MSXSL_E_MISSING_PARAM_EQUALS, MakeErrorString(_pwsName, _cwchName), NULL);
    }
    else if (_stateParse == StateNSEquals)
    {
        return Exception::CreateFromResource(MSXSL_E_MISSING_NS_EQUALS, MakeErrorString(_pwsName, _cwchName), NULL);
    }
    else if (_stateParse == StateParamValue)
    {
        return Exception::CreateFromResource(MSXSL_E_MISSING_PARAM_VALUE, MakeErrorString(_pwsName, _cwchName), NULL);
    }
    else if (_stateParse == StateURI)
    {
        return Exception::CreateFromResource(MSXSL_E_MISSING_URI_VALUE, MakeErrorString(_pwsName, _cwchName), NULL);
    }
    else if (!_pwsInput && !_pwsStylesheet)
    {
        return Exception::CreateFromResource(MSXSL_E_MULTIPLE_STDIN, NULL);
    }

    // Resolve all prefixes
    ChkSucc(_paramMgr.ResolvePrefixes(&_nsMgr));

    if (_pwsStartMode)
    {
        // Break start mode QName
        ChkSucc(_nsMgr.ParseQName(_pwsStartMode, _cwchStartMode, -1, &_bstrStartModeLocal, &_bstrStartModeURI));
    }

Cleanup:
    return hr;
}


/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

