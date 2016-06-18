/*
 * UARTCANExplorador.h
 *
 *  Created on: Dec 17, 2014
 *      Author: root
 */

#ifndef UARTCANEXPLORADOR_H_
#define UARTCANEXPLORADOR_H_

#include "Explorador.h"
#include "UARTCANPuerto.h"
#include "IQANEnlace.h"
#include "configini.h"
namespace container {

class UARTCANExplorador: public Explorador {
public:
  enum UARTCAN_STATUS {PRE_INIT,IDLE, WAIT_RESPONSE,SEND_DATA} ;
  UARTCANExplorador(Enlace* e, Puerto* p);
  UARTCANExplorador(Enlace* e, Puerto* p, string file);
  UARTCANExplorador(vector<Enlace*> e, Puerto* p,string file) ;
	virtual ~UARTCANExplorador();
	int Explora ();
	int SendData (int lenght ,unsigned short * data, char * body);
	IQANEnlace* GetIQANEnlace () {return  enlace;}
private:
	void ResetBuffers ();
	int SendResetRequest ();
	int SendRemoteRequest ();
	int Configura();
	void LanzarExplorador();
	void ExploraEquipo();
	UARTCAN_STATUS estado;
	pthread_mutex_t thread_mutex;
	Config* cfg;
  char bufferTX[256];
  char bufferRX[256];
  int  lTX = 0;
  int  lRX = 0;

  IQANEnlace* enlace ;
};

} /* namespace container */

#endif /* UARTCANEXPLORADOR_H_ */
