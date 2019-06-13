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

void writeToMem();
static int except_count = 0;
static int finally_count = 0;

int main()
{
    // Throw access violation exception and handle it using SEH in an intensive loop, 
    // to see if a CFI solution being tested provides semantic transparency.
    for (long long int i = 0; i < MAX_LOOP * SEHTS; ++i)
    {
        __try
        {
            writeToMem();
        }
        __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ?
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
        {
            except_count++;
        }
    }
    
    // Print results.
    printf("except_count is %d\nfinally_count is %d\n ", except_count++, finally_count);
    printf("SEH test passed.");

    return 0;
}

// A function that writes to invalid address. 
void writeToMem()
{
    __try
    {
        __try
        {
            *(unsigned int*)0x0 = 0;
        }
        __finally
        {
            finally_count++;
        }
    }
    __except (GetExceptionCode() == EXCEPTION_FLT_DIVIDE_BY_ZERO ?
        EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
    {
        printf("This should not be printed --- impossible!\n");
    }
}
