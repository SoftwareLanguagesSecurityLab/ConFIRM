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

void callee1();
void callee2();
// min estimated max size of callee1 and callee 2
#define ESTIMATED_FUNC_SIZE 100
static bool flag_1 = FALSE;
static bool flag_2 = FALSE;

typedef void(*FUNCPTR)();

int main()
{
    SYSTEM_INFO info;
    DWORD dwPageSize;
    FARPROC caller;
    BOOL retVP;
    BOOL retVF;
    DWORD dummy;
    unsigned int size1;
    unsigned int size2;

    // Get page size of current system.
    GetSystemInfo(&info);
    dwPageSize = info.dwPageSize;

    // Allocate a page of memory (RW).
    caller = (FARPROC)VirtualAlloc(
        NULL,
        dwPageSize,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );
    if (!caller)
    {
        printf("Memory allocation failed for caller");
        exit(1);
    }

    size1 = ((((unsigned int)callee1) + (ESTIMATED_FUNC_SIZE - 1)) / dwPageSize + 1) * dwPageSize - ((unsigned int)callee1);

    // Copy code to caller
    memcpy(caller, (void *)&callee1, size1);

    // Change memory protection to executable only.
    retVP = VirtualProtect(
        caller,
        dwPageSize,
        PAGE_EXECUTE,
        &dummy
    );
    if (!retVP)
    {
        printf("VirtualProtect failed");
        exit(1);
    }

    // Invoke caller, in which callee1 is called.
    ((FUNCPTR)caller)();

    // Change memory protection to RWX.
    retVP = VirtualProtect(
        caller,
        dwPageSize,
        PAGE_EXECUTE_READWRITE,
        &dummy
    );
    if (!retVP)
    {
        printf("VirtualProtect failed");
        exit(1);
    }

    size2 = ((((unsigned int)callee2) + (ESTIMATED_FUNC_SIZE - 1)) / dwPageSize + 1) * dwPageSize - ((unsigned int)callee2);

    // Copy code to caller.
    memcpy(caller, (void *)&callee2, size2);

    // Invoke caller. This time caller2 is called.
    ((FUNCPTR)caller)();

    // Free the page allocated.
    retVF = VirtualFree(caller, 0, MEM_RELEASE);
    if (!retVF)
    {
        printf("VirtualFree() failed freeing caller");
        exit(1);
    }

    if (flag_1 && flag_2)
    {
        printf("mem test passed\n");
    }
    else
    {
        printf("mem test failed\n");
    }

    return 0;
}

void callee1()
{
    flag_1 = TRUE;
}

void callee2()
{
    flag_2 = TRUE;
}
