/*
 * ControladorPesaje.h
 *
 *  Created on: May 22, 2016
 *      Author: root
 */

#ifndef CONTROLADORPESAJE_H_
#define CONTROLADORPESAJE_H_
#include "kemen.h"

namespace container {

  class ControladorPesaje {
    public:
      ControladorPesaje();
      virtual ~ControladorPesaje();
      virtual void lanzarControlador() {};
      virtual inline ESTADO_PESADA getEstadoPesaje () {return estadoPesaje ;}
      ESTADO_PESADA estadoPesaje;
      bool seguir ;
    protected:
      inline bool isIniciado () {return seguir;}


      pthread_t thControlador;
      pthread_mutex_t thread_mutex;
    private:

  };

} /* namespace container */

#endif /* CONTROLADORPESAJE_H_ */
