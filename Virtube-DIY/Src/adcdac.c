/** @file adcdac.c
  * internal adc and dac software interface
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

#ifndef __ADCDAC_C_INCLUDED
#define __ADCDAC_C_INCLUDED

#define HEADER_GLOBAL
#include "global.c"

/** sampling frequency */
#define ADC_FSAMPLE      40064.10256

/** adc input value */
#define ADC_IN           (((signed i32 int)AdcaResultRegs.ADCRESULT0-0x8000)*(2.5/0x10000))
/** adc interrupt acknowledge */
#define ADC_ACK()        PieCtrlRegs.PIEACK.bit.ACK1=1; AdcaRegs.ADCINTFLGCLR.all=0x0001

/** sets dac 1 output value */
#define DAC1_SET(x)      DacaRegs.DACVALS.all=0x800+0.5+(0x7FF*(x))
/** sets dac 2 output value */
#define DAC2_SET(x)      DacbRegs.DACVALS.all=0x800+0.5+(0x7FF*(x))

/** the pots values in percent */
extern i32 int AD_Pots[6];
/** the power supply voltage adc result in mV */
extern i32 int AD_PVDD;

/** initializes adc and dac
  * @param AdcIsr adc interrupt service routine, if NULL a simple reinitialization is done */
extern void AD_Init(void (*AdcIsr)(void));
/** gets poti values and more
  *
  * @see AD_Pots
  * @see AD_PVDD  */
extern void AD_GetMiscADCs(void);

#ifndef HEADER_ADCDAC

#define HEADER_MISC
#include "misc.c"
#define HEADER_POWERDAC
#include "powerdac.c"

i32 int AD_Pots[6];
i32 int AD_PVDD;

