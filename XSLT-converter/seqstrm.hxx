/*
 * @(#)InStrm.hxx 1.0 10/19/2000
 * 
 * Copyright (c) 2000 Microsoft Corporation. All rights reserved.
 *
 * Definition of FileInputStream and FileOutputStream classes.
 * 
 */

#ifndef INSTRM_HXX
#define INSTRM_HXX


/*  ----------------------------------------------------------------------------
    FileInputStream()

    Implements Read method of ISequentialStream on top of input stream HANDLE.
*/
class FileInputStream : public StackUnknown<ISequentialStream>
{
public:
    FileInputStream() {_pBuffer = NULL;}
    ~FileInputStream() {free(_pBuffer);}

    HRESULT Init(HANDLE h);

    /////////////////////////////////////////////////////////////////////////////
    // ISequentialStream
    //
    virtual HRESULT STDMETHODCALLTYPE Read(void * pv, ULONG cb, ULONG * pcbRead);
    virtual HRESULT STDMETHODCALLTYPE Write(void const * pv, ULONG cb, ULONG * pcbWritten) {return E_NOTIMPL;}

private:

#ifdef _DEBUG
    enum {BUFFER_SIZE = 64};
#else
    enum {BUFFER_SIZE = 8192};
#endif

    HANDLE  _h;
    BYTE *  _pBuffer;   // Pointer to intermediate caching buffer
    BYTE *  _pAvail;    // Pointer to next available byte in _rgbBuffer
    DWORD   _cbAvail;   // Count of available bytes, starting at offset
    bool    _fEOF;      // True when input is exhausted
    bool    _fConsole;  // True if input comes from the console
};


/*  ----------------------------------------------------------------------------
    FileOutputStream()

    Implements Write method of ISequentialStream on top of output stream HANDLE.
*/
class FileOutputStream : public StackUnknown<ISequentialStream>
{
public:
    FileOutputStream() {_fClose = false;}
    ~FileOutputStream() {Close();}

    HRESULT Init(HANDLE h) {Assert(h); _h = h; _fClose = false; return S_OK;}
    HRESULT Init(const WCHAR * pwszFileName);

    void Close() {if (_fClose) {::CloseHandle(_h); _fClose = false;} }

    /////////////////////////////////////////////////////////////////////////////
    // ISequentialStream
    //
    virtual HRESULT STDMETHODCALLTYPE Read(void * pv, ULONG cb, ULONG * pcbRead) {return E_NOTIMPL;}
    virtual HRESULT STDMETHODCALLTYPE Write(void const * pv, ULONG cb, ULONG * pcbWritten);

private:
    HANDLE  _h;
    bool    _fClose;    // Close handle only if this class opened it
};


#endif INSTRM_HXX

/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



