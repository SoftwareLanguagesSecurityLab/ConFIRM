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

#include "helper.h"

static int count_even = 0;
static int count_odd = 0;
NANOSECOND start_time;
NANOSECOND end_time;
NANOSECOND total_time;

// Declare two functions with same signature.
void printEven(int n);
void printOdd(int n);

int main()
{
    // Declare a function pointer. Each call via this pointer would be a indirect call.
    // It can be either a register-indirect call or a memory-indirect call based on 
    // different compiler options.
    void(*fptr) (int n);

    // Initialize random seed
    srand((unsigned int)time(NULL));
    // Record starting time
    start_time = get_wall_time();
    
    // call functions via a function pointer in an intensive loop.
    for (long long int i = 0; i < MAX_LOOP * FPTRTS; i++)
    {
        // Depending on parity of a randomized number, fptr(r) calls different functions.
        int r = rand();
        if (r % 2 == 0)
            fptr = printEven;
        else
            fptr = printOdd;
        fptr(r);
    }

    // Record ending time, and calculate running time elapsed in the loop.
    end_time = get_wall_time();
    total_time = end_time - start_time;

    // Print time elapsed in the loop.
    printf("total time in nanoseconds is %llu\n", (long long unsigned int) total_time);
    
    // Print counts of even and odd numbers so that a compiler doesn't optimize too much.
    printf("%d odd numbers\n", count_odd);
    printf("%d even numbers\n", count_even);

    return 0;
}

void printEven(int n)
{
    count_even++;
}
void printOdd(int n)
{
    count_odd++;
}
