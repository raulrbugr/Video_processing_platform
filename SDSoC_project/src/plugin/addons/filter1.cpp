/*
 * filter1.cpp
 *
 *  Created on: 6 de jun. de 2017
 *      Author: Raul
 */

#include "filter1.h"



#define ABS(x)          ((x>0)? x : -x)
#define length(x) (sizeof(x)/sizeof(x[0]))

#pragma SDS data mem_attribute(srcFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE,destFrame:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
#pragma SDS data access_pattern(srcFrame:SEQUENTIAL, destFrame:SEQUENTIAL)
void HardwareSobelFilter1(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS]){

}

void SoftwareSobelFilter1(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS]){

}

   	Filtro1::Filtro1(){
			name = "nombreFiltro";
			desc = "descripcionFiltro";
		}

   void Filtro1::apply (u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], int hwsw) {
		/*if(hwsw == 1)
			HardwareSobelFilter1(srcFramen,destFrame);
		else
			SoftwareSobelFilter1(srcFramen,destFrame);*/
	};

//to create the object decoment this, and this is automatically added to the plugin class
//static Filtro1 f;



