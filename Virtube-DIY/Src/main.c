/** @file main.c
  * the main program skeleton
  *
  * @author Edwin Krasser, Ingenieurb&uuml;ro f&uuml;r Elektrotechnik, Schwerpunkt F&amp;E Elektronik
  * @version 0.9
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

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define HEADER_ADCDAC
#include "adcdac.c"
#define HEADER_AMPS
#include "amps.c"
#define HEADER_CONSTS
#include "consts.c"
#define HEADER_EQUALIZER
#include "equalizer.c"
#define HEADER_GLOBAL
#include "global.c"
#define HEADER_I2C
#include "i2c.c"
#define HEADER_MISC
#include "misc.c"
#define HEADER_NOISEGATE
#include "noisegate.c"
#define HEADER_NOSPRINTF
#include "nosprintf.c"
#define HEADER_POWERDAC
#include "powerdac.c"
#define HEADER_REVERB
#include "reverb.c"
#define HEADER_SCIA
#include "scia.c"
#define HEADER_TUBESCR
#include "tubescr.c"

/** head phone detect */
#define HEADPHONE_DET      ((GpioDataRegs.GPCDAT.bit.GPIO73==0)? 1:0)
/** headphones flag */
static int HeadphoneFlag;

/** seconds counter */
static unsigned i32 int SecCounter;

/** adc interrupt service routine */
interrupt void AdcIsr(void)
{ float vi;                  /* input value */
  float vo;           /* output value */

  vi=ADC_IN;
  vo=AMPS_Calc(vi);
  vo=LIMIT(vo,-1.24,1.24);

  /* headphone flag check and output */
  if (HeadphoneFlag==0) DAC1_SET(vo*(1/1.25)); else DAC1_SET(0);
  /* set power dac, if there is none, it's no problem */
  if (HeadphoneFlag==0)
  { if(PDAC_Status.bit.V5Mode!=0) vo*=0.57;
    PDAC_SET((int)(vo*(0x7FFF/1.25)));
  } else
    PDAC_SET(0);
  /* headzhone output */
  DAC2_SET(vo*(1/1.25));

  ADC_ACK();
  /* check keys */
  MISC_CheckKeys();
}

/* include the protocol stuff to make the file shorter */
#include "mainprot.c"

void TheMain(void)
{ unsigned i32 int NextTicker1,NextTicker10,NextTicker25;

  /* initialize hardware */
  AD_Init(AdcIsr);
  I2C_Init();
  MISC_Init();
  PDAC_Init();
  AMPS_Init();
  SCIA_Init(115200,EVEN_PARITY);
  /* initialize effects */
  NG_Init();
  TS_Init();
  EQ_Init();
  RV_Init();

  G_EINT;
  SCIA_Transmit("\nVirtube says hello!",0); G_msWait(5);
  /* the main loop */
  for(SecCounter=0,NextTicker1=G_Ticker-CLK_FREQ,NextTicker10=G_Ticker-(CLK_FREQ/10),NextTicker25=G_Ticker-(0.5*CLK_FREQ/25);;)
  { /* idle ... */
    G_IDLE;
    /* poll for uart commands */
  	if (SCIA_Poll()!=NULL) ParseCommand(NULL,NULL);
  
  	/** poll the keys */
  	if (MISC_Keys!=0||MISC_PotiKeys>0)
  	{

      /* clear key press */
      MISC_Keys=MISC_PotiKeys=0;
  	}

    if ((signed long int)(G_Ticker-NextTicker25)<=0)
    { /* 25 times per second ... */

      /* set switches */
  	  AMPS_SetSwitches(SWITCH1|(SWITCH2<<1)|(SWITCH3<<2));
      /* get pots and start poti conversion */
      AD_GetMiscADCs();
      /* set amp parameters */
      AMPS_SetParams(AD_Pots);

      /* wait for 40 ms */
      NextTicker25-=(CLK_FREQ/25);
    }

    if ((signed long int)(G_Ticker-NextTicker10)<=0)
    { 
  

      /* check headphones */
      HeadphoneFlag=HEADPHONE_DET;
      /* check power dac */
      PDAC_Check();

      /* wait for 100 ms */
      NextTicker10-=(CLK_FREQ/10);
    }

    /* once per second */
    if ((signed long int)(G_Ticker-NextTicker1)<=0)
  	{

      SecCounter++;
	    /* update counter */
	    NextTicker1-=CLK_FREQ;
    }
  }
}

extern int RamfuncsLoadStart;
extern int RamfuncsLoadSize;
extern int RamfuncsRunStart;

#pragma CODE_SECTION(main,"Flash")
int main(void)
{ /* global initialization */
  G_Init();
  /* flash to ram copy */
  #ifdef FLASH
  memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(unsigned i32 int)&RamfuncsLoadSize);
  #endif
  TheMain();
}
