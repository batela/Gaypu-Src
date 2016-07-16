/*
 * ControladorPesadaDinamico.cpp
 *
 *  Created on: May 22, 2016
 *      Author: root
 */

#include "../include/ControladorPesajeIO.h"

extern DX80Enlace *dx80;
namespace container {

  extern log4cpp::Category &log;
  /**
   *
   */
  void * identificaPesada (void * a){

    ControladorPesajeIO *cpd =  (ControladorPesajeIO*) a;
    UARTCANExplorador* ex = (UARTCANExplorador*) cpd->explorador;
    IQANEnlace * io = cpd->enlace;
    BSCLEnlace *basc =  (BSCLEnlace *) cpd->bascula;

    struct timespec tim, tim2;
    tim.tv_sec  = 0;
    //tim.tv_nsec = 1 * 1000000L; //en milisegundos
    tim.tv_nsec = atoi(Env::getInstance()->GetValue("iqanperiod").data()) * 1000000L; //en milisegundos

    int isCarro = 0;
    int isPalpa = 0;
    int isTwisl = 0;
    int isSubir = 0;

    int ioCarro  = 0;
    int ioPalpa  = 0;
    int ioTwisl  = 0;
    int ioSubir  = 0;

    ioCarro  = atoi(Env::getInstance()->GetValue("iocarroenvia").data()); //restamos 1 para referencia en 0
    ioPalpa  = atoi(Env::getInstance()->GetValue("iopalpadores").data());
    ioTwisl  = atoi(Env::getInstance()->GetValue("iotwislock").data());
    ioSubir  = atoi(Env::getInstance()->GetValue("iomandosubir").data());

    pthread_yield();
    int ciclo = 0;
    while (cpd->seguir == true){

      // Se actualiza el IQAN cada 10 ciclos de lectura ojo con los tiempos
      if (ciclo++ == 0)      cpd->actualizaRemoto();
      else if (ciclo++ > 10) ciclo = 0;

      if (ex->Explora() == 0){
        isCarro = io->GetLocks()->GetLock(ioCarro);
        isPalpa = io->GetLocks()->GetLock(ioPalpa);
        isTwisl = io->GetLocks()->GetLock(ioTwisl);
        isSubir = io->GetLocks()->GetLock(ioSubir);

        log.info("%s: %s: %d-%d-%d-%d",__FILE__, "Leido: ", isCarro, isPalpa,isTwisl,isSubir);
        //log.info("%s: %s: %d-%d-%d-%d-%d-%d-%d-%d",__FILE__, "IO en Grua: ", isIOg0, isIOg1, isIOg2, isIOg3, isIOg4, isIOg5, isIOg6, isIOg7);
        basc->getBSCL()->SetIsFalloCom(io->GetIsFalloCom());
        basc->getBSCL()->SetIO(isCarro,isPalpa,isTwisl,isSubir);
        basc->getBSCL()->SetGruaIO(0, 0, 0, 0, 0, 0, 0, 0); // No se usa pero pongo a 0 por defecto.

        if (isPalpa && isTwisl){
          log.info("%s: %s",__FILE__, "CPD Estado SUBIENDO");
          cpd->estadoPesaje = SUBIENDO;
        }
        else if (!isPalpa && isTwisl )  {
          log.info("%s: %s",__FILE__, "CPD Estado PESANDO");
          cpd->estadoPesaje = PESANDO;
        }
        else if (!isTwisl) {
          log.info("%s: %s",__FILE__, "CPD Estado REPOSO");
          cpd->estadoPesaje = REPOSO;
        }

      }
      nanosleep(&tim , &tim2);
    }
    return 0;
  }

  /**
   *
   */
  ControladorPesajeIO::ControladorPesajeIO(UARTCANExplorador * ex,IQANEnlace * e ,BSCLEnlace *bscl) {
    enlace = e ;
    seguir = false ;
    explorador = ex;
    bascula = bscl;
    estadoPesaje = NO_DEFINIDO;
    thControlador = 0 ;
  }
  /**, bool lan
   *
   */
  ControladorPesajeIO::~ControladorPesajeIO() {

  }


  /**
   *
   */
  void ControladorPesajeIO::lanzarControlador (){
    

    if (this->seguir == true) {
      log.warn("%s: %s", __FILE__, "Controlador Pesaje lanzado..");
      return;
    }
    this->seguir = true;
    log.info("%s: %s", __FILE__, "Lanzado controlador de pesaje IO");

    int res = pthread_create( &thControlador, NULL, identificaPesada, this);

    struct sched_param params;
    params.sched_priority = sched_get_priority_max(SCHED_FIFO);
    res = pthread_setschedparam(thControlador, SCHED_FIFO, &params);
    if (res != 0) {
      log.error("%s: %s", __FILE__, "Unsuccessful in setting thread realtime prio" );
    }

    struct sched_param paramsn;
    int policy = 0;
    res = pthread_getschedparam(thControlador, &policy, &paramsn);
    if (res != 0) {
      log.error("%s: %s", __FILE__,"Couldn't retrieve real-time scheduling paramers");
    }
  }
  /**
   *
   */
  void ControladorPesajeIO::actualizaRemoto()
  {
    unsigned short data[4];
    unsigned short datasize[4];
    bool isFijo = dx80->getDX()->getIsFijo();

    static int turno = 1;


    printf ("\n\n\n\n***************************\n\n\n\n");
    printf ("\n\n\n\n***** ActualizaRemoto *****\n\n\n\n");
    printf ("\n\n\n\n*****   %d %d %d %d  *****\n\n\n\n",data[0],data[1],data[2],data[3]);
    printf ("\n\n\n\n***************************\n\n\n\n");


    if (turno == 1){
      data[0] = dx80->getDX()->getPeso();
      if ( isFijo==true ) data[1] = data[0];
      else data[1] = 0 ;
      data[2] = dx80->getDX()->getCMX();
      data[3] = dx80->getDX()->getCMY();
      datasize[0] = datasize[1] = 2;
      datasize[2] = datasize[3] = 1 ;
      this->explorador->SendData(4,datasize,data,"obj1");
    }
    else {
      data[0] = dx80->getDX()->getInput2Peso();
      data[1] = dx80->getDX()->getInput1Peso();
      data[2] = dx80->getDX()->getInput4Peso();
      data[3] = dx80->getDX()->getInput3Peso();
      datasize[0] = datasize[1] = datasize[2] = datasize[3] = 2 ;
      this->explorador->SendData(4,datasize,data,"obj2");
    }
    turno = turno * (-1);
  }

} /* namespace container */
