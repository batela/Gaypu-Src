/*
 * ControladorPesadaDinamico.cpp
 *
 *  Created on: May 22, 2016
 *      Author: root
 */

#include "../include/ControladorPesajeDinamico.h"

namespace container {
  extern log4cpp::Category &log;
  /**
   *
   */
  void * identificaPesadaDinamico (void * a){

    int pesajesOK   = 20;
    int pesajes     = 0 ;
    int estadoPesaje= 0 ;
    int isInestable = 5;

    float slope = 0;
    float avgY= 0;

    ControladorPesajeDinamico *cpd =  (ControladorPesajeDinamico*) a;
    DX80Enlace* ex = (DX80Enlace *) cpd->enlace;

    struct timespec tim, tim2;
    tim.tv_sec  = 0;
    tim.tv_nsec = 200 * 1000000L; //en milisegundos
    while (cpd->seguir == true){

      while (pesajes< pesajesOK){
        LR::getInstance()->addPoint(((DX80Enlace*)ex)->getDX()->getPeso(),pesajes++);
      }

      if (LR::getInstance()->evaluate(slope,avgY) == 1){
        if (slope > 0  && avgY != 0) {
          if (estadoPesaje != 1)  isInestable   = 5;
          if (isInestable == 0)   cpd->estadoPesaje  = SUBIENDO ; //subiendo
          else                    isInestable--;
        }
        else if (slope == 0 && avgY != 0){
          if (estadoPesaje != 2)  isInestable   = 5;
          if (isInestable == 0)   cpd->estadoPesaje  = PESANDO ; //pesando
          else                    isInestable--;
        }
        else if (slope < 0  && avgY != 0) {
          if (estadoPesaje != 3)  isInestable   = 5;
          if (isInestable == 0)   cpd->estadoPesaje  = BAJANDO ;//bajando
          else                    isInestable--;
        }
        else {
          isInestable   = 5;
          estadoPesaje = 0 ; //No hay contenedor
        }
      }
      pesajes = 0 ;
      log.debug("%s: %s %d",__FILE__, "Espera en Identifica pesada" , pesajes);
      nanosleep(&tim , &tim2);
    }
    return 0;
  }

  /**
   *
   */
  ControladorPesajeDinamico::ControladorPesajeDinamico(Enlace * e ) {
    enlace = e ;
    seguir = false ;
    estadoPesaje = NO_DEFINIDO;
    thControlador = 0 ;
  }
  /**, bool lan
   *
   */
  ControladorPesajeDinamico::~ControladorPesajeDinamico() {

  }

  /**
   *
   */
  void ControladorPesajeDinamico::lanzarControlador (){
    
    if (this->seguir == true) {
      log.warn("%s: %s", __FILE__, "Controlador Pesaje lanzado..");
      return;
    }
    log.info("%s: %s", __FILE__, "Abriendo puerto");

    int res = pthread_create( &thControlador, NULL, identificaPesadaDinamico, this);

    struct sched_param params;
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);
    res = pthread_setschedparam(thControlador, SCHED_FIFO, &params);
    if (res != 0) {
      log.error("%s: %s", __FILE__, "Unsuccessful in setting thread realtime prio" );
    }

    int policy = 0;
    res = pthread_getschedparam(thControlador, &policy, &params);
    if (res != 0) {
      log.error("%s: %s", __FILE__,"Couldn't retrieve real-time scheduling paramers");
    }
  }

} /* namespace container */
