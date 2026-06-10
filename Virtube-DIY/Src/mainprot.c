/** @file mainprot.c
  * the protocol stuff of the main program
  *
  * This module is simply included to avoid the declaration stuff.
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

/** gets hex value
  * @param s input string
  * @param Val return value if a valid hex number
  * @return 0 if a hex value in Val */
static int GetHex(char *s,i32 int *Val)
{ i32 int v;

  /* check if a valid hex number */
  if (s[0]!='0'||(s[1]&0xDF)!='X') return(1);
  /* parse to value */
  for(v=0,s+=2;s[0]!='\0';s++)
    v=(v<<4)+((s[0]<='9')? s[0]-'0':s[0]-'A'+0x0A);
  *Val=v;
  return(0);
}

/** buffer memory used for eeprom data transfer and string buffering */
static i32 int BufMem[256];
/** buffer memory used as string input buffer */
static char *RXBufMem=(char *)&BufMem[0];
/** buffer memory used as string output buffer */
static char *TXBufMem=(char *)&BufMem[128];

/** command characters decode macro */
#define COM(a,b) ((a)|(((int)(b))<<8))
/** parses command and starts the transmission of the reply
  * @param RXBuf receive buffer, if NULL SCIA_RXBuffer is used
  * @param TXBuf transmit buffer, if NULL reply will be sent over scia */
static void ParseCommand(char *RXBuf,char *TXBuf)
{ int i,n,Command,HexFlag;
  /** maximum number of parameters */
  #define MAX_PAR 20
  i32 int a[MAX_PAR];
  char *c;

  /* copy to rx buffer memory */
  strcpy(RXBufMem,(RXBuf!=NULL)? RXBuf:SCIA_RXBuffer);
  /* copy command */
  TXBufMem[0]=RXBufMem[0];
  if (RXBufMem[1]>' ') { TXBufMem[1]=RXBufMem[1]; TXBufMem[2]='\0'; } else TXBufMem[1]='\0';
  /* parse command and set up answer */
  HexFlag=0;
  /* extract parameters */
  strtok(RXBufMem," ,;/");
  for(n=0;n<MAX_PAR;n++)
  { c=strtok(NULL," ");
    if (c==NULL) break;
    if (GetHex(c,&a[n])!=0) a[n]=atol(c); else HexFlag=1;
   }
  /* command switch */
  Command=COM(RXBufMem[0],RXBufMem[1]);
  /* parse the stuff */
  switch(Command)
  { 
    case COM('r','s'): /* reset system */
      if (n==1&&a[0]==12345) G_SWReset();
      n=-1; break;

    case 'v': /* version */
      a[0]=MISC_HWVersion; a[1]=C_CompileDate; a[2]=C_CompileTime; n=3; break;

    default: /* default means error */
      n=-1; break;
  }
  /* setup output string */
  if (n>=0)
  { /* add parameters */
    for(i=0;i<n;i++)
      if (HexFlag==0)
        CSVLongCat(a[i],TXBufMem,' ');
      else
        CSVHexCat(a[i],TXBufMem,' ');
  } else
    strcpy(TXBufMem,"error");
  /* transmit reply or copy to output array */
  if (TXBuf==NULL) SCIA_Transmit(TXBufMem,0); else strcpy(TXBuf,TXBufMem);
}
