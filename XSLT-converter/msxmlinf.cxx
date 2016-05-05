/*
 * @(#)MSXMLInf.cxx 1.0 6/10/2001
 * 
 * Copyright (c) 2001 Microsoft Corporation. All rights reserved.
 *
 * Implementation of MSXMLInfo class.
 * 
 */

#include "core.hxx"
#pragma hdrstop

#include "msxmlinf.hxx"


const MSXMLInfo::StaticInfo MSXMLInfo::s_staticInfo26 =
{
    VERSION_26,
    L"2.6",

    "{f5078f21-c551-11d3-89b9-0000f81fe221}",
    &__uuidof(XSLTemplate26),
    L"MSXML2.XSLTemplate.2.6",

    &__uuidof(DOMDocument26),
    L"MSXML2.DOMDocument.2.6",

    &__uuidof(FreeThreadedDOMDocument26),
    L"MSXML2.FreeThreadedDOMDocument.2.6",
};


const MSXMLInfo::StaticInfo MSXMLInfo::s_staticInfo30 =
{
    VERSION_30,
    L"3.0",

    "{f5078f36-c551-11d3-89b9-0000f81fe221}",
    &__uuidof(XSLTemplate30),
    L"MSXML2.XSLTemplate.3.0",

    &__uuidof(DOMDocument30),
    L"MSXML2.DOMDocument.3.0",

    &__uuidof(FreeThreadedDOMDocument30),
    L"MSXML2.FreeThreadedDOMDocument.3.0",
};


const MSXMLInfo::StaticInfo MSXMLInfo::s_staticInfo40 =
{
    VERSION_40,
    L"4.0",

    "{88d969c3-f192-11d4-a65f-0040963251e5}",
    &__uuidof(XSLTemplate40),
    L"MSXML2.XSLTemplate.4.0",

    &__uuidof(DOMDocument40),
    L"MSXML2.DOMDocument.4.0",

    &__uuidof(FreeThreadedDOMDocument40),
    L"MSXML2.FreeThreadedDOMDocument.4.0",
};


// Entries are in priority order
const MSXMLInfo::StaticInfo * MSXMLInfo::s_rgpStaticInfo[] =
{
    &s_staticInfo40,
    &s_staticInfo30,
    &s_staticInfo26,
    NULL,
};


/*  ----------------------------------------------------------------------------
    FindMSXMLInfo()

    Populate this class with information about the specified version of MSXML.
*/
HRESULT
MSXMLInfo::FindMSXMLInfo(Version version)
{
    switch (version)
    {
    case VERSION_26:
        memcpy(&_staticInfo, &s_staticInfo26, sizeof(StaticInfo));
        break;

    case VERSION_30:
        memcpy(&_staticInfo, &s_staticInfo30, sizeof(StaticInfo));
        break;

    case VERSION_40:
        memcpy(&_staticInfo, &s_staticInfo40, sizeof(StaticInfo));
        break;

    default:
        Assert(FALSE);
        break;
    }

    return S_OK;
}


/*  ----------------------------------------------------------------------------
    FindLatestMSXMLInfo()

    Inspect the registry in order to find the latest version of MSXML?.DLL
    that is present and registered on this machine, and that can be used by
    this version of MSXSL.EXE.  Populate this class with information for that
    version.
*/
HRESULT
MSXMLInfo::FindLatestMSXMLInfo()
{
    HRESULT hr = S_OK;
    HKEY hkeyOuter = NULL, hkeyInner = NULL;
    LONG lRegRet;
    const StaticInfo ** ppStaticInfo;

    ChkReg(::RegOpenKeyExA(
                HKEY_CLASSES_ROOT,
                "CLSID",
                NULL,
                KEY_READ,
                &hkeyOuter));

    ppStaticInfo = s_rgpStaticInfo;
    Assert(*ppStaticInfo);
    while (*ppStaticInfo)
    {
        // Lookup XSLTemplate version dependent CLSID in registry
        lRegRet = ::RegOpenKeyExA(
                            hkeyOuter,
                            (*ppStaticInfo)->_pszTemplateCLSID,
                            NULL,
                            KEY_READ,
                            &hkeyInner);

        if (NO_ERROR == lRegRet)
        {
            // Found installed MSXML version, so copy information about it to this class
            memcpy(&_staticInfo, *ppStaticInfo, sizeof(StaticInfo));
            goto Cleanup;
        }
        else if (ERROR_FILE_NOT_FOUND != lRegRet)
        {
            // If the key is not found, just go to the next.  Otherwise, throw an exception.
            ChkReg(lRegRet);
        }

        ppStaticInfo++;
    }

    // MSXML could not be located
    hr = Exception::CreateFromResource(MSXSL_E_NO_MSXML, NULL);

Cleanup:
    SafeRegClose(hkeyInner);
    SafeRegClose(hkeyOuter);

    return hr;
}


/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

