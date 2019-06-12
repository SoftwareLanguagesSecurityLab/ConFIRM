/*************************************************************************************
 * Copyright (c) 2019 Xiaoyang Xu, Masoud Ghaffarinia, Wenhao Wang, and Kevin Hamlen *
 * The University of Texas at Dallas                                                 *
 *                                                                                   *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of   *
 * this software and associated documentation files (the "Software"), to deal in     *
 * the Software without restriction, including without limitation the rights to      *
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of  *
 * the Software, and to permit persons to whom the Software is furnished to do so,   *
 * subject to the following conditions:                                              *
 *                                                                                   *
 * The above copyright notice and this permission notice shall be included in all    *
 * copies or substantial portions of the Software.                                   *
 *                                                                                   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS  *
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR    *
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER    *
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN           *
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        *
 *************************************************************************************/
 
/* This file is part of the ConFIRM test suite, whose initial documentation can be
   found in the following publication:

   Xiaoyang Xu, Masoud Ghaffarinia, Wenhao Wang, Kevin W. Hamlen, and Zhiqiang Lin.
   "ConFIRM: Evaluating Compatibility and Relevance of Control-flow Integrity 
   Protections for Modern Software."  In Proceedings of the 28th USENIX Security
   Symposium, August 2019. */

// This microbenchmark should be compiled by Microsoft Visual Studio with additional
// linker option /DELAYLOAD:"user32.dll", so that user32.dll would be delay loaded.

#include "setup.h"

NANOSECOND start_time;
NANOSECOND end_time;
NANOSECOND total_time;

int main()
{    
    HWND hWindow = NULL;

    // Record starting time
    start_time = get_wall_time();    

    for (long long int i = 0; i < MAX_LOOP * DLTS; ++i)
    {
        // API GetForegroundWindow() is exported user32.dll. By delay-loading 
        // user32.dll, it is not loaded into memory until GetForegroundWindow() is
        // being called.
        hWindow = GetForegroundWindow();
        if (!hWindow)
        {
            printf("GetActiveWindow() failed.\n");
            exit(1);
        }
        hWindow = NULL;
    }

    // Record ending time
    end_time = get_wall_time();
    total_time = end_time - start_time;

    // Print time elapsed in the loop.
    printf("total time in nanoseconds is %llu\n", (long long unsigned int) total_time);
    return 0;
}
