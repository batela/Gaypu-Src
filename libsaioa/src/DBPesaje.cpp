/*
 * DBPesaje.cpp
 *
 *  Created on: Dec 8, 2014
 *      Author: root
 */

#include "../include/DBPesaje.h"

namespace container {
  extern log4cpp::Category &log ;
  DBPesaje::DBPesaje(string path) : DB(path) {

  }

  DBPesaje::~DBPesaje() {
  }

  int DBPesaje::ReadHistoricData (string startdate, string enddate,string &data){

    log.info("%s-%s: %s", __FILE__,__FUNCTION__, "Starting function..") ;
    pthread_mutex_lock( &thread_mutex );

    bool res = false ;
    string sql ="SELECT idCanal,valor,fecha,codigo FROM thistorico WHERE fecha>='" +startdate + "' and fecha<='" + enddate +"';";
    log.debug("%s-%s: %s %s", __FILE__,__FUNCTION__, "Executing query:" , sql.data()) ;

    int idCanal;
    double valor;
    string fecha;
    string codigo;
    sqlite3_stmt *statement;
    if ( sqlite3_prepare(db,sql.c_str(),-1,&statement,0) == SQLITE_OK ) {
      int ctotal = sqlite3_column_count(statement);
      int res = SQLITE_ROW;
      while ( res != SQLITE_DONE && res!=SQLITE_ERROR )  {
        res = sqlite3_step(statement);
        if ( res == SQLITE_ROW ){
          for ( int i = 0; i < ctotal; i++ ) {
            switch (i){
              case 0: idCanal =((int)sqlite3_column_int(statement, i));break;
              case 1:	valor 	= ((double)sqlite3_column_double(statement, i)); break;
              case 2:	fecha 	=((char*)sqlite3_column_text(statement, i)); break;
              case 3 : codigo =((char*)sqlite3_column_text(statement, i)); break;
            }
          }
          char raw[256];
          sprintf(raw,"%d;%.0f;%s;%s\n",idCanal,valor,fecha.data(),codigo.data());
          data = data + raw;
        }
      }
      sqlite3_finalize(statement);
    }
    if (data.compare("")== 0) data = "NO DATA\n";
    log.info("%s-%s: %s", __FILE__,__FUNCTION__, "Ending function..") ;

    pthread_mutex_unlock( &thread_mutex );
    return res ;
  }

  int DBPesaje::ReadMaxDayData (string startdate, string enddate, string count, string &data){

    log.info("%s-%s: %s", __FILE__,__FUNCTION__, "Starting function..") ;
    pthread_mutex_lock( &thread_mutex );

    bool res = false ;
    string sql ="SELECT idCanal,valor,fecha,codigo FROM thistorico WHERE fecha>='" +startdate + " 00:00:00' and fecha<='" + enddate +" 23:59:59'" +
        + "ORDER BY valor DESC LIMIT " + count ;
    log.debug("%s-%s: %s %s", __FILE__,__FUNCTION__, "Executing query:" , sql.data()) ;

    int idCanal;
    double valor = 0 ;
    string fecha;
    string codigo;
    sqlite3_stmt *statement;
    if ( sqlite3_prepare(db,sql.c_str(),-1,&statement,0) == SQLITE_OK ) {
      int ctotal = sqlite3_column_count(statement);
      int res = SQLITE_ROW;
      while ( res != SQLITE_DONE && res!=SQLITE_ERROR )  {
        res = sqlite3_step(statement);
        if ( res == SQLITE_ROW ){
          for ( int i = 0; i < ctotal; i++ ) {
            switch (i){
              case 0: idCanal =((int)sqlite3_column_int(statement, i));break;
              case 1:	valor 	= ((double)sqlite3_column_double(statement, i)); break;
              case 2:	fecha 	=((char*)sqlite3_column_text(statement, i)); break;
              case 3: codigo  =((char*)sqlite3_column_text(statement, i)); break;
            }
          }
          char raw[256];
          sprintf(raw,"%d;%.0f;%s;%s\n",idCanal,valor,fecha.data(),codigo.data());
          data = data + raw;
        }
      }
      sqlite3_finalize(statement);
    }
    if (data.compare("")== 0) data = "NO DATA\n";
    log.info("%s-%s: %s", __FILE__,__FUNCTION__, "Ending function..") ;

    pthread_mutex_unlock( &thread_mutex );
    return res ;
  }

