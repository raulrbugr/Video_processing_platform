/*
 * sepia.cpp
 *
 *  Created on: 6 de jun. de 2017
 *      Author: Raul
 */


#include "Sepia.h"

#define ABS(x)          ((x>0)? x : -x)
#define length(x) (sizeof(x)/sizeof(x[0]))

#pragma SDS data mem_attribute(srcFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, destFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
#pragma SDS data access_pattern(srcFrame:SEQUENTIAL, destFrame:SEQUENTIAL)
void HardwareSobelSepia(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS])
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



			}
			if(ycoi > 0 && xcoi > 0){

				pxlOut = (((rOut + 4) & 0x00F8) << (11-3)) | (((bOut + 4) & 0x00F8) << (6-3)) | (((gOut + 4) & 0x00F8) >> 2); //el valor +4 y el  0x00F8 es para normalizar
				destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlOut;
			}


			///IF END
		}//For2 END
	}//For1 END
}

void SoftwareSobelSepia(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS])
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




			if(ycoi > 0 && xcoi > 0){

				pxlOut = (((rOut + 4) & 0x00F8) << (11-3)) | (((bOut + 4) & 0x00F8) << (6-3)) | (((gOut + 4) & 0x00F8) >> 2); //el valor +4 y el  0x00F8 es para normalizar
				destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = pxlOut;
			}


			}///IF END
		}//For2 END
	}//For1 END
}


Sepia::Sepia(){
			name = "sepia";
			desc = "hola";
		}
	//Implementar esto, aqui se corre el filtro
void Sepia::apply (u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], int hwsw) {
		//cout<<"filtro2"<<endl;
		if(hwsw ==1)
			HardwareSobelSepia(srcFrame,destFrame);
		else
			SoftwareSobelSepia(srcFrame,destFrame);
	};


static Sepia f;


