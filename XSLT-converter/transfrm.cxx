/*
 * @(#)Transform.cxx 1.0 9/28/2000
 * 
 * Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
 *
 * Implementation of TransformHelper class.
 * 
 */

#include "core.hxx"
#pragma hdrstop

#include "transfrm.hxx"
#include "cmdline.hxx"


#define TIMER_RESET() {if (_fTimings) _timer.Reset();}
#define TIMER_STOP(varTime) {if (_fTimings) (varTime) = _timer.Stop();}


/*  ----------------------------------------------------------------------------
    TransformHelper()
*/
TransformHelper::TransformHelper()
{
    _bstrInput = _bstrStylesheet = _bstrOutput = NULL;
    _bstrStartModeQName = _bstrStartModeLocal = _bstrStartModeURI = NULL;
    _pwszStdIn = NULL;
    _fStripWS = _fTimings = _fValidate = false;
    _fResolveExt = true;
    _timeSource = _timeStylesheet = _timeCompile = _timeExecute = 0;
    _pIStream = NULL;
    _hStream = NULL;
}


/*  ----------------------------------------------------------------------------
    ~TransformHelper()
*/
TransformHelper::~TransformHelper()
{
    ::SysFreeString(_bstrInput);
    ::SysFreeString(_bstrStylesheet);
    ::SysFreeString(_bstrOutput);
    ::SysFreeString(_bstrStartModeQName);
    ::SysFreeString(_bstrStartModeLocal);
    ::SysFreeString(_bstrStartModeURI);
    free((void *) _pwszStdIn);
    SafeRelease(_pIStream);
}


/*  ----------------------------------------------------------------------------
    CombineUrl()

    Combine a base URL and a relative URL into a single URL.
*/
HRESULT
TransformHelper::CombineUrl(const WCHAR * pwszBaseUrl, const WCHAR * pwszUrl, BSTR * pbstrResultUrl)
{
    HRESULT hr = S_OK;
    DWORD dwBufLen = 0;

    // Get required result buffer size
    ::InternetCombineUrl(pwszBaseUrl, pwszUrl, NULL, &dwBufLen, ICU_NO_ENCODE);

    // Allocate buffer and combine Url's
    *pbstrResultUrl = ::SysAllocStringLen(NULL, dwBufLen);
    
    ChkWin(::InternetCombineUrl(pwszBaseUrl, pwszUrl, *pbstrResultUrl, &dwBufLen, ICU_NO_ENCODE));

 Cleanup:
    if (FAILED(hr))
    {
        ::SysFreeString(*pbstrResultUrl);
    }

    return hr;
}


/*  ----------------------------------------------------------------------------
    SetBSTROption()

    Set a BSTR transformation option.
*/
HRESULT
TransformHelper::SetBSTROption(const WCHAR * pwszOption, int cwchOption, BSTR * pbstrOption)
{
    Assert(!*pbstrOption);
    if (pwszOption)
    {
        *pbstrOption = ::SysAllocStringLen(pwszOption, cwchOption);
        if (!*pbstrOption)
            return E_OUTOFMEMORY;
    }
    return S_OK;
}


/*  ----------------------------------------------------------------------------
    GetStyleNameFromPI()

    Look for a top-level xml-stylesheet pi in the specified document.  Save the
    href="..." URL in _bstrStylesheet.
*/
HRESULT
TransformHelper::GetStyleNameFromPI(IXMLDOMDocument * pInput)
{
    HRESULT hr = S_OK;
    IXMLDOMNodeList * pNodeList = NULL;
    IXMLDOMNode * pNode = NULL;
    BSTR bstrText = NULL, bstrInputUrl = NULL;
    const WCHAR * pwszBegin, * pwsz;
    WCHAR wch;
    bool isHref;
    Assert(!_bstrStylesheet);

    ChkSuccCOM(pInput->selectNodes(L"processing-instruction('xml-stylesheet')", &pNodeList));
    ChkSuccCOM(pNodeList->nextNode(&pNode));

    if (pNode)
    {
        ChkSuccCOM(pNode->get_text(&bstrText));

        // Scan for href (which must be there, since original query used contains function)
        pwsz = bstrText;
        while (true)
        {
            pwszBegin = pwsz = ParseCommandLine::SkipWhitespace(pwsz);

            while (*pwsz != L'=')
            {
                if (!*pwsz)
                    goto InvalidPI;
                pwsz++;
            }

            // Look for 'href' tag
            isHref = !_wcsnicmp(pwszBegin, L"href", 4) && (pwszBegin[4] == L'=' || ParseCommandLine::IsWhitespaceChar(pwszBegin[4]));

            pwsz = ParseCommandLine::SkipWhitespace(pwsz + 1);

            // Look for ' or "
            wch = *pwsz++;
            if (wch != 39 && wch != L'"')
                goto InvalidPI;

            // Find end of quoted string
            pwszBegin = pwsz;
            while (*pwsz != wch)
            {
                if (!*pwsz)
                {
                    // Unterminated string
                    goto InvalidPI;
                }
                pwsz++;
            }

            if (isHref)
            {
                // Successfully parsed the href value
                // Resolve the href URL using the input document's URL as base
                ChkSuccCOM(pInput->get_url(&bstrInputUrl));
                const_cast<WCHAR *>(pwszBegin)[pwsz - pwszBegin] = 0;
                ChkSucc(CombineUrl(bstrInputUrl, pwszBegin, &_bstrStylesheet));
                goto Cleanup;
            }

            pwsz++;
        }
    }

InvalidPI:
    hr = Exception::CreateFromResource(MSXSL_E_INVALID_PI, NULL);

Cleanup:
    ::SysFreeString(bstrText);
    ::SysFreeString(bstrInputUrl);
    SafeRelease(pNode);
    SafeRelease(pNodeList);

    return hr;
}


