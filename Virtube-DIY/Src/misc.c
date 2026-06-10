/** @file misc.c
  * Miscellaneous stuff: LEDs, keys, misc. functions.
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

#ifndef __MISC_C_INCLUDED
#define __MISC_C_INCLUDED

#define HEADER_GLOBAL
#include "global.c"

/** running an launchpad */

/** number of keys version */
#define NO_KEYS ((MISC_HWVersion<=3)? 4:6)

/** key 1 */
#define KEY1    GpioDataRegs.GPBDAT.bit.GPIO58
/** key 2 */
#define KEY2    GpioDataRegs.GPADAT.bit.GPIO18
/** key 3 */
#define KEY3    GpioDataRegs.GPADAT.bit.GPIO19
/** key 4 */
#define KEY4    GpioDataRegs.GPADAT.bit.GPIO20
/** key 5 */
#define KEY5    GpioDataRegs.GPADAT.bit.GPIO21
/** key 6 */
#define KEY6    GpioDataRegs.GPDDAT.bit.GPIO99
/** toggle switch 1 */
#define SWITCH1 GpioDataRegs.GPADAT.bit.GPIO16
/** toggle switch 2 */
#define SWITCH2 GpioDataRegs.GPADAT.bit.GPIO17
/** toggle switch 3 */
#define SWITCH3 GpioDataRegs.GPBDAT.bit.GPIO60
/** key poti 1 */
#define KEY_P1  GpioDataRegs.GPBDAT.bit.GPIO61
/** key poti 2 */
#define KEY_P2  GpioDataRegs.GPCDAT.bit.GPIO90
/** key poti 3 */
#define KEY_P3  GpioDataRegs.GPADAT.bit.GPIO4
/** key poti 4 */
#define KEY_P4  GpioDataRegs.GPCDAT.bit.GPIO78
/** key poti 5 */
#define KEY_P5  GpioDataRegs.GPCDAT.bit.GPIO72
/** key poti 6 */
#define KEY_P6  GpioDataRegs.GPCDAT.bit.GPIO89

/** hardware version (-1 if running on launchpad) */
extern int MISC_HWVersion;
/** the keys status: set if key pressed, reset after processing
  *
  * 0..not pressed, 1..short press, 2..long press, two bits per key */
extern i32 int MISC_Keys;
/** the keys status: set if key pressed, reset after processing, or -1 if not present
  *
  * 0..not pressed, 1..short press, 2..long press, two bits per key */
extern i32 int MISC_PotiKeys;

/** initializes miscellaneous functions */
extern void MISC_Init(void);
/** gets one of the four status led
  * @param Index index of LED
  * @return enable flag */
extern int MISC_GetLED(int Index);
/** sets one of the four status led
  * @param Index index of LED
  * @param En enable flag */
extern void MISC_SetLED(int Index,int En);
/** sets the four status leds
  * @param Value binary coded leds */
extern void MISC_SetLEDs(int Value);
/** toggles one of the three status led
  * @param Index index of LED */
extern void MISC_ToggleLED(int Index);

/** checks keys
  * @see MISC_Keys */
extern void MISC_CheckKeys(void);

/** converts linear poti param to log poti param (middle=0.5 results in 0.25)
  * @param d input parametor (0..1)
  * @return log poti value (0.01..1) */
extern float MISC_LogParam(float d);

/** gets last prime number before
  * @param i integer number
  * @return last prime number lower than i */
extern int MISC_GetPrevPrime(int i);

#ifndef HEADER_MISC

#include <math.h>
#include <string.h>

#define HEADER_ADCDAC
#include "adcdac.c"
#define HEADER_POWERDAC
#include "powerdac.c"

/** time in ticks for short press */
#define KEY_SHORT   1000
/** time in ticks for long press */
#define KEY_LONG    (2*(i32 int)ADC_FSAMPLE)

/** led 1 */
#define LED1        GpioDataRegs.GPADAT.bit.GPIO10
/** led 2 */
#define LED2        GpioDataRegs.GPADAT.bit.GPIO11
/** led 3 */
#define LED3        GpioDataRegs.GPADAT.bit.GPIO13
/** led 4 */
#define LED4        GpioDataRegs.GPBDAT.bit.GPIO59
/** foot switch 1 */
#define FS1         GpioDataRegs.GPADAT.bit.GPIO2
/** foot switch 2 */
#define FS2         GpioDataRegs.GPADAT.bit.GPIO3

