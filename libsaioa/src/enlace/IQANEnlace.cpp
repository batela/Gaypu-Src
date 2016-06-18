/*
 * IcopCanOpen.cpp
 *
 *  Created on: Jun 4, 2016
 *      Author: root
 */

#include "../include/enlace/IQANEnlace.h"

namespace container {
  extern log4cpp::Category &log;


  IQANEnlace::IQANEnlace() {
    cfg = NULL;
  }

  IQANEnlace::~IQANEnlace() {
  }

  /**
   *
   */
  void IQANEnlace::Configure (string a){
    log.debug("%s: %s %s",__FILE__, "Using config file: ", a.data());
    cfg = NULL;
    if (ConfigReadFile(a.c_str(), &cfg) != CONFIG_OK) {
      log.error("%s: %s %s",__FILE__, "Error leyendo fichero de confguracion: ", a.data());
      cfg = NULL;
    }

    log.debug("%s: %s",__FILE__, "Configuracion hecha!");
  }

  int IQANEnlace::CreateInitUARTFrame(char * msj){
    pthread_mutex_lock( &thread_mutex );
    log.debug("%s: %s",__FILE__, "Inicio CreateInitUARTFrame");
    char codigo[20];

    sprintf (msj,"U!21\r");

    log.debug("%s: %s",__FILE__, "Fin CreateInitUARTFrame");
    pthread_mutex_unlock( &thread_mutex );
    return (strlen(msj));

  }
  /**
   * Devuelve 0 si no hay error
   */
  int IQANEnlace::analizaTrama (char *msj, int tipo){
    int res = -1;
    char C = (char)tipo;
    int d  = 0x00;
    switch (C){
      case 'U':
        log.debug("%s: %s",__FILE__, "Analizando trama reset");
        res = 0 ;
      break;
      case '?':
      break;
      case '$':
        (strstr(msj,"OK") != NULL)? res = 0 : res = -1 ;
      break;
      case 's':
        log.debug("%s: %s",__FILE__, "LLega trama tipo 's'");
        char datos[10];
        memcpy (datos, &msj[5],2);
        datos[2] = 0;
        d = atoi(datos);
        this->locks.SetLock01(1);
        this->locks.SetLock02(((d & 0x02) > 0) ? 1 : 0); //Palpadores
        this->locks.SetLock03(((d & 0x01) > 0) ? 1 : 0); //Twist LOCKS
        this->locks.SetLock04(1);

        log.debug("%s: %s %s %d %d %d %d ",__FILE__, "Valor recibido de IQAN: " , datos, this->GetLocks()->GetLock01(),this->GetLocks()->GetLock02(),this->GetLocks()->GetLock03(),this->GetLocks()->GetLock04());

      break;
      case '!':
      break;
    }
    return res;
  }

  Locks * IQANEnlace::GetLocks()
  {

    return &(this->locks);
  }
  /**
   *
   */
  int IQANEnlace::CreateSetRemoteRequest(char * data)
  {
    pthread_mutex_lock( &thread_mutex );
    log.debug("%s: %s",__FILE__, "Inicio CreateSetRemoteRequest");
    char msj[100];
    char C0,C1;
    memset(msj,0,100);
    char codigo[20];

    ConfigReadString(cfg,"spreader","iqanaddres",codigo,20,NULL);
    //sprintf (msj,"S%s0",codigo);
    sprintf (msj,"S06A0",codigo);
    printf ("codigo****** %s\n",msj);

    this->CalculateCHK(strlen(msj),msj,C0,C1);
    sprintf (data,"%s%c%c%c",msj,C0,C1,0x0d);

    log.debug("%s: %s",__FILE__, "Fin CreateSetRemoteRequest");
    pthread_mutex_unlock( &thread_mutex );
    return (strlen (data));
  }
  /**
   *
   */
  void IQANEnlace::SendRecStandardCanFrame(int id, int lenght ,char * data, char * msj)
  {
    pthread_mutex_lock( &thread_mutex );
    log.debug("%s: %s",__FILE__, "Inicio SendRecStandardCanFrame");

    sprintf (msj,"t%03X%c\n",id,lenght);

    log.debug("%s: %s",__FILE__, "Fin SendRecStandardCanFrame");
    pthread_mutex_unlock( &thread_mutex );
  }

  int IQANEnlace::CreateCanConfigurationFrame(int bauds , int T0, int F0, int M0, int T1, int F1, int M1, char * data)
  {

    log.debug("%s: %s",__FILE__, "CreateConfigurationFrame");
    pthread_mutex_lock( &thread_mutex );

    sprintf (data,"P5410%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d%01d",bauds,\
        T0, (F0 & 0x01),((F0>>1) & 0x01),((F0>>2) & 0x01),((F0>>3) & 0x01),((F0>>4) & 0x01),((F0>>5) & 0x01),((F0>>6) & 0x01),((F0>>7) & 0x01), \
        (M0 & 0x01),((M0>>1) & 0x01),((M0>>2) & 0x01),((M0>>3) & 0x01),((M0>>4) & 0x01),((M0>>5) & 0x01),((M0>>6) & 0x01),((M0>>7) & 0x01), \
        T1, (F1 & 0x01),((F1>>1) & 0x01),((F1>>2) & 0x01),((F1>>3) & 0x01),((F1>>4) & 0x01),((F1>>5) & 0x01),((F1>>6) & 0x01),((F1>>7) & 0x01),\
        (M1 & 0x01),((M1>>1) & 0x01),((M1>>2) & 0x01),((M1>>3) & 0x01),((M1>>4) & 0x01),((M1>>5) & 0x01),((M1>>6) & 0x01),((M1>>7) & 0x01));

    pthread_mutex_unlock( &thread_mutex );
    log.debug("%s: %s",__FILE__, "CreateCanConfigurationFrame");
    return strlen (data);
  }

  int IQANEnlace::CreateDataFrame(int lenght ,unsigned short * data, char * body)
  {

    log.debug("%s: %s",__FILE__, "CreateDataFrame");

    int l = 0 ;
    for (int i = 0 ; i< lenght; i++){
      if (data[i] > 255) {
        sprintf (&body[l],"%02X%02X", data[i] & 0xff, ((data[i]>>8) & 0xff) );
        l=+2;
      }
      else {
        sprintf (&body[l],"%02X",data[i]);
        l=+1;
      }
    }
    log.debug("%s: %s",__FILE__, "CreateDataFrame");
    return l;
  }

  void IQANEnlace::CalculateCHK(int l, char *d , char &C0, char &C1)
  {
    int total= 0;
      for(int i = 0;i < l; i++)
      total += (char)d[i];

      sprintf (&C0,"%1X",((total & 0xf0) >> 4));
      sprintf (&C1,"%1X",(total & 0x0f));
  }


} /* namespace container */
