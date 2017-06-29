/************************************************************************/
/*																		*/
/*	video_demo.c	--	ZYBO Video demonstration 						*/
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

/* ------------------------------------------------------------ */
/*				Include File Definitions						*/
/* ------------------------------------------------------------ */

#include "video_demo.h"
#include "video_capture/video_capture.h"
#include "display_ctrl/display_ctrl.h"
#include "intc/intc.h"
#include <stdio.h>
#include "xuartps.h"
#include "math.h"
#include <ctype.h>
#include <stdlib.h>
#include "xil_types.h"
#include "xil_cache.h"
#include "xil_io.h"
#include "timer_ps/timer_ps.h"
#include "xparameters.h"
#include "xtime_l.h"
#include "sd_capture/sd_capture.h"

#include <string.h>
/*
 * XPAR redefines
 */
#define DYNCLK_BASEADDR XPAR_AXI_DYNCLK_0_BASEADDR
#define VGA_VDMA_ID XPAR_AXIVDMA_0_DEVICE_ID
#define DISP_VTC_ID XPAR_VTC_0_DEVICE_ID
#define VID_VTC_ID XPAR_VTC_1_DEVICE_ID
#define VID_GPIO_ID XPAR_AXI_GPIO_VIDEO_DEVICE_ID
#define VID_VTC_IRPT_ID XPS_FPGA3_INT_ID
#define VID_GPIO_IRPT_ID XPS_FPGA4_INT_ID
#define SCU_TIMER_ID XPAR_SCUTIMER_DEVICE_ID
#define UART_BASEADDR XPAR_PS7_UART_1_BASEADDR

#define ABS(x)          ((x>0)? x : -x)

/* ------------------------------------------------------------ */
/*				Global Variables								*/
/* ------------------------------------------------------------ */

/*
 * Display and Video Driver structs
 */
DisplayCtrl dispCtrl;
XAxiVdma vdma;
VideoCapture videoCapt;
INTC intc;
char fRefresh; //flag used to trigger a refresh of the Menu on video detect
int width;
int height;

/*
 * SD Files
 */
struct Apps applications;
int n_applications;

/*
 * Framebuffers for video data
 */
u16 frameBuf[DISPLAY_NUM_FRAMES][DEMO_PIXELS] __attribute__((aligned(0x100)));
u16 *pFrames[DISPLAY_NUM_FRAMES] __attribute__((aligned(0x100))); //array of pointers to the frame buffers

/*
 * Interrupt vector table
 */
const ivt_t ivt[] = {
	videoGpioIvt(VID_GPIO_IRPT_ID, &videoCapt),
	videoVtcIvt(VID_VTC_IRPT_ID, &(videoCapt.vtc))
};



/* ------------------------------------------------------------ */
/*				Procedure Definitions							*/
/* ------------------------------------------------------------ */

int main(void)
{
	DemoInitialize();

	//Descomentar esto para ver todas las funcionalidades de la aplicaci�n
	DemoRun();

	//Test de prueba las funciones HW y Sw
	//TestApp();

	return 0;
}

void SDApplicationsInitialize(){
	int i,j=0;
	int *ptr;
	ptr = (int*)&applications;

	applications.grayscale=0;
	applications.edge=0;
	applications.sepia=0;
	applications.laplacian=0;
	applications.emphasize=0;

	xil_printf("SD_cosas2\r\n");
	n_applications = Sd_capture_Applications(&applications);



}

void DemoInitialize()
{
	int Status;
	XAxiVdma_Config *vdmaConfig;
	int i;



	/*
	 * Initialize an array of pointers to the 3 frame buffers
	 */
	for (i = 0; i < DISPLAY_NUM_FRAMES; i++)
	{
		pFrames[i] = frameBuf[i];
	}

	/*
	 * Initialize a timer used for a simple delay
	 */
	TimerInitialize(SCU_TIMER_ID);

	/*
	 * Initialize VDMA driver
	 */
	vdmaConfig = XAxiVdma_LookupConfig(VGA_VDMA_ID);
	if (!vdmaConfig)
	{
		xil_printf("No video DMA found for ID %d\r\n", VGA_VDMA_ID);
		return;
	}
	Status = XAxiVdma_CfgInitialize(&vdma, vdmaConfig, vdmaConfig->BaseAddress);
	if (Status != XST_SUCCESS)
	{
		xil_printf("VDMA Configuration Initialization failed %d\r\n", Status);
		return;
	}

	/*
	 * Initialize the Display controller and start it
	 */
	Status = DisplayInitialize(&dispCtrl, &vdma, DISP_VTC_ID, DYNCLK_BASEADDR, (u8**) pFrames, DEMO_STRIDE);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Display Ctrl initialization failed during demo initialization%d\r\n", Status);
		return;
	}
	Status = DisplayStart(&dispCtrl);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Couldn't start display during demo initialization%d\r\n", Status);
		return;
	}

	/*
	 * Initialize the Interrupt controller and start it.
	 */
	Status = fnInitInterruptController(&intc);
	if(Status != XST_SUCCESS) {
		xil_printf("Error initializing interrupts");
		return;
	}
	fnEnableInterrupts(&intc, &ivt[0], sizeof(ivt)/sizeof(ivt[0]));

	/*
	 * Initialize the Video Capture device
	 */
	Status = VideoInitialize(&videoCapt, &intc, &vdma, VID_GPIO_ID, VID_VTC_ID, VID_VTC_IRPT_ID, (u8**) pFrames , DEMO_STRIDE, DEMO_START_ON_DET);
	if (Status != XST_SUCCESS)
	{
		xil_printf("Video Ctrl initialization failed during demo initialization%d\r\n", Status);
		return;
	}

	/*
	 * Set the Video Detect callback to trigger the menu to reset, displaying the new detected resolution
	 */
	VideoSetCallback(&videoCapt, DemoISR, &fRefresh);

