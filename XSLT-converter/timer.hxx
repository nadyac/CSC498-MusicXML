/*
 * @(#)Timer.hxx 1.0 9/28/2000
 * 
 * Copyright (c) 2000 Microsoft Corporation. All rights reserved.
 *
 * Definition of Timer class.
 * 
 */

#ifndef TIMER_HXX
#define TIMER_HXX


/*  ----------------------------------------------------------------------------
    Timer()

    Utility class used for making performance timings.
*/
class Timer
{
public:
    Timer();

    void Reset();
    void Start();
    DWORD Stop();
    DWORD GetTime() {return _time;}

protected:
    __int64 GetCount();

private:
    __int64 _freq;
    __int64 _start;
    DWORD   _time;
};


#endif TIMER_HXX

/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

