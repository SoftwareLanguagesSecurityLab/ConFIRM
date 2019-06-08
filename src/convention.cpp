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

// Caller-pop, variadic function.
int CDECL variadicSum(int count, ...);

// Callee-pop function.
int STDCALL listSum(int count, const int * list);

// Callee-pop FASTCALL function. 
// Caller tries to pass first two arguments using registers.
int FASTCALL fastSum(int count, const int * list);

// Callee-pop THISCALL functions. 
// "this" pointer is passed to callee via ecx. Other arguments are pushed onto the stack.
class CMath
{
private:
    int * list;
    int count;
public:
    int THISCALL calculateSum();
    int THISCALL calculateAverage();
    void THISCALL setList(const int list_count, const int * int_list);
};

// 64-bit conventions allow up to 14 and 4 parameters passing in registers, 
// on Linux and Windows respectively. They both use caller-pop conventions by default.
// int and float arguments are passed using different registers. 
int IntSum(int count, ...);
double DoubleSum(int count, ...);

int main()
{
    // Call a CDECL function.
    int variadicSum_ans = 0;
    variadicSum_ans = variadicSum(5, 1, 2, 3, 4, 5);
    if (variadicSum_ans == 15)
    {
        printf("CDECL passed\n");
    }
    else
    {
        printf("CDECL failed!\n");
        return -1;
    }


    // Call a STDCALL function.
    int listSum_ans = 0;
    int int_list1[5] = { 1, 3, 5, 7, 9 };
    listSum_ans = listSum(sizeof(int_list1) / sizeof(int), int_list1);
    if (listSum_ans == 25)
    {
        printf("STDCALL passed\n");
    }
    else
    {
        printf("STDCALL failed!\n");
        return -1;
    }

    // Call a FASTCALL function
    int fastSum_ans = 0;
    int int_list2[4] = { 5, 3, 5, 1 };
    fastSum_ans = fastSum(sizeof(int_list2) / sizeof(int), int_list2);
    if (fastSum_ans == 14)
    {
        printf("FASTCALL passed\n");
    }
    else
    {
        printf("FASTCALL failed!\n");
        return -1;
    }

    // Call THISCALL functions
    CMath * myCMath = new CMath;
    int int_list3[6] = { 2, 1, 6, 3, 8, 9 };

    myCMath->setList(sizeof(int_list3) / sizeof(int), int_list3);
    int sum = myCMath->calculateSum();
    int avg = myCMath->calculateAverage();
    if ((sum == 29) && (avg == 4))
    {
        printf("THISCALL passed\n");
    }
    else
    {
        printf("THISCALL failed!\n");
        return -1;
    }

    // Tests for 64-bit calling conventions. 
    int intSum_ans = 0;
    intSum_ans = IntSum(16, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);

    double doubleSum_ans = 0.0;
    doubleSum_ans = DoubleSum(16, 1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9, 10.10, 11.11, 12.12, 13.13, 14.14, 15.15, 16.16);

    if ((intSum_ans == 136) && (doubleSum_ans - 141.41 <= 0.000000000001))
    {
        printf("64bit conventions passed\n");
    }
    else
    {
        printf("64bit conventions failed!\n");
        return -1;
    }

    printf("All conventions passed\n");
    return 0;
}

int THISCALL CMath::calculateAverage()
{
    int sum = 0;
    int avg = 0;

    sum = this->calculateSum();
    avg = sum / this->count;
    return avg;
}

int THISCALL CMath::calculateSum()
{
    int sum = 0;
    for (int i = 0; i < this->count; i++)
        sum += (this->list)[i];

    return sum;
}

void THISCALL CMath::setList(const int list_count, const int * int_list)
{
    this->list = new int[list_count];
    this->count = list_count;

    for (int i = 0; i < list_count; i++)
        (this->list)[i] = int_list[i];
}

int FASTCALL fastSum(int count, const int * list)
{
    int sum = 0;
    int i = 0;

    for (int i = 0; i < count; i++)
        sum += list[i];

    return sum;
}

int STDCALL listSum(int count, const int * list)
{
    int sum = 0;
    int i = 0;

    for (int i = 0; i < count; i++)
        sum += list[i];

    return sum;
}

int CDECL variadicSum(int count, ...)
{
    va_list ap;
    int sum = 0;

    va_start(ap, count);
    for (int i = 0; i < count; i++)
        sum += va_arg(ap, int);
    va_end(ap);

    return sum;
}

int IntSum(int count, ...)
{
    va_list ap;
    int sum = 0;

    va_start(ap, count);
    for (int i = 0; i < count; i++)
        sum += va_arg(ap, int);
    va_end(ap);

    return sum;
}

double DoubleSum(int count, ...)
{
    va_list ap;
    double sum = 0.0;
    double next = 0.0;

    va_start(ap, count);
    for (int i = 0; i < count; i++)
        sum += va_arg(ap, double);
    va_end(ap);

    return sum;
}