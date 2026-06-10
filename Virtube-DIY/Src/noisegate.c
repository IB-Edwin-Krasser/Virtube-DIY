/** @file noisegate.c
  * virtual noisegate
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

#ifndef __NOISEGATE_C_INCLUDED
#define __NOISEGATE_C_INCLUDED

#define HEADER_GLOBAL
#include "global.c"

/** flag if noise is enabled or not */
extern int NG_Flag;

/** initializes noise gate */
extern void NG_Init(void);
/** get noise gate params
  * @param p paremeter list
  * @return number of params */
extern int NG_GetParams(i32 int *p);
/** set noise gate parameters
  * @param p parameters */
extern void NG_SetParams(i32 int *p);
/** calculates noise gate
  * @param Vin input voltage
  * @return output after noise gate */
extern float NG_Calc(float Vin);

#ifndef HEADER_NOISEGATE

#include <math.h>

#define HEADER_ADCDAC
#include "adcdac.c"

/** sample rate */
#define FSAMPLE    ((float)ADC_FSAMPLE)

int NG_Flag;

/** the original threshold value */
static i32 int NG_Thresh;

/** the current noise gate state (0..gate, 1..attack, 2..hold) */
static int NG_State;
/** attack tics */
static i32 int AttackTicks;
/** hold tics */
static i32 int HoldTicks;
/** threshold voltage */
static float VThresh;

/** measured effective input voltage */
static float Veff;
/** discharge constant
  *
  * tau=2 ms
  * x_n+1=x_n*C; C=exp(-1/(f_Sample*tau) */
#define C_DISCHARGE  0.9875778
/** discharge sum, estimation for the equivalent number of ticks */
#define C_DISCHRGSUM (C_DISCHARGE/(1-C_DISCHARGE))

/** attack counter */
static i32 int AttackCounter;
/** hold counter */
static i32 HoldCounter;

void NG_Init(void)
{ static const i32 int Std[]={ 600, 30, 100 };

  NG_SetParams((i32 int *)Std);
  NG_State=0;
  Veff=0;
  AttackCounter=HoldCounter=0;
  NG_Flag=0;
}

float NG_Calc(float Vin)
{ if (NG_Flag!=0)
  { /* magnitude measurement */
    Veff+=SQR(Vin);
    Veff=Veff*C_DISCHARGE; /* real Veff would be sqrt(Veff/C_DISCHRGSUM) */
    /* noise gate states */
    switch(NG_State)
    { case 0: /* gate: this is more a reset state */
        /* threshold, go to attack */
        if (Veff>VThresh) NG_State++;
        AttackCounter=0;
        Vin=0;
        break;
      case 1: /* attack fade in */
        if (Veff>VThresh)
        { if (AttackCounter<AttackTicks)
            AttackCounter++;
          else
          { NG_State++;
            HoldCounter=0;
          }
        } else
        { if (AttackCounter>0)
            AttackCounter--;
        }
        Vin*=(((float)AttackCounter)/AttackTicks);
        break;
      case 2: /* hold time */
        if (Veff>=VThresh) HoldCounter=0; else HoldCounter++;
        if (HoldCounter>=HoldTicks)
          NG_State=0;
        break;
    }
  } else
  { NG_State=0;
    Veff=0;
  }
  return(Vin);
}

extern void NG_SetParams(i32 int *p)
{ NG_Thresh=LIMIT(p[0],0,1E4);
  VThresh=SQR(NG_Thresh/1E6)*C_DISCHRGSUM; /* area under C_DISCHARGE fade out, geometric series */
  AttackTicks=LIMIT(p[1],10,300)*(FSAMPLE/1000)+0.5;
  HoldTicks=LIMIT(p[2],10,1000)*(FSAMPLE/1000)+0.5;
}

int NG_GetParams(i32 int *p)
{ if (p!=NULL)
  { p[0]=NG_Thresh;
    p[1]=AttackTicks/(FSAMPLE/1000)+0.5;
    p[2]=HoldTicks/(FSAMPLE/1000)+0.5;
  }
  return(3);
}

#else
#undef HEADER_NOISEGATE
#endif

#endif


