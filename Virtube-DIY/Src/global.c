/** @file global.c
  * contains globally used values and functions
  *
  * standard configuration of dsp: PLL factor max, high speed and low speed clock at full speed
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

#ifndef __GLOBAL_C_INCLUDED
#define __GLOBAL_C_INCLUDED

/* declare all DSP28 registers and include C28 specific declarations */
#include "F2837xS_device.h"

/** oscillator frequency */
#define OSC_FREQ   10000000L
/** cpu clock frequency */
#define CLK_FREQ   (OSC_FREQ*20)
/** cpu low speed clock frequency (full speed, 1/2 is reset value) */
#define LSCLK_FREQ (CLK_FREQ)

/* macros */

/** pi */
#define PI       3.141592654
/** ln(10) */
#define LN10     2.302585093
/** 2^32 */
#define TWOPOW32 4294967296.0
/** sqrt(2) */
#define SQRT2    1.414213562
/** 2^(1/12) */
#define TWOPOW1DIV12 1.05946
/** square */
#define SQR(a)   ((a)*(a))
/** cube */
#define CUBE(a)  ((a)*(a)*(a))
/** minimum */
#define MIN(a,b) ((a)<(b)? (a):(b))
/** maximum */
#define MAX(a,b) ((a)>(b)? (a):(b))
/** limiter macro */
#define LIMIT(x,Min,Max)  (((x)<(Min))? (Min):((x)>(Max))? (Max):(x))
/** integer limiter macro */
#define ILIMIT(x,Min,Max) __IQsat(x,Max,Min)
/** error value */
#define ERRORVAL          -1000
/** error check */
#define ERROR(x,Min,Max)  (((x)<(Min))? ERRORVAL:((x)>=ERRORVAL)? ERRORVAL:(x))
/** delay loop
  * @param Loops delay in loops */
#define DELAY_LOOP(Loops) { volatile long int i; for(i=0;i<(Loops);i++); }
/** 16 bit int modifier */
#define i16 short
/** 32 bit int modifier */
#define i32 long
/** 64 bit int modifier */
#define i64 long long
/** never execute this line */
#define NEVER if (0)
/** no operation */
#define NOP()    asm(" NOP")
/** wait a very short time */
#define WAIT10() asm(" rpt #9 || NOP")
/** wait a short time */
#define WAIT()   asm(" rpt #255 || NOP")

/** enables interrupts (with depth control) */
#define G_EINT { G_IntDisDepth--; if (G_IntDisDepth<=0) { G_IntDisDepth=0; EINT; } }
/** disables interrupts (with depth control) */
#define G_DINT { G_IntDisDepth++; DINT; }
/** start idling */
#define G_IDLE asm(" IDLE");

/** global ticker */
#define G_Ticker     CpuTimer0Regs.TIM.all
/** ticks per second of global ticker */
#define G_TickerTPS  CLK_FREQ

/** main */
extern int main(void);

/* global variables */

/** interrupt disable depth */
extern int G_IntDisDepth;

/* functions */

/** unsecures dsp (disable csm, disable watchdog) */
extern void G_Unsecure(void);
/** software reset */
extern void G_SWReset(void);
/** global init function (for hardware and software), enable interrupts later in program
  * @return 0 if ok, != if error occured */
extern void G_Init(void);
/** wait function (using cpu timer0)
  * @param ms waiting time in ms
  *
  * @see CLKS_PER_US */
extern void G_msWait(unsigned long int ms);
/** wait function (using cpu timer0)
  * @param us waiting time in us
  *
  * @see CLKS_PER_US */
extern void G_usWait(unsigned long int us);
/** clocks per micro second */
#define CLKS_PER_US 200
/** wait function (using timer0)
  * @param us waiting time in us
  *
  * @see CLKS_PER_US */
extern void G_SetTimeout(unsigned long int us);
/** checks if timeout
  * @return !=0 if timeout */
extern int G_Timeout();

#ifndef HEADER_GLOBAL

#include <math.h>
/* define all DSP28 registers */
#include "../source/F2837xS_GlobalVariableDefs.c"

int G_IntDisDepth;
/** timeout ticker value */
unsigned long int G_TimeoutValue;

#pragma CODE_SECTION(G_Unsecure,"Flash")
void G_Unsecure(void)
{ /* allow access to protected memory regions */
  EALLOW;
  /* initialize system control registers */
  WdRegs.WDCR.all=0x6F;      /* disable watchdog and clear wdflag */

  DcsmZ1Regs.Z1_CSMKEY0=0xFFFFFFFF;
  DcsmZ1Regs.Z1_CSMKEY1=0xFFFFFFFF;
  DcsmZ1Regs.Z1_CSMKEY2=0xFFFFFFFF;
  DcsmZ1Regs.Z1_CSMKEY3=0xFFFFFFFF;

  DcsmZ2Regs.Z2_CSMKEY0=0xFFFFFFFF;
  DcsmZ2Regs.Z2_CSMKEY1=0xFFFFFFFF;
  DcsmZ2Regs.Z2_CSMKEY2=0xFFFFFFFF;
  DcsmZ2Regs.Z2_CSMKEY3=0xFFFFFFFF;

  /* disable access to protected memory regions */
  EDIS;
}

