/*
 * IBERCOMPEnlace.h
 *
 *  Created on: May 1, 2015
 *      Author: root
 */

#ifndef IBERCOMPENLACE_H_
#define IBERCOMPENLACE_H_


#include "DX80.h"
#include "enlace/Enlace.h"

namespace container {

class IBERCOMPEnlace: public Enlace {
public:
  IBERCOMPEnlace();
	virtual ~IBERCOMPEnlace();
	int analizaTrama (char *buffer, int tipo=0);
	int trataError ();
	int VerificaTrama (char *buffer);
	void VerificaEstadoRadio (char* buffer);
	void Configure (string a);
	DX80 * getDX () {return &dx;} ;
private:

	float lecturaEnVacio ;
	float lecturaEnPeso ;
	float pesoConocido ;
	float pendiente ;
	int  CalculaPeso();
	void CalculaAlarmas();
	int Redondea(int num);
	DX80 dx;

	int pma;
	int offsetpeso;
	int precisionpesada;

};

} /* namespace container */

#endif /* DX80ENLACE_H_ */
