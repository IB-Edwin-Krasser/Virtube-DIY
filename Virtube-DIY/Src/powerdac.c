/** @file powerdac.c
  * dac with power amplifier interface to TAS5828 (Texas Instruments) connected over mcbspb, i2ca and some gpios
  *
  * There are three power modes: 24V suply, 12V supply and 4.5V supply (USB).
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

#ifndef __POWERDAC_C_INCLUDED
#define __POWERDAC_C_INCLUDED

#define HEADER_ADCDAC
#include "adcdac.c"
#define HEADER_GLOBAL
#include "global.c"

/** mcbsp clock frequency */
#define MCBSP_FDACCLOCK (ADC_FSAMPLE*64)
/** sets power dac output */
#define PDAC_SET(x)     McbspbRegs.DXR1.all=(unsigned i16 int)(x)
/** pdac fatal error */
#define PDAC_FATAL()    (((PDAC_Status.all&0x0F)!=0)? 1:0)

/** type of power dac
  *
  * 0..not available, 1..tas5828, ... */
extern int PDAC_Type;
/** power dac errors
  *
  * The first four are fatal. */
extern union PDAC_StatusBits
{ /** all */
  unsigned i16 int all;
  /** the single bits */
  struct
  { /** 1: not existant */
    unsigned i16 int NotExistant:1;
    /** 2: wrong die id */
    unsigned i16 int DieID:1;
    /** 4: init error */
    unsigned i16 int InitError:1;
    /** 8: i2c error */
    unsigned i16 int I2CError:1;
    /** 16: USB 5V-mode */
    unsigned i16 int V5Mode:1;
    /** 32: 12V mode */
    unsigned i16 int V12Mode:1;
    /** 64: 17V mode (24V with half output power, because of 4 Ohm load or over temperature warning) */
    unsigned i16 int V17Mode:1;
    /** 128: 24V mode */
    unsigned i16 int V24Mode:1;
  } bit;
} PDAC_Status;
/** power dac supply voltage in V */
extern float PDAC_PVDD;
/** warnings register */
extern union PDAC_WarningBits
{ /** all */
  unsigned i16 int all;
  /** the single bits */
  struct
  { /** 1: overtemperature 112°C */
    unsigned i16 int OverTemp112:1;
    /** 2: overtemperature 122°C */
    unsigned i16 int OverTemp122:1;
    /** 4: overtemperature 134°C */
    unsigned i16 int OverTemp134:1;
    /** 8: overtemperature 146°C */
    unsigned i16 int OverTemp146:1;
    /** 16: left channel overgurrent warning */
    unsigned i16 int LeftOverCurrent:1;
    /** 32: right channel overgurrent warning */
    unsigned i16 int RightOverCurrent:1;
  } bit;
} PDAC_Warnings;
/** fault register */
extern union PDAC_FaultsBits
{ /** all */
  unsigned i32 int all;
  struct
  { /* register 0x72 */
    /** 1: over temperature */
    unsigned i32 int OverTemp:1;
    /** 2: left channel cycle by cycle overcurrent */
    unsigned i32 int LeftOverCycleByCycleCurrent:1;
    /** 4: right channel cycle by cycle overcurrent */
    unsigned i32 int RigdtOverCycleByCycleCurrent:1;
    /** reserved */
    unsigned i32 int Reserved3:5;

    /* register 0x71 */
    /** 1: pvdd under voltage */
    unsigned i32 int PVDDUnderVoltage:1;
    /** 2: pvdd over voltage */
    unsigned i32 int PVDDOverVoltage:1;
    /** 4: clock */
    unsigned i32 int Clock:1;
    /** reserved */
    unsigned i32 int Reserved2:2;
    /** 32: eeprom */
    unsigned i32 int EEPROM:1;
    /** 64: bq write */
    unsigned i32 int BQWrite:1;
    /** 128: otp crc */
    unsigned i32 int OTPCRC:1;

    /* register 0x70 */
    /** 1: right channel over current fault */
    unsigned i32 int RightOverCurrent:1;
    /** 2: left channel over current fault */
    unsigned i32 int LeftOverCurrent:1;
    /** 4: right channel dc fault */
    unsigned i32 int RightDC:1;
    /** 16: right channel dc fault */
    unsigned i32 int LeftDC:1;
    /** left channel dc fault */
    unsigned i32 int Reserved1:4;

    /** the unused byte */
    unsigned i32 int Reserved4:8;
  } bit;
} PDAC_Faults;
/** the fault counter */
extern int PDAC_FaultCounter;