int MISC_HWVersion;
i32 int MISC_Keys;
i32 int MISC_PotiKeys;

void MISC_Init(void)
{ MISC_HWVersion=(((GpioDataRegs.GPBDAT.all>>30)&0x03)|((GpioDataRegs.GPCDAT.all&0x07)<<2));
  if (MISC_HWVersion==31) MISC_HWVersion=-1;
  /* allow access to protected memory regions */
  EALLOW;
  /* led outputs */
  GpioCtrlRegs.GPADIR.bit.GPIO10=
  GpioCtrlRegs.GPADIR.bit.GPIO11=
  GpioCtrlRegs.GPADIR.bit.GPIO13=
  GpioCtrlRegs.GPBDIR.bit.GPIO59=1;
  /* finish hardware setup, disable access to protected memory regions */
  EDIS;
  MISC_SetLEDs(0);
  MISC_Keys=MISC_PotiKeys=0;
}

int MISC_GetLED(int Index)
{ int RetVal;

  RetVal=0;
  switch(Index)
  { case 0: RetVal=LED1; break;
    case 1: RetVal=LED2; break;
    case 2: RetVal=LED3; break;
    case 3: RetVal=LED4; break;
  }
  return(RetVal);
}

void MISC_SetLED(int Index,int En)
{ switch(Index)
  { case 0: LED1=En; break;
    case 1: LED2=En; break;
    case 2: LED3=En; break;
    case 3: LED4=En; break;
  }
}

void MISC_SetLEDs(int Value)
{ MISC_SetLED(0,Value&1);
  MISC_SetLED(1,(Value>>1)&1);
  MISC_SetLED(2,(Value>>2)&1);
  MISC_SetLED(3,(Value>>3)&1);
}

void MISC_ToggleLED(int Index)
{ switch(Index)
  { case 0: LED1=!LED1; break;
    case 1: LED2=!LED2; break;
    case 2: LED3=!LED3; break;
    case 3: LED4=!LED4; break;
  }
}

/** the key counters */
static i32 int KeyCounters[9]={ 0,0,0,0,0,0,0,0,0 },PotiKeyCounters[6]={ 0,0,0,0,0,0 };

