/** @file equalizer.c
  * simple five band equalizer
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

#ifndef __EQUALIZER_C_INCLUDED
#define __EQUALIZER_C_INCLUDED

#define HEADER_GLOBAL
#include "global.c"

/** reverb flag */
extern int EQ_Flag;

/** initializes equalizer */
extern void EQ_Init(void);
/** gets equalizer params
  * @param p paremeter list
  * @return number of params */
extern int EQ_GetParams(i32 int *p);
/** sets equalizer parameters
  * @param p parameters (delay , gain) */
extern void EQ_SetParams(i32 int *p);
/** calculates equalizer
  * @param x input value
  * @return output value */
extern float EQ_Calc(float x);

#ifndef HEADER_EQUALIZER

#include <string.h>

#define HEADER_ADCDAC
#include "adcdac.c"
#define HEADER_FILTER
#include "filter.c"
#define HEADER_MISC
#include "misc.c"

#include "fpu_math.h"

int EQ_Flag;

/** start frequency */
#define FSTART 100
/** frequency multiply factor per stage */
#define FMULT  2.5
/** band pass gain */
#define BPF_A  0.6

/** the normalized band pass filter */
const static Biquad BPF2thBiquad={ 1, 1, 0, 1, 0 };
/* the iir band pass filter stuff */
static SecondOrderIIRFilter F[3];
/** first low pass and last high pass filter */
static SinglePoleIIR S[2];
/** filter gain factors */
static float EQ_Gains[5];

void EQ_Init(void)
{ int i;
  float f0;
  static const i32 int Std[]={ 100, 100, 100, 100, 100 };

  /* setup filters */
  f0=FSTART/(float)ADC_FSAMPLE;
  /* first low pass filter */
  F_Calc1stOrderIIRLPFCoeffiecients(f0,&S[0]);
  f0*=FMULT;
  /* the three band pass filters in the middle three frequency bands */
  for(i=0;i<3;i++,f0*=FMULT)
  { F[i].Scale=(i!=1)? BPF_A:BPF_A/1.5;
    F_Init2ndOrderIIR(&F[i],(Biquad *)&BPF2thBiquad,f0);
  }
  /* last high pass filter */
  F_Calc1stOrderIIRHPFCoeffiecients(f0,&S[1]);
  /* set standard parameters */
  EQ_Flag=0;
  EQ_SetParams((i32 int *)Std);
}

int EQ_GetParams(i32 int *p)
{ int i;

  if (p!=NULL)
  { for(i=0;i<5;i++)
      p[i]=100*EQ_Gains[i]+0.5;
  }
  return(5);
}

void EQ_SetParams(i32 int *p)
{ int i;

  if (EQ_Flag==0)
  { /* reset delay line if not currently running */
    S[0].xz=S[0].yz=S[1].xz=S[1].yz=0;
    for(i=0;i<3;i++) memset(F[i].Delay,0,4*sizeof(float));
  }
  /* config */
  for(i=0;i<5;i++)
    EQ_Gains[i]=0.01*LIMIT(p[i],0,400);
}

float EQ_Calc(float x)
{ int i;
  float y;

  S[0].yz=(x+S[0].xz)*S[0].A-S[0].yz*S[0].B; S[0].xz=x; /* calc low pass filter */
  S[1].yz=(x-S[1].xz)*S[1].A-S[1].yz*S[1].B; S[1].xz=x; /* calc high pass filter */
  for(i=0;i<3;i++) { F[i].x=x; IIR_f32_calc(&F[i].f); } /* calc band pass filters */
  /* sum up with gain factors */
  y=(EQ_Gains[0]*S[0].yz);
  for(i=0;i<3;i++) y+=(EQ_Gains[i+1]*F[i].y);
  y+=(EQ_Gains[4]*S[1].yz);
  /* cannot understand why 1.4  */
  return(1.4*y);
}

#else
#undef HEADER_EQUALIZER
#endif

#endif


