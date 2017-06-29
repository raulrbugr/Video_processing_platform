/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

//============================================================
// zybo_hdmi_in platform 
#include "xgpio.h"
#include "xstatus.h"
#include "xil_printf.h"

/* definitions */
#define GPIO_DEVICE_ID  XPAR_AXI_GPIO_LED_DEVICE_ID	/* GPIO device that LEDs are connected to */
#define LED_DELAY 50000000				/* Software delay length */
#define LED_CHANNEL 1					/* GPIO port for LEDs */
#define printf xil_printf				/* smaller, optimised printf */
//============================================================

#ifdef __SDSCC__
#include "sds_lib.h"
#else
#define sds_alloc(x) malloc(x)
#define sds_free(x) free(x)
#endif

#include "arraycopy.h"

void arraycopy(uint32 a[N], uint32 b[N])
{
     int i;
     for (i = 0; i < N; i++)
          b[i] = a[i];
}

void arraycopy_sw(uint32 a[N], uint32 b[N])
{
     int i;
     for (i = 0; i < N; i++)
          b[i] = a[i];
}

int compare_uint32_arrays(uint32 *A, uint32 *B)
{
     int i;
     for (i = 0; i < N; i++) {
          if (A[i] != B[i]) {
               printf("A[%d] = %x, B[%d] = %x do not match\n", i, A[i], i, B[i]);
               return -1;
          }
     }
     return 0;
}

int main()
{
     XGpio Gpio;    /* GPIO Device driver instance */

     int Status = XGpio_Initialize(&Gpio, GPIO_DEVICE_ID);
     if (Status != XST_SUCCESS) {
          return XST_FAILURE;
     }
     /*Set the direction for the LEDs to output. */
     XGpio_SetDataDirection(&Gpio, LED_CHANNEL, 0x00);
        
     printf("Allocating arrays\n");
     uint32 *A, *B;
     A = (uint32 *)sds_alloc(NUMBYTES);
     B = (uint32 *)sds_alloc(NUMBYTES);
        
     if (A == NULL)      printf("Could not allocate memory A - exiting\n");
     else if (B == NULL) printf("Could not allocate memory B - exiting\n");
     else {
          int i, j;
          int success = 1;
          printf("LEDs match iteration index...\n");
          for (j = 0; j < 64; j++) {
               XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, (unsigned char) j);
               for (i = N-1; i >= 0; i-- ) {
                    A[i] = rand() % N ;
                    B[i] = 0;
               }
               printf("A: ");
               for (i = 0; i < N; i++)
                    printf("%5d", A[i]);
               printf("\n");
                  
               arraycopy(A, B);
                  
               if (compare_uint32_arrays(A, B)) {
                    success = 0;
                    break;
               }
               printf("Iteration %2d matched.  Update LEDs and spin CPU to make change visible.\n",  j);
               for (i=0; i < 50000000; i++);
          }
          printf("Freeing array\n");
          sds_free(A);
          printf("%s\n", success ? "Test passed!" : "Better luck next time...");
     } 

     return 0;
}
