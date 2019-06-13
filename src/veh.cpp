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

// This benchmark is Windows specific.
 
#include "setup.h"

// Define a vectored exception handler.
LONG WINAPI VectoredHandlerSkip(struct _EXCEPTION_POINTERS *ExceptionInfo);

int main()
{
    PVOID pv = NULL;

    // Register a vectored exception handler.
    pv = AddVectoredExceptionHandler(0, VectoredHandlerSkip);

    // Trigger a hardware exception by writing to DS:[0] in a loop, to check whether 
    // a CFI solution being tested provides semantic transparency.
    for (long long int i = 0; i < MAX_LOOP * SEHTS; ++i)
    {
        *(unsigned int*)0x0 = 0;
    }

    // Remove the vectored exception handler.
    RemoveVectoredExceptionHandler(pv);

    // Print results
    printf("VEH test passed\n");
    return 0;
}

LONG WINAPI VectoredHandlerSkip(struct _EXCEPTION_POINTERS *ExceptionInfo)
{
    PCONTEXT Context;    
    Context = ExceptionInfo->ContextRecord;
    
    // Add program counter by 1
#ifdef AMD64
    Context->Rip = Context->Rip + 1;
#elif defined INTEL_X86
    Context->Eip = Context->Eip + 1;
#else 
#error This benchmark contains x86/x64-specific assembly code that may be \
incompatible with the current hardware architecture.
#endif    
    
    // Continue execution with the new EIP value.
    return EXCEPTION_CONTINUE_EXECUTION;
}