//	DemoPrintTest(dispCtrl.framePtr[dispCtrl.curFrame], dispCtrl.vMode.width, dispCtrl.vMode.height, dispCtrl.stride, DEMO_PATTERN_1);

	/*
	 * Initialize applications in the SD Card
	 */
	xil_printf("SD_cosas\r\n");
	SDApplicationsInitialize();

	return;
}

void DemoRun()
{
	char userInput = 0;

	/* Flush UART FIFO */
	while (XUartPs_IsReceiveData(UART_BASEADDR))
	{
		XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
	}

	while (userInput != 'q')
	{
		fRefresh = 0;
		DemoPrintMenu();

		/* Wait for data on UART */
		while (!XUartPs_IsReceiveData(UART_BASEADDR) && !fRefresh)
		{}

		/* Store the first character in the UART receive FIFO and echo it */
		if (XUartPs_IsReceiveData(UART_BASEADDR))
		{
			userInput = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
			xil_printf("%c", userInput);
		}
		else  //Refresh triggered by video detect interrupt
		{
			userInput = 'r';
		}

		switch (userInput)
		{
		case '1':
			DemoChangeRes();
			break;
//		case '2':
//			nextFrame = dispCtrl.curFrame + 1;
//			if (nextFrame >= DISPLAY_NUM_FRAMES)
//			{
//				nextFrame = 0;
//			}
//			DisplayChangeFrame(&dispCtrl, nextFrame);
//			break;
		case '2':
			if (videoCapt.state == VIDEO_STREAMING)
				VideoStop(&videoCapt);
			else
				VideoStart(&videoCapt);
			break;
//		case '6':
//			nextFrame = videoCapt.curFrame + 1;
//			if (nextFrame >= DISPLAY_NUM_FRAMES)
//			{
//				nextFrame = 0;
//			}
//			VideoChangeFrame(&videoCapt, nextFrame);
		case '3':
			if (videoCapt.state == VIDEO_DISCONNECTED)
			{
				xil_printf("\n\rERROR: HDMI Disconnected!");
				TimerDelay(500000);
			}
			else
				DemoStream();
			break;

//		case '7':
//			DemoPrintTest(pFrames[dispCtrl.curFrame], dispCtrl.vMode.width, dispCtrl.vMode.height, DEMO_WIDTH, DEMO_PATTERN_1);
//			break;
//		case '8':
//			nextFrame = videoCapt.curFrame + 1;
//			if (nextFrame >= DISPLAY_NUM_FRAMES)
//			{
//				nextFrame = 0;
//			}
//			VideoStop(&videoCapt);
//			DemoScaleFrame(pFrames[videoCapt.curFrame], pFrames[nextFrame], videoCapt.timing.HActiveVideo, videoCapt.timing.VActiveVideo, dispCtrl.vMode.width, dispCtrl.vMode.height, DEMO_STRIDE);
//			VideoStart(&videoCapt);
//			DisplayChangeFrame(&dispCtrl, nextFrame);
//			break;
		case 'q':
			break;
		case 'r':
			break;
		default :
			xil_printf("\n\rInvalid Selection");
			TimerDelay(500000);
		}
	}

	return;
}

