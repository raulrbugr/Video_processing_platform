/*
 * laplacian.cpp
 *
 *  Created on: 10 de jun. de 2017
 *      Author: Raul
 */

#include "laplacian.h"

#define ABS(x)          ((x>0)? x : -x)
#define length(x) (sizeof(x)/sizeof(x[0]))

#pragma SDS data mem_attribute(srcFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE, destFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
#pragma SDS data access_pattern(srcFrame:SEQUENTIAL, destFrame:SEQUENTIAL)
void HardwareSobelLaplacian(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS])
{
	int xcoi, ycoi;
		u32 frameWidth;

		ap_linebuffer<u8, 3, DEMO_WIDTH> buff_A;
		ap_window<u8,3,3> buff_C;
		//frameWidth = (fApp >> 16);



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

					if( ycoi <= 1 || xcoi <= 0 || ycoi == DEMO_HEIGHT || xcoi == DEMO_WIDTH) //Si no se dan las condiciones
						edge=0;
					else{
						short x_weight = 0, y_weight = 0;
						u8 i, j;

						//Kernel para deteccion de bordes con sobel edge
						const short x_op[3][3] = { {0,-1,0},{-1,4,-1},{0,-1,0}};
						const short y_op[3][3] = { {1,1,1},{-1,8,-1}, {-1,-1,-1}};

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


				/*
				 * Fin deteccion de bordes
				 */
			if(ycoi > 0 && xcoi > 0){
				destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((edge) & 0x00F8) << (11-3)) | (((edge) & 0x00F8) << (6-3)) | (((edge) & 0x00F8) >> 2);
			}
			}//For2 END
		}//For1 END
}

void SoftwareSobelLaplacian(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS])
{
	int xcoi, ycoi;
		u32 frameWidth;

		ap_linebuffer<u8, 3, DEMO_WIDTH> buff_A;
		ap_window<u8,3,3> buff_C;
		//frameWidth = (fApp >> 16);



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



					buff_A.insert_bottom2(grayIn,xcoi); 						//Insertamos el valor al final del linebuffer de la fila que toque

				}

				//movemos todo el contenido del buffer C de la ventana a la derecha ( esto lo hace en escala de grises, es para que la imagen se quede detro de los bordesde la pantalla)
				buff_C.shift_right2();
				if(xcoi < DEMO_WIDTH){
					buff_C.insert2(buff_A.getval2(2,xcoi),0,2); //insertamos los valores que hemos tomado en el buffer A
					buff_C.insert2(temp,1,2);
					buff_C.insert2(grayIn,2,2);
				}


				/*
				 * Procesamiento para la detección de bordes
				 */

					if( ycoi <= 1 || xcoi <= 0 || ycoi == DEMO_HEIGHT || xcoi == DEMO_WIDTH) //Si no se dan las condiciones
						edge=0;
					else{
						short x_weight = 0, y_weight = 0;
						u8 i, j;

						//Kernel para deteccion de bordes con sobel edge
						const short x_op[3][3] = { {0,-1,0},{-1,4,-1},{0,-1,0}};
						const short y_op[3][3] = { {1,1,1},{-1,8,-1}, {-1,-1,-1}};

						for(i=0; i < 3; i++){
							for(j = 0; j < 3; j++){

								x_weight = x_weight + (buff_C.getval2(i,j) * x_op[i][j]);
								y_weight = y_weight + (buff_C.getval2(i,j) * y_op[i][j]);

							}
						}
						edge = ABS(x_weight) + ABS(y_weight);

						if(edge > 200)	edge = 255;
						else if(edge < 100)		edge = 0;

					}


				/*
				 * Fin deteccion de bordes
				 */

				destFrame[(ycoi-1)*DEMO_WIDTH+(xcoi-1)] = (((edge) & 0x00F8) << (11-3)) | (((edge) & 0x00F8) << (6-3)) | (((edge) & 0x00F8) >> 2);

			}//For2 END
		}//For1 END
}


Laplacian::Laplacian(){
			name = "laplacian";
			desc = "hola";
		}
	//Implementar esto, aqui se corre el filtro
void Laplacian::apply (u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], int hwsw) {
		//cout<<"filtro2"<<endl;
		if(hwsw ==1)
			HardwareSobelLaplacian(srcFrame,destFrame);
		else
			SoftwareSobelLaplacian(srcFrame,destFrame);
	};


static Laplacian f;




