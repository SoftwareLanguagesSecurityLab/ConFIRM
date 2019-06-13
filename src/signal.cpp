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

#include <signal.h>
#include <setjmp.h>
#include <stdio.h>
#include "setup.h"

sigjmp_buf mark;

// Define a hanlder that restores the environment saved by sigsetjmp.
void myhandler(int signo) {
    siglongjmp(mark, -1);
}

// A function that writes to invalid address.
void exception_loop(void) {
    char *p = NULL;
    *p = 5;
}

int main(void) {
    // Set up a signal handler that handles invalid memory reference (SIGSEGV).    
    struct sigaction myhandle;
    myhandle.sa_handler = myhandler;
    sigemptyset(&myhandle.sa_mask);
    myhandle.sa_flags = SA_NODEFER;
    sigaction(SIGSEGV, &myhandle, NULL);
    
    // Write to invalid address in an intensive loop, to see if a CFI solution being tested
    // provides semantic transparency.
    long long int i = 0;
    sigsetjmp(mark, 0);
    if (i++ < MAX_LOOP * LHWXTS)
        exception_loop();

    printf("signal test passed.\n");

    return 0;
}
