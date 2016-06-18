/*
 * DX80Enlace.h
 *
 *  Created on: May 1, 2015
 *      Author: root
 */

#ifndef IQANENLACE_H_
#define IQANENLACE_H_


#include "IQAN.h"
#include "enlace/Enlace.h"
#include "Locks.h"

namespace container {

  class IQANEnlace: public Enlace {
  public:
    IQANEnlace();
    virtual ~IQANEnlace();
    int CreateInitUARTFrame(char * msj) ;
    int analizaTrama (char *buffer, int tipo=0);

    void Configure (string a);

    void SendRecStandardCanFrame(int id, int lenght ,char * data, char * msj);
    int  CreateCanConfigurationFrame(int bauds , int T0, int F0, int M0, int T1, int F1, int M1, char * data);
    int  CreateSetRemoteRequest(char * data);
    Locks * GetLocks() ;
  private:
      Locks locks;
      pthread_mutex_t thread_mutex;
      int CreateDataFrame(int lenght ,unsigned short * data, char * body);
      void CalculateCHK(int l, char *d , char &C0, char &C1);
  };

} /* namespace container */

#endif /* IQANENLACE_H_ */
