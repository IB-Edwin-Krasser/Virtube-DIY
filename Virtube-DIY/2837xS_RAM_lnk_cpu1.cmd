 /* Virtube – Non-Commercial License
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
  
MEMORY
{
PAGE 0 :
   /* BEGIN is used for the "boot to SARAM" bootloader mode   */

   BEGIN           	: origin = 0x000000, length = 0x000002
   RAMM0           	: origin = 0x000122, length = 0x0002DE
   RAMM1            : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */
   RAMD            	: origin = 0x00B000, length = 0x001000

   RAMLS           	: origin = 0x008000, length = 0x003000
   RAMGS            : origin = 0x00C000, length = 0x010000

   RESET           	: origin = 0x3FFFC0, length = 0x000002

   BOOT_RSVD        : origin = 0x000002, length = 0x000120     /* Part of M0, BOOT rom will use this for stack */

   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400
}


SECTIONS
{
   codestart        : > BEGIN,     PAGE = 0
   ramfuncs         : > RAMM0      PAGE = 0
   .text            : >>RAMM0 | RAMD |  RAMLS | RAMGS,   PAGE = 0
   .TI.ramfunc      : >>RAMM0 | RAMD |  RAMLS | RAMGS,   PAGE = 0
   .cinit           : > RAMGS,     PAGE = 0
   .pinit           : > RAMM0,     PAGE = 0
   .switch          : > RAMM0,     PAGE = 0
   .reset           : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */
   IDAddr           : > RAMM0,     PAGE = 0

   .stack           : > RAMM1,     PAGE = 0
   .ebss            : > RAMLS,     PAGE = 0

   .econst          : > RAMGS,     PAGE = 0
   .esysmem         : > RAMLS,     PAGE = 0

   Filter_RegsFile  : > RAMD, 	   PAGE = 0
   Flash            : > RAMM0,     PAGE = 0
   ramgs            : > RAMGS,     PAGE = 0
   
   FPUmathTables    : > RAMGS,      PAGE = 0
   FPUfftTables     : > RAMGS,      PAGE = 0

   /* The following section definitions are required when using the IPC API Drivers */
   GROUP : > CPU1TOCPU2RAM, PAGE = 0
   {
     PUTBUFFE
     PUTWRITEIDX
     GETREADIDX
   }

   GROUP : > CPU2TOCPU1RAM, PAGE = 0
   {
     GETBUFFER :    TYPE = DSECT
     GETWRITEIDX :  TYPE = DSECT
     PUTREADIDX :   TYPE = DSECT
   }

   Filter1_RegsFile : > RAMGS,  PAGE = 0, align=0x100, fill=0x1111
   Filter2_RegsFile : > RAMGS,  PAGE = 0, align=0x100, fill=0x2222
   Filter3_RegsFile : > RAMGS,  PAGE = 0, align=0x100, fill=0x3333
   Filter4_RegsFile : > RAMGS,  PAGE = 0, align=0x100, fill=0x4444
   Filter5_RegsFile : > RAMGS,  PAGE = 0, align=0x100, fill=0x5555
   Filter6_RegsFile : > RAMGS,  PAGE = 0, align=0x100, fill=0x6666
   Filter7_RegsFile : > RAMGS,  PAGE = 0, align=0x100, fill=0x7777
   Filter8_RegsFile : > RAMGS,  PAGE = 0, align=0x100, fill=0x8888
   Filter9_RegsFile : > RAMGS,  PAGE = 0, align=0x100, fill=0x9999
   Filter0_RegsFile : > RAMGS,  PAGE = 0, align=0x100, fill=0x0000
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/
