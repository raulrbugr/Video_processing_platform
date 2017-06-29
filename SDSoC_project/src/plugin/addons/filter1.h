/*
 * filter1.h
 *
 *  Created on: 6 de jun. de 2017
 *      Author: Raul
 */

#ifndef SRC_ADDONS_FILTER1_H_
#define SRC_ADDONS_FILTER1_H_

#include "../plugin.h"


class Filtro1 : public Plugin {
  	public:
   		Filtro1();

   		//Call to HW or SW function
	void apply (u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], int hwsw);
};




#endif /* SRC_ADDONS_FILTER1_H_ */
