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
#include <iostream>
#include <fstream>

using namespace std;

#define ABS(x)          ((x>0)? x : -x)
#define length(x) (sizeof(x)/sizeof(x[0]))


typedef struct{

	u16 green :6;
	u16 blue :5;
	u16 red :5;

}pixel_t;

void DemoSobelFrameHw(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], u32 fApp,  Apps * apps, char input)
{
	int xcoi, ycoi;
	u32 frameWidth;

	ap_linebuffer<u8, 3, DEMO_WIDTH> buff_A;
	ap_window<u8,3,3> buff_C;
	frameWidth = (fApp >> 16);



	for(ycoi = 0; ycoi < DEMO_HEIGHT+1; ycoi++){
		for(xcoi = 0; xcoi < DEMO_WIDTH+1; xcoi++){
#pragma AP PIPELINE II = 1
			u8 temp, grayIn = 0,sepiaIn=0,grayInOld=0;

			u16 rIn, gIn, bIn,rOut,gOut,bOut;
			u16 pxlIn, pxlInOld = 0,pxlOut;
			u16 edge;


			//Metemos en xcoi al linebuffer y guardamos en temp el valor que hemos metido al final del linebuffer
			if(xcoi < DEMO_WIDTH){
				buff_A.shift_up(xcoi);
				temp = buff_A.getval(0,xcoi);
			}
			if((xcoi < DEMO_WIDTH) & (ycoi < DEMO_HEIGHT)){
				pxlInOld = pxlIn;

				//cogemos el pixel de esta posición
				pxlIn = srcFrame[ycoi*DEMO_WIDTH+xcoi];

				//Descomponemos el punto en sus 3 componentes RGB
				rIn = ((pxlIn & 0xF800) >> (11-3));	//5 bits para el rojo
				bIn = ((pxlIn & 0x07C0) >> (6-3));	//5 bits para el azul
				gIn = ((pxlIn & 0x003F) << 2);		//6 bits para el verde+

				//Calculamos el valor gris a partir del punto original
				grayIn = (rIn * 76+ gIn *150 + bIn*29 +128 ) >> 8;
				grayInOld = grayIn;


				if(grayIn > 255)
					grayIn=255;
				if(grayIn < 0)
					grayIn=0;


				/*
				 * Entramos para tintar en Sepia, si lo especificamos en el fichero de addons
				 */

				if(apps->sepia > 0 && input =='3'){
					//Tintamos en sepia
					rOut = (grayIn+grayIn *112) >> 8;
					gOut = (grayIn+grayIn *66) >> 8;
					bOut = (grayIn+grayIn *20) >> 8;

					//saturar todo
					if(rOut> 255)
						rOut = 255;
					if(rOut < 0)
						rOut = 0;
					if(gOut > 255)
						gOut = 255;
					if(gOut < 0)
						gOut = 0;
					if(bOut > 255)
						bOut = 255;
					if(bOut < 0)
						bOut = 0;

					/*
					 * Salimos de sepia
					 */

				}



				buff_A.insert_bottom(grayIn,xcoi); 						//Insertamos el valor al final del linebuffer de la fila que toque

			}

			//movemos todo el contenido del buffer C de la ventana a la derecha ( esto lo hace en escala de grises, es para que la imagen se quede detro de los bordesde la pantalla)
			buff_C.shift_right();
			if(xcoi < DEMO_WIDTH){
				buff_C.insert(buff_A.getval(2,xcoi),0,2); //insertamos los valores que hemos tomado en el buffer A
				buff_C.insert(temp,1,2);
				buff_C.insert(grayIn,2,2);
			}


			/*
			 * Procesamiento para la detección de bordes
			 */
			if((apps->laplacian > 0 && input =='4') || apps->edge > 0 && input =='2'){ // Si esto está activado, lo calculamos.
				if( ycoi <= 1 || xcoi <= 0 || ycoi == DEMO_HEIGHT || xcoi == DEMO_WIDTH) //Si no se dan las condiciones
					edge=0;
				else{
					short x_weight = 0, y_weight = 0;
					u8 i, j;

					//Kernel para deteccion de bordes con sobel edge
					const short x_op[3][3] = { {-1,0,1},{-2,0,2},{-1,0,1}};
					const short y_op[3][3] = { {1,2,1},	{0,0,0}, {-1,-2,-1}};

					//Kernel para convolucion sobel Laplacian
					const short x_opC[3][3] = { {0,-1,0},{-1,4,-1},{0,-1,0}};
					const short y_opC[3][3] = { {1,1,1},{-1,8,-1}, {-1,-1,-1}};


					for(i=0; i < 3; i++){
						for(j = 0; j < 3; j++){
							if(input == '2'){
								x_weight = x_weight + (buff_C.getval(i,j) * x_op[i][j]);
								y_weight = y_weight + (buff_C.getval(i,j) * y_op[i][j]);
							}
							else{
								x_weight = x_weight + (buff_C.getval(i,j) * x_opC[i][j]);
								y_weight = y_weight + (buff_C.getval(i,j) * y_opC[i][j]);
							}
						}
					}
					edge = ABS(x_weight) + ABS(y_weight);

					if(input == '2'){
						if(edge > 200)	edge = 255;
						else if(edge < 100)		edge = 0;
					}
					else{
						if(edge > 250)	edge = 255;
						else if(edge < 10)		edge = 0;
					}
				}
			}//End IF optimizado

			/*
			 * Fin deteccion de bordes
			 */




			if(ycoi > 0 && xcoi > 0){
				/*
				 *Salida en funcion de los SW de la placa
				 */

				/*if ((fAux1) && (apps->sepia==1)){//sepia
					pxlOut = (((rOut + 4) & 0x00F8) << (11-3)) | (((bOut + 4) & 0x00F8) << (6-3)) | (((gOut + 4) & 0x00F8) >> 2); //el valor +4 y el  0x00F8 es para normalizar
					destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlOut;
				}
				if (fApp & 0x01)//grises
				{
					//if (xcoi < (frameWidth / 2)){

						//Recomponemos el pixel
						//pxlOut = (((rOut + 4) & 0x00F8) << (11-3)) | (((bOut + 4) & 0x00F8) << (6-3)) | (((gOut + 4) & 0x00F8) >> 2); //el valor +4 y el  0x00F8 es para normalizar
						//destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlOut;

						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((grayInOld) & 0x00F8) << (11-3)) | (((grayInOld) & 0x00F8) << (6-3)) | (((grayInOld) & 0x00F8) >> 2);

					/*}
					else
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((buff_C.getval(1,1)) & 0x00F8) << (11-3)) | (((buff_C.getval(1,1)) & 0x00F8) << (6-3)) | (((buff_C.getval(1,1)) & 0x00F8) >> 2);
				}*/
				/*
				 * Procesamiento para la detección de bordes
				 */
				/*else
				{
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((edge) & 0x00F8) << (11-3)) | (((edge) & 0x00F8) << (6-3)) | (((edge) & 0x00F8) >> 2);
				}

				/*
				 * Fin de las salidas
				 *
				 */

				/*
				 * Entradas en funcion de inputs UART
				 */

				if(input != '0'){
					if(input =='1' && apps->grayscale==1){//grises
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((grayInOld) & 0x00F8) << (11-3)) | (((grayInOld) & 0x00F8) << (6-3)) | (((grayInOld) & 0x00F8) >> 2);
					}
					if(input == '2' && apps->edge==1){//edge
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((edge) & 0x00F8) << (11-3)) | (((edge) & 0x00F8) << (6-3)) | (((edge) & 0x00F8) >> 2);
					}

					if (input == '3' && apps->sepia==1){//sepia
						pxlOut = (((rOut + 4) & 0x00F8) << (11-3)) | (((bOut + 4) & 0x00F8) << (6-3)) | (((gOut + 4) & 0x00F8) >> 2); //el valor +4 y el  0x00F8 es para normalizar
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlOut;
					}
					if (input == '5' && apps->emphasize==1){//emphasize RED color

						if(rIn>100 && gIn<100 && bIn <100)
							destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlIn;
						else{
							destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((grayInOld) & 0x00F8) << (11-3)) | (((grayInOld) & 0x00F8) << (6-3)) | (((grayInOld) & 0x00F8) >> 2);
						}
					}
					if (input == '4' && apps->laplacian==1){//convolucion
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((edge) & 0x00F8) << (11-3)) | (((edge) & 0x00F8) << (6-3)) | (((edge) & 0x00F8) >> 2);
					}

				}
				else{
					destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlIn;

				}

			}///IF END
		}//For2 END
	}//For1 END
}

