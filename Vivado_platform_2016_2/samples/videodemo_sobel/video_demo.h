/************************************************************************/
/*																		*/
/*	video_demo.h	--	ZYBO Video demonstration 						*/
/*																		*/
/************************************************************************/
/*	Author: Sam Bobrowicz												*/
/*	Copyright 2015, Digilent Inc.										*/
/************************************************************************/
/*  Module Description: 												*/
/*																		*/
/*		This file contains code for running a demonstration of the		*/
/*		Video input and output capabilities on the ZYBO. It is a good	*/
/*		example of how to properly use the display_ctrl and				*/
/*		video_capture drivers.											*/
/*																		*/
/*																		*/
/************************************************************************/
/*  Revision History:													*/
/* 																		*/
/*		11/25/2015(SamB): Created										*/
/*																		*/
/************************************************************************/

#ifndef VIDEO_DEMO_H_
#define VIDEO_DEMO_H_
#ifdef __cplusplus
extern "C" {
#endif
/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "xil_types.h"

/* ------------------------------------------------------------ */
/*					Miscellaneous Declarations					*/
/* ------------------------------------------------------------ */

#define DEMO_PATTERN_0 0
#define DEMO_PATTERN_1 1

#define DEMO_MAX_FRAME (1920*1080*2)
#define DEMO_STRIDE (1920 * 2)
#define DEMO_HEIGHT (1080)
#define DEMO_WIDTH (1920)
#define DEMO_PIXELS (1920 * 1080)

/*
 * Configure the Video capture driver to start streaming on signal
 * detection
 */
#define DEMO_START_ON_DET 1

/* ------------------------------------------------------------ */
/*					Procedure Declarations						*/
/* ------------------------------------------------------------ */

void DemoInitialize();
void DemoRun();
void DemoPrintMenu();
void DemoChangeRes();
void DemoCRMenu();
void DemoStream();

//void DemoInvertStream(u8 fHw);
//void DemoInvertFrameSw(u16 *srcFrame, u16 *destFrame);
//#pragma SDS data mem_attribute(srcFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, destFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
//#pragma SDS data access_pattern(srcFrame:SEQUENTIAL, destFrame:SEQUENTIAL)
//void DemoInvertFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS]);

//void DemoGrayStream(u8 fHw);
void DemoGrayFrameSw(u16 *srcFrame, u16 *destFrame);
//#pragma SDS data mem_attribute(srcFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, destFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
//#pragma SDS data access_pattern(srcFrame:SEQUENTIAL, destFrame:SEQUENTIAL)
//void DemoGrayFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS]);

//void DemoSobelStream(u8 fHw);
void DemoSobelFrameSw(u16 *srcFrame, u16 *destFrame);
#pragma SDS data mem_attribute(srcFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, destFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
#pragma SDS data access_pattern(srcFrame:SEQUENTIAL, destFrame:SEQUENTIAL)
void DemoSobelFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], u32 fApp);

//void DemoScaleStream(u8 fHw);
//void DemoScaleFrameSw(u16 *srcFrame, u16 *destFrame, u32 srcWidth, u32 srcHeight, u32 destWidth, u32 destHeight, u32 stride);

void DemoISR(void *callBackRef, void *pVideo);

//void DemoPrintTest(u16 *frame, u32 width, u32 height, u32 stride, int pattern);
/* ------------------------------------------------------------ */

/************************************************************************/

#ifdef __cplusplus
};
#endif
#endif /* VIDEO_DEMO_H_ */
