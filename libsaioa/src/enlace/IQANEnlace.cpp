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
    thread_mutex = PTHREAD_MUTEX_INITIALIZER;
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
        res = 0 ;
      break;
      case '!':
        (strstr(msj,"OK") != NULL)? res = 0 : res = -1 ;
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
  int IQANEnlace::CreateCANConfRequest(char * data)
  {
    pthread_mutex_lock( &thread_mutex );
    log.debug("%s: %s",__FILE__, "Inicio CreateCANConfRequest");

    char msj[100];
    char C0,C1;

    memset(msj,0,100);
    sprintf (msj,"P526050000000000000000010000000000000000");

    printf ("<<<<<<<<<< :%s: %d\n",msj,strlen(msj));
    this->CalculateCHK(40,msj,C0,C1);
    sprintf (data,"%s%c%c%c",msj,C0,C1,0x0d);

    printf ("<<<<<<<<<< :%s: \n",data);

    log.debug("%s: %s",__FILE__, "Fin CreateCANConfRequest");
    pthread_mutex_unlock( &thread_mutex );
    return (strlen (data));
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

    ConfigReadString(cfg,"spreader","iqanaddres",codigo,20,"");
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
  int IQANEnlace::SendRecStandardCanFrame(int id, int lenght ,unsigned short *datasize, unsigned short * data, char * msj)
  {
    pthread_mutex_lock( &thread_mutex );
    log.debug("%s: %s %03X:%d",__FILE__, "Inicio SendRecStandardCanFrame: codigo:longitud", id, lenght);
    char C0, C1;
    int longi;
    char aux[100];
    memset (msj,0,100);
    longi = CreateDataFrame(lenght ,datasize,data,aux);
    sprintf (msj,"s%03X%d%s",id,longi,aux);
    int longtrama = strlen (msj);
    CalculateCHK(longtrama,msj,C0,C1);
    sprintf (&msj[longtrama],"%c%c%c",C0,C1,0x0d);

    log.debug("%s: %s",__FILE__, "Fin SendRecStandardCanFrame");
    pthread_mutex_unlock( &thread_mutex );
    return (strlen(msj));
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

  int IQANEnlace::CreateDataFrame(int lenght , unsigned short *datasize, unsigned short * data, char * body)
  {

    log.debug("%s: %s %d",__FILE__, "CreateDataFrame: longitud" ,lenght);

    int l = 0 ;
    for (int i = 0 ; i< lenght; i++){
      if (datasize[i]== 2){
        sprintf (&body[l],"%02X%02X", data[i] & 0xff, ((data[i]>>8) & 0xff) );
        l+= 4 * sizeof(char);
      }
      else if (datasize[i]==1){
        sprintf (&body[l],"%02X", data[i] & 0xff);
        l+= 2 * sizeof(char);
      }

    }
    log.debug("%s: %s",__FILE__, "CreateDataFrame");
    return (l/2); //Cada uno de los datos, un byte dos chars
  }

  void IQANEnlace::CalculateCHK(int l, char *d , char &C0, char &C1)
  {
    int total= 0;

    for(int i = 0;i < l; i++) total += d[i];

    char c0[2] ;
    char c1[2] ;
    sprintf (c0,"%1X",((total & 0xf0) >> 4));
    sprintf (c1,"%1X",(total & 0x0f));

    C0=c0[0];
    C1=c1[0];
  }


} /* namespace container */
