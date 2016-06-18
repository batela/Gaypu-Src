/*
 * RS232Puerto.cpp
 *
 *  Created on: Sep 21, 2014
 *      Author: borja
 */

#include "UARTCANPuerto.h"

namespace container {
extern log4cpp::Category &log;



  UARTCANPuerto::UARTCANPuerto(string id,int baudRate,int charSize,int parity,int stopBits) : Puerto(id,UARTCAN)
	{
		this->baudRate  = baudRate;
		this->charSize  = charSize;
		this->parity    = parity;
		this->stopBits = stopBits;
	}

  UARTCANPuerto::~UARTCANPuerto() {

	}

	int UARTCANPuerto::abrir(){
		log.info("%s: %s %s",__FILE__, "Comienza abrir puerto..",this->getName().data());

		serial_port = open(name.data(), O_RDWR);

		if (serial_port == -1) {
		  return -1;
		}
		tcgetattr(serial_port, &options);
		this->configuraVelocidad(this->baudRate);
		this->configuraParidad(this->parity);
		this->configuraBitsDatos(this->stopBits);
		options.c_oflag     =   0;
		options.c_cc[VMIN] = 0;     //min carachters to be read
		options.c_cc[VTIME] = 5;    //Time to wait for data (tenths of seconds)
		options.c_cflag |= (CLOCAL | CREAD);

		options.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG); // make raw

		tcflush(serial_port, TCIFLUSH);
		if (tcsetattr(serial_port, TCSANOW, &options)==-1)
		{
		  return -1;
		}

		this->setIsOpen(true);
		this->resetSerial();
		log.info("%s: %s",__FILE__, "Puerto abierto!!");
		return 0 ;
	}

	void UARTCANPuerto::cerrar(){
	  tcsetattr(serial_port, TCSANOW, &oldterminfo);

	  if (close(serial_port) < 0)
	    log.warn ("%s: %s",__FILE__, "Puerton no cerrado!!");


		this->setIsOpen(false);
	}

	int UARTCANPuerto::reabrir(){
		this->cerrar();
		return ( this->abrir() );
	}
	/**
	 * Devuelve -1 si hay error.
	 */
	int UARTCANPuerto::leer (char buffer[]){
		int count = 0 ;
		int res = 0;

		char C;

		while ( (res=read(serial_port, &C, 1)) >0)   {
			if (count < 255) buffer[count++] = C;
			if (C =='\n' || C =='\r') break;
		}

		if (count >= 255 || res < 0 ) {
		  count = -1;
		  buffer[0]= 0 ;
		}
		else
		  buffer[count]= 0 ;

		log.debug("%s: Lectura puerto %s bytes %d",__FILE__, buffer, count);
		return count;
	}

	int UARTCANPuerto::resetSerial ()
	{
	  int res = 0 ;
	  res = setRTS(1);
	  usleep(5000);       /* pause 5 microseconds */
	  res = setRTS(0);
	  usleep(5000);
	  return res ;
	}


	int UARTCANPuerto::escribir (char buffer[], int count){
	    int res = 0 ;
	    log.info("%s: %s %s",__FILE__, "Escribimos en el ",this->getName().data());
	    res = write(serial_port,buffer , count);
	    log.debug("%s: Escritura puerto bytes %d datos %s ",__FILE__, count,buffer);
			return res;
	}

	void UARTCANPuerto::configuraVelocidad (int a){
		switch (a){
			case 1200:
			  cfsetispeed(&options, B1200);
			  cfsetospeed(&options, B1200);
			break;
			case 9600:
			  cfsetispeed(&options, B9600);
			  cfsetospeed(&options, B9600);
			break;
			case 115200:
			  cfsetispeed(&options, B115200);
			  cfsetospeed(&options, B115200);
			break;
			case 19200:
			  cfsetispeed(&options, B19200);
			  cfsetospeed(&options, B19200);
			break;
			default:
				log.error("%s: %s %d %s",__FILE__, "Velocidad de puerto no definida: ", a , " usando 115200");
				cfsetispeed(&options, B115200);
				cfsetospeed(&options, B115200);
			break;
		}
	}
	void UARTCANPuerto::configuraParidad (int a){
		switch (a){
			case 0:
			  options.c_cflag &= ~PARENB; //No paridad
			break;
			case 1:
			  options.c_cflag &= PARENB; //duda
			break;
			default:
				log.error("%s: %s %d %s",__FILE__, "Paridad de puerto no definida: ", a , " usando PAR");
				options.c_cflag &= ~PARENB; //No paridad
			break;
		}
	}

	int UARTCANPuerto::setRTS (int level)
	{
	    int status;

	    if (ioctl(serial_port, TIOCMGET, &status) == -1) {
	      log.error("%s: %s ",__FILE__, "Error cambiando estado RTS");
	      return -1;
	    }
	    if (level)
	        status |= TIOCM_RTS;
	    else
	        status &= ~TIOCM_RTS;
	    if (ioctl(serial_port, TIOCMSET, &status) == -1) {
	      log.error("%s: %s ",__FILE__, "Error cambiando estado RTS");
	      return -1;
	    }

	    return 0;
	}
	void UARTCANPuerto::configuraBitsDatos (int a){

	  options.c_cflag &= ~CSTOPB;
	  options.c_cflag &= ~CSIZE;
	  options.c_cflag |= CS8;
	  options.c_cflag &= ~CRTSCTS;
	  switch (a){
			case 7:
			  options.c_cflag |= CS7;
			break;
			case 8:
			  options.c_cflag |= CS8;
			break;
			default:log.error("%s: %s %d %s",__FILE__, "Bits de Datos de puerto no definida: ", a , " usando 8");
		  options.c_cflag |= CS8;
			break;
		}

	}

} /* namespace container */
