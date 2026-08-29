/* TCA9548A.c for TCA9548A Switch */

/*
BSD-3-Clause

Copyright (c) 2026 Bruce Stephens
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.

IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* TCA9548A.c for TCA9548 Switch
 * March 2025.
 * Amended April 2025.
 * Fixed August 2026.
 *
 * This is a very simple piece of software that configures a remarkably complex i2c switch.
 * Provides a simple command line operation to set a TCA9548 port either ON or OFF.
 * Takes a HEX value as an input. 0x00... 0x01... ... 0xFF for all on.
 *
 */

/* Defines needed for the i2cDevicesHeader.h */
#define TCA9548A
#define FNSIZE 4

#if __has_include("i2cIncludes.h")
#include "i2cIncludes.h"
#endif

   struct timespec ts; /* Used in i2cConvenientBits.h */

int main (int argc, char ** argv) {
   FILE * _ptr;
   char _str[FNSIZE] = {'\0'}; /* Fixed array rather than malloc. */
   char * fname = "/tmp/tca9548a.txt"; /* Small, known, fixed and removeable. */
   int  devResult = 0;
   int  debug     = 1; /* By definition, debug is ON. */
   int  fp        = 0;
   int  i         = 0;
   int  changed   = 0;
   __u8 reg       = 0x70; /* Hardwired but set below just in case */
   __u8 value     = 0x00;

/* Basic check the basic Command Line arguments... */
   if (argc > 3) {
      printf("\nExpected command is %s [nodebug] 0x?? <ret> where 0x?? is in HEX 0x00..0xFF\n", argv[0]);
      printf("TCA9548A control register is unchanged.\n\n");
      devResult = __i2cScanAdapter(1, 0);
      exit(0);
   }

/* debug is on by default. */
   for (i = 1; i < argc; i++) if (strcmp(toLower(argv[i]),"nodebug") == 0) debug = 0;

/* Recover the previously stored value from a temporary file.
 * If the device has been scanned or the application has been restarted,
 * the control register responds with the register address 0x70.
 * The recovered value is not used, display reference only.
 * The recovered str is checked in inHex... see i2cConvenientBits.h in /headers/.
 */
   if (debug) {
      _ptr = fopen(fname, "r"); /* r or w for write */
      if (_ptr != NULL) {
         fgets(_str, FNSIZE, _ptr);
         fclose(_ptr);
         _str[2] = '\0'; _str[3] = '\0'; /* Just in case. */
         if (isHex(_str)) printf("\nTDA9548A control register: current setting is: 0x%s\n", _str);
      }
      else printf("\nTDA9548A control register: current setting is unknown\n");
   }

/* Scan. */
   if (argc == 1) {
      printf("\n");
      devResult = __i2cScanAdapter(1, 0);
      exit(0);
   }

/* First of all, find the device at the default address... */
   fp = i2cPrepareDevice(g_Adapter, g_SlaveAddress);
   if (fp <= 0) {
      printf("\nTCA9548A NOT found on /dev/i2c-%d, i2cAddress 0x%02X.\n\n", g_Adapter, g_SlaveAddress);
      exit(0);
   }

/* Device found... */
/* if (debug) printf("\nTCA9548A found on /dev/i2c-%d, i2cAddress 0x%02X.\n", g_Adapter, g_SlaveAddress); */

/* Recover hex value to set the Control Register. */
   for (i = 1; i < argc; i++) {
      if ( (argv[i][0] == '0') && ((argv[i][1] == 'x') || (argv[i][1] == 'X')) ) {
         if ( (strtol(argv[i], NULL, 0) >= 0) && (strtol(argv[i], NULL, 0) < 256) ) {
            value = (__u8)( (int)(strtol(argv[i], NULL, 0)) ); 
            changed = 1;
         }
      }
   }

/* Set reg just in case a different g_SlaveAddress has been defined. */
   reg = g_SlaveAddress;

/* Proceed to update the configuration if necessary. */
   if (changed) {
      devResult = i2c_smbus_write_byte_data(fp, reg, value);
      if (devResult < 0) {
         printf("\nTCA9548A Write fail, status: %d.\n", devResult);
         close(fp);
         exit(0);
      }
      devResult = i2c_smbus_read_byte(fp);
      if (debug) printf("\nTCA9548A control register: new setting is: 0b%08b [0x%02X]\n\n", devResult, devResult);

      _ptr = fopen(fname, "w"); /* r or w for write */
       if (_ptr != NULL) {
         fprintf(_ptr, "%02X\n", devResult); 
         fclose(_ptr);
      } 
   }

/* Print the active ports results. */
   if (debug) devResult = __i2cScanAdapter(1, 0);

   close(fp);
   exit(0);
}