  int DBPesaje::ReadLastTenData (string &data){

    log.info("%s-%s: %s", __FILE__,__FUNCTION__, "Starting function..") ;
    pthread_mutex_lock( &thread_mutex );

    int isError = 1;
    string sql ="SELECT idCanal,valor,fecha,codigo FROM thistorico ORDER BY fecha DESC LIMIT 10;";
    log.debug("%s-%s: %s %s", __FILE__,__FUNCTION__, "Executing query:" , sql.data()) ;

    int idCanal;
    double valor = 0 ;
    string fecha;
    string codigo;
    sqlite3_stmt *statement;
    if ( sqlite3_prepare(db,sql.c_str(),-1,&statement,0) == SQLITE_OK ) {
      int ctotal = sqlite3_column_count(statement);
      int res = SQLITE_ROW;
      while ( res != SQLITE_DONE && res!=SQLITE_ERROR )  {
        res = sqlite3_step(statement);
        if ( res == SQLITE_ROW ){
          for ( int i = 0; i < ctotal; i++ ) {
            switch (i){
              case 0: idCanal =((int)sqlite3_column_int(statement, i));break;
              case 1:	valor 	= ((double)sqlite3_column_double(statement, i)); break;
              case 2:	fecha 	=((char*)sqlite3_column_text(statement, i)); break;
              case 3: codigo   =((char*)sqlite3_column_text(statement, i)); break;
            }
          }
          char raw[256];
          sprintf(raw,"%d;%.0f;%s;%s\n",idCanal,valor,fecha.data(),codigo.data());
          data = data + raw;
        }
      }
      isError = 0;
      sqlite3_finalize(statement);
    }
    if (data.compare("")== 0) data = "NO DATA\n";
    log.info("%s-%s: %s", __FILE__,__FUNCTION__, "Ending function..") ;

    pthread_mutex_unlock( &thread_mutex );
    return isError ;
  }

  int DBPesaje::InsertData (int canal,float valor){

    log.info("%s-%s: %s", __FILE__,__FUNCTION__, "Starting function..") ;
    pthread_mutex_lock( &thread_mutex );

    bool res = false ;
    time_t rawtime;
    struct tm * timeinfo;
    char now [50];
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    strftime (now,50,"%F %T",timeinfo);
    char query[100];

    sprintf (query,"UPDATE tvalores SET valor=%d,fecha='%s',codigo ='------' WHERE idCanal=%d",(int)valor,now,canal);
    //string sql ="UPDATE tvalores SET valor=" +buffer+",fecha='" + now +"' WHERE idCanal=" + canal;
    log.debug("%s-%s: %s %s", __FILE__,__FUNCTION__, "Executing query:" , query) ;

    sqlite3_stmt *statement;
    bool done = false ;
    int contador = 0;
    while ((done == false) && (contador++ < 3)){
      if ( sqlite3_prepare(db,query,-1,&statement,0) == SQLITE_OK ) {
        res = sqlite3_step(statement);
        sqlite3_finalize(statement);
        done = true;
      }
      else {
        log.error("%s-%s: %s", __FILE__,__FUNCTION__, "Error al ejecutar query en tvalores") ;
        usleep (500 * 1000);
      }
    }

    log.info("%s-%s: %s", __FILE__,__FUNCTION__, "Ending function..") ;
    pthread_mutex_unlock( &thread_mutex );
    return res ;
  }


  int DBPesaje::InsertNewCodigoPesaje (string data){

    log.info("%s-%s: %s", __FILE__,__FUNCTION__, "Starting function..") ;
    pthread_mutex_lock( &thread_mutex );

    bool res = false ;
    char query[100];

    sprintf (query,"UPDATE thistorico SET codigo='%s' WHERE id=(SELECT max(id) FROM thistorico)",data.c_str());
    log.debug("%s-%s: %s %s", __FILE__,__FUNCTION__, "Executing query:" , query) ;

    sqlite3_stmt *statement;
    bool done = false ;
    int contador = 0;
    while ((done == false) && (contador++ < 3)){
      if ( sqlite3_prepare(db,query,-1,&statement,0) == SQLITE_OK ) {
        res = sqlite3_step(statement);
        sqlite3_finalize(statement);
        done = true;
      }
      else {
        log.error("%s-%s: %s", __FILE__,__FUNCTION__, "Error al ejecutar query thistorico") ;
        usleep (500 * 1000);
      }
    }
    log.info("%s-%s: %s", __FILE__,__FUNCTION__, "Ending function..") ;
    pthread_mutex_unlock( &thread_mutex );
    return res ;
  }

} /* namespace container */