/** initializes power dac
  * @return 0 if ok. !=0 if an error occured
  *
  * MISC_Init must be done before because of hardware version */
extern int PDAC_Init(void);
/** checks power amplifier end modifies setup if necessary
  * @return PDAC_Status
  *
  * @see PDAC_Status */
extern int PDAC_Check(void);
/** reads from power dac
  * @param Addr byte address in power dac
  * @param Data pointer to data buffer
  * @param Len length of data in words (maximum of 8)
  * @return 0 if successful, !=0 if error occured */
extern int PDAC_I2CRead(unsigned int Addr,unsigned i16 int *Data,int Len);
/** writes to power dac
  * @param Addr byte address in power dac
  * @param Data pointer to data bytes (every int is treated as one byte!)
  * @param Len length of data in words (maximum of 8)
  * @return 0 if successful, !=0 if error occured */
extern int PDAC_I2CWrite(unsigned int Addr,const unsigned i16 int *Data,int Len);

#ifndef HEADER_POWERDAC

#define SETUP_INTERNAL 0

#if SETUP_INTERNAL!=0
/* the ppc software generated init file */
#include "tas5828_init.h"
#endif

#define HEADER_I2C
#include "i2c.c"
#define HEADER_MISC
#include "misc.c"

/** the slave address */
#define SLAVE_ADDR 0x60

/** not power down output */
#define NPDN   GpioDataRegs.GPCDAT.bit.GPIO69
/** not mute output */
#define NMUTE  GpioDataRegs.GPCDAT.bit.GPIO70
/** not fault input */
#define NFAULT GpioDataRegs.GPCDAT.bit.GPIO71
/** power down detect input */
#define PD_DET GpioDataRegs.GPBDAT.bit.GPIO41

int PDAC_Type;
union PDAC_StatusBits PDAC_Status;
float PDAC_PVDD;
union PDAC_WarningBits PDAC_Warnings;
union PDAC_FaultsBits PDAC_Faults;
int PDAC_FaultCounter;
/** length of receive transmit buffer */
#define RTBUF_LEN 10
/** receive transmit buffer, used as bytes */
static unsigned i16 int RTBuf[RTBUF_LEN];

/** initiasization data, address 0xFF is stop command */
static const unsigned i16 int PDAC_I2CInitData[][2]=
{ /* power path */
  { 0x01, 0x11 }, /* RESET_CTRL: reset */
  { 0x01, 0x00 }, /* RESET_CTRL: relinquish from reset */
  { 0x02, 0x04 }, /* DEVICE_CTRL_1: 384 kHz, PBTL (mono), bd mode */
  { 0x03, 0x02 }, /* DEVICE_CTRL_2: normal operation, start with hi z ... switch to play mode after complete initialization */
  { 0x04, 0x03 }, /* PVDD_DROP_DETECTION_CTRL1: keep playing but output pvdd detection, output detection result using 16 samples */
  { 0x05, 0x4C }, /* PVDD_DROP_DETECTION_CTRL2: detection voltage about 9V */
  /* digital interface: most registers remain in reset values */
  { 0x28, 0x00 }, /* SIG_CH_CTRL1: standard values, 16 bit data */
  { 0x29, 0x10 }, /* CLOCK_DET_CTRL: disable frame sync (or samling rate) failure detection:
                   * There is a sampling rate monitoring inside the ic, which compares the input frequengy with common values like 32k, 44k, 48k ... to allow
                   * other sampling rates than these values we need to disable this fault detection. Because it is no fault! */
  { 0x33, 0x00 }, /* SAP_CTRL1: standard i2s but just 16 bit word length */
  /* behaviour stuff */
  { 0x50, 0x03 }, /* AUTO_MUTE_CTRL: enable in standard time */
  { 0x54, 0x1F }, /* AGAIN to 5V mode */

  { 0xFF, 0xFF }  /* end value */
};