void DemoSobelFrameHw2(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], u32 fApp,  Apps * apps, char input)
{
	int xcoi, ycoi;
	u32 frameWidth;

	ap_linebuffer<u8, 3, DEMO_WIDTH> buff_A;
	ap_window<u8,3,3> buff_C;
	frameWidth = (fApp >> 16);


	//cout << "\n\r llega los for";
	for(ycoi = 0; ycoi < DEMO_HEIGHT+1; ycoi++){
		for(xcoi = 0; xcoi < DEMO_WIDTH+1; xcoi++){
#pragma AP PIPELINE II = 1
			u8 temp, grayIn = 0,sepiaIn=0,grayInOld=0;
			pixel_t *pixel;
			u16 rIn, gIn, bIn,rOut,gOut,bOut;
			u16 pxlIn, pxlInOld = 0,pxlOut;
			u16 edge;


			//Metemos en xcoi al linebuffer y guardamos en temp el valor que hemos metido al final del linebuffer
			if(xcoi < DEMO_WIDTH){
				buff_A.shift_up2(xcoi);
				temp = buff_A.getval2(0,xcoi);
			}
			if((xcoi < DEMO_WIDTH) & (ycoi < DEMO_HEIGHT)){
				pxlInOld = pxlIn;

				//cogemos el pixel de esta posición
				pxlIn = srcFrame[ycoi*DEMO_WIDTH+xcoi];

				//Descomponemos el punto en sus 3 componentes RGB
				rIn = ((pxlIn & 0xF800) >> (11-3));	//5 bits para el rojo
				bIn = ((pxlIn & 0x07C0) >> (6-3));	//5 bits para el azul
				gIn = ((pxlIn & 0x003F) << 2);		//6 bits para el verde+

				//Calculamos el valor gris a partir del punto original
				grayIn = (rIn * 76+ gIn *150 + bIn*29 +128 ) >> 8;
				grayInOld = grayIn;


				if(grayIn > 255)
					grayIn=255;
				if(grayIn < 0)
					grayIn=0;


				/*
				 * Entramos para tintar en Sepia, si lo especificamos en el fichero de addons
				 */

				if(apps->sepia > 0 && input =='3'){
					//Tintamos en sepia
					rOut = (grayIn+grayIn *112) >> 8;
					gOut = (grayIn+grayIn *66) >> 8;
					bOut = (grayIn+grayIn *20) >> 8;

					//saturar todo
					if(rOut> 255)
						rOut = 255;
					if(rOut < 0)
						rOut = 0;
					if(gOut > 255)
						gOut = 255;
					if(gOut < 0)
						gOut = 0;
					if(bOut > 255)
						bOut = 255;
					if(bOut < 0)
						bOut = 0;

					/*
					 * Salimos de sepia
					 */
					//cout << "\n\r salimos de sepia";
				}



				buff_A.insert_bottom2(grayIn,xcoi); 						//Insertamos el valor al final del linebuffer de la fila que toque
				//cout << "\n\r llega a meter en el buffer de la matriz 1 a la derecha";
			}

			//movemos todo el contenido del buffer C de la ventana a la derecha ( esto lo hace en escala de grises, es para que la imagen se quede detro de los bordesde la pantalla)
			buff_C.shift_right2();
			if(xcoi < DEMO_WIDTH){
				buff_C.insert2(buff_A.getval2(2,xcoi),0,2); //insertamos los valores que hemos tomado en el buffer A
				buff_C.insert2(temp,1,2);
				buff_C.insert2(grayIn,2,2);
			}
			//cout << "\n\r todo movido";

			/*
			 * Procesamiento para la detección de bordes
			 */
			if((apps->laplacian > 0 && input =='4') || apps->edge > 0 && input =='2'){ // Si esto está activado, lo calculamos.
				if( ycoi <= 1 || xcoi <= 0 || ycoi == DEMO_HEIGHT || xcoi == DEMO_WIDTH) //Si no se dan las condiciones
					edge=0;
				else{
					short x_weight = 0, y_weight = 0;
					u8 i, j;

					//Kernel para deteccion de bordes con sobel edge
					const short x_op[3][3] = { {-1,0,1},{-2,0,2},{-1,0,1}};
					const short y_op[3][3] = { {1,2,1},	{0,0,0}, {-1,-2,-1}};

					//Kernel para convolucion sobel Laplacian
					const short x_opC[3][3] = { {0,-1,0},{-1,4,-1},{0,-1,0}};
					const short y_opC[3][3] = { {1,1,1},{-1,8,-1}, {-1,-1,-1}};


					for(i=0; i < 3; i++){
						for(j = 0; j < 3; j++){
							if(input == '2'){
								x_weight = x_weight + (buff_C.getval2(i,j) * x_op[i][j]);
								y_weight = y_weight + (buff_C.getval2(i,j) * y_op[i][j]);
							}
							else{
								x_weight = x_weight + (buff_C.getval2(i,j) * x_opC[i][j]);
								y_weight = y_weight + (buff_C.getval2(i,j) * y_opC[i][j]);
							}
						}
					}
					edge = ABS(x_weight) + ABS(y_weight);

					if(input == '2'){
						if(edge > 200)	edge = 255;
						else if(edge < 100)		edge = 0;
					}
					else{
						if(edge > 250)	edge = 255;
						else if(edge < 10)		edge = 0;
					}
				}
			}//End IF optimizado
			//cout << "\n\r se han calculado los edge";
			/*
			 * Fin deteccion de bordes
			 */




			if(ycoi > 0 && xcoi > 0){
				/*
				 *Salida en funcion de los SW de la placa
				 */

				/*if ((fAux1) && (apps->sepia==1)){//sepia
					pxlOut = (((rOut + 4) & 0x00F8) << (11-3)) | (((bOut + 4) & 0x00F8) << (6-3)) | (((gOut + 4) & 0x00F8) >> 2); //el valor +4 y el  0x00F8 es para normalizar
					destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlOut;
				}
				if (fApp & 0x01)//grises
				{
					//if (xcoi < (frameWidth / 2)){

						//Recomponemos el pixel
						//pxlOut = (((rOut + 4) & 0x00F8) << (11-3)) | (((bOut + 4) & 0x00F8) << (6-3)) | (((gOut + 4) & 0x00F8) >> 2); //el valor +4 y el  0x00F8 es para normalizar
						//destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlOut;

						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((grayInOld) & 0x00F8) << (11-3)) | (((grayInOld) & 0x00F8) << (6-3)) | (((grayInOld) & 0x00F8) >> 2);

					/*}
					else
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((buff_C.getval(1,1)) & 0x00F8) << (11-3)) | (((buff_C.getval(1,1)) & 0x00F8) << (6-3)) | (((buff_C.getval(1,1)) & 0x00F8) >> 2);
				}*/
				/*
				 * Procesamiento para la detección de bordes
				 */
				/*else
				{
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((edge) & 0x00F8) << (11-3)) | (((edge) & 0x00F8) << (6-3)) | (((edge) & 0x00F8) >> 2);
				}

				/*
				 * Fin de las salidas
				 *
				 */

				/*
				 * Entradas en funcion de inputs UART
				 */

				if(input != '0'){
					if(input =='1' && apps->grayscale==1){//grises
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((grayInOld) & 0x00F8) << (11-3)) | (((grayInOld) & 0x00F8) << (6-3)) | (((grayInOld) & 0x00F8) >> 2);
					}
					if(input == '2' && apps->edge==1){//edge
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((edge) & 0x00F8) << (11-3)) | (((edge) & 0x00F8) << (6-3)) | (((edge) & 0x00F8) >> 2);
					}

					if (input == '3' && apps->sepia==1){//sepia
						pxlOut = (((rOut + 4) & 0x00F8) << (11-3)) | (((bOut + 4) & 0x00F8) << (6-3)) | (((gOut + 4) & 0x00F8) >> 2); //el valor +4 y el  0x00F8 es para normalizar
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlOut;
					}
					if (input == '5' && apps->emphasize==1){//emphasize RED color

						if(rIn>100 && gIn<100 && bIn <100)
							destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlIn;
						else{
							destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((grayInOld) & 0x00F8) << (11-3)) | (((grayInOld) & 0x00F8) << (6-3)) | (((grayInOld) & 0x00F8) >> 2);
						}
					}
					if (input == '4' && apps->laplacian==1){//convolucion
						destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((edge) & 0x00F8) << (11-3)) | (((edge) & 0x00F8) << (6-3)) | (((edge) & 0x00F8) >> 2);
					}
					//cout << "\n\r llega input no 0";
				}
				else{
					destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlIn;
					//cout << "\n\r llega input 0";
				}

			}///IF END
		}//For2 END
	}//For1 END
}

