/*****************************************************************************
 *
 *     Author: Xilinx, Inc.
 *
 *     This text contains proprietary, confidential information of
 *     Xilinx, Inc. , is distributed by under license from Xilinx,
 *     Inc., and may be used, copied and/or disclosed only pursuant to
 *     the terms of a valid license agreement with Xilinx, Inc.
 *
 *     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
 *     AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
 *     SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
 *     OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
 *     APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
 *     THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
 *     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
 *     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
 *     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
 *     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
 *     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
 *     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE.
 *
 *     Xilinx products are not intended for use in life support appliances,
 *     devices, or systems. Use in such applications is expressly prohibited.
 *
 *     (c) Copyright 2011 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/*
 * Sobel demo code 
 */

#include <stdio.h>
#include "ap_video.h"
#include "video_demo.h"

#define ABS(x)          ((x>0)? x : -x)

void DemoSobelFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], u32 fApp)
{
	int xcoi, ycoi;
	u32 frameWidth;

	ap_linebuffer<u8, 3, DEMO_WIDTH> buff_A;
	ap_window<u8,3,3> buff_C;

	frameWidth = (fApp >> 16);

	for(ycoi = 0; ycoi < DEMO_HEIGHT+1; ycoi++){
		for(xcoi = 0; xcoi < DEMO_WIDTH+1; xcoi++){
#pragma AP PIPELINE II = 1
			u8 temp, grayIn = 0;
			u16 rIn, gIn, bIn;
			u16 pxlIn, pxlInOld = 0;
			u16 edge;

			if(xcoi < DEMO_WIDTH){
				buff_A.shift_up(xcoi);
				temp = buff_A.getval(0,xcoi);
			}
			if((xcoi < DEMO_WIDTH) & (ycoi < DEMO_HEIGHT)){
				pxlInOld = pxlIn;
				pxlIn = srcFrame[ycoi*DEMO_WIDTH+xcoi];
				rIn = ((pxlIn & 0xF800) >> (11-3));
				bIn = ((pxlIn & 0x07C0) >> (6-3));
				gIn = ((pxlIn & 0x003F) << 2);
				grayIn = (rIn * 76 + gIn * 150 + bIn * 29 + 128) >> 8;
				buff_A.insert_bottom(grayIn,xcoi);
			}
			buff_C.shift_right();
			if(xcoi < DEMO_WIDTH){
				buff_C.insert(buff_A.getval(2,xcoi),0,2);
				buff_C.insert(temp,1,2);
				buff_C.insert(grayIn,2,2);
			}

			if( ycoi <= 1 || xcoi <= 0 || ycoi == DEMO_HEIGHT || xcoi == DEMO_WIDTH)
				edge=0;
			else{
				short x_weight = 0, y_weight = 0;
				u8 i, j;
				const short x_op[3][3] = { {-1,0,1},{-2,0,2},{-1,0,1}};
				const short y_op[3][3] = { {1,2,1},	{0,0,0}, {-1,-2,-1}};

				for(i=0; i < 3; i++){
					for(j = 0; j < 3; j++){
						x_weight = x_weight + (buff_C.getval(i,j) * x_op[i][j]);
						y_weight = y_weight + (buff_C.getval(i,j) * y_op[i][j]);
					}
				}
				edge = ABS(x_weight) + ABS(y_weight);
				if(edge > 200)	edge = 255;
				else if(edge < 100)		edge = 0;
			}

			if(ycoi > 0 && xcoi > 0){
				if (fApp & 0x01)
				{
					if (xcoi < (frameWidth / 2))
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlInOld;
					else
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((buff_C.getval(1,1)) & 0x00F8) << (11-3)) | (((buff_C.getval(1,1)) & 0x00F8) << (6-3)) | (((buff_C.getval(1,1)) & 0x00F8) >> 2);
				}
				else
				{
					if (xcoi < (frameWidth / 2))
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlInOld;
					else
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((edge) & 0x00F8) << (11-3)) | (((edge) & 0x00F8) << (6-3)) | (((edge) & 0x00F8) >> 2);
				}
			}
		}
	}
}