/** reads from power dac
  * @param Addr byte address in power dac
  * @param Data pointer to data buffer
  * @param Len length of data in words (maximum of 8)
  * @return 0 if successful, !=0 if error occured */
int PDAC_I2CRead(unsigned int Addr,unsigned i16 int *Data,int Len)
{ int RetVal,Counter;

  RTBuf[0]=Addr;
  if (I2C_TXFrame(SLAVE_ADDR,RTBuf,1,I2C_NOSTOP)==0)
  { RetVal=I2C_RXFrame(SLAVE_ADDR,RTBuf,Len,I2C_STOP);
    for(Counter=0;Counter<Len;Counter++) Data[Counter]=RTBuf[Counter];
    return(RetVal);
  } else
    return(1);
}

/** writes to power dac
  * @param Addr byte address in power dac
  * @param Data pointer to data bytes (every int is treated as one byte!)
  * @param Len length of data in words (maximum of 8)
  * @return 0 if successful, !=0 if error occured */
int PDAC_I2CWrite(unsigned int Addr,const unsigned i16 int *Data,int Len)
{ int RetVal,Counter;

  RTBuf[0]=Addr;
  for(Counter=0;Counter<Len;Counter++) RTBuf[Counter+1]=Data[Counter];
  RetVal=I2C_TXFrame(SLAVE_ADDR,RTBuf,Len+1,I2C_STOP);
  return(RetVal);
}

