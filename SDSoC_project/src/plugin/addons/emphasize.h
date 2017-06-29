/*
 * emphasize.h
 *
 *  Created on: 10 de jun. de 2017
 *      Author: Raul
 */

#ifndef SRC_PLUGIN_ADDONS_EMPHASIZE_H_
#define SRC_PLUGIN_ADDONS_EMPHASIZE_H_

#include "../plugin.h"

class Emphasize : public Plugin {
  	public:
	Emphasize();
	//Implementar esto, aqui se corre el filtro
	void apply (u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], int hwsw);
};




#endif /* SRC_PLUGIN_ADDONS_EMPHASIZE_H_ */
