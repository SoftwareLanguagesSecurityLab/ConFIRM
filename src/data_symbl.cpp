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
#include "inc.h"

int main()
{
    int imp_null = -1;
    DWORD p_exp_null = NULL;
    
    // IAT data movement
    imp_null = (int) exp_null;
    // If imp_null is not NULL, the enfoced CFI framework must have done something wrong.
    if (imp_null != 0)
    {
        printf("imp_null should be NULL (current value: %d).\n", imp_null);
        exit(1);
    }    
    
    // Check if an exported symbol can be read correctly via pointer.
    p_exp_null = &exp_null;
    if (*p_exp_null != 0)
    {
        printf("*p_exp_null should be NULL (current value: %d).\n", *p_exp_null);
        exit(1);
    }
    
    // Check if the exported symbol can still be read correctly after some wierd pointer arithmetic. 
    p_exp_null ++;
    p_exp_null += (DWORD)(imp_null * 1024);
    p_exp_null --;
    if (*p_exp_null != 0)
    {
        printf("*p_exp_null should be NULL (current value: %d).\n", *p_exp_null);
        exit(1);
    }
    
    printf("All tests passed.\n");
    return 0;
}
