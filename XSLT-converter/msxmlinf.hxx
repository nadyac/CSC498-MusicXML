/*
 * @(#)MSXMLInf.hxx 1.0 6/10/2001
 * 
 * Copyright (c) 2001 Microsoft Corporation. All rights reserved.
 *
 * Definition of MSXMLInfo class.
 * 
 */

#ifndef MSXMLINF_HXX
#define MSXMLINF_HXX


/*  ----------------------------------------------------------------------------
    MSXMLInfo()

    Class which collects and stores information about a particular version of
    the MSXML DLL.
*/
class MSXMLInfo
{
public:
    MSXMLInfo() {}

    enum Version {VERSION_NONE, VERSION_26, VERSION_30, VERSION_40};

    HRESULT FindMSXMLInfo(Version version);
    HRESULT FindLatestMSXMLInfo();

    Version getVersion() {return _staticInfo._version;}
    const WCHAR * getVersionString() {return _staticInfo._pwszVersion;}

    REFCLSID getTemplateCLSID() {return *_staticInfo._clsidTemplate;}
    const WCHAR * getTemplateProgID() {return _staticInfo._pwszTemplateProgID;}

    REFCLSID getDocumentCLSID() {return *_staticInfo._clsidDocument;}
    const WCHAR * getDocumentProgID() {return _staticInfo._pwszDocumentProgID;}

    REFCLSID getFreeDocumentCLSID() {return *_staticInfo._clsidFreeDocument;}
    const WCHAR * getFreeDocumentProgID() {return _staticInfo._pwszFreeDocumentProgID;}

private:

    struct StaticInfo
    {
        Version         _version;
        const WCHAR *   _pwszVersion;

        const char *    _pszTemplateCLSID;
        const CLSID *   _clsidTemplate;
        const WCHAR *   _pwszTemplateProgID;

        const CLSID *   _clsidDocument;
        const WCHAR *   _pwszDocumentProgID;

        const CLSID *   _clsidFreeDocument;
        const WCHAR *   _pwszFreeDocumentProgID;
    } _staticInfo;

    static const StaticInfo s_staticInfo26;
    static const StaticInfo s_staticInfo30;
    static const StaticInfo s_staticInfo40;
    static const StaticInfo * s_rgpStaticInfo[];
};

#endif MSXMLINF_HXX

/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

