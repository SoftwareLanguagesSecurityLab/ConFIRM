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

void NTAPI TLSEntry(PVOID DllHandle, DWORD dwReason, PVOID);
DWORD WINAPI ThreadProc(CONST LPVOID lpParam);

// Linker spec for using TLS callback
#ifdef _WIN32
#pragma comment (linker, "/INCLUDE:__tls_used")
#pragma comment (linker, "/INCLUDE:_tls_callback")
#else
#pragma comment (linker, "/INCLUDE:_tls_used")
#pragma comment (linker, "/INCLUDE:tls_callback")
#endif  
#ifdef _WIN32
#pragma data_seg(".CRT$XLF")
EXTERN_C
#else
#pragma const_seg(".CRT$XLF")
EXTERN_C const
#endif
PIMAGE_TLS_CALLBACK tls_callback = TLSEntry;
#ifdef _WIN32
#pragma data_seg()
#else
#pragma const_seg()
#endif

int main()
{
    const int thread_count = 10;
    HANDLE handles[thread_count];

    // Creating/terminating new thread invokes a TLS callback.
    for (int i = 0; i < thread_count; i++)
    {
        handles[i] = CreateThread(NULL, 0, &ThreadProc, 0, 0, NULL);
    }
    // Wairing for thread to terminate.
    WaitForMultipleObjects(thread_count, handles, TRUE, INFINITE);

    for (int i = 0; i < thread_count; i++)
    {
        CloseHandle(handles[i]);
    }
    
    printf("TLS callback test passed.\n");

    return 0;
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam)
{
    ExitThread(0);
}

void NTAPI TLSEntry(PVOID DllHandle, DWORD dwReason, PVOID)
{
    if (dwReason == DLL_THREAD_ATTACH)
    {
        printf("DLL_THREAD_ATTACH\n");
    }

    if (dwReason == DLL_THREAD_DETACH)
    {
        printf("DLL_THREAD_DETACH\n");
    }

    if (dwReason == DLL_PROCESS_ATTACH)
    {
        printf("DLL_PROCESS_ATTACH\n");
    }

    if (dwReason == DLL_PROCESS_DETACH)
    {
        printf("DLL_PROCESS_DETACH\n");
    }
}
