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

static HANDLE hChild;
static DWORD trials, hijack_addr;
static volatile int hijacked = 0;

// This function runs the child thread that attempts to hijack the main thread.
// The retaddr_ptr argument points to the main thread's return address slot.
DWORD WINAPI ChildThread(LPVOID retaddr_ptr)
{
    // Repeatedly write <hijack_addr> into the main thread's return address slot.
    // The write is attempted <trials> times.
    _asm {
        mov eax, retaddr_ptr
        mov ebx, hijack_addr
        mov ecx, trials
        L : mov [eax], ebx
            loop L
    }

    // If the hijack was successful, the main thread sets the global "hijacked" variable to 1.
    if (!hijacked)
    {
        printf("All trials complete.  Hijack unsuccessful.\n");
		// The main thread loops infinitely, so to terminate it, we use ExitProcess.
        ExitProcess(0);  
    }
	
    return 0;
}

int main(int argc, char* argv[])
{
    DWORD idChild;
    LPVOID esp_addr;

    printf("Enter number of trials: ");
    scanf_s("%u", &trials);

    _asm {
        mov hijack_addr, offset HIJACK    // Store the address to which the hijacker should redirect in a global var.
        lea eax, [esp - 4]                // Store the address of the forthcoming return address in esp_addr.
        mov esp_addr, eax
    }
	
	// Spawn the child (hijacker) thread.
    hChild = CreateThread(          
        NULL,
        0,
        ChildThread,
        esp_addr,
        0,
        &idChild);
    if (!hChild) abort();

    // Repeat a guarded return in the tightest possible infinite loop.
    _asm {
        mov eax, offset L2
        L1 : call L3
             L2 : jmp L1
                  L3 : mov[esp], eax
                       ret
    }

    printf("Infinite loop ended normally---impossible!\n");
    abort();

    // If the hijacker is successful, the "ret" above will jump here.
HIJACK:
    hijacked = 1;
	
    _asm {
        push 0    // The child thread might still be running, so push a dummy return address for it to continue hijacking.
    }
	
    printf("Hijack successful!\n");
	
	// Wait for the child thread to terminate normally.
    WaitForSingleObject(hChild, INFINITE);  
	
	// Pop the dummy return address.
    _asm { pop eax }  
	
    return 0;
}