int PDAC_Init(void)
{ int i;
  unsigned i16 int Data;

  /* disable power down output */
  EALLOW; GpioCtrlRegs.GPCDIR.bit.GPIO69=0; EDIS;
  G_usWait(10);
  /* check power down pull down */
  PDAC_Status.all=0;
  if (NPDN!=0)
  { PDAC_Type=0;
    PDAC_Status.bit.NotExistant=1;
    return(1);
  } else
    PDAC_Type=1;
  /* allow access to protected memory regions */
  EALLOW;

  /* enable mcbsp clock */
  CpuSysRegs.PCLKCR11.bit.McBSP_B=1;
  NOP();

  /* setup mcbspb as i2s output */
  McbspbRegs.SPCR2.all=0x0000;  /* bit 15,14,13,12,11,10=reserved
                                 * bit 9,8=00: stop immediately on emulation suspend
                                 * bit 7=0: reset frame synchronisation generator
                                 * bit 6=0: reset sample generator
                                 * bit 5,4=00: transmit interrupt mode
                                 * bit 3=0: no transmit synchronisation error
                                 * bit 2,1=read only status bits
                                 * bit 0=0: reset serial port transmitter */
  McbspbRegs.SPCR1.all=0x4000;  /* bit 15=0: digital loopback mode off
                                 * bit 14,13=10: left justify (fill lsb with zeros)
                                 * bit 12,11=00: clock stop mode disabled
                                 * bit 10,9,8=reserved
                                 * bit 7=0: dx enabler off
                                 * bit 6=0: abis mode disable
                                 * bit 5,4=00: receive interrupt driven by receive ready
                                 * bit 3=0: clear synchronisation error
                                 * bit 2,1=read only status bits
                                 * bit 0=0: reset receiver */
  McbspbRegs.XCR1.all=0x0040;   /* bit 15=reserved
                                 * bit 14,13,12,11,10,9,8=0000000: 1 words tansmitter frame length of phase 1
                                 * bit 7,6,5=010: 16 bit tansmitter word length of phase 1
                                 * bit 4,3,2,1,0=reserved */
  McbspbRegs.XCR2.all=0x0001;   /* bit 15=0: single phase frame
                                 * bit 14,13,12,11,10,9,8=0000000: tansmitter frame length of phase 2
                                 * bit 7,6,5=0: tansmitter word length of phase 2
                                 * bit 4,3=00: no companding
                                 * bit 2=0: every frame synchronisation pulse
                                 * bit 1,0=01: one bit data delay */

  McbspbRegs.SRGR2.all=0x303F;  /* bit 15=0: sample rate generator is free running
                                 * bit 14=reserved
                                 * bit 13=1: use internal low speed clock
                                 * bit 12=1: transmit frame sync signal driven by sample rate generator
                                 * bit 11,10,9,8,7,6,5,4,3,2,1,0=000000111111: frame sync signal every 64 clock cycles */
  McbspbRegs.SRGR1.bit.FWID=31; /* 32 clock cycles frame sync pulse */
  McbspbRegs.SRGR1.bit.CLKGDV=LSCLK_FREQ/MCBSP_FDACCLOCK-1;
                                /* set mcbsp clock divider */

  McbspbRegs.SPCR2.all|=0x00C1; /* relinquish from reset */
  McbspbRegs.SPCR1.all|=0x0001; /* relinquish from reset */

  McbspbRegs.MCR2.all=0x0000;   /* no multichannel mode */
  McbspbRegs.MCR1.all=0x0000;   /* no multichannel mode */

  McbspbRegs.PCR.all=0x0F0F;    /* bit 15,14,13,12=reserved
                                 * bit 11=1: transmit frame sync generated by sample rate generator
                                 * bit 10=1: receive frame sync generated by sample rate generator
                                 * bit 9=1: transmitter clock is driven by sample rate generator
                                 * bit 8=1: receiver clock is driven by sample rate generator
                                 * bit 7=0: use internal low speed clock
                                 * bit 6=read only status bit
                                 * bit 5=0: set transmit pin to 0V
                                 * bit 4=read only status bit
                                 * bit 3,2=11: frame sync is active low for transmitting and receiving
                                 * bit 1=1: sample transmit data on falling clock edge
                                 * bit 0=1: sample receive data on rising clock edge */
  McbspbRegs.DXR2.all=0;
  McbspbRegs.DXR1.all=0;

  /* use pins 12, 14, 15 for mcbspb */
  GpioCtrlRegs.GPAMUX1.bit.GPIO12=
  GpioCtrlRegs.GPAMUX1.bit.GPIO14=
  GpioCtrlRegs.GPAMUX1.bit.GPIO15=3;

  /* control connections: mute and power down are outputs, disable pull ups and set values */
  GpioCtrlRegs.GPCDIR.bit.GPIO69=GpioCtrlRegs.GPCDIR.bit.GPIO70=1;
  GpioCtrlRegs.GPCPUD.bit.GPIO69=GpioCtrlRegs.GPCPUD.bit.GPIO70=1;
  NMUTE=0;

  /* power up and wait a little time */
  NPDN=1;
  G_msWait(1);
  /* check die id */
  if (PDAC_I2CRead(0x67,&Data,1)==0&&Data==0x98) /* die id 0x98, the 0x95 in the datasheet is wrong */
  { /* initialize power dac */
    for(i=0;PDAC_I2CInitData[i][0]!=0xFF;i++)
      if (PDAC_I2CWrite(PDAC_I2CInitData[i][0],&PDAC_I2CInitData[i][1],1)!=0)
      { PDAC_Status.bit.InitError=1;
        break;
      }
    PDAC_Status.bit.V5Mode=1;
    /* output zero */
    PDAC_SET(0);
    #if SETUP_INTERNAL!=0
    /* setup internal register stuff */
    for(i=0;i<(sizeof(registers)/sizeof(registers[0]));i++)
    { Data=registers[i].value;
      if (PDAC_I2CWrite(registers[i].offset,&Data,1))
      { PDAC_Status.bit.InitError=1;
        break;
      }
    }
    #endif
   if (PDAC_Status.bit.InitError==0)
   { /* leave high z mode and start playing mode */
     G_msWait(1); Data=0x03; PDAC_I2CWrite(0x03,&Data,1);
   }
  } else
  { PDAC_Status.bit.DieID=1;
    NPDN=0;
  }

  /* finish hardware setup */
  EDIS; /* disable access to protected memory regions */
  PDAC_FaultCounter=0;

  return((PDAC_Status.all==0)? 0:1);
}