void AD_Init(void (*AdcIsr)(void))
{
  /* allow access to protected memory regions */
  EALLOW;

  CpuSysRegs.PCLKCR13.bit.ADC_B=1;    /* enable adcb clock */
  CpuSysRegs.PCLKCR16.bit.DAC_A=1;    /* enable daca clock */
  CpuSysRegs.PCLKCR16.bit.DAC_B=1;    /* enable dacb clock */
  CpuSysRegs.PCLKCR0.bit.TBCLKSYNC=1; /* enable clock sync */

  CpuSysRegs.PCLKCR13.bit.ADC_A=1;    /* enable adca clock */
  CpuSysRegs.PCLKCR2.bit.EPWM1=1;     /* enable epwm1 clock */

  /* power down adcs and wait a little bit */
  AdcaRegs.ADCCTL1.bit.ADCPWDNZ=0;
  G_msWait(5);

  /* setup adcb for guitar signal */
  AdcaRegs.ADCCTL1.all=0x0082;         /* bit 15-8: read only
                                        * bit 7=1: power up adc
                                        * bit 6-3: reserved
                                        * bit 2=1: interrupt pulse at end of conversion
                                        * bit 1-0: reserved */
  AdcaRegs.ADCCTL2.all=0x00CF;         /* bit 15-8: read only
                                        * bit 7=1: differential input
                                        * bit 6=1: 16 bit resolution
                                        * bit 5-4: reserved
                                        * bit 3-0=1111, prescaler 8.5 */

  AdcaRegs.ADCBURSTCTL.all=0x0000;     /* no burst mode */
  AdcaRegs.ADCINTSEL1N2.all=0x0020;    /* bit 15-8: adcint2, no interest here
                                        * bit 7: reserved
                                        * bit 6=0: no continuous adc int
                                        * bit 5=1: enable adcint1
                                        * bit 4: reserved
                                        * bit 3-0=0: eoc0 triggers adcint1 */

  AdcaRegs.ADCSOC0CTL.all=0x00528080;  /* bit 24-20=00101: epwm1, adcsoca
                                        * bit 19: reserved
                                        * bit 18-15=0101: adcin4/5
                                        * bit 14-9: reserved
                                        * bit 8-0=0x080: 128 samples acquisition */

                                       /* configure adc interrupt */
  PieVectTable.ADCA1_INT=AdcIsr;       /* set adc interrupt vector */
  PieCtrlRegs.PIEIER1.bit.INTx1=1;     /* enable adc interrupt in pie */
  IER|=M_INT1;                         /* enable global interrupt 1: adc */

  /* configure epwm1 */
  EPwm1Regs.TBCTL.all=0x2040;          /* bit 15,14=00: stop on emulation suspend
                                        * bit 13=0: count up after sync
                                        * bit 12,11,10=000: clock prescaler 0
                                        * bit 9,8,7=000: hispeed clock prescaler 0
                                        * bit 6=1: no software forced synchronisation
                                        * bit 5,4=0: synci of no interest
                                        * bit 3=0: load period value from shadow register
                                        * bit 2=0: counter cannot be loaded from phase register
                                        * bit 1,0=00: count up */
  EPwm1Regs.TBPRD=CLK_FREQ/ADC_FSAMPLE-1;
  ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV=0;
                                       /* set timer period */
  EPwm1Regs.ETSEL.all=0x0900;          /* bit 15=0: disable socb pulse
                                        * bit 14,13,12=000: spare socb select
                                        * bit 11=1: enable soca pulse
                                        * bit 10,9,8=001: enable counter equals zero event
                                        * bit 7,6,5,4=reserved
                                        * bit 3=0: no timer interrupt
                                        * bit 2,1,0=000: spare timer interrupt */
  EPwm1Regs.ETPS.bit.SOCAPRD=1;        /* generate pulse on first event */

  AdcbRegs.ADCCTL1.bit.ADCPWDNZ=0;
  G_msWait(5);
  /* setup adc b for poti positions */
  AdcbRegs.ADCCTL1.all=0x0080;         /* bit 15-8: read only
                                        * bit 7=1: power up adc
                                        * bit 6-3: reserved
                                        * bit 2=0: interrupt pulse at end of conversion
                                        * bit 1-0: reserved */
  AdcbRegs.ADCCTL2.all=0x004F;         /* bit 15-8: read only
                                        * bit 7=0: single ended input
                                        * bit 6=1: 16 bit resolution (in output registers)
                                        * bit 5-4: reserved
                                        * bit 3-0=1111, prescaler 8.5 */
  AdcbRegs.ADCBURSTCTL.all=0x0000;     /* no burst mode */
  AdcbRegs.ADCINTSEL1N2.all=0x0000;    /* bit 15-8: adcint2, no interest here
                                        * bit 7: reserved
                                        * bit 6=0: no adcint1
                                        * bit 5=0: disable adcint1
                                        * bit 4: reserved
                                        * bit 3-0=0: eoc0 triggers adcint1 */

  AdcbRegs.ADCSOC0CTL.all=0x00000080;  /* bit 24-20=00000: trigger by software
                                        * bit 19: reserved
                                        * bit 18-15=0000: adcinb0
                                        * bit 14-9: reserved
                                        * bit 8-0=0x080: 128 samples acquisition */
  AdcbRegs.ADCSOC1CTL.all=AdcbRegs.ADCSOC0CTL.all; AdcbRegs.ADCSOC1CTL.bit.CHSEL=1;  /* inb1 */
  AdcbRegs.ADCSOC2CTL.all=AdcbRegs.ADCSOC0CTL.all; AdcbRegs.ADCSOC2CTL.bit.CHSEL=2;  /* inb2 */
  AdcbRegs.ADCSOC3CTL.all=AdcbRegs.ADCSOC0CTL.all; AdcbRegs.ADCSOC3CTL.bit.CHSEL=3;  /* inb3 */
  AdcbRegs.ADCSOC4CTL.all=AdcbRegs.ADCSOC0CTL.all; AdcbRegs.ADCSOC4CTL.bit.CHSEL=4;  /* inb4 */
  AdcbRegs.ADCSOC5CTL.all=AdcbRegs.ADCSOC0CTL.all; AdcbRegs.ADCSOC5CTL.bit.CHSEL=5;  /* inb5 */
  AdcbRegs.ADCSOC6CTL.all=AdcbRegs.ADCSOC0CTL.all; AdcbRegs.ADCSOC6CTL.bit.CHSEL=14; /* in14 */
  AD_GetMiscADCs();

  /* setup dac */
  DacaRegs.DACCTL.all=0x0005;          /* bit 15-8: reserved
                                        * bit 7-4=0: EPWM1SYNCPER
                                        * bit 3: reserver
                                        * bit 2=1: load on next EPWMSYNCPER
                                        * bit 1: reserved
                                        * bit 0=1: adc voltage reference */
  DacaRegs.DACOUTEN.bit.DACOUTEN=1;    /* enable output: adcina1 is dacb output */

  /* same for dac b */
  DacbRegs.DACCTL.all=DacaRegs.DACCTL.all;
  DacbRegs.DACOUTEN.bit.DACOUTEN=DacaRegs.DACOUTEN.bit.DACOUTEN;

  /* finish hardware setup */
  EDIS; /* disable access to protected memory regions */

  AD_PVDD=0;
}

void AD_GetMiscADCs(void)
{ int i;

  /* get adc value */
  AD_Pots[0]=AdcbResultRegs.ADCRESULT0;
  AD_Pots[1]=AdcbResultRegs.ADCRESULT1;
  AD_Pots[2]=AdcbResultRegs.ADCRESULT2;
  AD_Pots[3]=AdcbResultRegs.ADCRESULT3;
  AD_Pots[4]=AdcbResultRegs.ADCRESULT4;
  AD_Pots[5]=AdcbResultRegs.ADCRESULT5;
  for(i=0;i<6;i++)
  { AD_Pots[i]=((AD_Pots[i]*106)>>16)-3;
    AD_Pots[i]=LIMIT(AD_Pots[i],0,100);
  }
  AD_PVDD=AdcbResultRegs.ADCRESULT6*(1+180.0/22.0)*(2500.0/0x10000);
  AdcbRegs.ADCSOCFRC1.all=0x007F;
}

#else
#undef HEADER_ADCDAC
#endif

#endif