void HardwareSobelprueba(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS])
{
	int xcoi, ycoi;
	u32 frameWidth;

	ap_linebuffer<u8, 3, DEMO_WIDTH> buff_A;
	ap_window<u8,3,3> buff_C;



	for(ycoi = 0; ycoi < DEMO_HEIGHT+1; ycoi++){
		for(xcoi = 0; xcoi < DEMO_WIDTH+1; xcoi++){
#pragma AP PIPELINE II = 1
			u8 temp, grayIn = 0,sepiaIn=0,grayInOld=0;

			u16 rIn, gIn, bIn,rOut,gOut,bOut;
			u16 pxlIn, pxlInOld = 0,pxlOut;
			u16 edge;


			//Metemos en xcoi al linebuffer y guardamos en temp el valor que hemos metido al final del linebuffer
			if(xcoi < DEMO_WIDTH){
				buff_A.shift_up(xcoi);
				temp = buff_A.getval(0,xcoi);
			}
			if((xcoi < DEMO_WIDTH) & (ycoi < DEMO_HEIGHT)){
				pxlInOld = pxlIn;

				//cogemos el pixel de esta posición
				pxlIn = srcFrame[ycoi*DEMO_WIDTH+xcoi];

				//Descomponemos el punto en sus 3 componentes RGB
				rIn = ((pxlIn & 0xF800) >> (11-3));	//5 bits para el rojo
				bIn = ((pxlIn & 0x07C0) >> (6-3));	//5 bits para el azul
				gIn = ((pxlIn & 0x003F) << 2);		//6 bits para el verde+

				//Calculamos el valor gris a partir del punto original
				grayIn = (rIn * 76+ gIn *150 + bIn*29 +128 ) >> 8;
				grayInOld = grayIn;


				if(grayIn > 255)
					grayIn=255;
				if(grayIn < 0)
					grayIn=0;



			}

			if(ycoi > 0 && xcoi > 0){

					destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((grayInOld) & 0x00F8) << (11-3)) | (((grayInOld) & 0x00F8) << (6-3)) | (((grayInOld) & 0x00F8) >> 2);

			}///IF END
		}//For2 END
	}//For1 END
}