/*  ----------------------------------------------------------------------------
    CreateObject()

    Creates an MSXML object using CoCreateInstance.
*/
HRESULT
TransformHelper::CreateObject(REFCLSID rclsid, REFIID riid, const WCHAR * pwszVersion, const WCHAR * pwszProgID, void ** ppv)
{
    HRESULT hr;

    hr = ::CoCreateInstance(
                rclsid,
                NULL,
                CLSCTX_SERVER,
                riid,
                ppv);

    if (FAILED(hr))
    {
        hr = Exception::CreateFromResource(MSXSL_E_BAD_MSXML, pwszProgID, pwszVersion, NULL);
        Exception::SetContext(MSXSL_E_CREATE_CTXT, NULL);
    }

    return hr;
}


/*  ----------------------------------------------------------------------------
    LoadDOM()

    Loads XML from bstrFileName into pDOM, using options previously set by
    the user.
*/
HRESULT
TransformHelper::LoadDOM(IXMLDOMDocument * pDOM, BSTR bstrFileName)
{
    HRESULT hr = S_OK;
    VARIANT vInput;
    VARIANT_BOOL fSuccess;
    IXMLDOMParseError * pParseError = NULL;
    BSTR bstrUrl = NULL, bstrReason = NULL;
    long ulLine, ulColumn, ulCode;
    WCHAR rgwchLine[16], rgwchColumn[16];
    bool fLoading = true;

    if (bstrFileName)
    {
        V_VT(&vInput) = VT_BSTR;
        V_BSTR(&vInput) = bstrFileName;
    }
    else
    {
        ChkSucc(_inputStrm.Init(g_hStdIn));
        V_VT(&vInput) = VT_UNKNOWN;
        V_UNKNOWN(&vInput) = &_inputStrm;
    }

    if (!_fStripWS)
    {
        // preserveWhiteSpace defaults to VARIANT_FALSE
        ChkSuccCOM(pDOM->put_preserveWhiteSpace(VARIANT_TRUE));
    }

    if (!_fResolveExt)
    {
        // resolveExternals defaults to VARIANT_TRUE
        ChkSuccCOM(pDOM->put_resolveExternals(VARIANT_FALSE));
    }

    if (!_fValidate)
    {
        // validateOnParse defaults to VARIANT_TRUE
        ChkSuccCOM(pDOM->put_validateOnParse(VARIANT_FALSE));
    }

    ChkSuccCOM(pDOM->put_async(VARIANT_FALSE));
    ChkSuccCOM(pDOM->load(vInput, &fSuccess));
    if (fSuccess == VARIANT_FALSE)
    {
        ChkSuccCOM(pDOM->get_parseError(&pParseError));
        ChkSuccCOM(pParseError->get_reason(&bstrReason));
        ChkSuccCOM(pParseError->get_errorCode(&ulCode));
        ChkSuccCOM(pParseError->get_line(&ulLine));

        if (ulLine != 0)
        {
            // It's a parsing error
            fLoading = false;
            ChkSuccCOM(pParseError->get_url(&bstrUrl));
            ChkSuccCOM(pParseError->get_linepos(&ulColumn));
            Exception::CreateFromResource(MSXSL_E_PARSE_ERROR,
                                          bstrUrl ? bstrUrl : _pwszStdIn,
                                          _itow((int) ulLine, rgwchLine, 10),
                                          _itow((int) ulColumn, rgwchColumn, 10),
                                          bstrReason);
        }
        else
        {
            if ((HRESULT) ulCode == INET_E_OBJECT_NOT_FOUND)
            {
                // Workaround for poor error message when URL is local and URLMON isn't loaded
                Exception::CreateFromResource(MSXSL_E_OBJECT_NOT_FOUND, NULL);
            }
            else
            {
                Exception::CreateFromText((HRESULT) ulCode, bstrReason);
            }
        }
        ChkSucc((HRESULT) ulCode);
    }

Cleanup:
    ::SysFreeString(bstrUrl);
    ::SysFreeString(bstrReason);
    SafeRelease(pParseError);

    if (FAILED(hr))
    {
        // Add context information to the exception
        if (fLoading)
        {
            Exception::SetContext(MSXSL_E_LOAD_CTXT, bstrFileName ? bstrFileName : _pwszStdIn, NULL);
        }
        else
        {
            Exception::SetContext(MSXSL_E_PARSE_CTXT, NULL);
        }
    }

    return hr;
}