void DemoPrintMenu()
{
	xil_printf("\x1B[H"); //Set cursor to top left of terminal
	xil_printf("\x1B[2J"); //Clear terminal
	xil_printf("**************************************************\n\r");
	xil_printf("*                ZYBO SDSoC Demo                 *\n\r");
	xil_printf("**************************************************\n\r");
	xil_printf("*Display Resolution: %28s*\n\r", dispCtrl.vMode.label);
	//xil_printf("*Display Frame Index: %27d*\n\r", dispCtrl.curFrame);
	if (videoCapt.state == VIDEO_DISCONNECTED) xil_printf("*Video Capture Resolution: %22s*\n\r", "<HDMI UNPLUGGED>");
	else xil_printf("*Video Capture Resolution: %17dx%-4d*\n\r", videoCapt.timing.HActiveVideo, videoCapt.timing.VActiveVideo);
	//xil_printf("*Video Frame Index: %29d*\n\r", videoCapt.curFrame);
	xil_printf("**************************************************\n\r");
	xil_printf("\n\r");
	xil_printf("1 - Change Display Resolution\n\r");
	xil_printf("2 - Start/Stop Video stream into Video Framebuffer\n\r");
	xil_printf("3 - Run video demo\n\r");
	xil_printf("q - Quit\n\r");
	xil_printf("\n\r");
	xil_printf("\n\r");
	xil_printf("Enter a selection:");
}

void DemoChangeRes()
{
	int fResSet = 0;
	int status;
	char userInput = 0;

	/* Flush UART FIFO */
	while (XUartPs_IsReceiveData(UART_BASEADDR))
	{
		XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
	}

	while (!fResSet)
	{
		DemoCRMenu();

		/* Wait for data on UART */
		while (!XUartPs_IsReceiveData(UART_BASEADDR))
		{}

		/* Store the first character in the UART recieve FIFO and echo it */
		userInput = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
		xil_printf("%c", userInput);
		status = XST_SUCCESS;
		switch (userInput)
		{
		case '1':
			status = DisplayStop(&dispCtrl);
			DisplaySetMode(&dispCtrl, &VMODE_640x480);
			DisplayStart(&dispCtrl);
			fResSet = 1;
			width=640;
			height=480;
			break;
		case '2':
			status = DisplayStop(&dispCtrl);
			DisplaySetMode(&dispCtrl, &VMODE_800x600);
			DisplayStart(&dispCtrl);
			fResSet = 1;
			width=800;
			height=600;
			break;
		case '3':
			status = DisplayStop(&dispCtrl);
			DisplaySetMode(&dispCtrl, &VMODE_1280x720);
			DisplayStart(&dispCtrl);
			fResSet = 1;
			width=1280;
			height=720;
			break;
		case '4':
			status = DisplayStop(&dispCtrl);
			DisplaySetMode(&dispCtrl, &VMODE_1280x1024);
			DisplayStart(&dispCtrl);
			fResSet = 1;
			width=1280;
			height=1024;
			break;
		case '5':
			status = DisplayStop(&dispCtrl);
			DisplaySetMode(&dispCtrl, &VMODE_1920x1080);
			DisplayStart(&dispCtrl);
			fResSet = 1;
			width=1920;
			height=1080;
			break;
		case 'q':
			fResSet = 1;
			break;
		default :
			xil_printf("\n\rInvalid Selection");
			TimerDelay(500000);
		}
		if (status == XST_DMA_ERROR)
		{
			xil_printf("\n\rWARNING: AXI VDMA Error detected and cleared\n\r");
		}
	}
}

void DemoCRMenu()
{
	xil_printf("\x1B[H"); //Set cursor to top left of terminal
	xil_printf("\x1B[2J"); //Clear terminal
	xil_printf("**************************************************\n\r");
	xil_printf("*                ZYBO Video Demo                 *\n\r");
	xil_printf("**************************************************\n\r");
	xil_printf("*Current Resolution: %28s*\n\r", dispCtrl.vMode.label);
	printf("*Pixel Clock Freq. (MHz): %23.3f*\n\r", dispCtrl.pxlFreq);
	xil_printf("**************************************************\n\r");
	xil_printf("\n\r");
	xil_printf("1 - %s\n\r", VMODE_640x480.label);
	xil_printf("2 - %s\n\r", VMODE_800x600.label);
	xil_printf("3 - %s\n\r", VMODE_1280x720.label);
	xil_printf("4 - %s\n\r", VMODE_1280x1024.label);
	xil_printf("5 - %s\n\r", VMODE_1920x1080.label);
	xil_printf("q - Quit (don't change resolution)\n\r");
	xil_printf("\n\r");
	xil_printf("Select a new resolution:");
}

