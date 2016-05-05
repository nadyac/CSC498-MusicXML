/*
 * @(#)Timer.cxx 1.0 9/28/2000
 * 
 * Copyright (c) 2000 - 2001 Microsoft Corporation. All rights reserved.
 *
 * Implementation of Timer class.
 * 
 */

#include "core.hxx"
#pragma hdrstop

#include "timer.hxx"


/*  ----------------------------------------------------------------------------
    Timer()
*/
Timer::Timer()
{
    _time = 0;
    _start = 0;

    if (!::QueryPerformanceFrequency((LARGE_INTEGER *)&_freq) || _freq > INT_MAX)
    {
        // Counter not available
        _freq = 0;
    }
}


/*  ----------------------------------------------------------------------------
    GetCount()

    Get count from timer.
*/
__int64
Timer::GetCount()
{
    __int64 count;

    // Use high resolution tick count
    ::QueryPerformanceCounter((LARGE_INTEGER *) &count);

    return count;
}


/*  ----------------------------------------------------------------------------
    Reset()

    Reset the accumulated time and start the timer.
*/
void
Timer::Reset()
{
    _time = 0;
    Start();
}


/*  ----------------------------------------------------------------------------
    Start()

    Record the start time.
*/
void
Timer::Start() 
{
    _start = GetCount();
}


/*  ----------------------------------------------------------------------------
    Stop()

    Calculate the difference between the start and the end time and add it
    to the accumulated time count.
*/
DWORD
Timer::Stop() 
{ 
    __int64 diff;

    if (_freq)
    {
        diff = GetCount() - _start;
        if (diff <= INT_MAX)
        {
            _time += ::MulDiv((int) diff, 1000000, (int) _freq);
        }
    }
 
    return _time;
}


/// End of file ///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

