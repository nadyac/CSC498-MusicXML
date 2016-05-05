/*
 * @(#)CRT.cxx 1.0 10/6/2000
 * 
 * Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
 *
 * Small memory footprint CRT routines.
 * 
 */

#include "core.hxx"
#pragma hdrstop


/*  ----------------------------------------------------------------------------
    xtow()

    Helper function for converting number to Unicode string.
*/
static void __cdecl
xtow(unsigned long ulVal, WCHAR * pwszBuf, unsigned uiRadix, int fIsNeg)
{
    WCHAR * pwsz;
    WCHAR * pwchFirstDig;
    WCHAR wchTemp;
    unsigned uiDigitVal;

    pwsz = pwszBuf;

    if (fIsNeg)
    {
        // Negative, so output '-' and negate
        *pwsz++ = '-';
        ulVal = (unsigned long)(-(long)ulVal);
    }

    // Save pointer to first digit
    pwchFirstDig = pwsz;

    do
    {
        // Get next digit
        uiDigitVal = (unsigned) (ulVal % uiRadix);
        ulVal /= uiRadix;

        // Convert to Unicode character and store
        if (uiDigitVal > 9)
            *pwsz++ = (WCHAR) (uiDigitVal - 10 + L'a'); // Letter
        else
            *pwsz++ = (WCHAR) (uiDigitVal + L'0');      // Digit
    }
    while (ulVal > 0);

    // We now have the digit of the number in the buffer, but in reverse
    // order.  Thus we reverse them now.

    // Terminate string; p points to last digit
    *pwsz-- = L'\0';

    do
    {
        wchTemp = *pwsz;
        *pwsz = *pwchFirstDig;
        *pwchFirstDig = wchTemp;    // Swap *pwsz and *pwchFirstDig
        --pwsz;
        ++pwchFirstDig;             // Advance to next two digits
    }
    while (pwchFirstDig < pwsz);    // Repeat until halfway
}


/*  ----------------------------------------------------------------------------
    malloc()

    Allocates memory blocks.
*/
void * __cdecl
malloc(size_t size)
{
    return ::LocalAlloc(NONZEROLPTR, size);
}


/*  ----------------------------------------------------------------------------
    free()

    Frees memory blocks allocated by malloc.
*/
void __cdecl
free(void * pv)
{
    ::LocalFree(pv);
}


/*  ----------------------------------------------------------------------------
    _strndup()

    Duplicate the pszSrc string.
*/
char * __cdecl
_strndup(const char * psSrc, int cchSrc)
{
    char * pszDest;

    pszDest = (char *) malloc(cchSrc * sizeof(char) + 1);
    if (pszDest)
    {
        memcpy(pszDest, psSrc, cchSrc * sizeof(char));
        pszDest[cchSrc] = 0;
    }
    return pszDest;
}


/*  ----------------------------------------------------------------------------
    _wcsdup()

    Duplicate the pwszSrc string.
*/
WCHAR * __cdecl
_wcsndup(const WCHAR * pwsSrc, int cwchSrc)
{
    WCHAR * pwszDest;

    pwszDest = (WCHAR *) malloc(cwchSrc * sizeof(WCHAR) + 2);
    if (pwszDest)
    {
        memcpy(pwszDest, pwsSrc, cwchSrc * sizeof(WCHAR));
        pwszDest[cwchSrc] = 0;
    }
    return pwszDest;
}


/*  ----------------------------------------------------------------------------
    _itow()

    Convert integer to a Unicode string.
*/
WCHAR * __cdecl
_itow(int iVal, WCHAR * pwszBuf, int iRadix)
{
    if (iRadix == 10 && iVal < 0)
        xtow((unsigned long) iVal, pwszBuf, iRadix, 1);
    else
        xtow((unsigned long)(unsigned int) iVal, pwszBuf, iRadix, 0);

    return pwszBuf;
}


/*  ----------------------------------------------------------------------------
    wcscpy()

    Copy pwszSrc to pwszDest.
*/
WCHAR * __cdecl
wcscpy(WCHAR * pwszDest, const WCHAR * pwszSrc)
{
    WCHAR * pwsz = pwszDest;
    Assert(pwszDest && pwszSrc);

    do
    {
        *pwsz++ = *pwszSrc;
    }
    while (*pwszSrc++);

    return pwszDest;
}


/*  ----------------------------------------------------------------------------
    _wcsnicmp()

    Case-insensitive compare of two strings.
    NOTE: This version is not sensitive to locale and only handles a-z, A-Z.
*/
int __cdecl
_wcsnicmp(const WCHAR * pwsz1, const WCHAR * pwsz2, size_t cwch)
{
    WCHAR wch1, wch2;
    int iRes = 0;

    if (cwch)
    {
        do
        {
            wch1 = *pwsz1++;
            if (wch1 >= L'A' && wch1 <= L'Z')
            {
                wch1 += L'a' - L'A';
            }

            wch2 = *pwsz2++;
            if (wch2 >= L'A' && wch2 <= L'Z')
            {
                wch2 += L'a' - L'A';
            }
        }
        while (--cwch && wch1 && wch1 == wch2);

        iRes = (int) (wch1 - wch2);
    }

    return iRes;
}


#ifdef _DEBUG
/*  ----------------------------------------------------------------------------
    _assert()

    Assert implementation that dumps assert text to console.
*/
void __cdecl
_assert(const char * psz, const char * pszFile, unsigned iLine)
{
    WCHAR rgwchLine[16];

    WriteAsMBCS(g_hStdErr, "Assert Failed\n");
    WriteAsMBCS(g_hStdErr, "   File: ");
    WriteAsMBCS(g_hStdErr, pszFile);
    WriteAsMBCS(g_hStdErr, "\r\n   Line: ");
    WriteAsMBCS(g_hStdErr, _itow((int) iLine, rgwchLine, 10));
    WriteAsMBCS(g_hStdErr, "\r\n   Text: ");
    WriteAsMBCS(g_hStdErr, psz);
    WriteAsMBCS(g_hStdErr, "\r\n");

    ::ExitProcess(1);
}
#endif

/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