/*  ----------------------------------------------------------------------------
    Transform()

    Set transformation options from command line and initiate transformation.
*/
HRESULT
TransformHelper::Transform(MSXMLInfo * pMSXMLInfo, MSXMLCommandLine * pCmdLine)
{
    HRESULT hr = S_OK;

    ChkSucc(SetBSTROption(pCmdLine->GetInputName(), pCmdLine->GetInputLength(), &_bstrInput));
    ChkSucc(SetBSTROption(pCmdLine->GetStylesheetName(), pCmdLine->GetStylesheetLength(), &_bstrStylesheet));
    ChkSucc(SetBSTROption(pCmdLine->GetOutputName(), pCmdLine->GetOutputLength(), &_bstrOutput));
    ChkSucc(SetBSTROption(pCmdLine->GetStartModeQName(), pCmdLine->GetStartModeQNameLength(), &_bstrStartModeQName));
    ChkSucc(SetBSTROption(pCmdLine->GetStartModeLocalName(), pCmdLine->GetStartModeLocalNameLength(), &_bstrStartModeLocal));
    ChkSucc(SetBSTROption(pCmdLine->GetStartModeURI(), pCmdLine->GetStartModeURILength(), &_bstrStartModeURI));

    _fStripWS = pCmdLine->StripWhitespace();
    _fResolveExt = pCmdLine->ResolveExternals();
    _fValidate = pCmdLine->ValidateOnParse();
    _fTimings = pCmdLine->DoTimings();
    _fStyleFromPI = pCmdLine->StyleFromPI();

    _pParamMgr = pCmdLine->GetParameters();

    ChkSucc(Transform(pMSXMLInfo));

Cleanup:
    return hr;
}


/*  ----------------------------------------------------------------------------
    SetStartMode()

    Set the starting mode of the XSLProcessor.
*/
HRESULT
TransformHelper::SetStartMode(IXSLProcessor * pProcessor)
{
    HRESULT hr = S_OK;

    if (_bstrStartModeLocal)
    {
        ChkSuccCOM(pProcessor->setStartMode(_bstrStartModeLocal, _bstrStartModeURI));
    }

Cleanup:
    if (FAILED(hr))
    {
        // Add context information to the exception
        Exception::SetContext(MSXSL_E_MODE_CTXT, _bstrStartModeQName, NULL);
    }

    return hr;
}


/*  ----------------------------------------------------------------------------
    AddParameters()

    Add parameters to the XSLProcessor.
*/
HRESULT
TransformHelper::AddParameters(IXSLProcessor * pProcessor)
{
    HRESULT hr = S_OK;
    int iParam;

    // Add parameters
    for (iParam = 0; iParam < _pParamMgr->GetParameterCount(); iParam++)
    {
        ChkSuccCOM(pProcessor->addParameter(
                        _pParamMgr->GetLocalName(iParam),
                        *_pParamMgr->GetValue(iParam),
                        _pParamMgr->GetURI(iParam)));
    }

Cleanup:
    if (FAILED(hr))
    {
        // Add context information to the exception
        Exception::SetContext(MSXSL_E_PARAM_CTXT, _pParamMgr->GetQName(iParam), NULL);
    }

    return hr;
}


/*  ----------------------------------------------------------------------------
    Compile()

    Compile the XSL stylesheet.
*/
HRESULT
TransformHelper::Compile(IXSLTemplate * pTemplate, IXMLDOMDocument * pStylesheet)
{
    HRESULT hr = S_OK;

    // Compile stylesheet
    TIMER_RESET();
    ChkSuccCOM(pTemplate->putref_stylesheet(pStylesheet));
    TIMER_STOP(_timeCompile);

Cleanup:
    if (FAILED(hr))
    {
        // Add context information to the exception
        Exception::SetContext(MSXSL_E_COMPILE_CTXT, _bstrStylesheet ? _bstrStylesheet : _pwszStdIn, NULL);
    }

    return hr;
}


