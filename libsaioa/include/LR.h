/*
 * LR.h
 *
 *  Created on: Jul 1, 2014
 *      Author: borja
 *  Class to calculate the linear regression for certain set of points
 */

#ifndef LR_H_
#define LR_H_

#include 	<string>
#include	<vector>
#include <math.h>


#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/NDC.hh"
#include "log4cpp/PropertyConfigurator.hh"

using namespace std;

namespace container {

struct Point
{
   double x;
   double y;
};


class LR {
  public:
    static bool initialized ;
    static LR * lr;
    static LR* getInstance();
    virtual ~LR();
    void clear();
    void addPoint(float x , float y);
    int  evaluate(float &slope,float &avgY);
  private:
    double slope  ;    //slope of regression line
    double y_intercept ; //y intercept of regression line
    double SUM_Yres ; //sum of squared of the discrepancies
    double AVGy ;     //mean of y
    double AVGx ;     //mean of x
    double Yres ;     //squared of the discrepancies
    double Rsqr ;     //coefficient of determination
    double Serr ;     // Standard error
    double Sdev ;     // Standard deviation
    float leastSqrRegression(struct Point* xyCollection, int dataSize);
    struct Point*  prepareData (int &dataSize);
    LR();
    vector<Point> data;
  };

} /* namespace container */
#endif /* LR_H_ */
