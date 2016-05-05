/*
 * @(#)CmdLine.hxx 1.0 9/28/2000
 * 
 * Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
 *
 * Definition of ParseCommandLine and MSXMLCommandLine classes.
 * 
 */

#ifndef CMDLINE_HXX
#define CMDLINE_HXX

#ifndef MSXMLINF_HXX
#include "msxmlinf.hxx"
#endif


/*  ----------------------------------------------------------------------------
    ParseCommandLine()

    Helper class for parsing command line strings.
*/
class NOVTABLE ParseCommandLine
{
public:
    HRESULT Parse() {return Parse(GetCommandLineW());}
    HRESULT Parse(const WCHAR * pwszCmdLine);

    static const WCHAR * SkipWhitespace(const WCHAR * pwsz);
    static const WCHAR * SkipNonWhitespace(const WCHAR * pwsz);

    static bool IsWhitespaceChar(WCHAR wch);

protected:

    enum TokenType {TokFileName, TokSwitch, TokOption, TokQuotedOption};

    virtual HRESULT BeginParsing();
    virtual HRESULT ParsedToken(const WCHAR * pws, const WCHAR ** ppwsEnd, TokenType type) = 0;
    virtual HRESULT EndParsing();
};


/*  ----------------------------------------------------------------------------
    MSXMLCommandLine()

    Specialization of ParseCommandLine class for the msxml4 command line.
*/
class MSXMLCommandLine : public ParseCommandLine
{
public:
    MSXMLCommandLine() {_pwszError = _bstrStartModeLocal = _bstrStartModeURI = NULL;}
    ~MSXMLCommandLine() {free((void *) _pwszError); ::SysFreeString(_bstrStartModeLocal); ::SysFreeString(_bstrStartModeURI);}

    const WCHAR * GetInputName() {return _pwsInput;}
    int GetInputLength() {return _cwchInput;}

    const WCHAR * GetOutputName() {return _pwsOutput;}
    int GetOutputLength() {return _cwchOutput;}

    const WCHAR * GetStylesheetName() {return _pwsStylesheet;}
    int GetStylesheetLength() {return _cwchStylesheet;}

    const WCHAR * GetStartModeQName() {return _pwsStartMode;}
    int GetStartModeQNameLength() {return _cwchStartMode;}

    const WCHAR * GetStartModeLocalName() {return (const WCHAR *) _bstrStartModeLocal;}
    int GetStartModeLocalNameLength() {return ::SysStringLen(_bstrStartModeLocal);}

    const WCHAR * GetStartModeURI() {return (const WCHAR *) _bstrStartModeURI;}
    int GetStartModeURILength() {return ::SysStringLen(_bstrStartModeURI);}

    ParameterMgr * GetParameters() {return &_paramMgr;}

    bool StripWhitespace() {return _fStripWS;}
    bool ResolveExternals() {return _fResolveExt;}
    bool ValidateOnParse() {return _fValidate;}
    bool DoTimings() {return _fTimings;}
    bool StyleFromPI() {return _fStyleFromPI;}
    MSXMLInfo::Version GetMSXMLVersion() {return _version;}

protected:

    virtual HRESULT BeginParsing();
    virtual HRESULT ParsedToken(const WCHAR * pws, const WCHAR ** ppwsEnd, TokenType type);
    virtual HRESULT EndParsing();

    const WCHAR * MakeErrorString(const WCHAR * pws, int cwch);
    HRESULT ParseName(const WCHAR * pwsName, int cwchName);

    enum ParseState {StateOption, StateOutput, StateMode, StateVersion, StateNSEquals, StateParamEquals, StateURI, StateParamValue};
    enum OptionState {StateInput, StateStylesheet, StateParam};

private:
    const WCHAR *       _pwsInput;
    int                 _cwchInput;
    const WCHAR *       _pwsStylesheet;
    int                 _cwchStylesheet;
    const WCHAR *       _pwsOutput;
    int                 _cwchOutput;
    const WCHAR *       _pwsStartMode;
    int                 _cwchStartMode;
    const WCHAR *       _pwsName;
    int                 _cwchName;
    BSTR                _bstrStartModeLocal;
    BSTR                _bstrStartModeURI;
    bool                _fStripWS;
    bool                _fResolveExt;
    bool                _fValidate;
    bool                _fTimings;
    bool                _fStyleFromPI;
    MSXMLInfo::Version  _version;
    ParseState          _stateParse;
    OptionState         _stateOption;
    const WCHAR *       _pwszError;
    NamespaceMgr        _nsMgr;
    ParameterMgr        _paramMgr;
};

#endif CMDLINE_HXX

/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


