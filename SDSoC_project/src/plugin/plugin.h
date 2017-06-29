/*
 * plugin.h
 *
 *  Created on: 2 de jun. de 2017
 *      Author: Raul
 */

#ifndef SRC_PLUGIN_H_
#define SRC_PLUGIN_H_


#include <iostream>
#include <string>
#include "../video_demo.h"
#include "../ap_video.h"
#include <stdio.h>
#include <fstream>

using namespace std;

//Max number of plugins
#define MAX_PLUGINS 20



class Plugin {
 	protected:
		string name;
		string desc;


		// Array to store the plugin list
   		static Plugin * plugins[MAX_PLUGINS];
   		static int n;


  	public:

   		//Contructor
		Plugin();

		//to get name
   		string getName();

   		//to get description
		string getDesc();

		//Abstract method, this method must be implemend in sons classes
		virtual void apply (u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], int hwsw) = 0; // pure specifier

		//If you want to call a method with the class not with the object
		static int n_plugins();


		//Return the called plugin
		static Plugin* getPlugin(string name);

		static Plugin* getPlugin(const int &v);


};


#endif /* SRC_PLUGIN_H_ */
