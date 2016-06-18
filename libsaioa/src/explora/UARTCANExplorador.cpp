/*
 * UARTCANExplorador.cpp
 *
 *  Created on: Dec 17, 2014
 *      Author: root
 */

#include <UARTCANExplorador.h>
#include "IQANEnlace.h"

#define noop (void)0

namespace container {
extern log4cpp::Category &log;

  void* lectorUARTCAN(void * explorador){
    log.debug("%s: %s",__FILE__, "Lanzado thread lector...");

    UARTCANExplorador * exp = (UARTCANExplorador*)explorador;
    int contador = 0, res= 0 ;
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = atoi(Env::getInstance()->GetValue("uartcanperiod").data()) * 1000000L; //en milisegundos

    while (exp->sigue){
      res = 0;

      if (exp->getPuerto()->getIsOpen()== false)
        res = exp->getPuerto()->abrir();
      else
        res = exp->Explora();

      // Si ha ido mal cerramos el puerto.
      if (res < 0)  exp->getPuerto()->cerrar();

      nanosleep(&tim , &tim2);
    }
    printf ("Salimos thread explorador..");
  }

 /***
 *
 */

  UARTCANExplorador::UARTCANExplorador(Enlace* e, Puerto* p) : Explorador (e,p,false){
    cfg = NULL;
    estado = PRE_INIT;
    this->enlace = (IQANEnlace *)e;

    thread_mutex = PTHREAD_MUTEX_INITIALIZER;
    Abrir();
  }

  UARTCANExplorador::UARTCANExplorador(Enlace* e, Puerto* p,string file) : Explorador (e,p,false){
       estado = PRE_INIT;
       thread_mutex = PTHREAD_MUTEX_INITIALIZER;
       cfg = NULL;

       this->enlace =  (IQANEnlace *) e;
       Abrir(); //Esta funcion abre el puerto

       int lanzar = 0 ;
       if (ConfigReadFile(file.data(), &cfg) != CONFIG_OK) {
         log.error("%s: %s %s",__FILE__, "Error leyendo fichero de confguracion: ", file.data());
         cfg = NULL;
       }
       else {
         ConfigReadInt(cfg,"general","lanzar",&lanzar,0);
         if (lanzar >0) {
           this->sigue= true ;
           LanzarExplorador();
         }
       }
       Configura();
  }
/**
 *
 */
  UARTCANExplorador::UARTCANExplorador(vector<Enlace*> e, Puerto* p,string file) : Explorador (e,p,false){

    estado = PRE_INIT;
    thread_mutex = PTHREAD_MUTEX_INITIALIZER;
    cfg = NULL;
    printf ("Lamo a abrir\n");
    Abrir(); //Esta funcion abre el puerto
    this->enlace = (IQANEnlace *) e[0];
    int lanzar = 0 ;
    if (ConfigReadFile(file.data(), &cfg) != CONFIG_OK) {
      log.error("%s: %s %s",__FILE__, "Error leyendo fichero de confguracion: ", file.data());
      cfg = NULL;
    }
    else {
      ConfigReadInt(cfg,"general","lanzar",&lanzar,0);
      if (lanzar >0) {
        this->sigue= true ;
        LanzarExplorador();
      }
    }
    Configura();
  }
/***
 *
 */
  UARTCANExplorador::~UARTCANExplorador() {
  }
/**
 *
 */
  void UARTCANExplorador::LanzarExplorador (){
    log.info("%s: %s", __FILE__, "Abriendo puerto");
    if (this->getPuerto()->getIsOpen()== false && this->getPuerto()->abrir() != 0) log.error("%s: %s", __FILE__, "Error abriendo puerto!!");
    int res = pthread_create( &idThLector, NULL, lectorUARTCAN, this);

    struct sched_param params;
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);
    res = pthread_setschedparam(idThLector, SCHED_FIFO, &params);
    if (res != 0) {
      log.error("%s: %s", __FILE__, "Unsuccessful in setting thread realtime prio" );
    }

    int policy = 0;
    res = pthread_getschedparam(idThLector, &policy, &params);
    if (res != 0) {
      log.error("%s: %s", __FILE__,"Couldn't retrieve real-time scheduling paramers");
    }


  }
