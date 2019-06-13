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

// Base class
class base
{
public:
    int count_even = 0;
    int count_odd = 0;

    virtual int random() { return 0;  };
    virtual void accEven() {};
    virtual void accOdd() {};
    virtual bool isOdd(int) { return FALSE;  };
};

// Derived class
class derived: public base
{
public:
    int random()
    {
        return rand();
    }

    void accEven()
    {
        count_even++;
    }

    void accOdd()
    {
        count_odd++;
    }

    bool isOdd(int n)
    {
        return (n % 2);
    }
};

NANOSECOND start_time;
NANOSECOND end_time;
NANOSECOND total_time;

int main()
{        
    // Define a derived class instance and a pointer that has type of base class
    derived p;
    base *pn = &p;    
    int n;

    // Initialize random seed
    srand((unsigned int)time(NULL));
    // Record starting time
    start_time = get_wall_time();

    // Call virtual functions in an intensive loop
    for (long long int i = 0; i < MAX_LOOP * VTABTS; ++i)
    {
        n = pn->random();
        if (pn->isOdd(n))
        {
            pn->accOdd();
        }
        else
        {
            pn->accEven();
        }
    }

    // Record ending time
    end_time = get_wall_time();
    total_time = end_time - start_time;
    
    // Print time elapsed in the loop.
    printf("total time in nanoseconds is %llu\n", (long long unsigned int) total_time);
    
    // Print counts so that a compiler doesn't optimize too much.
    printf("%d odd numbers\n", pn->count_odd);
    printf("%d even numbers\n", pn->count_even);

    return 0;
}
