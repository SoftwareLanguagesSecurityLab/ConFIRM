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

void callee1();
void callee2();

typedef void(*FUNCPTR)();

BYTE code[] = {
    // Prologue
    0x55,             // push ebp/rbp
    0x48,             // dec  eax if compiled for 32bit otherwise extend ebp and esp to rbp and rsp for next ins
    0x8B, 0xEC,       // mov  ebp, esp
    0x53,             // push ebx/rbx
    0x56,             // push esi/rsi
    0x57,             // push edi/rdi

    // Computed call to a function in the untrusted module
    0x48,             // dec  eax if compiled for 32bit otherwise extend ebx to rbx for next ins
    0xbb,             // mov  ebx, 0x00000000
    0x00,
    0x00,
    0x00,
    0x00,
    0x90,
    0x90,
    0x90,
    0x90,             // 8 bytes of address need to be patched -- NOP is added so that it works for both 32-bit and 64-bit machine
    0xff, 0x13,       // call ptr [ebx/rbx]

    // Epilogue
    0x5F,             // pop edi/rdi
    0x5E,             // pop esi/rsi
    0x5b,             // pop ebx/rbx
    0x48,             // dec  eax if compiled for 32bit otherwise extend ebp and esp to rbp and rsp for next ins
    0x8B, 0xE5,       // mov esp, ebp
    0x5D,             // pop ebp/rbp
    0xC3              // retn
};

int main()
{
    SYSTEM_INFO info;
    DWORD dwPageSize;
    FARPROC caller;
    BOOL retVP;
    BOOL retVF;
    DWORD dummy;

    // Get page size of current system.
    GetSystemInfo(&info);
    dwPageSize = info.dwPageSize;

    // Allocate a page of memory (RW).
    caller = (FARPROC)VirtualAlloc(NULL, dwPageSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!caller)
    {
        printf("Memory allocation failed for caller");
        exit(1);
    }

    // Patch code with callee1.
    FUNCPTR pcallee1 = &callee1;
    void *ppcallee1 = &pcallee1;
    memcpy(&(code[9]), &ppcallee1, sizeof(pcallee1));

    // Copy code to caller
    memcpy(caller, code, sizeof(code));

    // Change memory protection to executable only.
    retVP = VirtualProtect(caller, dwPageSize, PAGE_EXECUTE, &dummy);
    if (!retVP)
    {
        printf("VirtualProtect failed");
        exit(1);
    }

    // Invoke caller, in which callee1 is called.
    ((FUNCPTR)caller)();

    // Change memory protection to RWX.
    retVP = VirtualProtect(caller, dwPageSize, PAGE_EXECUTE_READWRITE, &dummy);
    if (!retVP)
    {
        printf("VirtualProtect failed");
        exit(1);
    }

    // Patch code with callee2.
    FUNCPTR pcallee2 = &callee2;
    void *ppcallee2 = &pcallee2;
    memcpy(&(code[9]), &ppcallee2, sizeof(pcallee2));

    // Copy code to caller.
    memcpy(caller, code, sizeof(code));

    // Invoke caller. This time caller2 is called.
    ((FUNCPTR)caller)();

    // Free the page allocated
    retVF = VirtualFree(caller, 0, MEM_RELEASE);
    if (!retVF)
    {
        printf("VirtualFree() failed freeing caller");
        exit(1);
    }

    return 0;
}

void callee1()
{
    printf("This is a message in callee1()\n");
}

void callee2()
{
    printf("This is a message in callee2()\n");
}