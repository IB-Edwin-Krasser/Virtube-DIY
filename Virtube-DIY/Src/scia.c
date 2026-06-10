/** @file scia.c
  * scia software interface
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

#ifndef __SCIA_C_INCLUDED
#define __SCIA_C_INCLUDED

#define HEADER_GLOBAL
#include "global.c"

/** sci baud rate */
#define SCIA_BAUDRATE 115200
/** no parity (see CCR) */
#define NO_PARITY   0x00
/** odd parity (see CCR) */
#define ODD_PARITY  0x20
/** even parity (see CCR) */
#define EVEN_PARITY 0x60

/** terminator in command string */
#define SCIA_TERMCHAR '\n'
/** size of send buffer */
#define SCIA_TXBUFFERSIZE 128
/** size of receive buffer */
#define SCIA_RXBUFFERSIZE 128
/** receive buffer */
extern char SCIA_RXBuffer[SCIA_RXBUFFERSIZE];
/** transmit buffer */
extern char SCIA_TXBuffer[SCIA_TXBUFFERSIZE];
/** transmit buffer position */
extern int SCIA_TXBufferPos;
/** actual baudrate */
extern i32 int SCIA_Baudrate;
/** actual parity (EVEN_PARITY..text protocal, NO_PARITY..bluetooth module)*/
extern i16 int SCIA_Parity;

/** checks if transmitter ready */
#define SCIA_TXREADY()      ((SCIA_TXBufferPos==0&&SciaRegs.SCICTL2.bit.TXRDY!=0)? 1:0)
/** acknowledges receive interrupt */
#define SCIA_RECEIVE_ACK()  PieCtrlRegs.PIEACK.bit.ACK9=1; SciaRegs.SCIFFRX.bit.RXFFINTCLR=1
/** acknowledges transmit interrupt */
#define SCIA_TRANSMIT_ACK() PieCtrlRegs.PIEACK.bit.ACK9=1; SciaRegs.SCIFFTX.bit.TXFFINTCLR=1

/** initializes sci interface
  * @param BaudRate baud rate of scia 
  * @param Parity parity (0..none, 1..odd, 2..even) */
extern void SCIA_Init(long int BaudRate,int Parity);
/** gets sci command
  * @return 0 if empty, >0 if command in buffer */
extern int SCIA_Poll(void);
/** empties receive buffer */
extern void SCIA_EmptyRX(void);
/** transmits sci data
  * @param Data characters to transmit
  * @param Len length of data or 0 if string */
extern void SCIA_Transmit(char *Data,int Len);
/** wait till transmission completed */
extern void SCIA_TXWait(void);

#ifndef HEADER_SCIA

#include <stdio.h>
#include <string.h>

char SCIA_RXBuffer[SCIA_RXBUFFERSIZE];
char SCIA_TXBuffer[SCIA_TXBUFFERSIZE];
int SCIA_TXBufferPos;
int i32 SCIA_Baudrate;
i16 int SCIA_Parity;

/** sci baudrate */
static long int SCIA_BaudRate;
/** receive buffer position */
static int SCIA_RXBufferPos;
/** transmit buffer data length */
static int SCIA_TXLen;
/** flag telling you when a answer is stored in sci buffer */
static int SCIA_ReplyFlag;

/** sci receive interrupt service routine */
static interrupt void SCIA_ReceiveISR(void)
{ static char Data;

  if (SciaRegs.SCIRXST.bit.RXERROR!=0)
  { /* if error occured, do software reset of sci */
    SciaRegs.SCICTL1.bit.SWRESET=0;
    SciaRegs.SCICTL1.bit.SWRESET=1;
    SCIA_RXBufferPos=0;
  } else
  { Data=SciaRegs.SCIRXBUF.all;
    if (Data==SCIA_TERMCHAR) /* check parity for text communiciaton */
    { SCIA_RXBuffer[SCIA_RXBufferPos]='\0';
      SCIA_ReplyFlag=1;
    } else
      if (SCIA_RXBufferPos<(SCIA_RXBUFFERSIZE-1)&&(Data>=' '))
      { SCIA_RXBuffer[SCIA_RXBufferPos]=Data;
        SCIA_RXBufferPos++;
      }
  }
  /* acknowledge interrupt */
  SCIA_RECEIVE_ACK();
}

/** sci transmit interrupt service routine */
static interrupt void SCIA_TransmitISR(void)
{ if (SCIA_TXBufferPos>0)
  { if (SCIA_TXLen==0)
    { if (SCIA_TXBuffer[SCIA_TXBufferPos]!='\0')
      { SciaRegs.SCITXBUF.all=SCIA_TXBuffer[SCIA_TXBufferPos];
        SCIA_TXBufferPos++;
      } else
      { SciaRegs.SCITXBUF.all=SCIA_TERMCHAR;
        SCIA_TXBufferPos=0;
      }
    } else
    { SciaRegs.SCITXBUF.all=SCIA_TXBuffer[SCIA_TXBufferPos];
      SCIA_TXBufferPos++;
      if (SCIA_TXBufferPos>=SCIA_TXLen) SCIA_TXBufferPos=0;
    }
  }
  /* acknowledge interrupt */
  SCIA_TRANSMIT_ACK();
}

