/*
 *In this file you only have to use the plugin API
 * With Plugin:: you can call the plugin class and you don't have to create one Plugin object
 * but Plugin have had statics members
 */

#include <iostream>
#include "plugin.h"
#include <stdio.h>
#include <string.h>
#include "../video_demo.h"

using namespace std;

char ** testPlugin() {
	int nplug=0;
	char str[20];
	char *str2[20];
	nplug=Plugin::n_plugins();
	cout << "Plugins added: " << nplug << endl;


	char * nombrePlugins[nplug];

	for(int i=0; i<nplug ; i++){
		strcpy (str,Plugin::getPlugin(i)->getName().c_str());
		nombrePlugins[i]=str;
		cout<< i <<" "<<nombrePlugins[i]<<endl;

	}

	//nplugins=nplug;
   	return nombrePlugins;
}

void executeFilter(u16 srcFrame[DEMO_PIXELS], u16 destFrame[DEMO_PIXELS], char v, int hwsw){

	//Para pasar de char a int
	int i = v - '0';
	Plugin::getPlugin(i)->apply(srcFrame,destFrame,hwsw);
}

int numberPlugins(){
	return Plugin::n_plugins();

}











