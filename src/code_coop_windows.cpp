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
 
// For a detailed explanation of CODE-COOP attack, see https://www.utdallas.edu/~kxh060100/wang17ccs.pdf

#include "helper.h"
#include "wininet.h"
#include "winnls.h"
#include "shobjidl.h"
#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"
#include "shlobj.h"

#pragma warning ( disable : 4789 )

int main()
{
    HRESULT hres;
    IShellLink* pad;

    CoInitialize(NULL);
    
    // CoCreateInstance() obtains an object reference (pad) of ActiveDesktop. And later QueryInterface() 
    // obtains a new interface (puk) to ActiveDesktop.
    hres = CoCreateInstance(CLSID_ActiveDesktop, NULL, CLSCTX_INPROC_SERVER, IID_IActiveDesktop, (LPVOID*)&pad);

    if (SUCCEEDED(hres))
    {
        // Code containing a data corruption vulnerability
        char buffer[1];
        printf("Any input can lead to buffer overwrite: \n");
        scanf("%s", buffer);

        // CFI solutions guarantee that the following function call targets a valid QueryInterface().
        // Unfortunately, every implementation of QueryInterface() calls other virtual function(e.g. AddRef())
        // by default. With the above data corruption vulnerability, an attacker can tailer counterfeit
        // object and then redirect control flow to arbitrary code. 
        IPersistFile* puk;
        hres = pad-> QueryInterface(IID_IUnknown, (LPVOID*)&puk);
        if (SUCCEEDED(hres))
        {
            printf("QueryInterface succeed.\n");
        }
        else
        {
            printf("QueryInterface failed.\n");
        }
    }
    else
    {
        printf("CoCreateInstance failed.\n");
    }

    CoUninitialize();

    return 0;
}

