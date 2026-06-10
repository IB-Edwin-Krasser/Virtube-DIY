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

   BEGIN            : origin = 0x080000, length = 0x000002
   RAMM0            : origin = 0x000122, length = 0x0002DE
   RAMM1            : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */
   RAMD             : origin = 0x00B000, length = 0x001000

   RAMLS            : origin = 0x008000, length = 0x003000
   RAMGS            : origin = 0x00C000, length = 0x010000

   RESET            : origin = 0x3FFFC0, length = 0x000002

   BOOT_RSVD        : origin = 0x000002, length = 0x000120     /* Part of M0, BOOT rom will use this for stack */
   
   /* Flash sectors */
   FLASHA           : origin = 0x080002, length = 0x001FFE  /* on-chip Flash */
   FLASHB           : origin = 0x082000, length = 0x002000  /* on-chip Flash */
   FLASHC           : origin = 0x084000, length = 0x002000  /* on-chip Flash */
   FLASHDEF         : origin = 0x086000, length = 0x012000
   /* FLASHD           : origin = 0x086000, length = 0x002000 */ /* on-chip Flash */
   /* FLASHE           : origin = 0x088000, length = 0x008000 */ /* on-chip Flash */
   /* FLASHF           : origin = 0x090000, length = 0x008000 */ /* on-chip Flash */
   FLASHG           : origin = 0x098000, length = 0x008000  /* on-chip Flash */
   FLASHH           : origin = 0x0A0000, length = 0x008000  /* on-chip Flash */
   FLASHI           : origin = 0x0A8000, length = 0x008000  /* on-chip Flash */
   FLASHJ           : origin = 0x0B0000, length = 0x008000  /* on-chip Flash */
   FLASHK           : origin = 0x0B8000, length = 0x002000  /* on-chip Flash */
   FLASHL           : origin = 0x0BA000, length = 0x002000  /* on-chip Flash */
   FLASHM           : origin = 0x0BC000, length = 0x002000  /* on-chip Flash */
   FLASHN           : origin = 0x0BE000, length = 0x002000  /* on-chip Flash */
}

SECTIONS
{
   /* Allocate program areas: */
   .cinit              : > FLASHB      PAGE = 0, ALIGN(16)
   .pinit              : > FLASHB,     PAGE = 0, ALIGN(16)
  .text               : >> FLASHB | FLASHC      PAGE = 0, ALIGN(16)
   codestart           : > BEGIN       PAGE = 0, ALIGN(16)
   Flash               : > FLASHB,     PAGE = 0, ALIGN(16)

   /* put the c startup functions into flash? never worked well ...
   .TI.flashfunc : {}     LOAD = FLASHB | FLASHC, PAGE = 0, ALIGN(16)
   {
     rts2800_fpu32.lib<args_main*> (.text)
     rts2800_fpu32.lib<boot28*> (.text)
     rts2800_fpu32.lib<exit*> (.text)
     rts2800_fpu32.lib<_lock*> (.text)
     rts2800_fpu32.lib<pre_init*> (.text)
     rts2800_fpu32.lib<startup*> (.text)

     * (Flash)
   } */

    .TI.ramfunc : {}     LOAD = FLASHDEF
                         RUN = RAMGS,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
                         PAGE = 0, ALIGN(16)
   { "../lib/C28_DSP_FPU_Lib/lib/c28x_fpu_dsp_library.lib"
/*     "../lib/C28_DSP_FPU_Lib/lib/c28x_fpu_dsp_library_coff.lib" */
/*     "../lib/C28_FPU_FastRTS/lib/rts2800_fpu32_fast_supplement.lib" (.text) */
     "../lib/C28_FPU_FastRTS/lib/rts2800_fpu32_fast_supplement_coff.lib"
/*     "../lib/C28_FPU_FastRTS/lib/rts2800_fpu32_fast_supplement.lib" */

     adcdac.obj (.text)
     amps.obj (.text)
     equalizer.obj (.text)
     filtdim.obj (.text)
     filter.obj (.text)
     global.obj (.text)
     i2c.obj (.text)
     main.obj (.text)
     misc.obj (.text)
     noisegate.obj (.text)
     nosprintf.obj (.text)
     powerdac.obj (.text)
     reverb.obj (.text)
     scia.obj (.text)
     tubescr.obj (.text)
   }

   /* Allocate uninitalized data sections: */
   .stack              : > RAMM1       PAGE = 0
   .ebss               : > RAMGS       PAGE = 0
   .ebss_gs            : > RAMGS,     PAGE = 0, ALIGN(4)
   .esysmem            : > RAMLS       PAGE = 0

   /* Initalized sections go in Flash */
   .econst             : >> FLASHG | FLASHH      PAGE = 0, ALIGN(16)
   /*.switch             : > FLASHB      PAGE = 0, ALIGN(16) */
   IDAddr              : > FLASHB,     PAGE = 0, ALIGN(16)
   
   .reset              : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */

   ramgs            : > RAMGS,  PAGE = 0

   /* The following section definition are for SDFM examples */
   Filter1_RegsFile : > RAMGS,  PAGE = 0, align=0x100
   Filter2_RegsFile : > RAMGS,  PAGE = 0, align=0x100
   Filter3_RegsFile : > RAMGS,  PAGE = 0, align=0x100
   Filter4_RegsFile : > RAMGS,  PAGE = 0, align=0x100
   Filter5_RegsFile : > RAMGS,  PAGE = 0, align=0x100
   Filter6_RegsFile : > RAMGS,  PAGE = 0, align=0x100
   Filter7_RegsFile : > RAMGS,  PAGE = 0, align=0x100
   Filter8_RegsFile : > RAMGS,  PAGE = 0, align=0x100
   Filter9_RegsFile : > RAMGS,  PAGE = 0, align=0x100
   Filter0_RegsFile : > RAMGS,  PAGE = 0, align=0x100
 }

/*
//===========================================================================
// End of file.
//===========================================================================
*/
