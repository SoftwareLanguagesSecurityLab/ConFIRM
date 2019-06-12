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

// This micro benchmark emulates a specific Windows API hooking behavior observed in 
// various component-driven software, including Microsoft Office software. Please 
// see the ConFIRM paper for detailed explanation. This benchmark redirects function 
// NtClose to a wrapper function, to show this API hooking behavior.

#pragma warning( disable : 4005)
#include "helper.h"
#include <NTSTATUS.H>

// Make realNtClose a global variable as it's delared in AppvIsvSubsystems32.dll
static FARPROC realNtClose;
// A global randomized integer
static int r;

void cpyInt(DWORD startAddr, DWORD dwSize);
void hookedNtClose();

int main(void) {
    HANDLE ghSemaphore;
    HMODULE hNtdll;
    SYSTEM_INFO info;
    DWORD dwPageSize;
    DWORD oldProtect;
    BOOL retVP; 
    BOOL retCH;    
    NTSTATUS status;
    NTSTATUS(*pNtClose) (HANDLE);

    // Get page size of current system.
    GetSystemInfo(&info);
    dwPageSize = info.dwPageSize;

    // ntdll.dll should be loaded.
    hNtdll = GetModuleHandle(L"ntdll.dll");
    if (!hNtdll)
    {
        printf("ntdll is not loaded");
        exit(1);
    }

    // Get function pointer to ntdll::NtClose.
    // pNtClose      ->    mov eax, 0c
    // pNtClose+5    ->    xor ecx, ecx
    //             ...
    pNtClose = (NTSTATUS(*)(HANDLE))GetProcAddress(hNtdll, "NtClose");
    if (!pNtClose)
    {
        printf("GetProcAddress() failed requesting function NtClose()");
        exit(1);
    }

    // Allocate memory for realNtClose.
    realNtClose = (FARPROC)VirtualAlloc(
        NULL, 
        dwPageSize, 
        MEM_COMMIT | MEM_RESERVE, 
        PAGE_EXECUTE_READWRITE
        );
    if (!realNtClose)
    {
        printf("VirtualAlloc() failed allocating for realNtClose()");
        exit(1);
    }

    // Fill the allocated page by 0xCC (INT)
    cpyInt((DWORD)realNtClose, dwPageSize);

    // Write to realNtClose. 
    // After writing:
    // realNtClose   -> mov eax, 0c    (same to the first 5 bytes of NtClose)
    // realNtClose+5 -> jmp pNtClose+5 (xor ecx, ecx)
    //                  INT            (should never executed)
    __asm {
        // Copy the first 4 bytes from NtClose (in ntdll) to realNtClose.
        mov esi, pNtClose
        mov edi, realNtClose
        mov eax, dword ptr[esi]
        mov dword ptr[edi], eax

        // Copy the fifth byte
        add esi, 4
        add edi, 4
        mov al, byte ptr[esi]
        mov byte ptr[edi], al

        // Write a jmp instruction at offset 5 of realNtClose.   
        inc edi
        mov byte ptr[edi], 0xe9
        mov eax, edi
        add eax, 5
        inc edi
        inc esi
        sub esi, eax
        mov dword ptr[edi], esi
    }

    // Make the first 5 bytes of ntdll::NtClose R+W+X.
    retVP = VirtualProtect(pNtClose, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    if (!retVP)
    {
        printf("VirtualProtect() failed on ntdll::NtClose -> R+W+X");
        exit(1);
    }

    // Write the fst 5 bytes of ntdll::NtClose to force it jump to hookedNtClose.
    // After writing:
    // pNtClose   -> jmp hookedNtClose
    // pNtClose+5 -> xor ecx, ecx
    //             ... 
    __asm {
        mov edi, pNtClose
        mov byte ptr[edi], 0xe9
        mov ecx, hookedNtClose
        mov eax, edi
        inc edi
        add eax, 5
        sub ecx, eax
        mov dword ptr[edi], ecx
    }

    // Make the first 5 bytes of ntdll::NtClose E+WriteCopy.
    retVP = VirtualProtect(pNtClose, 5, PAGE_EXECUTE_WRITECOPY, &oldProtect);
    if (!retVP)
    {
        printf("VirtualProtect() failed on ntdll::NtClose -> E+WriteCopy");
        exit(1);
    }

    // Create a semaphore and then close the semaphore handle by calling NtClose().
    ghSemaphore = CreateSemaphore(
        NULL,
        1,
        1,
        NULL);

    // Initialize seed
    srand((unsigned int)time(NULL));
    // Close sematphore handle
    do
    {
        r = rand();
        printf("Trying to close semaphore handle...\n");
        status = pNtClose(ghSemaphore);
    } while (status != STATUS_SUCCESS);

    printf("Semaphore handle closed.\n");

    return 0;
}

void cpyInt(DWORD startAddr, DWORD dwSize)
{
    __asm {
        mov eax, 0xCCCCCCCC
        mov edi, startAddr
        mov ecx, dwSize
        shr ecx, 2        
        rep stos dword ptr[edi] // this is semanticly equivelent to "rep stosd"
    }
}

// If randomized int r is even -> return STATUS_UNSUCCESSFUL
//                   r is odd  -> jump to realNtClose
__declspec(naked)
void hookedNtClose()
{
    if (r % 2 == 0)
    {
        __asm {
            mov eax, 0xC0000001
            ret
        }
    }

    __asm {
        jmp realNtClose
    }

    printf("Program should not reach here --- impossible!");

    __asm {
        ret
    }
}
