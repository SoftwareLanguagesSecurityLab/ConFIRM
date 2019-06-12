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

static int count_0 = 0, count_1 = 0, count_2 = 0;
NANOSECOND start_time;
NANOSECOND end_time;
NANOSECOND total_time;

// Define three callback functions 
void *theadProc0(void *);
void *theadProc1(void *);
void *theadProc2(void *);

int main()
{
    pthread_t threads[3];

    // Record starting time
    start_time = get_wall_time();

    // int pthread_create(
        // pthread_t *thread, 
        // const pthread_attr_t *attr,
        // void *(*start_routine) (void *), 
        // void *arg);
    // pthread_create() (signature above) takes a callback pointer (start_routine) 
    // that points to a application-defined function to be executed by the new 
    // created thread.
    
    // Create threads in an intensive loop.
    for (long long int i = 0; i < MAX_LOOP * CALTS; i++)
    {
        pthread_create(&threads[0], NULL, &theadProc0, NULL);
        pthread_create(&threads[1], NULL, &theadProc1, NULL);
        pthread_create(&threads[2], NULL, &theadProc2, NULL);
    }

    // Record ending time
    end_time = get_wall_time();
    total_time = end_time - start_time;

    // Print time elapsed in the loop.     
    printf("total time in nanoseconds is %llu\n", (long long unsigned int) total_time);
    
    // Print counts so that a compiler doesn't optimize too much.
    printf("%d, %d, %d\n", count_0, count_1, count_2);

    pthread_exit(NULL);
}

void *theadProc0(void *)
{
    count_0++;
    pthread_exit(NULL);
}

void *theadProc1(void *)
{
    count_1++;
    pthread_exit(NULL);
}

void *theadProc2(void *)
{
    count_2++;
    pthread_exit(NULL);
}
