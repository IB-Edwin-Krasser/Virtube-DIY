/** @file virtube.c
  * virtual tube gain stage
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

#ifndef __VIRTUBE_C_INCLUDED
#define __VIRTUBE_C_INCLUDED

typedef
/** vacuum tube parameters: Ia=I1*(Vg-(C0+Ca*Va))^(3/2) */
struct
{ /** current factor */
  float I1;
  /** constant C0: some kind of threshold */
  float C0;
  /** constant Ca: foctor of anode or grit (pentode) voltage on threshold */
  float Ca;
  /** maximum on conductance */
  float Gmax;
  /** gate cathode current transconductance */
  float Ggcc;
} TubeParameters;

/** maximum newton iterations */
#define MAX_ITERATIONS 20
/** maximum iterations */
extern int VT_MaxIterations;

/** sets zero values for bias and normal calculations
  * @param Bias bias zero in V
  * @param Calc calculation zero in V*/
extern void VT_SetZeros(float Bias,float Calc);
/** sets zero values for bias and normal calculations
  * @param Bias bias zero in V
  * @param Calc calculation zero in V*/
extern void VT_GetZeros(float *Bias,float *Calc);
/** calculate bias point of cathode gain stage
  * @param i tube parameters index
  * @param Vdd supply voltage
  * @param Ra anode resistance value
  * @param Gc cathode conductance value
  * @param Vg gate voltage (zero for bias)
  * @param Va anode voltage output
  * @param Vc cathode voltage output */
void VT_CGSFullCalc(int i,float Vdd,float Ra,float Gc,float Vg,float *Va,float *Vc);
/** calculates cathode gain stage
  * @param i tube parameters index
  * @param Vdd supply voltage
  * @param Ga anode conductance value
  * @param Vi input voltage
  * @param Vo output voltage (if !=0 it's the start point for the iterations) */
void VT_CGSCalc(int i,float Vdd,float Ga,float Vi,float *Vo);
/** calculates cathode follower stage
  * @param i tube parameters index
  * @param Vdd supply voltage
  * @param Gc cathode conductance value
  * @param Vi input voltage
  * @param Vo output voltage (if !=0 it's the start point for the iterations) */
void VT_CFSCalc(int i,float Vdd,float Gc,float Vi,float *Vo);
/** calc output stage differtial current
  * @param i parameters index
  * @param Vdd supply voltage
  * @param Vi1 input voltage of first tube
  * @param Vi2 input voltage of second tube
  * @return current into transformer */
extern float VT_OSCalc(int i,float Vdd,float Vi1,float Vi2);
/** gets grid current factor
  * @param i index of tube model
  * @return grid current factor in uA/V */
extern int VT_GetGridCurrentFactor(int i);
/** sets grid current factor
  * @param i index of tube model
  * @param f grid current factor in uA/V */
extern void VT_SetGridCurrentFactor(int i,int f);

#ifndef HEADER_VIRTUBE

#include <math.h>

#define HEADER_GLOBAL
#include "global.c"

/** use simple gate cathode current model stuff */
#define USE_GATECATHODE_CURRENT 1

/** maximum newton iterations */
#define MAX_ITERATIONS          20
/** zero value for bias point calculations */
#define BIAS_ZERO               1E-4
/** zero value for gain calculations
  * take it a little bit large than the 1mV, because after the step it will be below 1mV */
#define CALC_ZERO               1E-2

int VT_MaxIterations;

/** zero values */
static float VT_BiasZero,VT_CalcZero;

/** derivate of Ia in respect to Vc of previous calculation */
float dIadVc;
/** derivate of Ia in respect to Va of previous calculation */
float dIadVa;

/** tube parameters for different tube types
  *
  * 0..12AX7 (ECC83), 1..6L6 (EL34) */