/***
 * Esta funcion explora un solo equipo que bien tiene una configuración predefinida o tiene
 * un fichero de configuracion, tenderá a desaparcer.
 */
  int UARTCANExplorador::Explora (){

    log.debug("%s: %s",__FILE__, "Comienza exploracion UARTCAN");
    int res = 1 ;
    //ResetBuffers();
    switch (estado){
      case PRE_INIT:
        noop;
      break;
      case IDLE:
        lRX = this->getPuerto()->leer(bufferRX);
        res = enlace->analizaTrama(bufferRX, (char)bufferRX[0]);
        res  = 0 ; //FIXME Btl
      break;
      case WAIT_RESPONSE:
        lRX = this->getPuerto()->leer(bufferRX);
        res = enlace->analizaTrama(bufferRX, (char)bufferRX[0]);
        estado = IDLE;
      break;
      case SEND_DATA:
        this->getPuerto()->escribir(bufferTX,lTX);
        estado = WAIT_RESPONSE;
      break;
    }
    log.debug("%s: %s",__FILE__, "Termina exploracion UARTCAN");
    return res;
  }
  /**
   *
   */
  void UARTCANExplorador::ResetBuffers (){
    lRX = lTX = 0;
    memset (bufferRX,0,256);
    memset (bufferTX,0,256);
  }

  /**
   *
   */
  int UARTCANExplorador::Configura (){

      log.debug("%s: %s",__FILE__, "Comienza configuracion UARTCAN");
      IQANEnlace enlace ;
      int res = 1 ;
      while (estado == PRE_INIT){
        Abrir();
        res = this->SendResetRequest();
        if (res == 0){
          res = this->SendRemoteRequest();

        }
        if (res == 0)
          estado = IDLE;
        else {
          log.debug("%s: %s",__FILE__, "Esperamos un segundo");
          sleep (1);
          Cerrar();
        }

      }
      log.debug("%s: %s",__FILE__, "Termina configuracion UARTCAN");
      return res;
    }

  int UARTCANExplorador::SendData (int lenght ,unsigned short * data, char * body){

    log.debug("%s: %s",__FILE__, "Comienza Request Remote");

    int res = 1 ;
    ResetBuffers();
    //lTX = enlace.CreateDataFrame(lenght ,data, bufferTX);
    if (lTX > 0){
      estado = SEND_DATA;
    }

    log.debug("%s: %s",__FILE__, "Termina Request Remote");
    return res;
  }

    /**
     * Devuelve 0 si OK
     */
    int UARTCANExplorador::SendResetRequest (){

      log.debug("%s: %s",__FILE__, "Comienza Reset MCU");

      int res = 1 ;
      char bufferTX[256];
      char bufferRX[256];
      int  lTX = 0;
      int  lRX = 0;
      lTX = enlace->CreateInitUARTFrame(bufferTX);
      if (lTX > 0){
        this->getPuerto()->escribir(bufferTX,lTX);
        lRX = this->getPuerto()->leer(bufferRX);
        if ((char)bufferRX[0] == 'U')
          res = enlace->analizaTrama(bufferRX, (char)bufferRX[0]);
        else {

          log.warn("%s: %s",__FILE__, "Error al reset MCU");
          res = 1;
        }

      }

      log.debug("%s: %s",__FILE__, "Termina Reset MCU");
      return res;
    }
    /**
     * Devuelve 0 si OK
     */
    int UARTCANExplorador::SendRemoteRequest (){

      log.debug("%s: %s",__FILE__, "Comienza Request Remote");

      int res = 1 ;
      char bufferTX[256];
      char bufferRX[256];
      int  lTX = 0;
      int  lRX = 0;
      lTX = enlace->CreateSetRemoteRequest(bufferTX);
      if (lTX > 0){
        this->getPuerto()->escribir(bufferTX,lTX);
        lRX = this->getPuerto()->leer(bufferRX);
        res = enlace->analizaTrama(bufferRX, (char)bufferRX[0]);
      }

      log.debug("%s: %s",__FILE__, "Termina Request Remote");
      return res;
    }
} /* namespace container */
