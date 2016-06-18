/*
 * IcopCanOpen.cpp
 *
 *  Created on: Jun 4, 2016
 *      Author: root
 */

#include <IQANEnlace.h>

namespace container {
  extern log4cpp::Category &log;

  IcopCanOpen* IcopCanOpen::m_pInstance = NULL;

  IcopCanOpen::IcopCanOpen(IcopCanOpen const&) {
    thread_mutex = PTHREAD_MUTEX_INITIALIZER;
  }

  IcopCanOpen::IcopCanOpen() {
    thread_mutex = PTHREAD_MUTEX_INITIALIZER;
  }

  IcopCanOpen::~IcopCanOpen() {
  }

  IcopCanOpen* IcopCanOpen::Instance()
  {
    if (!m_pInstance)   // Only allow one instance of class to be generated.
      m_pInstance = new IcopCanOpen();
    return m_pInstance;

  }

  void IcopCanOpen::SendRecStandardCanFrame(int id, int lenght ,char * data, char * msj)
  {
    pthread_mutex_lock( &thread_mutex );
    log.debug("%s: %s",__FILE__, "SendRecStandardCanFrame");

    sprintf (msj,"t%03X%c\n",id,lenght);
    log.debug("%s: %s",__FILE__, "SendRecStandardCanFrame");
    pthread_mutex_unlock( &thread_mutex );
  }

  int IcopCanOpen::CreateCanConfigurationFrame(int bauds , int T0, int F0, int M0, int T1, int F1, int M1, char * data)
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

  int IcopCanOpen::CreateDataFrame(int lenght ,unsigned short * data, char * body)
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

  unsigned char IcopCanOpen::CalculateCHK(int l, char *d , char &C0, char &C1)
  {
    int total= 0;
      for(int i = 0;i < l; i++)
      total += (char)d[i];

      sprintf (&C0,"%1X",((total & 0xf0) >> 4));
      sprintf (&C1,"%1X",(total & 0x0f));

  }


} /* namespace container */
