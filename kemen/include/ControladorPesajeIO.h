/*
 * ControladorPesadaIO.h
 *
 *  Created on: May 22, 2016
 *      Author: root
 */

#ifndef CONTROLADORPESAJEIO_H_
#define CONTROLADORPESAJEIO_H_

#include <unistd.h>
#include <string>
#include <vector>

#include "DX80.h"
#include "DX80Enlace.h"
#include "Env.h"
#include "LR.h"
#include "ControladorPesaje.h"
#include "MODBUSExplorador.h"
#include "UARTCANExplorador.h"

namespace container {

  class ControladorPesajeIO : public ControladorPesaje {
    public:
      ControladorPesajeIO(UARTCANExplorador *ex,IQANEnlace * e ,BSCLEnlace *bscl);
      virtual ~ControladorPesajeIO();
      friend void *identificaPesada( void *ptr );
      virtual void lanzarControlador();
      void actualizaRemoto ();
      //inline bool isIniciado () {return seguir;}
      //inline ESTADO_PESADA getEstadoPesaje () {return estadoPesaje ;}

    protected:
//      pthread_t thControlador;
//      pthread_mutex_t thread_mutex;

    private:
      //void lanzarControlador();
      UARTCANExplorador * explorador;
      IQANEnlace * enlace;
      BSCLEnlace *bascula;
    //  bool seguir ;
    //  ESTADO_PESADA estadoPesaje;

  };

} /* namespace container */

#endif /* CONTROLADORPESADADINAMICO_H_ */