void DemoStream()
{
	char userInput = '0',lastUserInput='0';
	char  * pluginList;
	int n_plugins=0;
	u32 fHw = 0, fApp = 0, fAux1 =0, fAux2=0;
	XTime tStartSW, tEndSW, tStartHW, tEndHW;

	DisplayChangeFrame(&dispCtrl, 1);
	VideoStart(&videoCapt);
	n_plugins=numberPlugins();
	/*xil_printf("\x1B[H"); //Set cursor to top left of terminal
	xil_printf("\x1B[2J"); //Clear terminal
	xil_printf("***Running Video Processing Demo...\n\r");
	xil_printf("----------------------------------------------------------\n\r");
	xil_printf("SW0 on the ZYBO selects the video processing application:\n\r");
	xil_printf("   Up  -- Gray Scale Conversion\n\r");
	xil_printf("  Down -- Sobel Filter\n\r");
	xil_printf("----------------------------------------------------------\n\r");
	xil_printf("SW3 on the ZYBO selects the video processing function used:\n\r");
	xil_printf("   Up  -- Software function executed by Processor\n\r");
	xil_printf("  Down -- SDSoC Hardware function in the FPGA\n\r");
	xil_printf("----------------------------------------------------------\n\r");
	xil_printf("Press 'q' to stop and return to main menu\n\r");*/

	/*
	 * Nuevo men� de elecci�n de filtros
	 */


	xil_printf("\x1B[H"); //Set cursor to top left of terminal
	xil_printf("\x1B[2J"); //Clear terminal
	xil_printf("***Running Video Processing Demo...\n\r");
	xil_printf("----------------------------------------------------------\n\r");

	if(applications.grayscale==1){
			xil_printf("Filter 1 is grayscale, available\n\r");
		}
		else{
			xil_printf("Filter 1 is grayscale, not available\n\r");
		}

		if(applications.edge==1){
			xil_printf("Filter 2 is edge, available\n\r");
		}
		else{
				xil_printf("Filter 2 is edge, not available\n\r");
		}

		if(applications.sepia==1){
			xil_printf("Filter 3 is sepia, available\n\r");
		}
		else{
				xil_printf("Filter 3 is sepia, not available\n\r");
		}

		if(applications.laplacian==1){
			xil_printf("Filter 4 is laplacian, available\n\r");
		}
		else{
				xil_printf("Filter 4 is laplacian, not available\n\r");
		}


		if(applications.emphasize==1){
			xil_printf("Filter 5 is emphasize, available\n\r");
		}
		else{
				xil_printf("Filter 5 is emphasize, not available\n\r");
		}

		xil_printf("----------Filters----------\n\r");
		pluginList = testPlugin();

		xil_printf("Take Picture press p\n\r");
		xil_printf("----------------------------------------------------------\n\r");
		xil_printf("SW3 on the ZYBO selects the video processing function used:\n\r");
		xil_printf("   Up  -- Software function executed by Processor\n\r");
		xil_printf("  Down -- SDSoC Hardware function in the FPGA\n\r");
		xil_printf("----------------------------------------------------------\n\r");
		xil_printf("Press 'q' to stop and return to main menu\n\r");



	while (userInput != 'q')
	{

		fApp = Xil_In32(XPAR_AXI_GPIO_SW_BASEADDR) & 0x1;
		//fAux1 = Xil_In32(XPAR_AXI_GPIO_SW_BASEADDR) & 0x2; //Para nueva funcionalidad
		fAux2 = Xil_In32(XPAR_AXI_GPIO_SW_BASEADDR) & 0x6; //Para nueva funcionalidad
		fHw = Xil_In32(XPAR_AXI_GPIO_SW_BASEADDR) & 0x8;

		if (fHw)//Parte SW
		{




			if((userInput- '0')>= 0 &&(userInput- '0')<=n_plugins-1 ){
				XTime_GetTime(&tStartSW);
				executeFilter(pFrames[0], pFrames[1],userInput,0);
				XTime_GetTime(&tEndSW);


				printf("SOFTWARE, Output hardware took %llu clock cycles.\n\r", 2*(tEndSW - tStartSW));		//Salida de tiempo
				printf("Output took %.2f us.\n\r",1.0 * (tEndSW - tStartSW) / (COUNTS_PER_SECOND/1000000));
			}
			else
				userInput=lastUserInput;

			//XTime_GetTime(&tStartSW);												//Empezamos a hacer tomas de tiempo
					//fApp = fApp | (((u32) videoCapt.timing.HActiveVideo) << 16);
			//DemoSobelFrameHw2(pFrames[0], pFrames[1], fApp, &applications,lastUserInput);

			//XTime_GetTime(&tEndSW);
			/*xil_printf("SOFTWARE\n");
			xil_printf("Output took software %llu clock cycles.\n", 2*(tEndSW - tStartSW));//Salida toma de tiempo
			printf("Output took %.2f us.\n",1.0 * (tEndSW - tStartSW) / (COUNTS_PER_SECOND/1000000));*/
			//xil_printf("\n\r Pasa SW");

		}
		else//Parte HW,
		{
			fApp = fApp | (((u32) videoCapt.timing.HActiveVideo) << 16);

			//Este if accede al filtro en el plugin

			if((userInput - '0')>= 0 &&(userInput - '0')<=n_plugins-1 ){
				XTime_GetTime(&tStartHW);
				executeFilter(pFrames[0], pFrames[1],userInput,1);
				XTime_GetTime(&tEndHW);


				printf("HARDWARE, Output hardware took %llu clock cycles.\n\r", 2*(tEndHW - tStartHW));		//Salida de tiempo
				printf("Output took %.2f us.\n\r",1.0 * (tEndHW - tStartHW) / (COUNTS_PER_SECOND/1000000));
				//HardwareSobelprueba(pFrames[0], pFrames[1]);
				//executeFilter(pFrames[0], pFrames[1],userInput,1);
			}
			else
				userInput=lastUserInput;


																	//Empezamos a hacer tomas de tiempo
			//Esta funci�n tiene todos los filtros integrados
			//DemoSobelFrameHw(pFrames[0], pFrames[1], fApp, &applications,userInput);

			/*xil_printf("HARDWARE\n");
			printf("Output hardware took %llu clock cycles.\n", 2*(tEndHW - tStartHW));		//Salida de tiempo
			printf("Output took %.2f us.\n",1.0 * (tEndHW - tStartHW) / (COUNTS_PER_SECOND/1000000));*/


		}


		if(userInput =='p'){
			TakePicturePress(pFrames[1],width,height);
			xil_printf("Picture Take\n\r");
			userInput='1';
		}

		while (XUartPs_IsReceiveData(UART_BASEADDR) && userInput != 'q')
		{
			userInput = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
		}
		lastUserInput=userInput;
	}

	/* Flush UART FIFO */
	while (XUartPs_IsReceiveData(UART_BASEADDR))
	{
		XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
	}
	DisplayChangeFrame(&dispCtrl, 0);

}



