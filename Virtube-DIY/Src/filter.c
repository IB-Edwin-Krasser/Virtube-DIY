/** @file filter.c
  * filter calculations (filter, coefficients)
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

#ifndef FILTER_C_INCLUDED
#define FILTER_C_INCLUDED

#include <math.h>

#ifndef _FPU_FILTER_H_
/** this typedef is needed by fpu_filter.h */
typedef void (*v_pfn_v)(void *);
#include "fpu_filter.h"
#endif

/** single pole (first order) iir filter
  *
  * low pass filter: y=A*[x+xz)]+B*yz
  * high pass filter: y=A*[x-xz)]+B*yz */
typedef struct
{ /** coefficient A */
  float A;
  /** coefficient B */
  float B;
  /** previous input value */
  float xz;
  /** previous output value */
  float yz;
} SinglePoleIIR;

/** biquad transfer function (in float or integer)
  *
  * Laplace-transformed: A(s)=(b0+b1*s+b2*s^2)/(1+a1*s+a2*s^2),
  * or z-transformed: H(z)=(b0+b1*z^-1+b2*z^-2)/(1+a1*^z^-1+a2*z^-2),
  * b...numerator coefficients,
  * a...denominator coefficients */
typedef struct
{ /** float denominator coefficients */
  float a2,a1;
  /** float numerator coefficients */
  float b2,b1,b0;
} Biquad;

/* the iir band pass filter stuff */
typedef struct IIRFilterSecondOrder
{ /** input */
  float x;
  /** output */
  float y;
  /** denominator coefficents */
  float ACoeff[3];
  /** numerator coefficents */
  float BCoeff[3];
  /** delay line */
  float Delay[4];
  /** scale factor */
  float Scale;
  /** iir filter structure */
  IIR_f32 f;
} SecondOrderIIRFilter;

/** calculates 1st order iir low pass filter coefficients y=A*[x+xz)]+B*yz
  * @param Fc normalized corner frequency
  * @param Filter single pole IIR filter structure */
extern void F_Calc1stOrderIIRLPFCoeffiecients(float Fc,SinglePoleIIR *Filter);
/** calculates 1st order iir high pass filter coefficients y=A*[x-xz)]+B*yz
  * @param Fc normalized corner frequency
  * @param Filter single pole IIR filter structure */
extern void F_Calc1stOrderIIRHPFCoeffiecients(float Fc,SinglePoleIIR *Filter);
/** calculates single pole filter group delay
  * @param Filter single pole IIR filter structure
  * @param Type filter type (0..lpf, 1..hpf)
  * @param F normalized frequency
  * @return group delay */
extern float F_Calc1stOrderGroupDelay(SinglePoleIIR *Filter,int Type,float F);
/** converts biquad transfer function to z-transformed biquad function using bilinear transformation (S=l*(z-1)/(z+1))
  * @param SIn Laplace-transformed input biquad transfer function (normalized S)
  * @param ZOut z-transformed output biquad transfer function
  * @param F0 normalized corner frequency of z-transformed output */
extern void F_BiquadS2Z(Biquad *SIn,Biquad *ZOut,float F0);
/** convert iir biquad to IIR_f32 A/B coeff style
  * @param ZIn z-transformed input biquad transfer function
  * @param A coeff pointer A
  * @param B coeff pointer B */
extern void F_BiquadZ2IIR_f32(Biquad *ZIn,float *A,float *B);
/** sets up second order iir filter
  * @param F the second order iir filter structure
  * @param Coefficients biquad coefficients
  * @param f0 the goal frequency to transform to */
extern void F_Init2ndOrderIIR(SecondOrderIIRFilter *F,Biquad *Coeffs,float f0);

#endif

#ifndef HEADER_FILTER

#include <string.h>

#define HEADER_GLOBAL
#include "global.c"

void F_Calc1stOrderIIRLPFCoeffiecients(float Fc,SinglePoleIIR *Filter)
{ float K=tan(PI*Fc);

  Filter->A=K/(K+1);
  Filter->B=(K-1)/(K+1);
}

void F_Calc1stOrderIIRHPFCoeffiecients(float Fc,SinglePoleIIR *Filter)
{ float K=tan(PI*Fc);

  Filter->A=1/(K+1);
  Filter->B=(K-1)/(K+1);
}

void F_BiquadS2Z(Biquad *SIn,Biquad *ZOut,float F0)
{ float l,ll,D;

  l=1/tan(PI*F0);
  ll=l*l;
  D=1+SIn->a1*l+SIn->a2*ll;
  /* numerator */
  ZOut->b0=(SIn->b0+SIn->b1*l+SIn->b2*ll)/D;
  ZOut->b1=2*(SIn->b0-SIn->b2*ll)/D;
  ZOut->b2=(SIn->b0-SIn->b1*l+SIn->b2*ll)/D;
  /* denomiator */
  ZOut->a1=2*(1-SIn->a2*ll)/D;
  ZOut->a2=(1-SIn->a1*l+SIn->a2*ll)/D;
}

void F_BiquadZ2IIR_f32(Biquad *ZIn,float *A,float *B)
{ A[0]=1;       A[1]=-ZIn->a1; A[2]=-ZIn->a2;
  B[0]=ZIn->b0; B[1]=ZIn->b1;  B[2]=ZIn->b2;
}

void F_Init2ndOrderIIR(SecondOrderIIRFilter *F,Biquad *Coeffs,float f0)
{ Biquad o;

  F_BiquadS2Z(Coeffs,&o,f0);
  memset(F->Delay,0,4*sizeof(float));
  /* initialize filter structure */
  F_BiquadZ2IIR_f32(&o,F->ACoeff,F->BCoeff);
  IIR_f32_setDelayLinePtr(&F->f,F->Delay);
  IIR_f32_setCoefficientsAPtr(&F->f,F->ACoeff);
  IIR_f32_setCoefficientsBPtr(&F->f,F->BCoeff);
  IIR_f32_setOrder(&F->f,2);
  IIR_f32_setScalePtr(&F->f,&F->Scale);
  IIR_f32_setInputPtr(&F->f,&F->x);
  IIR_f32_setOutputPtr(&F->f,&F->y);
}

#else
#undef HEADER_FILTER
#endif

