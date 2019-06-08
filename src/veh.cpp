/**************************************************************************************************
 * Copyright (c) 2019 Xiaoyang Xu, Masoud Ghaffarinia, Wenhao Wang, Kevin W. Hamlen, Zhiqiang Lin *
 *                                                                                                *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software  *
 * and associated documentation files (the "Software"), to deal in the Software without           * 
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,     *
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the  *
 * Software is furnished to do so, subject to the following conditions:                           *
 *                                                                                                *
 * The above copyright notice and this permission notice shall be included in all copies or       *
 * substantial portions of the Software.                                                          *
 *                                                                                                *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING  *
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND     *
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        *
 **************************************************************************************************/

// This benchmark is Windows specific.
 
#include "helper.h"

// Define a vectored exception handler.
LONG WINAPI VectoredHandlerSkip(struct _EXCEPTION_POINTERS *ExceptionInfo);

int main()
{
    DWORD loop_count = 1024L;
    PVOID pv = NULL;

    // Register a vectored exception handler.
    pv = AddVectoredExceptionHandler(0, VectoredHandlerSkip);

    // Triger a hardware exception by writing to DS:[0] in a loop, to check if 
	// a CFI solution being tested provides semantic transparency.
    __asm {
        mov eax, 0
        mov ecx, loop_count
        L : mov [eax], 0
            loop L
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
    // The instruction "mov [eax], 0" has length of 3, eip += 3 will redirect 
    // the control flow to the next "loop L" instruction.
    Context->Eip = Context->Eip + 3;
	// Continue execution with the new EIP value.
    return EXCEPTION_CONTINUE_EXECUTION;
}