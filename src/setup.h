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
   
// This is the master configuration file. 
// Users of ConFIRM can specialize their own parameters.
   
#pragma once

// Benchmarks in Windows and Linux require different header files
#ifdef _WIN32
#include <iostream>
#include <windows.h>
#include <time.h>
#include <random>

#elif __linux__
#include <iostream>
#include <time.h>
#include <stdio.h> 
#include <limits.h>
#include <stdint.h>
#include <cstdlib>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#define FALSE false
#define BYTE unsigned char

#else
#endif

// Loop iteration counts for different benchmarks.
// With the current counts, each benchmark runs around 2 min.
#define MAX_LOOP 1024L //base loop count
#define CPEHTS 5       //cppeh
#define SEHTS 5       //seh
#define VEHTS 5        //veh
#define DYNLTS 0.3     //run_time_dynlnk
#define FPTRTS 500     //fptr
//#define IMPDTS 6500    //data_symbl
#define INDCTS 360     //tail_call
#define JITCTS 2500
#define CALTS 0.4      //callback
#define LHWXTS 20      //signal
#define LTTX 1.2       //load_time_dynlnk
#define RETTS 5700     //ret
#define SWTCTS 590     //switch
#define VTABTS 460     //vtbl_call
#define DLTS 500

// Macros for different calling conventions in Windows and Linux
#ifdef _WIN32
#define CDECL __cdecl
#define STDCALL __stdcall
#define FASTCALL __fastcall
#define THISCALL __thiscall

#elif __linux__
#define CDECL __attribute__((cdecl))
#define STDCALL
#define FASTCALL __attribute__((fastcall))
#define THISCALL __attribute__((thiscall))

#else
#endif

// A helper function that can be used to calculate elapsed time in nanosecond.
#define NANOSECOND uint64_t
#define BILLION 1000000000L
extern NANOSECOND get_wall_time();

// Pre-defined compiler macros for x86/x64
#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64) 
#define AMD64
#endif

#if defined(i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__i386) || defined(__IA32__) || defined(_M_IX86) || defined(__X86__) || defined(_X86_) || defined(__I86__) || defined(__INTEL__) || defined(__386) || defined(__THW_INTEL__)
#define INTEL_X86
#endif

// Pre-defined compiler macros for processor architectures supported by different compilers.
#if defined(_M_ALPHA) || defined(_M_AMD64) || defined(_M_X64) || defined(_M_ARM) || defined(_M_ARMT) || defined(_M_I86) || defined(_M_IX86) || defined(_M_IA64) || defined(_M_PPC)
#define MSVC_ARCH
#endif

#if defined(__alpha__) || defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(__arm__) || defined(__thumb__) || defined(__aarch64__) || defined(__bfin) || defined(__BFIN__) || defined(__convex__) || defined(__hppa__) || defined(i386) || defined(__i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(__m68k__) || defined(__mips__) || defined(mips) || defined(__mips) || defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__ppc64__) || defined(__PPC__) || defined(__PPC64__) || defined(_ARCH_PPC) || defined(_ARCH_PPC64) || defined(__sparc__) || defined(__sparc_v8__) || defined(__sparc_v9__) || defined(__sh__) || defined(__s390__) || defined(__s390x__)
#define GNU_ARCH
#endif

#if defined(__zarch__) || defined(__x86_64__) || defined(__ARM_ARCH) || defined(__arm__) || defined(__aarch64__)
#define CLANG_ARCH
#endif
