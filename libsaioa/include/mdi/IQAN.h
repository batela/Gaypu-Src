/*
 * IcopCanOpen.h
 *
 *  Created on: Jun 4, 2016
 *      Author: root
 */

#ifndef SRC_ICOPCANOPEN_H_
#define SRC_ICOPCANOPEN_H_
#include <pthread.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include "Env.h"

namespace container {

  class IcopCanOpen {

    public:
      static IcopCanOpen* Instance();
      void SendRecStandardCanFrame(int id, int lenght ,char * data, char * msj);
      int  CreateCanConfigurationFrame(int bauds , int T0, int F0, int M0, int T1, int F1, int M1, char * data);
    private:
      IcopCanOpen();
      IcopCanOpen(IcopCanOpen const&); // copy constructor is private
      virtual ~IcopCanOpen();
      static IcopCanOpen* m_pInstance;
      pthread_mutex_t thread_mutex;
      int CreateDataFrame(int lenght ,unsigned short * data, char * body);
      unsigned char CalculateCHK(int l, char *d , char &C0, char &C1);

  };

} /* namespace container */

#endif /* SRC_ICOPCANOPEN_H_ */