static TubeParameters TP[]=
{ { 0.00100, -0.5, -0.0111, 1/11E3,  1E-3 }, /* 12AX7 (ECC83), grid current effect regarding the resistance to grid */
  { 0.00165, -1.0, -0.1100, 1/200.0, 5E-3 }, /* 6L6 (EL34), grid current effect regarding the resistance to grid */
};

extern void VT_SetZeros(float Bias,float Calc)
{ if (Bias<=0||Calc<=0)
  { VT_BiasZero=BIAS_ZERO;
    VT_CalcZero=CALC_ZERO;
  } else
  { VT_BiasZero=Bias;
    VT_CalcZero=Calc;
  }
}

extern void VT_GetZeros(float *Bias,float *Calc)
{ *Bias=VT_BiasZero;
  *Calc=VT_CalcZero;
}

/** calculates anode current (Vc=0)
  * @param i tube parameters index
  * @param Vg gate voltage
  * @param Va anode voltage
  * @return anode current (equals cathode current) */
static float Ia(int i,float Vg,float Va)
{ float x,y,s;

  x=Vg-(TP[i].C0+TP[i].Ca*Va);
  if (x>0)
  { s=sqrt(x);
    y=TP[i].I1*s;
    dIadVc=1.5*y;
    dIadVa=-dIadVc*TP[i].Ca;
    y*=x;
    #if USE_GATECATHODE_CURRENT!=0
    if (Vg>0)
    { y-=(TP[i].Ggcc*Vg);
      dIadVc+=TP[i].Ggcc;
    }
    #endif
    /* never allow higher conductance as Gmax */
    if (y>Va*TP[i].Gmax)
    { y=Va*TP[i].Gmax;
      dIadVa=TP[i].Gmax;
      dIadVc=0;
    }
  } else
  { y=0;
    dIadVc=dIadVa=0;
  }
  return(y);
}

void VT_CGSFullCalc(int i,float Vdd,float Ra,float Gc,float Vg,float *Va,float *Vc)
{ float vc,in,din;
  int j;

  vc=*Vc;
  for(j=0;j<VT_MaxIterations;j++)
  { in=Ia(i,Vg-vc,Vdd-vc*(1+Ra*Gc))-vc*Gc;
    din=-dIadVc-dIadVa*(1+Ra*Gc)-Gc;
    vc=vc-in/din;
    if (fabs(in)<fabs(din*VT_BiasZero)) break;
  }
  *Vc=vc;
  *Va=Vdd-vc*(1+Ra*Gc);
}

void VT_CGSCalc(int i,float Vdd,float Ga,float Vi,float *Vo)
{ int j;
  float vo,in,din;

  vo=LIMIT(*Vo,0,Vdd);
  for(j=0;j<VT_MaxIterations;j++)
  { in=Ia(i,Vi,vo)-(Vdd-vo)*Ga;
    din=dIadVa+Ga;
    vo=vo-in/din;
    if (fabs(in)<fabs(din*VT_CalcZero)) break;
  }
  *Vo=vo;
}

void VT_CFSCalc(int i,float Vdd,float Gc,float Vi,float *Vo)
{ int j;
  float vo,in,din;

  vo=Vi;
  for(j=0;j<VT_MaxIterations;j++)
  { in=Ia(i,Vi-vo,Vdd-vo)-vo*Gc;
    din=-dIadVc-dIadVa-Gc;
    vo=vo-in/din;
    if (fabs(in)<fabs(din*VT_CalcZero)) break;
  }
  *Vo=vo;
}

float VT_OSCalc(int i,float Vdd,float Vi1,float Vi2)
{ return(Ia(i,Vi1,Vdd)-Ia(i,Vi2,Vdd));
}

int VT_GetGridCurrentFactor(int i)
{ return(TP[LIMIT(i,0,1)].Ggcc*1E6);
}

void VT_SetGridCurrentFactor(int i,int f)
{ TP[LIMIT(i,0,1)].Ggcc=1E-6*LIMIT(f,0,(i==0)? 5000:20000);
}

#else
#undef HEADER_VIRTUBE
#endif

#endif

