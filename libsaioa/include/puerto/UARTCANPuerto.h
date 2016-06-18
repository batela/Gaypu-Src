/*
 * UARTCANPuerto.h
 *
 *  Created on: Sep 21, 2014
 *      Author: borja
 */

#ifndef UARTCANPUERTO_H_
#define UARTCANPUERTO_H_


#include "Puerto.h"
#include <stdio.h> /* Standard input/output definitions */
#include <string.h> /* String function definitions */
#include <unistd.h> /* UNIX standard function definitions */
#include <iostream>
#include <fstream>
#include <termios.h>  /* POSIX terminal control definitions */

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Env.h"


using namespace std;
namespace container {

class UARTCANPuerto: public container::Puerto {
public:

  UARTCANPuerto(string id,int baudRate,int charSize,int parity,int stopBits);
	virtual ~UARTCANPuerto();
	int abrir();
	void cerrar();
	int reabrir();
	int leer (char buffer[]);
	int leerSimple (char &data);
	int escribir (char buffer[], int count);
private:
	int resetSerial ();
	int setRTS (int level);
	void configuraVelocidad (int a);
	void configuraParidad 	(int a);
	void configuraBitsDatos (int a);

	int   baudRate;
	int   charSize;
	int   parity;
	int 	stopBits;
	int   serial_port ;
  struct termios options;
  struct termios oldterminfo;

};

} /* namespace container */
#endif /* RS232PUERTO_H_ */
