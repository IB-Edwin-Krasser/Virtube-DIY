/** @file nosprintf.c
  * functions for string output to avoid sprintf
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

#ifndef NOSPRINTF_C_INCLUDED
#define NOSPRINTF_C_INCLUDED

/** converts long int value to string
  * @param l long int input
  * @param S string to store result
  * @param DP decimal places (for leading zeros)
  * @return string length */
extern int Long2String(long int l,char *S,int DP);

/** converts long int value to hex string
  * @param l long int input
  * @param S string to store result
  * @param DP decimal places (for leading zeros)
  * @return string length */
extern int Long2HexString(long int l,char *S,int DP);

/** converts long int value to string and concatenates it to a string with seperator character
  * @param l long int input
  * @param S string to store result
  * @param Sep seperator character */
extern void CSVLongCat(long int l,char *S,char Sep);

/** converts long int value to hex string and concatenates it to a string with seperator character
  * @param l long int input
  * @param S string to store result
  * @param Sep seperator character */
extern void CSVHexCat(long int l,char *S,char Sep);

#ifndef HEADER_NOSPRINTF

#include <limits.h>
#include <math.h>
#include <string.h>

int Long2String(long int l,char *S,int DP)
{ int i,Len;
  char s[10];

  /* check if input is too negative */
  if (l==LONG_MIN)
  { strcpy(S,"-2147483648");
    return(11);
  }
  /* check if input equals zero */
  if (l==0)
  { if (DP<1) DP=1;
    for(i=0;i<DP;i++) (*(S++))='0';
    Len=DP;
  } else
  { Len=0;
    /* write sign */
    if (l<0) { (*(S++))='-'; l=-l; Len++; }
    /* write value to temporary string c (wrong order) */
    for(i=0;l!=0||DP>0;l/=10,i++,Len++,DP--) s[i]=(l%10)+'0';
    /* change order */
    for(i--;i>=0;i--) (*(S++))=s[i];
  }
  /* terminate string */
  *S='\0';

  return(Len);
}

int Long2HexString(long int l,char *S,int DP)
{ int i,Len;
  char s[10];

  /* convert to hex, wrong order */
  for(i=0;l>0||DP>0;DP--,i++,l>>=4)
    s[i]=((l&0x0F)<=9)? (l&0x0F)+'0':(l&0x0F)-0x0A+'A';
  /* reorder */
  Len=i+2;
  S[0]='0'; S[1]='x'; S+=2;
  while(i>0) { i--; *S=s[i]; S++; }
  /* terminate string */
  *S='\0';

  return(Len);
}

void CSVLongCat(long int l,char *S,char Sep)
{ char s[16];

  s[0]=Sep;
  Long2String(l,s+1,0);
  strcat(S,s);
}

void CSVHexCat(long int l,char *S,char Sep)
{ char s[16];

  s[0]=Sep;
  Long2HexString(l,s+1,2);
  strcat(S,s);
}

#else
#undef HEADER_NOSPRINTF
#endif

#endif