/*
 *
 *
 */

void DemoGrayFrameSw(u16 *srcFrame, u16 *destFrame)
{
	u32 xcoi, ycoi;

	for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++)
	{
		for(xcoi = 0; xcoi < DEMO_WIDTH; xcoi++)
		{
			u16 r, g, b, gray;
			u16 pxlIn, pxlOut;

			pxlIn = srcFrame[xcoi + ycoi * DEMO_WIDTH];
			r = ((pxlIn & 0xF800) >> (11-3));
			b = ((pxlIn & 0x07C0) >> (6-3));
			g = ((pxlIn & 0x003F) << 2);
			gray = (r * 76 + g * 150 + b * 29 + 128) >> 8;
			pxlOut = (((gray + 4) & 0x00F8) << (11-3)) | (((gray + 4) & 0x00F8) << (6-3)) | (((gray + 4) & 0x00F8) >> 2);
			if (xcoi < (videoCapt.timing.HActiveVideo / 2))
				destFrame[xcoi + ycoi * DEMO_WIDTH] = pxlIn;
			else
				destFrame[xcoi + ycoi * DEMO_WIDTH] = pxlOut;
		}
	}
	/*
	 * Flush the framebuffer memory range to ensure changes are written to the
	 * actual memory, and therefore accessible by the VDMA.
	 */
	Xil_DCacheFlushRange((unsigned int) destFrame, DEMO_MAX_FRAME);
}

//void DemoGrayFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS])
//{
//	u32 xcoi, ycoi;
//
//	for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++)
//	{
//		for(xcoi = 0; xcoi < DEMO_WIDTH; xcoi++)
//		{
//#pragma AP PIPELINE II = 1
//			u16 r, g, b, gray;
//			u16 pxlIn, pxlOut;
//
//			pxlIn = srcFrame[xcoi + ycoi * DEMO_WIDTH];
//			r = ((pxlIn & 0xF800) >> (11-3));
//			b = ((pxlIn & 0x07C0) >> (6-3));
//			g = ((pxlIn & 0x003F) << 2);
//			gray = (r * 76 + g * 150 + b * 29 + 128) >> 8;
//			pxlOut = (((gray + 4) & 0x00F8) << (11-3)) | (((gray + 4) & 0x00F8) << (6-3)) | (((gray + 4) & 0x00F8) >> 2);
//			destFrame[xcoi + ycoi * DEMO_WIDTH] = pxlOut;
//		}
//	}
//}