int PDAC_Check(void)
{ unsigned i16 int Data,u[3],AGain;

  /* check status for fatal flags at begin */
  if (PDAC_FATAL()==0)
  { /* get status bits ...
     * ... not available in software mode */
    /* get warnings and fault registers */
    PDAC_I2CRead(0x73,&Data,1);
    if (Data!=0) PDAC_Warnings.all=Data;
    PDAC_I2CRead(0x70,u,3);
    if (u[0]!=0||u[1]!=0||u[1]!=0)
    { PDAC_Faults.all=((unsigned i32 int)u[0]<<16)|(u[1]<<8)|u[2];
      /* reset faults */
      if (PDAC_Faults.all!=0&&PDAC_FaultCounter<10)
      { PDAC_FaultCounter++;
        Data=0x80;
        PDAC_I2CWrite(0x78,&Data,1);
      }
    }
    /* get pdac voltage */
    PDAC_I2CRead(0x5E,&Data,1);
    PDAC_PVDD=Data*(1/8.428);
    /* get mode voltage */
    if (PDAC_PVDD>20) Data=24; else
      if (PDAC_PVDD>14) Data=17; else
        if (PDAC_PVDD>8) Data=12; else Data=5;
    /* temperature warning: half power with 17V mode */
    if ((PDAC_Warnings.all&0xFC)!=0&&Data==24) Data=17;
    /* detect voltage change */
    AGain=0;
    switch(Data)
    { case 24: if (PDAC_Status.bit.V24Mode==0) { AGain=0x06; /* 20.8V */ PDAC_Status.bit.V24Mode=1; PDAC_Status.bit.V5Mode=PDAC_Status.bit.V12Mode=PDAC_Status.bit.V17Mode=0; } break;  /* 29.4V*( -6*0.5dB)=20.8V ... approx. 54W at 8Ohm */
      case 17: if (PDAC_Status.bit.V17Mode==0) { AGain=0x0C; /* 14.7V */ PDAC_Status.bit.V17Mode=1; PDAC_Status.bit.V5Mode=PDAC_Status.bit.V12Mode=PDAC_Status.bit.V24Mode=0; } break;  /* 29.4V*(-12*0.5dB)=14.7V ... approx. 27W at 8Ohm or 54W at 4Ohm */
      case 12: if (PDAC_Status.bit.V12Mode==0) { AGain=0x12; /* 10.4V */ PDAC_Status.bit.V12Mode=1; PDAC_Status.bit.V5Mode=PDAC_Status.bit.V17Mode=PDAC_Status.bit.V24Mode=0; } break;  /* 29.4V*(-18*0.5dB)=10.4V ... approx. 13W at 8Ohm */
      default: if (PDAC_Status.bit.V5Mode==0)  { AGain=0x1F; /* 4.95V */ PDAC_Status.bit.V5Mode=1;  PDAC_Status.bit.V12Mode=PDAC_Status.bit.V17Mode=PDAC_Status.bit.V24Mode=0; } break; /* 29.4V*(-31*0.5dB)=4.95V ... output limited to 2.8V (1W at 8Ohm) by multiplication by 0.57 */
    }
    /* set new analog gain */
    if (AGain!=0)
    { Data=0x02; PDAC_I2CWrite(0x03,&Data,1); /* set hi z mode ... */
      if (PDAC_I2CWrite(0x54,&AGain,1)!=0) PDAC_Status.bit.I2CError=1;
      G_msWait(1); Data=0x03; PDAC_I2CWrite(0x03,&Data,1); /* ... back to play mode */
    }
    /* check correct function */
    if (NFAULT==0) return(1);
  } else
    /* if fatal, turn off */
    NPDN=0;
  return(PDAC_Status.all);
}

#else
#undef HEADER_POWERDAC
#endif

#endif
