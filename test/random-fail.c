/* -*- mode: c; tab-width: 2; indent-tabs-mode: nil; -*-
Modified by Erik Engstrom 1/24/2020
Copyright (c) 2012 Marcus Geelnard

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.

   Erik Engstrom 1/24/2020
   Update the main function and added the addOneThread based on "Fallacy 3"  
   and Fiugre 5.4 "A simple program" by Chris Hobbs 
   Embedded Software Development for Safety-Critical Systems (p. 69). CRC Press. Kindle Edition.

   Quote below 
   
   "Fallacy 3 It is often said (and repeated in many standards) that  software does not 
   fail randomly — all software failure is systematic  and anyone analyzing software failure
   statistically falls into a state of sin. There are many academic papers explaining why
   this is a fallacy,  but one simple program should be enough: see Figure 5.4.
   This program, when executed on any computer, particularly one with  multiple cores, 
   will crash with a floating point exception every few  thousands of millions of times it is executed.
   The presence of other programs also executing on the same computer will change the probability of failure." 
   
   As threads 1 and 2 job increase in size the time to failiure decreases
   
*/

#include <stdio.h>
#include <tinycthread.h>

volatile int x = 0;

/* This is the child thread function */
int addOneThread(void * aArg)
{
  //(void)aArg;
  //printf("Enter Thread %d \n", aArg);
  int y;  
  
  for (int i = 0; i < aArg; i++)
  {
      y = x+1;
      x = y;
  }
   
  //printf("Exit Thread %d \n", aArg);
  return 0;
}

/* This is the main program (i.e. the main thread) */
int main()
{
  /* Start the child thread */
  thrd_t t1;
  thrd_t t2;

#define T1_JOB 1000
#define T2_JOB 3000

  
  printf("Thread1 Job Size is %d\n", T1_JOB);
  printf("Thread2 Job Size is %d\n", T2_JOB);
  printf("Decreasing the job size greatly increase the time it takes for a failure\n\n");

  for (int cycles=0; cycles <5; cycles++)
  {
      int results = T1_JOB + T2_JOB;
      int loop_count = 1;
      // This loop keeps running to an unexpected occurance occurs
      while (results == T1_JOB + T2_JOB)
      {
          int t1State = thrd_create(&t1, addOneThread, (void*)T1_JOB);
          int t2State = thrd_create(&t2, addOneThread, (void*)T2_JOB);

          if (t1State == thrd_success)
          {
              /* Wait for the thread to finish */
              thrd_join(t1, NULL);
          }

          if (t2State == thrd_success)
          {
              /* Wait for the thread to finish */
              thrd_join(t2, NULL);
          }
          results = x;
          x = 0;
          loop_count++;
      }
      printf("After %d interations X = %d instead of expected %d\n", loop_count, results, T1_JOB + T2_JOB);
      //printf("%d\n", 18827 / (x - 5));   Original failure point with jobs both set to 100
  }
  
  return 0;
}