/*  ----------------------------------------------------------------------------
    Execute()

    Execute the XSL stylesheet over the pInput document.
*/
HRESULT
TransformHelper::Execute(IXSLTemplate * pTemplate, IXMLDOMDocument * pInput)
{
    HRESULT hr = S_OK;
    LARGE_INTEGER start = {0, 0};
    IXSLProcessor * pProcessor = NULL;
    FileOutputStream outStrm;
    VARIANT vOut, vInput;
    VARIANT_BOOL fDone;

    ChkSuccCOM(pTemplate->createProcessor(&pProcessor));

    // Set start mode of processor
    ChkSucc(SetStartMode(pProcessor));

    // Add parameters to the processor
    ChkSucc(AddParameters(pProcessor));

    V_VT(&vInput) = VT_UNKNOWN;
    V_UNKNOWN(&vInput) = (IUnknown *) pInput;
    ChkSuccCOM(pProcessor->put_input(vInput));

    // Set processor's output to FileOutputStream
    if (_bstrOutput)
    {
        hr = outStrm.Init(_bstrOutput);
        if (FAILED(hr))
        {
            // Add context information to the exception
            Exception::SetContext(MSXSL_E_CREATE_FILE_CTXT, _bstrOutput, NULL);
            goto Cleanup;
        }
    }
    else
    {
        ChkSucc(outStrm.Init(g_hStdOut));
    }

    V_VT(&vOut) = VT_UNKNOWN;
    V_UNKNOWN(&vOut) = &outStrm;

    ChkSuccCOM(pProcessor->put_output(vOut));

    // Execute stylesheet
    TIMER_RESET();
    ChkSuccCOM(pProcessor->transform(&fDone));
    TIMER_STOP(_timeExecute);

Cleanup:
    SafeRelease(pProcessor);

    if (FAILED(hr))
    {
        // Add context information to the exception
        Exception::SetContext(MSXSL_E_EXECUTE_CTXT, _bstrStylesheet ? _bstrStylesheet : _pwszStdIn, NULL);
    }

    return hr;
}


/*  ----------------------------------------------------------------------------
    Transform()

    Load source XML file and stylesheet XSL file.  Perform transformation and
    write output to in-memory stream.
*/
HRESULT
TransformHelper::Transform(MSXMLInfo * pMSXMLInfo)
{
    HRESULT hr = S_OK;
    IXMLDOMDocument * pInput = NULL;
    IXMLDOMDocument * pStylesheet = NULL;
    IXSLTemplate * pTemplate = NULL;

    if (!_bstrInput || (!_bstrStylesheet && !_fStyleFromPI))
    {
        // If input or stylesheet comes from stdin, preload "<from stdin" string used in errors
        Assert(_bstrInput || _bstrStylesheet);
        ChkSucc(Resources::FormatResourceMessage(MSXSL_E_FROM_STDIN, &_pwszStdIn, NULL));
    }

    ChkSucc(CreateObject(
                pMSXMLInfo->getDocumentCLSID(),
                __uuidof(IXMLDOMDocument),
                pMSXMLInfo->getVersionString(),
                pMSXMLInfo->getDocumentProgID(),
                (void **) &pInput));

    // Load source document
    TIMER_RESET();
    ChkSucc(LoadDOM(pInput, _bstrInput));
    TIMER_STOP(_timeSource);

    if (_fStyleFromPI)
    {
        // Get the stylesheet URL from the input document's xml-stylesheet pi and save it in _bstrStylesheet
        ChkSucc(GetStyleNameFromPI(pInput));
    }

    ChkSucc(CreateObject(
                pMSXMLInfo->getFreeDocumentCLSID(),
                __uuidof(IXMLDOMDocument),
                pMSXMLInfo->getVersionString(),
                pMSXMLInfo->getFreeDocumentProgID(),
                (void **) &pStylesheet));

    // Load stylesheet document
    TIMER_RESET();
    ChkSucc(LoadDOM(pStylesheet, _bstrStylesheet));
    TIMER_STOP(_timeStylesheet);

    ChkSucc(CreateObject(
                pMSXMLInfo->getTemplateCLSID(),
                __uuidof(IXSLTemplate),
                pMSXMLInfo->getVersionString(),
                pMSXMLInfo->getTemplateProgID(),
                (void **) &pTemplate));

    // Compile the stylesheet document
    ChkSucc(Compile(pTemplate, pStylesheet));

    // Execute the stylesheet
    ChkSucc(Execute(pTemplate, pInput));

Cleanup:
    SafeRelease(pInput);
    SafeRelease(pStylesheet);
    SafeRelease(pTemplate);

    return hr;
}


/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

