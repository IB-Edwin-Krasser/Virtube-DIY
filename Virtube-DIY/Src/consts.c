/** @file consts.c
  * constant definitions stored in flash memory used in loader and program and some misc macros (data,time)
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

#ifndef CONSTS_C_INCLUDED
#define CONSTS_C_INCLUDED

/** month letters */
#define _ML   (((char *)__DATE__)[0]+((char *)__DATE__)[1]+((char *)__DATE__)[2])
/** compile year value */
#define _YEAR ((((char *)__DATE__)[9]-'0')*10+(((char *)__DATE__)[10])-'0')
/** compile month value */
#define _MON  ((_ML==('J'+'a'+'n'))? 1:(_ML==('F'+'e'+'b'))? 2: (_ML==('M'+'a'+'r'))? 3:(_ML==('A'+'p'+'r'))? 4: \
               (_ML==('M'+'a'+'y'))? 5:(_ML==('J'+'u'+'n'))? 6: (_ML==('J'+'u'+'l'))? 7:(_ML==('A'+'u'+'g'))? 8: \
               (_ML==('S'+'e'+'p'))? 9:(_ML==('O'+'c'+'t'))? 10:(_ML==('N'+'o'+'v'))? 11:12)
/** compile day value */
#define _DAY  (((((char *)__DATE__)[4]==' ')? 0:(((char *)__DATE__)[4]-'0'))*10L+(((char *)__DATE__)[5])-'0')

/** date string macro */
#define DATE_STRING       ((char *)__DATE__)[9],((char *)__DATE__)[10],'0'+(_MON/10),'0'+(_MON%10),(((char *)__DATE__)[4]==' ')? '0':((char *)__DATE__)[4],((char *)__DATE__)[5]
/** time string macro */
#define TIME_STRING       ((char *)__TIME__)[0],((char *)__TIME__)[1],((char *)__TIME__)[3],((char *)__TIME__)[4],((char *)__TIME__)[6],((char *)__TIME__)[7]
/** date in decimal format */
#define DATE_DEC          (10000L*_YEAR+100L*_MON+_DAY)
/** time in decimal format */
#define TIME_DEC          (100000L*(__TIME__[0]-'0')+10000L*(__TIME__[1]-'0')+1000L*(__TIME__[3]-'0')+100L*(__TIME__[4]-'0')+10L*(__TIME__[6]-'0')+(__TIME__[7]-'0'))

/** compile date in decimal format (__DATE__) */
extern const unsigned long int C_CompileDate;
/** compile time in decimal format (__TIME__) */
extern const unsigned long int C_CompileTime;
/** compile date in string format (__DATE__) */
extern const char C_CompileDateString[7];
/** compile time in string format (__TIME__) */
extern const char C_CompileTimeString[7];

#ifndef HEADER_CONSTS

const unsigned long int C_CompileDate=DATE_DEC;
const unsigned long int C_CompileTime=TIME_DEC;

/* not used ...
 * const char C_CompileDateString[7]={DATE_STRING};
 * const char C_CompileTimeString[7]={TIME_STRING}; */

#else
#undef HEADER_CONSTS
#endif

#endif
