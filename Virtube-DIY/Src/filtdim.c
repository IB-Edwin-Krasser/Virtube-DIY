/** @file filtdim.c
  *
  * fir filter dimensioning functions
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

#ifndef __FILTDIM_C_INCLUDED
#define __FILTDIM_C_INCLUDED

#define HEADER_FILTER
#include "filter.c"

/** rectangular window */
#define WINDOW_RECT     0
/** kaiser window, paramter is used
  *
  * @see FD_CalcLPFCoeff */
#define WINDOW_KAISER   1
/** calculates fir low pass filter coefficients
  * @param Order order of fir filter
  * @param Fc corner frequency, if fc equals 0 then only the window function is calculated
  * @param WType window type
  * @param WParam parameter for window
  * @param Data destination data buffer */
extern void FD_CalcLPFCoeffs(int Order,float Fc,int WType,float WParam,float *Data);

#ifndef HEADER_FILTDIM

#include <math.h>
#include <string.h>

#define HEADER_GLOBAL
#include "global.c"

/***************************************************************************/
/************************* FIR FILTER DIMENSIONING *************************/
/***************************************************************************/

/** sinc function
  * @param x argument
  * @return result */
static float Sinc(float x)
{ float f;

  if (x!=0)
  { f=PI*x;
    return(sin(f)/f);
  } else
    return(1);
}

/** modified bessel function order 0
  * @param x argument
  * @return result */
static float BesselI0(float x)
{ int i;
  float phi,dphi,Sum;

  /** number of steps in calculating the integral */
  #define BESSEL_STEPS 32
  /* calculate bessel function */
  for(i=0,phi=0,dphi=PI/BESSEL_STEPS,Sum=0;i<BESSEL_STEPS;i++,phi+=dphi)
    Sum+=exp(x*cos(phi));
  Sum/=BESSEL_STEPS;
  /* return value */
  return(Sum);
}

void FD_CalcLPFCoeffs(int Order,float Fc,int WType,float WParam,float *Data)
{ int n;
  float f,h,I0Beta,s;

  /* safety limits */
  Order=LIMIT(Order,0,1024);
  Fc=LIMIT(Fc,0,0.5);
  /* calc implse response */
  switch(WType)
  { case WINDOW_RECT:
      for(n=0;n<Order;n++)
        Data[n]=2*Fc*Sinc(Fc*(2*n-Order+1));
      break;
    case WINDOW_KAISER:
      I0Beta=BesselI0(WParam);
      for(n=0;n<Order;n++)
      { h=Fc!=0? 2*Fc*Sinc(Fc*(2*n-Order+1)):1;
        f=(float)(2*n)/(Order-1)-1;
        Data[n]=h*(BesselI0(WParam*sqrt(1-f*f))/I0Beta);
      }
      break;
  }
  /* normalize area */
  for(n=0,s=0;n<Order;n++) s+=Data[n];
  s=1/s;
  for(n=0;n<Order;n++) Data[n]*=s;
}

#else
#undef HEADER_FIRDIM
#endif

#endif
