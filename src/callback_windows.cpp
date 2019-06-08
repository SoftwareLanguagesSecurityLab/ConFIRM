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

static int count_1 = 0, count_2 = 0, count_3 = 0;
NANOSECOND start_time;
NANOSECOND end_time;
NANOSECOND total_time;

// Define three callback functions 
DWORD WINAPI theadProc1(LPVOID lpParam);
DWORD WINAPI theadProc2(LPVOID lpParam);
DWORD WINAPI theadProc3(LPVOID lpParam);

int main()
{
    HANDLE h1 = 0;
    HANDLE h2 = 0;
    HANDLE h3 = 0;
    HANDLE handles[3];

    // Record starting time
    start_time = get_wall_time();
    
    // HANDLE CreateThread(
        // LPSECURITY_ATTRIBUTES   lpThreadAttributes,
        // SIZE_T                  dwStackSize,
        // LPTHREAD_START_ROUTINE  lpStartAddress,
        // __drv_aliasesMem LPVOID lpParameter,
        // DWORD                   dwCreationFlags,
        // LPDWORD                 lpThreadId
    // );    
    // The CreateThread() API (signature above) takes a callback pointer (lpStartAddress)
    // that points to a application-defined function to be executed by the new created thread.
    
    // Create threads and wait for them to terminate in an intensive loop.
    for (long long int i = 0; i < MAX_LOOP * CALTS; i++)
    {
        h1 = CreateThread(NULL, 0, theadProc1, NULL, 0, NULL);
        h2 = CreateThread(NULL, 0, theadProc2, NULL, 0, NULL);
        h3 = CreateThread(NULL, 0, theadProc3, NULL, 0, NULL);

        // Wait for the treads to terminate and close handles.		
        handles[0] = h1;
        handles[1] = h2;
        handles[2] = h3;
        WaitForMultipleObjects(3, handles, TRUE, INFINITE);
        CloseHandle(h1);
        CloseHandle(h2);
        CloseHandle(h3);
    }

    // Record ending time
    end_time = get_wall_time();
    total_time = end_time - start_time;

    // Print time elapsed in the loop.  
    printf("total time in nanoseconds is %llu\n", (long long unsigned int) total_time);
	
    // Print counts so that a compiler doesn't optimize too much.
    printf("%d, %d, %d", count_1, count_2, count_3);

    return 0;
}

DWORD WINAPI theadProc1(LPVOID lpParam)
{
    count_1++;
    return NULL;
}

DWORD WINAPI theadProc2(LPVOID lpParam)
{
    count_2++;
    return NULL;
}

DWORD WINAPI theadProc3(LPVOID lpParam)
{
    count_3++;
    return NULL;
}
