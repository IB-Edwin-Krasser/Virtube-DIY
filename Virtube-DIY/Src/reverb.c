/** @file reverb.c
  * simple reverb calculation
  *
  * @author Edwin Krasser, Ingenieurb&uuml;ro f&uuml;r Elektrotechnik, Schwerpunkt F&amp;E Elektronik
  * @version 1.0
  *
  * Virtube – Non-Commercial License
  * Copyright © 2025 Edwin Krasser (ib.ekrasser@gmail.com)
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to use,
  * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  * the Software **for non-commercial purposes only** (private use, education,
  * research, non-profit projects).
  *
  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.
  *
  * **Commercial use is explicitly prohibited**, including but not limited to:
  * - sale of the Software or derivatives
  * - incorporation into commercial products
  * - production or distribution for profit
  * - any revenue-generating activity
  *
  * For any commercial licensing, distribution or use inquiries, please contact:
  * ib.ekrasser@gmail.com
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  * THE SOFTWARE. */

#ifndef __REVERB_C_INCLUDED
#define __REVERB_C_INCLUDED

#define HEADER_GLOBAL
#include "global.c"

/** maximum reverb delay */
#define REVERB_MAX_DELAY 4000
/** number of ticks per ms */
#define REVERB_MSTICKS   40

/** reverb flag */
extern int RV_Flag;

/** initializes reverb */
extern void RV_Init(void);
/** gets reverb params
  * @param p parameter list
  * @return number of params */
extern int RV_GetParams(i32 int *p);
/** sets reverb parameters
  *
  * delay in ms
  * @param p parameters (delay , gain) */
extern void RV_SetParams(i32 int *p);
/** calculates reverb
  * @param x input value
  * @return output value */
extern float RV_Calc(float x);

#ifndef HEADER_REVERB

#include <math.h>
#include <string.h>

#define HEADER_MISC
#include "misc.c"

int RV_Flag;

/** reverb gain factor value */
static float RV_Gain;
/** reverb gain factors of the single reverb lines */
static float RV_Gains[3];
/** reverb delay ticks value */
static int RV_DelayTicks;
/** delay ticks of the single reverb lines */
static int DelayTicks[3];
/** delay line indices */
static int DelayLineIndices[3];
/** delay line */
static float DelayLine[REVERB_MAX_DELAY];

void RV_Init(void)
{ static const i32 int Std[]={ 10, 70 };

  RV_SetParams((i32 int *)Std);
  RV_Flag=0;
}

int RV_GetParams(i32 int *p)
{ if (p!=NULL)
  { p[0]=(RV_DelayTicks+REVERB_MSTICKS/2)/REVERB_MSTICKS;
    p[1]=RV_Gain*100+0.5;
  }
  return(2);
}

void RV_SetParams(i32 int *p)
{ if (RV_Flag==0)
  { /* reset delay line if not currently running */
    memset(DelayLine,0,sizeof(DelayLine));
    memset(DelayLineIndices,0,3*sizeof(int));
  }
  /* config */
  RV_DelayTicks=LIMIT(LIMIT(p[0],10,100)*REVERB_MSTICKS,1,REVERB_MAX_DELAY-1);
  RV_Gain=LIMIT(p[1],1,99)*0.01;
  /* the three single reverbs */
  DelayTicks[0]=MISC_GetPrevPrime(RV_DelayTicks*0.582); RV_Gains[0]=pow(RV_Gain,(float)DelayTicks[0]/RV_DelayTicks);
  DelayTicks[1]=MISC_GetPrevPrime(RV_DelayTicks*0.276); RV_Gains[1]=pow(RV_Gain,(float)DelayTicks[1]/RV_DelayTicks);
  DelayTicks[2]=MISC_GetPrevPrime(RV_DelayTicks*0.142); RV_Gains[2]=pow(RV_Gain,(float)DelayTicks[2]/RV_DelayTicks);
}

float RV_Calc(float x)
{ int i,First;
  float y;

  for(i=First=0,y=0;i<3;i++,x=y)
  { /* calc output */
    y=-RV_Gains[i]*x+DelayLine[First+DelayLineIndices[i]];
    /* feed delay line */
    DelayLine[First+DelayLineIndices[i]]=x+y*RV_Gains[i];
    DelayLineIndices[i]++;
    if (DelayLineIndices[i]>=DelayTicks[i]) DelayLineIndices[i]=0;
    First+=DelayTicks[i];
  }
  /* return result */
  return(y);
}

#else
#undef HEADER_REVERB
#endif

#endif