//
//void DemoSobelStream(u8 fHw)
//{
//	char userInput = 0;
//
//	DisplayChangeFrame(&dispCtrl, 1);
//	VideoStart(&videoCapt);
//
//	xil_printf("\x1B[H"); //Set cursor to top left of terminal
//	xil_printf("\x1B[2J"); //Clear terminal
//	if (!fHw)
//		xil_printf("Running Sobel Filter using Processor...\n\r");
//	else
//		xil_printf("Running Sobel Filter conversion using FPGA...\n\r");
//	xil_printf("Press 'q' to stop and return to main menu\n\r");
//
//	while (userInput != 'q')
//	{
//		if (!fHw)
//			DemoSobelFrameSw(pFrames[0], pFrames[1]);
//		else
//			DemoSobelFrameHw(pFrames[0], pFrames[1]);
//
//		while (XUartPs_IsReceiveData(UART_BASEADDR) && userInput != 'q')
//		{
//			userInput = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
//		}
//	}
//
//	/* Flush UART FIFO */
//	while (XUartPs_IsReceiveData(UART_BASEADDR))
//	{
//		XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
//	}
//	DisplayChangeFrame(&dispCtrl, 0);
//
//}

void DemoSobelFrameSw(u16 *srcFrame, u16 *destFrame)
{
	int xcoi, ycoi;

	for(ycoi = 0; ycoi < DEMO_HEIGHT; ycoi++)
	{
		for(xcoi = 0; xcoi < DEMO_WIDTH; xcoi++)
		{
			u16 edge;

			if( ycoi == 0 || xcoi == 0 || ycoi == (DEMO_HEIGHT-1) || xcoi == (DEMO_WIDTH-1))
				edge=0;
			else{
				short x_weight = 0, y_weight = 0;
				int i, j;
				u16 r, g, b;
				u16 pxlIn;
				short gray;
				const short x_op[3][3] = { {-1,0,1},{-2,0,2},{-1,0,1}};
				const short y_op[3][3] = { {1,2,1},	{0,0,0}, {-1,-2,-1}};

				for(i=0; i < 3; i++){
					for(j = 0; j < 3; j++){
						pxlIn = srcFrame[j-1+xcoi+((i-1+ycoi) * DEMO_WIDTH)];
						r = ((pxlIn & 0xF800) >> (11-3));
						b = ((pxlIn & 0x07C0) >> (6-3));
						g = ((pxlIn & 0x003F) << 2);
						gray = (short) ((r * 76 + g * 150 + b * 29 + 128) >> 8);
						x_weight = x_weight + gray * x_op[i][j];
						y_weight = y_weight + gray * y_op[i][j];
					}
				}
				edge = ABS(x_weight) + ABS(y_weight);
				if(edge > 200)	edge = 255;
				else if(edge < 100)		edge = 0;
			}
			if (xcoi < (videoCapt.timing.HActiveVideo / 2))
				destFrame[xcoi + ycoi * DEMO_WIDTH] = srcFrame[xcoi + ycoi * DEMO_WIDTH];
			else
				destFrame[xcoi + ycoi * DEMO_WIDTH] = (((edge) & 0x00F8) << (11-3)) | (((edge) & 0x00F8) << (6-3)) | (((edge) & 0x00F8) >> 2);
		}
	}
	/*
	 * Flush the framebuffer memory range to ensure changes are written to the
	 * actual memory, and therefore accessible by the VDMA.
	 */
	Xil_DCacheFlushRange((unsigned int) destFrame, DEMO_MAX_FRAME);
}


//void DemoScaleStream(u8 fHw)
//{
//	char userInput = 0;
//
//	DisplayChangeFrame(&dispCtrl, 1);
//	VideoStart(&videoCapt);
//
//	xil_printf("\x1B[H"); //Set cursor to top left of terminal
//	xil_printf("\x1B[2J"); //Clear terminal
//	if (!fHw)
//		xil_printf("Scaling the Video Stream using Processor...\n\r");
//	else
//		xil_printf("Scaling the Video Stream using FPGA...\n\r");
//	xil_printf("Press 'q' to stop and return to main menu\n\r");
//
//	while (userInput != 'q')
//	{
//		if (!fHw)
//			DemoScaleFrameSw(pFrames[0], pFrames[1], videoCapt.timing.HActiveVideo, videoCapt.timing.VActiveVideo, dispCtrl.vMode.width, dispCtrl.vMode.height, DEMO_WIDTH);
//		else
//			DemoScaleFrameSw(pFrames[0], pFrames[1], videoCapt.timing.HActiveVideo, videoCapt.timing.VActiveVideo, dispCtrl.vMode.width, dispCtrl.vMode.height, DEMO_WIDTH);
//
//		while (XUartPs_IsReceiveData(UART_BASEADDR) && userInput != 'q')
//		{
//			userInput = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
//		}
//	}
//
//	/* Flush UART FIFO */
//	while (XUartPs_IsReceiveData(UART_BASEADDR))
//	{
//		XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
//	}
//	DisplayChangeFrame(&dispCtrl, 0);
//
//}

/*
 * Bilinear interpolation algorithm. Assumes both frames have the same stride.
 */