void SCIA_Init(long int BaudRate,int Parity)
{ unsigned int BRR;

  SCIA_BaudRate=BaudRate;
  SCIA_Parity=Parity;
  /* allow access to protected memory regions */
  EALLOW;

  /* enable sci clock */
  CpuSysRegs.PCLKCR7.bit.SCI_A=1;
  NOP();

  /** initialize sci a */
  SciaRegs.SCICCR.all=0x07|Parity;/* bit 7=0: one stop bit
                                   * bit 6=0/1: even parity?
                                   * bit 5=0/1: parity generation?
                                   * bit 4=0: disable loopback mode
                                   * bit 3=0: idle line mode protocol
                                   * bit 2-0=111: 8 data bits */
  SciaRegs.SCICTL1.all=0x03;      /* bit 7=reserved
                                   * bit 6=0: receive error interrupt disabled
                                   * bit 5=0: do state machine reset
                                   * bit 4=reserved
                                   * bit 3=0: transmitter wake up not used
                                   * bit 2=0: sleep mode disabled
                                   * bit 1=1: enable transmitting
                                   * bit 0=1: enable receive buffers */
  SciaRegs.SCICTL1.bit.SWRESET=1; /* relinquish sci b from reset */
  SciaRegs.SCICTL2.all=0x03;      /* bit 7,6=read only
                                   * bit 5-2=reserved
                                   * bit 1=1: enable receive buffer interrupt
                                   * bit 0=1: enable transmit buffer interrupt */
                                  /* set baud rate */
  BRR=LSCLK_FREQ/((unsigned long int)SCIA_BaudRate*8)-1;
  BRR=LIMIT(BRR,3,0xFFFF);
  SciaRegs.SCIHBAUD.all=BRR>>8;
  SciaRegs.SCILBAUD.all=BRR&0xFF;
  SCIA_Baudrate=(LSCLK_FREQ/8.0)/(BRR+1)+0.5;

  /* fifo reset values */
  SciaRegs.SCIFFRX.all=0x2021;
  SciaRegs.SCIFFTX.all=0xA000;

  /** set interrupts */
  PieVectTable.SCIA_RX_INT=SCIA_ReceiveISR;  /* set sci a receive interrupt vector */
  PieVectTable.SCIA_TX_INT=SCIA_TransmitISR; /* set sci a transmit interrupt vector */
  PieCtrlRegs.PIEIER9.bit.INTx1=1;           /* enable sci receive interrupt */
  PieCtrlRegs.PIEIER9.bit.INTx2=1;           /* enable sci transmit interrupt */
  IER|=M_INT9;                               /* enable global interrupt 8: sci */

  /** set gpio pins */
  GpioCtrlRegs.GPBGMUX1.bit.GPIO42=           /* use GPIOs 42 and 43 for sci */
  GpioCtrlRegs.GPBGMUX1.bit.GPIO43=3;
  GpioCtrlRegs.GPBMUX1.bit.GPIO42=
  GpioCtrlRegs.GPBMUX1.bit.GPIO43=3;

  /* disable access to protected memory regions */
  EDIS;

  SCIA_RXBufferPos=0;
  SCIA_TXBufferPos=0;
  SCIA_ReplyFlag=0;
}

int SCIA_Poll(void)
{ /* check text protocol */
  if (SCIA_ReplyFlag!=0)
  { SCIA_ReplyFlag=0;
    SCIA_RXBufferPos=0;
    return(1);
  }
  return(0);
}

void SCIA_EmptyRX(void)
{ SCIA_RXBufferPos=0;
}

void SCIA_Transmit(char *Data,int Len)
{ /* check for a running transmission */
  if (SCIA_TXREADY()==0) return;
  /* if ready start new transmission */
  if (Data!=SCIA_TXBuffer)
  { if (Len==0) strcpy(SCIA_TXBuffer,Data); 
           else memcpy(SCIA_TXBuffer,Data,Len);
  }
  if (SCIA_TXBuffer[0]!='\0')
  { SCIA_TXBufferPos=1;
    SCIA_TXLen=Len;
    SciaRegs.SCITXBUF.all=SCIA_TXBuffer[0];
  }
}

void SCIA_TXWait(void)
{ volatile int i;

  for(i=1;i>0;i=SCIA_TXBufferPos);
  G_msWait(1);
}

#else
#undef HEADER_SCIA
#endif

#endif

