/*
 * sd_capture.c
 *
 *  Created on: 27 de abr. de 2017
 *      Author: Raul
 */


//#include <iostream.h>
#include <string.h>
//#include <vector.h>
#include <stdlib.h>
#include "sd_capture.h"


//using namespace std;

int Sd_capture_Applications(struct Apps *apps)
{
	FRESULT Res;
	UINT NumBytesRead;
	UINT NumBytesWritten;
	UINT ReadSize = 1024;
	u32 BuffCnt;
	u32 FileSize = (8*1024*1024);
	char components[200];
	int n_components=0;

	/*
	 * To test logical drive 0, Path should be "0:/"
	 * For logical drive 1, Path should be "1:/"
	 */
	TCHAR *Path = "0:/";

	Platform = XGetPlatform_Info();
	if (Platform == XPLAT_ZYNQ_ULTRA_MP) {
		/*
		 * Since 8MB in Emulation Platform taking long time, reduced
		 * file size to 8KB.
		 */
		FileSize = 8*1024;
	}

	for(BuffCnt = 0; BuffCnt < FileSize; BuffCnt++){
		SourceAddress[BuffCnt] = TEST + BuffCnt;
	}

	/*
	 * Register volume work area, initialize device
	 */
	Res = f_mount(&fatfs, Path, 0);

	/*if (Res != FR_OK) {
		return XST_FAILURE;
	}

	/*
	 * Path - Path to logical driver, 0 - FDISK format.
	 * 0 - Cluster size is automatically determined based on Vol size.
	 */
	/*Res = f_mkfs(Path, 0, 0);
	if (Res != FR_OK) {
		return XST_FAILURE;
	}*/

	/*
	 * Open file with required permissions.
	 * Here - Creating new file with read/write permissions. .
	 * To open file with write permissions, file system should not
	 * be in Read Only mode.
	 */

	SD_File = (char *)FileName;

	Res = f_open(&fil, SD_File, FA_READ);
	/*if (Res) {
		return XST_FAILURE;
	}



	std::string hola = "hola";
	char salida[hola.size()];

	/*
	 * Write in the file
	 */
	/*for(BuffCnt = 0; BuffCnt < hola.size(); BuffCnt++){
			SourceAddress[BuffCnt] = hola[BuffCnt];
		}
	Res = f_write(&fil, (const void*)SourceAddress, hola.size(),&NumBytesWritten);
		if (Res) {
			return XST_FAILURE;
		}

		xil_printf("Numero de bytes escritos: \r\n");
		std::cout << NumBytesWritten<<std::endl;

		/*
		 * Pointer to beginning of file .
		 */
		Res = f_lseek(&fil, 0);
		/*if (Res) {
			return XST_FAILURE;
		}

		/*
		 * Read from the file
		 */

	Res = f_read(&fil, ( void*)DestinationAddress, ReadSize,&NumBytesRead);
		/*if (Res) {
					return XST_FAILURE;
				}*/

		for(BuffCnt = 0; BuffCnt < ReadSize && DestinationAddress[BuffCnt] != '\0'; BuffCnt++){
				//components.push_back( DestinationAddress[BuffCnt]);
				components[BuffCnt]=DestinationAddress[BuffCnt];
				/*if(DestinationAddress[BuffCnt]=='\n');
					n_components++;*/
			}
		components[BuffCnt]='\0';

		//xil_printf("Palabra: \r\n");
		//xil_printf(components);


	/*
	 * Close file.
	 */
	Res = f_close(&fil);
	/*if (Res) {
		return XST_FAILURE;
	}*/

	/*
	 * En esta seccion vamos activando los componentes que tenemos en la SD.
	 * Si queremos tener mas opciones, añadimos el filtro al core IP, su nombre en el struct y la comparación aquí
	 */

	if(strstr(components, "grayscale") != NULL){
		apps->grayscale=1;
		n_components++;
	}
	if(strstr(components, "edge") != NULL){
		apps->edge=1;
		n_components++;
	}
	if(strstr(components, "sepia") != NULL){
		apps->sepia=1;
		n_components++;
	}

	if(strstr(components, "laplacian") != NULL){
		apps->laplacian=1;
		n_components++;
	}
	if(strstr(components, "emphasize") != NULL){
		apps->emphasize=1;
		n_components++;
	}

	xil_printf("SD_cosas3\r\n");
	TakePicture();
	xil_printf("SD_cosas10\r\n");

	return n_components;
}




