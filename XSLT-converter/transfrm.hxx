/*
 * @(#)Transform.hxx 1.0 9/28/2000
 * 
 * Copyright (c) 2000 Microsoft Corporation. All rights reserved.
 *
 * Definition of TransformHelper class.
 * 
 */

#ifndef TRANSFRM_HXX
#define TRANSFRM_HXX

#include "timer.hxx"
#include "seqstrm.hxx"

class MSXMLCommandLine;
class MSXMLInfo;


/*  ----------------------------------------------------------------------------
    TransformHelper()

    Transformation helper class.
*/
class TransformHelper
{
public:
    TransformHelper();
    ~TransformHelper();

    HRESULT Transform(MSXMLInfo * pMSXMLInfo);
    HRESULT Transform(MSXMLInfo * pMSXMLInfo, MSXMLCommandLine * pCmdLine);

    DWORD GetSourceTime() {return _timeSource;}
    DWORD GetStylesheetTime() {return _timeStylesheet;}
    DWORD GetCompileTime() {return _timeCompile;}
    DWORD GetExecutionTime() {return _timeExecute;}

protected:
    HRESULT CombineUrl(const WCHAR * pwszBaseUrl, const WCHAR * pwszUrl, BSTR * pbstrResultUrl);
    HRESULT SetBSTROption(const WCHAR * pwszOption, int cwchOption, BSTR * pbstrOption);
    HRESULT GetStyleNameFromPI(IXMLDOMDocument * pInput);
    HRESULT CreateObject(REFCLSID rclsid, REFIID riid, const WCHAR * pwszVersion, const WCHAR * pwszProgID, void ** ppv);
    HRESULT LoadDOM(IXMLDOMDocument * pDOM, BSTR bstrFileName);
    HRESULT SetStartMode(IXSLProcessor * pProcessor);
    HRESULT AddParameters(IXSLProcessor * pProcessor);
    HRESULT Compile(IXSLTemplate * pTemplate, IXMLDOMDocument * pStylesheet);
    HRESULT Execute(IXSLTemplate * pTemplate, IXMLDOMDocument * pInput);

private:
    BSTR        _bstrInput;         // Name of file containing source XML
    BSTR        _bstrStylesheet;    // Name of file containing stylesheet
    BSTR        _bstrOutput;        // Name of output file
    BSTR        _bstrStartModeQName;// XSLT start mode QName
    BSTR        _bstrStartModeLocal;// Local name part of XSLT start mode
    BSTR        _bstrStartModeURI;  // URI part of XSLT start mode
    bool        _fStripWS;          // If true, setPreserveWhitespace=false
    bool        _fResolveExt;       // If true, resolveExternals=true
    bool        _fValidate;         // If true, validateOnParse=true
    bool        _fTimings;          // If true, make timings
    bool        _fStyleFromPI;      // If true, get stylesheet URL from input document's xml-stylesheet pi

    ParameterMgr * _pParamMgr;      // Manages parameters to be passed to the stylesheet
    const WCHAR * _pwszStdIn;       // If input or stylesheet is from stdin, use this in errors

    Timer       _timer;             // Timer class
    DWORD       _timeCompile;       // Time to compile the stylesheet, in milliseconds
    DWORD       _timeExecute;       // Time to execute the transform, in milliseconds
    DWORD       _timeSource;        // Time to load source document, in milliseconds
    DWORD       _timeStylesheet;    // Time to load stylesheet document, in milliseconds

    IStream *   _pIStream;          // In-memory output stream
    HGLOBAL     _hStream;           // Handle to stream memory

    FileInputStream _inputStrm;     // Wraps stdin HANDLE when XSL or XML file comes from stdin
};

#endif TRANSFRM_HXX

/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