//void DemoScaleFrameSw(u16 *srcFrame, u16 *destFrame, u32 srcWidth, u32 srcHeight, u32 destWidth, u32 destHeight, u32 stride)
//{
//	float xInc, yInc; // Width/height of a destination frame pixel in the source frame coordinate system
//	float xcoSrc, ycoSrc; // Location of the destination pixel being operated on in the source frame coordinate system
//	u16 x1y1, x2y1, x1y2, x2y2; //Used to store the color data of the four nearest source pixels to the destination pixel
//	int ix1y1, ix2y1, ix1y2, ix2y2; //indexes into the source frame for the four nearest source pixels to the destination pixel
//	float xDist, yDist; //distances between destination pixel and x1y1 source pixels in source frame coordinate system
//
//	int xcoDest, ycoDest; // Location of the destination pixel being operated on in the destination coordinate system
//	int iy1; //Used to store the index of the first source pixel in the line with y1
//	int iDest; //index of the pixel data in the destination frame being operated on
//
//	u16 pxlOut = 0;
//
//	xInc = ((float) srcWidth - 1.0) / ((float) destWidth);
//	yInc = ((float) srcHeight - 1.0) / ((float) destHeight);
//
//	ycoSrc = 0.0;
//	for (ycoDest = 0; ycoDest < destHeight; ycoDest++)
//	{
//		iy1 = ((int) ycoSrc) * stride;
//		yDist = ycoSrc - ((float) ((int) ycoSrc));
//
//		/*
//		 * Save some cycles in the loop below by presetting the destination
//		 * index to the first pixel in the current line
//		 */
//		iDest = ycoDest * stride;
//
//		xcoSrc = 0.0;
//		for (xcoDest = 0; xcoDest < destWidth; xcoDest++)
//		{
//			ix1y1 = iy1 + ((int) xcoSrc);
//			ix2y1 = ix1y1 + 1;
//			ix1y2 = ix1y1 + stride;
//			ix2y2 = ix1y1 + stride + 1;
//
//			xDist = xcoSrc - ((float) ((int) xcoSrc));
//
//			x1y1 = srcFrame[ix1y1];
//			x2y1 = srcFrame[ix2y1];
//			x1y2 = srcFrame[ix1y2];
//			x2y2 = srcFrame[ix2y2];
//			/*
//			 * Bilinear interpolation function
//			 */
//			pxlOut = 0;
//			pxlOut |= ((u16) ((1.0-yDist)*((1.0-xDist)*((float) (x1y1 & 0x003F))+xDist*((float) (x2y1 & 0x003F))) + yDist*((1.0-xDist)*((float) (x1y2 & 0x003F))+xDist*((float) (x2y2 & 0x003F))))) & 0x003F;
//			pxlOut |= (((u16) ((1.0-yDist)*((1.0-xDist)*((float) ((x1y1 & 0x07C0) >> 6))+xDist*((float) ((x2y1 & 0x07C0) >> 6))) + yDist*((1.0-xDist)*((float) ((x1y2 & 0x07C0) >> 6))+xDist*((float) ((x2y2 & 0x07C0) >> 6))))) & 0x001F) << 6;
//			pxlOut |= (((u16) ((1.0-yDist)*((1.0-xDist)*((float) ((x1y1 & 0xF800) >> 11))+xDist*((float) ((x2y1 & 0xF800) >> 11))) + yDist*((1.0-xDist)*((float) ((x1y2 & 0xF800) >> 11))+xDist*((float) ((x2y2 & 0xF800) >> 11))))) & 0x001F) << 11;
//
//			destFrame[iDest] = pxlOut;
//			iDest++;
//
//			xcoSrc += xInc;
//		}
//		ycoSrc += yInc;
//	}
//
//	/*
//	 * Flush the framebuffer memory range to ensure changes are written to the
//	 * actual memory, and therefore accessible by the VDMA.
//	 */
//	Xil_DCacheFlushRange((unsigned int) destFrame, DEMO_MAX_FRAME);
//
//	return;
//}

