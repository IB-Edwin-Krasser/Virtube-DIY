/** @file tubescr.c
  * virtual tube screamer, three parameters (as everybody should know)
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

#ifndef __TUBESCR_C_INCLUDED
#define __TUBESCR_C_INCLUDED

#define HEADER_GLOBAL
#include "global.c"

/** flag if tube screamer is enabled or not */
extern int TS_Flag;

/** init virtual tube screamer */
extern void TS_Init(void);
/** get tube screamer params
  * @param p paremeter list
  * @return number of params */
extern int TS_GetParams(i32 int *p);
/** set tube screamer parameter
  * @param i parameter index
  * @param p parameter */
extern void TS_SetParam(int i,int p);
/** set tube screamer parameters
  * @param p parameters */
extern void TS_SetParams(i32 int *p);
/** calculate tube screamer
  * @param vi input voltage
  * @return output voltage */
extern float TS_Calc(float vi);
/** calculates frequency phase pesponse
  * @param F normalized frequency
  * @return phase */
extern float TS_CalcPhase(float F);
/** calculates group delay
  * @param F normalized frequency
  * @return group delay */
extern float TS_CalcGroupDelay(float F);

#ifndef HEADER_TUBESCR

#include <math.h>

#define HEADER_ADCDAC
#include "adcdac.c"
#define HEADER_AMPS
#include "amps.c"
#define HEADER_GLOBAL
#include "global.c"
#define HEADER_FILTER
#include "filter.c"
#define HEADER_FILTDIM
#include "filtdim.c"
#define HEADER_MISC
#include "misc.c"

#include "fpu_math.h"

/** maximum newton iterations for tube screamer */
#define MAX_TS_ITERATIONS 3
/** sample rate */
#define FSAMPLE ((float)ADC_FSAMPLE)

int TS_Flag;

/** parameters of tube screaber */
static float Params[3];
/** some single pole filters
  *
  *  0..diode gain input and offset removal, 1..output low pass filter */
static SinglePoleIIR Filters[2];
/** gain */
static float Gain;
/** drive conductance */
static float GDrive;
/** level */
static float Level;

/** aaf order */
#define AAF_ORDER 31
/** aaf */
static FIR_f32 TS_AAF;
/** aaf coefficients */
static float TS_AAFCoeffs[AAF_ORDER];
/** aaf delay line (length 32 for even adress values) */
static float TS_AAFDelay[AAF_ORDER+1];

#pragma DATA_SECTION(TS_AAFCoeffs,"Filter0_RegsFile");

void TS_Init(void)
{ static const i32 int Std[]={ 50, 50, 50 };

  /* single pole filters */
  F_Calc1stOrderIIRHPFCoeffiecients(720/FSAMPLE,&Filters[0]);
  Filters[1].xz=Filters[1].yz=0;
  /* init aaf filter */
  FD_CalcLPFCoeffs(AAF_ORDER,0.33,1,10,TS_AAFCoeffs);
  TS_AAF.order=AAF_ORDER-1;
  TS_AAF.coeff_ptr=TS_AAFCoeffs;
  TS_AAF.dbuffer_ptr=TS_AAFDelay;
  FIR_f32_init(&TS_AAF);
  /* set standard parameters */
  TS_SetParams((i32 int *)Std);
  TS_Flag=0;
}

int TS_GetParams(i32 int *p)
{ int i;

  if (p!=NULL)
    for(i=0;i<3;i++)
      p[i]=Params[i]*100+0.5;
  return(3);
}

void TS_SetParam(int i,int p)
{ Params[i]=(float)LIMIT(p,0,100)/100;
  /* initialize to the new parameters */
  TS_SetParams(NULL);
}

void TS_SetParams(i32 int *p)
{ int i;

  if (p!=NULL)
  { for(i=0;i<3;i++)
      Params[i]=(float)LIMIT(p[i],0,100)/100;
  }
  /* initialize to the new parameters */
  Gain=MISC_LogParam(Params[0]);
  GDrive=1/(51E3+500E3*Gain);
  Gain=1/(4.7E3*GDrive);
  Level=MISC_LogParam(Params[2]);
  F_Calc1stOrderIIRLPFCoeffiecients((float)(300+800*Params[1])/FSAMPLE,&Filters[1]);
  /* Filters[1].xz=Filters[1].yz=0; */
}

/** 1N4148 parameters */
#define IS 0.1E-12
#define VT 0.026
#define M  1
/** calculates diode 1N4148
  * @param v diode voltage in v
  * @param didv derivate of i to v
  * @return diode current in A */
static float D1N4148(float v,float *didv)
{ float i;

  i=IS*(exp(v*(1/(M*VT)))-1);
  *didv=i*(1/(M*VT));
  return(i);
}
/** calculates diode 1N4148
  * @param i diode current in a
  * @return diode voltage in v */
static float InvD1N4148(float i)
{ return((M*VT)*log(i/IS+1));
}

float TS_Calc(float vi)
{ /* int j; */
  float i,vo,in,din,didv1,didv2,vTone,delta;

  /* calc IIR high pass filter (offset removal and diode input gain current) */
  Filters[0].yz=(vi-Filters[0].xz)*Filters[0].A-Filters[0].yz*Filters[0].B;
  Filters[0].xz=vi;
  vi=Filters[0].yz;
  /* input current and start voltage */
  i=vi/4.7E3;
  if (i>0) vo=2*InvD1N4148(i); else vo=-InvD1N4148(-i);
  /* start with minimum of diode clipping and linear gain */
  if (fabs(vi*Gain)<fabs(vo)) vo=vi*Gain;
  /* newton iterations: just one, because the starting point must be close enough */
  /* for(j=0;j<MAX_TS_ITERATIONS;j++)
  { */
    if (i>0) { in=vo*GDrive+D1N4148(vo/2,&didv2)-i; din=GDrive+didv2/2; }
        else { in=vo*GDrive-D1N4148(-vo,&didv1)-i;  din=GDrive+didv1; }
    delta=in/din;
    vo=vo-delta;
    vo=LIMIT(vo,-0.6,1.2);
  /*  if (fabs(delta)<0.001) break;
    */
  /* tone */
  vTone=Filters[1].yz=(vo+Filters[1].xz)*Filters[1].A-Filters[1].yz*Filters[1].B;
  Filters[1].xz=vo;
  #if USE_AAF!=0
  TS_AAF.input=vTone; FIR_f32_calc(&TS_AAF); vTone=TS_AAF.output;
  #endif
  /* and return with output level */
  return(vTone*Level);
}

#else
#undef HEADER_TUBESCR
#endif

#endif


