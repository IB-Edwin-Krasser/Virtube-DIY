/** @file i2c.c
  * i2c software interface using i2ca (without interrupts)
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

#ifndef __I2C_C_INCLUDED
#define __I2C_C_INCLUDED

#define HEADER_GLOBAL
#include "global.c"

/** send no stop condition */
#define I2C_NOSTOP    0
/** send stop condition */
#define I2C_STOP      1

/** initialized i2c interface */
extern void I2C_Init(void);
/** sends data stream
  * @param SlaveAddr slave address 
  * @param TXData data array for transmitting (each int represents on byte)
  * @param Len data array length
  * @param SSFlags start/stop condition flags (0..none, 1..stop, 2..start, 3..both)
  * @return 0 if successful, !=0 if error occured */
extern int I2C_TXFrame(int SlaveAddr,unsigned i16 int *TXData,int Len,int SSFlags);
/** sends and receives complete data stream
  * @param SlaveAddr slave address 
  * @param RXData data array for reception (each int represents on byte)
  * @param Len data array length
  * @param SSFlags start/stop condition flags (0..none, 1..stop, 2..start, 3..both)
  * @return 0 if successful, !=0 if error occured */
extern int I2C_RXFrame(int SlaveAddr,unsigned i16 int *RXData,int Len,int SSFlags);

#ifndef HEADER_I2C

/** input clock frequency */
#define I2C_INCLK_FREQ 10000000L
/** i2c clock frequency */
#define I2C_SCL_FREQ   100000L

void I2C_Init(void)
{ int Ticks;

  EALLOW;
  /* enable i2ca clock */
  CpuSysRegs.PCLKCR9.bit.I2C_A=1;
  NOP();
  
  I2caRegs.I2CMDR.all=0;                            /* reset i2c module */
  G_usWait(100);                                    /* wait a little bit */
  I2caRegs.I2CPSC.all=CLK_FREQ/I2C_INCLK_FREQ-1;    /* clock prescaler: 10 MHz input clock frequency */
  Ticks=I2C_INCLK_FREQ/I2C_SCL_FREQ-10;             /* sum of i2c clock values */
  I2caRegs.I2CCLKH=Ticks/2;                         /* set hi/lo clock values */
  I2caRegs.I2CCLKL=Ticks-I2caRegs.I2CCLKH;
  
  I2caRegs.I2CMDR.all=0x0020;                       /* take i2c out of reset */

  I2caRegs.I2CFFTX.all=0x6000;                      /* enable fifo mode and txfifo */
  I2caRegs.I2CFFRX.all=0x2000;                      /* enable rxfifo */  
    
  GpioCtrlRegs.GPCGMUX2.bit.GPIO91=                 /* use GPIOs 91 and 92 for i2c a */
  GpioCtrlRegs.GPCGMUX2.bit.GPIO92=1;
  GpioCtrlRegs.GPCMUX2.bit.GPIO91=
  GpioCtrlRegs.GPCMUX2.bit.GPIO92=2;

  EDIS;    
}

/** waits until the read or write transfer completed
  * @return 0 if ok, !=0 if an error occured (e.g. missing acknowledge */
static int I2C_Wait()
{ int NACK;

  G_usWait((12E6/I2C_SCL_FREQ)*(I2caRegs.I2CCNT+1)); /* 20 percent for safety */
  NACK=I2caRegs.I2CSTR.bit.NACK;
  I2caRegs.I2CSTR.bit.NACK=1;
  if (NACK!=0&&I2caRegs.I2CSTR.bit.BB!=0) I2C_Init();
  return(NACK);
}

int I2C_TXFrame(int SlaveAddr,unsigned i16 int *TXData,int Len,int SSFlags)
{ int i;
  
  /* check if busy */
  if (I2caRegs.I2CSTR.bit.BB!=0) return(1);
  /* copy data to fifo and start transfer */
  I2caRegs.I2CSAR.bit.SAR=SlaveAddr;
  for(i=0;i<Len;i++) I2caRegs.I2CDXR.all=TXData[i];
  I2caRegs.I2CCNT=Len;
  I2caRegs.I2CMDR.all=0x2620|(((SSFlags&1)!=0)? 0x0800:0);
  return(I2C_Wait());
}

int I2C_RXFrame(int SlaveAddr,unsigned i16 int *RXData,int Len,int SSFlags)
{ int i;

  /* copy data to fifo and start read transfer */
  I2caRegs.I2CSAR.bit.SAR=SlaveAddr;
  I2caRegs.I2CCNT=Len;
  I2caRegs.I2CMDR.all=0x2420|(((SSFlags&1)!=0)? 0x0800:0);
  I2C_Wait();
  if (I2caRegs.I2CFFRX.bit.RXFFST!=Len) return(1);
  for(i=0;i<Len;i++) RXData[i]=I2caRegs.I2CDRR.all;
  return(0);
}

#else
#undef HEADER_I2C
#endif

#endif