void G_SWReset(void)
{ EALLOW;
  if (WdRegs.SCSR.bit.WDOVERRIDE==0) WdRegs.SCSR.bit.WDOVERRIDE=0x01;
                             /* allow watchdog registers modification */
  WdRegs.WDCR.all=0x0028;    /* bit 15,14,13,12,11,10,9,8=reserved
                              * bit 7=0: no write to WDFLAG
                              * bit 6=0: enable watchdog
                              * bit 5,4,3=101: allow write to register
                              * bit 2,1,0=000: maximum watchdog clock */
  EDIS;
  DINT;
  /* wait for reset */
  for(;;);
}

#pragma CODE_SECTION(G_Init,"Flash")
void G_Init(void)
{ volatile int i;
  /* intialize hardware */

  /* unsecure dsp in RAM version, begause in flash versions unsecure is called before copying code tu ram */
  G_Unsecure();

  /* allow access to protected memory regions */
  EALLOW;
  /* initialize cpu timer 0 for 32 bit counting */
  CpuSysRegs.PCLKCR0.bit.CPUTIMER0=1;
  CpuTimer0Regs.PRD.all=0xFFFFFFFF;
  CpuTimer0Regs.TPR.bit.TDDR=0;
  CpuTimer0Regs.TPRH.bit.TDDRH=0;
  CpuTimer0Regs.TCR.all=0x8020; /* bit 15=1: clear interrupt flag
                                 * bit 14=0: disable cpu timer 0 interrupt
                                 * bit 13,12=reserved
                                 * bit 11,10=00: stop immediately on emulation suspend
                                 * bit 9,8,7,6=reserved
                                 * bit 5=1: reload timer
                                 * bit 4=0: start timer
                                 * bit 3,2,1,0=reserved */
  /* disable and clear interrupts */
  DINT;       /* disable all interrupts */
  IER=0x0000; /* clear all interrupts */
  IFR=0x0000;

  /* initialize PIE control registers */
  PieCtrlRegs.PIECTRL.bit.ENPIE=1;
  PieCtrlRegs.PIEACK.all=0xFFFF;
  /* use external crystal oscillator and turn off internal oscillator */
  ClkCfgRegs.CLKSRCCTL1.bit.XTALOFF=0;      /* turn on XTALOSC */
  ClkCfgRegs.CLKSRCCTL1.bit.OSCCLKSRCSEL=1; /* Clk Src = XTAL */

  ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN=0;
  asm(" rpt #20 || nop");
  ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV=0;
  /* Lock the PLL five times. This helps ensure a successful start. */
  for(i=0;i<5;i++)
  { ClkCfgRegs.SYSPLLCTL1.bit.PLLEN=0;        /* Turn off PLL */
    asm(" rpt #20 || nop");
    ClkCfgRegs.SYSPLLMULT.all=0x0014;         /* Write multiplier, which automatically turns on the PLL */
    while(ClkCfgRegs.SYSPLLSTS.bit.LOCKS!=1); /* Wait for the SYSPLL lock counter */
  }
  /* low speed clock at full speed */
  ClkCfgRegs.LOSPCP.bit.LSPCLKDIV=0;

  /* some stuff copied from the InitSysPLL function of the examples */
  ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV=1;
  DevCfgRegs.SYSDBGCTL.bit.BIT_0=1;
  ClkCfgRegs.SYSPLLCTL1.bit.PLLCLKEN=1;
  asm(" RPT #20 || NOP");
  DevCfgRegs.SYSDBGCTL.bit.BIT_0 = 0;
  asm(" RPT #200 || NOP");
  ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV=0;

  /* enable all pull up resistors */
  GpioCtrlRegs.GPAPUD.all=
  GpioCtrlRegs.GPBPUD.all=
  GpioCtrlRegs.GPCPUD.all=
  GpioCtrlRegs.GPDPUD.all=0;

  /* finish hardware setup */
  EDIS; /* disable access to protected memory regions */
        /* enable all interrupts later in program */

  /* interrupts disable */
  G_IntDisDepth=0;
  G_DINT;
}

void G_usWait(unsigned long int us)
{ unsigned long int StopTime;

  asm(" clrc ovm");
  StopTime=CpuTimer0Regs.TIM.all-((unsigned long int)CLKS_PER_US)*us;
  while((signed long int)(CpuTimer0Regs.TIM.all-StopTime)>0);
}

void G_msWait(unsigned long int ms)
{ G_usWait(1000*ms);
}

void G_SetTimeout(unsigned long int us)
{ asm(" clrc ovm");
  G_TimeoutValue=G_Ticker-CLKS_PER_US*us;
}

int G_Timeout()
{ return((signed long int)(G_Ticker-G_TimeoutValue)<0);
}

#else
#undef HEADER_GLOBAL
#endif

#endif
