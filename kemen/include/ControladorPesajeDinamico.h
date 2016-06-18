/*
 * ControladorPesadaDinamico.h
 *
 *  Created on: May 22, 2016
 *      Author: root
 */

#ifndef CONTROLADORPESADADINAMICO_H_
#define CONTROLADORPESADADINAMICO_H_

#include <unistd.h>
#include <string>
#include <vector>

#include "DX80.h"
#include "DX80Enlace.h"
#include "Env.h"
#include "LR.h"
#include "ControladorPesaje.h"

namespace container {

  class ControladorPesajeDinamico : public ControladorPesaje
  {
    public:
      ControladorPesajeDinamico(Enlace *);
      virtual ~ControladorPesajeDinamico();
      friend void *identificaPesadaDinamic( void *ptr );
      virtual void lanzarControlador();
      Enlace * enlace;
      //inline ESTADO_PESADA getEstadoPesaje () {return estadoPesaje ;}
    protected:
//      pthread_t thControlador;
//      pthread_mutex_t thread_mutex;
    private:


      //bool seguir ;

  };

} /* namespace container */

#endif /* CONTROLADORPESADADINAMICO_H_ */
