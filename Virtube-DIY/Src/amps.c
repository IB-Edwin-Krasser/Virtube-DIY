/** @file amps.c
  * Amplifier module – skeleton / DIY starting point (no full implementation)
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

#ifndef __AMPS_C_INCLUDED
#define __AMPS_C_INCLUDED

#define HEADER_GLOBAL
#include "global.c"
#define HEADER_MISC
#include "misc.c"

/** init amp */
extern void AMPS_Init(void);
/** calculate amp
  * @param Vi input voltage
  * @return output voltage */
extern float AMPS_Calc(float Vi);
/** get amp params
  * @param p paremeter list
  * @return number of params */
extern int AMPS_GetParams(i32 int *p);
/** set amp parameter
  * @param i parameter index
  * @param p parameter */
extern void AMPS_SetParam(int i,i32 int p);
/** set amp params
  * @param p paremeter list */
extern void AMPS_SetParams(i32 int *p);
/** gets switches
  * @return binary coded switches */
extern int AMPS_GetSwitches(void);
/** sets switches
  * @param s binary coded switches */
extern void AMPS_SetSwitches(int s);

#ifndef HEADER_AMPS

#include <math.h>

#define HEADER_ADCDAC
#include "adcdac.c"

#include "fpu_math.h"

/** sample rate */
#define FSAMPLE    ((float)ADC_FSAMPLE)
/** number of parameters */
#define AMP_PARAMS 6

/** parameters of amplifier */
static float Params[AMP_PARAMS];
/** logarithmic parameters of amplifier
  *
  * @see Params */
static float LogParams[AMP_PARAMS];
/** switches, binary coded  */
static int Switches;

void AMPS_Init(void)
{ int i;

  /* standard parameters */
  Params[0]=0.5;
  Params[1]=0.5;
  Params[2]=0.5;
  Params[3]=0.5;
  Params[4]=0.5;
  Params[5]=0.5;
  Switches=0;
  for(i=0;i<AMP_PARAMS;i++) LogParams[i]=MISC_LogParam(Params[i]);
}

float AMPS_Calc(float Vi)
{ float Vo;

  /* 
   * ... code!
   */
  Vi*=(1+(Switches&1));

  Vo=Vi*LogParams[4]*4; /* 4..volume at front */

  Vo*=(1+((Switches&4)>>2));
  return(Vo);
}

int AMPS_GetParams(i32 int *p)
{ int i;

  if (p!=NULL)
    for(i=0;i<AMP_PARAMS;i++)
	    p[i]=Params[i]*100+0.5;
  return(AMP_PARAMS);
}

void AMPS_SetParam(int i,i32 int p)
{ Params[LIMIT(i,0,AMP_PARAMS)]=(float)LIMIT(p,0,100)/100;
  LogParams[i]=MISC_LogParam(Params[i]);
}

void AMPS_SetParams(i32 int *p)
{ int i;

  for(i=0;i<AMP_PARAMS;i++)
  { if (p!=NULL) Params[i]=(float)LIMIT(p[i],0,100)/100;
    LogParams[i]=MISC_LogParam(Params[i]);
  }
}

int AMPS_GetSwitches(void)
{ return(Switches);
}

void AMPS_SetSwitches(int s)
{ Switches=s&0x07;
}

#else
#undef HEADER_AMPS
#endif

#endif