void MISC_CheckKeys(void)
{ i32 int Keys,PotiKeys;
  int FS;

  Keys=0;
  if (KEY1==0) { KeyCounters[0]++; if (KeyCounters[0]==KEY_LONG) Keys+=(2<<0);  } else { if (KeyCounters[0]>KEY_SHORT&&KeyCounters[0]<KEY_LONG) Keys+=(1<<0);  KeyCounters[0]=0; }
  if (KEY2==0) { KeyCounters[1]++; if (KeyCounters[1]==KEY_LONG) Keys+=(2<<2);  } else { if (KeyCounters[1]>KEY_SHORT&&KeyCounters[1]<KEY_LONG) Keys+=(1<<2);  KeyCounters[1]=0; }
  if (KEY3==0) { KeyCounters[2]++; if (KeyCounters[2]==KEY_LONG) Keys+=(2<<4);  } else { if (KeyCounters[2]>KEY_SHORT&&KeyCounters[2]<KEY_LONG) Keys+=(1<<4);  KeyCounters[2]=0; }
  if (KEY4==0) { KeyCounters[3]++; if (KeyCounters[3]==KEY_LONG) Keys+=(2<<6);  } else { if (KeyCounters[3]>KEY_SHORT&&KeyCounters[3]<KEY_LONG) Keys+=(1<<6);  KeyCounters[3]=0; }
  if (KEY5==0) { KeyCounters[4]++; if (KeyCounters[4]==KEY_LONG) Keys+=(2<<8);  } else { if (KeyCounters[4]>KEY_SHORT&&KeyCounters[4]<KEY_LONG) Keys+=(1<<8);  KeyCounters[4]=0; }
  if (KEY6==0) { KeyCounters[5]++; if (KeyCounters[5]==KEY_LONG) Keys+=(2<<10); } else { if (KeyCounters[5]>KEY_SHORT&&KeyCounters[5]<KEY_LONG) Keys+=(1<<10); KeyCounters[5]=0; }
  /* foot switch */
  FS=(FS1|(FS2<<1))^0x03;
  if (FS==1) { KeyCounters[6]++; if (KeyCounters[6]==KEY_LONG) Keys+=(2<<12);  } else { if (KeyCounters[6]>KEY_SHORT&&KeyCounters[6]<KEY_LONG) Keys+=(1<<12);  KeyCounters[6]=0; }
  if (FS==2) { KeyCounters[7]++; if (KeyCounters[7]==KEY_LONG) Keys+=(2L<<14); } else { if (KeyCounters[7]>KEY_SHORT&&KeyCounters[7]<KEY_LONG) Keys+=(1<<14);  KeyCounters[7]=0; }
  if (FS==3) { KeyCounters[8]++; if (KeyCounters[8]==KEY_LONG) Keys+=(2L<<16); } else { if (KeyCounters[8]>KEY_SHORT&&KeyCounters[8]<KEY_LONG) Keys+=(1L<<16); KeyCounters[8]=0; }
  /* poti keys */
  PotiKeys=0;
  if (KEY_P1==0) { PotiKeyCounters[0]++; if (PotiKeyCounters[0]==KEY_LONG) PotiKeys+=(2<<0);  } else { if (PotiKeyCounters[0]>KEY_SHORT&&PotiKeyCounters[0]<KEY_LONG) PotiKeys+=(1<<0);  PotiKeyCounters[0]=0; }
  if (KEY_P2==0) { PotiKeyCounters[1]++; if (PotiKeyCounters[1]==KEY_LONG) PotiKeys+=(2<<2);  } else { if (PotiKeyCounters[1]>KEY_SHORT&&PotiKeyCounters[1]<KEY_LONG) PotiKeys+=(1<<2);  PotiKeyCounters[1]=0; }
  if (KEY_P3==0) { PotiKeyCounters[2]++; if (PotiKeyCounters[2]==KEY_LONG) PotiKeys+=(2<<4);  } else { if (PotiKeyCounters[2]>KEY_SHORT&&PotiKeyCounters[2]<KEY_LONG) PotiKeys+=(1<<4);  PotiKeyCounters[2]=0; }
  if (KEY_P4==0) { PotiKeyCounters[3]++; if (PotiKeyCounters[3]==KEY_LONG) PotiKeys+=(2<<6);  } else { if (PotiKeyCounters[3]>KEY_SHORT&&PotiKeyCounters[3]<KEY_LONG) PotiKeys+=(1<<6);  PotiKeyCounters[3]=0; }
  if (KEY_P5==0) { PotiKeyCounters[4]++; if (PotiKeyCounters[4]==KEY_LONG) PotiKeys+=(2<<8);  } else { if (PotiKeyCounters[4]>KEY_SHORT&&PotiKeyCounters[4]<KEY_LONG) PotiKeys+=(1<<8);  PotiKeyCounters[4]=0; }
  if (KEY_P6==0) { PotiKeyCounters[5]++; if (PotiKeyCounters[5]==KEY_LONG) PotiKeys+=(2<<10); } else { if (PotiKeyCounters[5]>KEY_SHORT&&PotiKeyCounters[5]<KEY_LONG) PotiKeys+=(1<<10); PotiKeyCounters[5]=0; }
  if (PotiKeys!=0) MISC_PotiKeys=PotiKeys;
  /* return key */
  if (Keys!=0)
    MISC_Keys=(NO_KEYS==6)? Keys:(Keys&0x3F000)|((Keys&0x0FFF)>>2);
}

float MISC_LogParam(float d)
{ d=(0.1*exp(LN10*d)*1.1-0.1);
  return(LIMIT(d,0,1));
}

int MISC_CompareIntArrays(i32 int *a,i32 int *b,int l)
{ int i;

  for(i=0;i<l&&a[i]==b[i];i++);
  return((i==l)? 0:1);
}

int MISC_GetPrevPrime(int i)
{ int j;

  /* biggest odd number */
  if ((i%2)==0) i--;
  /* try ... */
  for(;i>1;i-=2)
  { j=sqrt(i);
    if ((j%2)==0) j--;
    for(;j>1&&(i%j)!=0;j-=2);
    if (j<=1) break;
  }
  return(i);
}

#else
#undef HEADER_MISC
#endif

#endif
