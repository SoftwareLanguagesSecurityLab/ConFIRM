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

#include "setup.h"

NANOSECOND start_time;
NANOSECOND end_time;
NANOSECOND total_time;

// Define a function pointer type for retrieving function pointer
// returned by GetProcAddress()/dlsym().
typedef void(*INC)(int&);

int main()
{
    #ifdef _WIN32
    HINSTANCE libHandle = NULL;
    BOOL retFree = FALSE;
    #elif __linux__
    void *libHandle;
    char *error;
    #endif    
    INC pInc = NULL;
    int count = 0;

    // record starting time
    start_time = get_wall_time();    

    for (long long int i = 0; i < MAX_LOOP * DYNLTS; ++i)
    {
        // Load an untrusted module libinc.dll/.so
        #ifdef _WIN32
        libHandle = LoadLibrary(L"libinc.dll");
        #elif __linux__
        libHandle = dlopen("./libinc.so", RTLD_LAZY);
        #endif
        if (!libHandle)
        {
            printf("Loading library failed.\n");
            exit(1);
        }

        // Get a function pointer exported by libinc.dll/.so
        #ifdef _WIN32
        pInc = (INC)GetProcAddress(libHandle, "increment");
        #elif __linux__
        pInc = (INC) dlsym(libHandle, "increment");
        #endif
        if (!pInc)
        {
            printf("Finding symbol failed.\n");
            exit(1);
        }
        
        // Call the returned function.
        pInc(count);

        // Unload libinc.dll/.so
        #ifdef _WIN32
        retFree = FreeLibrary(libHandle);
        if (!retFree)
        {
            printf("FreeLibrary() failed.\n");
            exit(1);
        }
        #elif __linux__
        dlclose(libHandle);
        #endif
    }

    // record ending time
    end_time = get_wall_time();
    total_time = end_time - start_time;

    // print results
    printf("total time in nanoseconds is %llu\n", (long long unsigned int) total_time);
    printf("count is %d\n", count);
    return 0;
}