///*
// * Bilinear interpolation algorithm. Assumes both frames have the same stride.
// */
//void DemoScaleFrameHw(u8 *srcFrame, u8 *destFrame, u32 srcWidth, u32 srcHeight, u32 destWidth, u32 destHeight, u32 stride)
//{
//	float xInc, yInc; // Width/height of a destination frame pixel in the source frame coordinate system
//	float xcoSrc, ycoSrc; // Location of the destination pixel being operated on in the source frame coordinate system
//	float x1y1, x2y1, x1y2, x2y2; //Used to store the color data of the four nearest source pixels to the destination pixel
//	int ix1y1, ix2y1, ix1y2, ix2y2; //indexes into the source frame for the four nearest source pixels to the destination pixel
//	float xDist, yDist; //distances between destination pixel and x1y1 source pixels in source frame coordinate system
//
//	int xcoDest, ycoDest; // Location of the destination pixel being operated on in the destination coordinate system
//	int iy1; //Used to store the index of the first source pixel in the line with y1
//	int iDest; //index of the pixel data in the destination frame being operated on
//
//	int i;
//
//	xInc = ((float) srcWidth - 1.0) / ((float) destWidth);
//	yInc = ((float) srcHeight - 1.0) / ((float) destHeight);
//
//	ycoSrc = 0.0;
//	for (ycoDest = 0; ycoDest < destHeight; ycoDest++)
//	{
//		iy1 = ((int) ycoSrc) * stride;
//		yDist = ycoSrc - ((float) ((int) ycoSrc));
//
//		/*
//		 * Save some cycles in the loop below by presetting the destination
//		 * index to the first pixel in the current line
//		 */
//		iDest = ycoDest * stride;
//
//		xcoSrc = 0.0;
//		for (xcoDest = 0; xcoDest < destWidth; xcoDest++)
//		{
//			ix1y1 = iy1 + ((int) xcoSrc) * 3;
//			ix2y1 = ix1y1 + 3;
//			ix1y2 = ix1y1 + stride;
//			ix2y2 = ix1y1 + stride + 3;
//
//			xDist = xcoSrc - ((float) ((int) xcoSrc));
//
//			/*
//			 * For loop handles all three colors
//			 */
//			for (i = 0; i < 3; i++)
//			{
//				x1y1 = (float) srcFrame[ix1y1 + i];
//				x2y1 = (float) srcFrame[ix2y1 + i];
//				x1y2 = (float) srcFrame[ix1y2 + i];
//				x2y2 = (float) srcFrame[ix2y2 + i];
//
//				/*
//				 * Bilinear interpolation function
//				 */
//				destFrame[iDest] = (u8) ((1.0-yDist)*((1.0-xDist)*x1y1+xDist*x2y1) + yDist*((1.0-xDist)*x1y2+xDist*x2y2));
//				iDest++;
//			}
//			xcoSrc += xInc;
//		}
//		ycoSrc += yInc;
//	}
//
//	/*
//	 * Flush the framebuffer memory range to ensure changes are written to the
//	 * actual memory, and therefore accessible by the VDMA.
//	 */
//	Xil_DCacheFlushRange((unsigned int) destFrame, DEMO_MAX_FRAME);
//
//	return;
//}

/*
 * Funcion para testear la plataforma sin tener que pasar por los menus
 * En esta funci�n hacemos las configuraciones de video para que se pueda lanzar todo correctamente
 */


void TestApp(){

	int fResSet = 0;
	int status;
	char userInput = 0;
	u32 fApp =0;
	int counter =0;

	//Iniciamos las capturas de imagenes y la resoluci�n, por defecto esta 1920x1080
	if (videoCapt.state == VIDEO_DISCONNECTED) xil_printf("*Video Capture Resolution: %22s*\n\r", "<HDMI UNPLUGGED>");
	else xil_printf("*Video Capture Resolution: %17dx%-4d*\n\r", videoCapt.timing.HActiveVideo, videoCapt.timing.VActiveVideo);

	//Cambiamos la resoluci�n de la pantalla
	status = DisplayStop(&dispCtrl);
	DisplaySetMode(&dispCtrl, &VMODE_1920x1080);
	DisplayStart(&dispCtrl);
	fResSet = 1;

	//llamamos a la operacion que lanza la ejecuci�n
	TestApp2();

}

/*
 * Funcion que lanza la ejecucion tanto SW como HW
 *
 */

void TestApp2(){
	u32 fApp =0, fAux1 =0;
	int counter =0;


	DisplayChangeFrame(&dispCtrl, 1);
	VideoStart(&videoCapt);

	fApp = Xil_In32(XPAR_AXI_GPIO_SW_BASEADDR) & 0x1;
	fAux1 = Xil_In32(XPAR_AXI_GPIO_SW_BASEADDR) & 0x2;

	while(1){
		if(counter <= 3 && fApp){
			xil_printf("SOFTWARE\n");
			DemoSobelFrameHw2(pFrames[0], pFrames[1], fApp,  &applications, '1');
		}
		if(counter > 3 && fApp){
			xil_printf("HARDWARE\n");
			fApp = fApp | (((u32) videoCapt.timing.HActiveVideo) << 16);
			DemoSobelFrameHw(pFrames[0], pFrames[1], fApp,  &applications, '1');
		}
		if(counter > 6)
			counter =0;

		counter++;
	}


}

void DemoISR(void *callBackRef, void *pVideo)
{
	char *data = (char *) callBackRef;
	*data = 1; //set fRefresh to 1
}

