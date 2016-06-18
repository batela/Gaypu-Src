/*
 * Env.cpp
 *
 *  Created on: Jul 1, 2014
 *      Author: borja
 */

#include "../include/LR.h"


namespace container {
  extern log4cpp::Category &log   ;

  bool LR::initialized= false ;
  LR* LR::lr = NULL;

  LR::LR() {
    this->clear();
  }

  LR::~LR() {
    // TODO Auto-generated destructor stub
  }

  LR * LR::getInstance(){
    if (!initialized){
      lr =new LR();
      initialized = true;
    }
    return lr;
  }

  void LR::clear(){
    slope = 0;    //slope of regression line
    y_intercept = 0; //y intercept of regression line
    SUM_Yres = 0; //sum of squared of the discrepancies
    AVGy = 0;     //mean of y
    AVGx = 0;     //mean of x
    Yres = 0;     //squared of the discrepancies
    Rsqr = 0;     //coefficient of determination
    Serr = 0;     // Standard error
    Sdev = 0;     // Standard deviation
    data.clear();
  }

  /**
   *
   */
  void LR::addPoint(float x , float y){
    Point p;
    p.x = x;
    p.y = y;
    data.push_back(p);
  }

  /**
   * return 1 if OK, return 0 if error
   */
  int  LR::evaluate(float &slope,float &avgY){
    int res = 0;
    int items;
    struct Point *data = prepareData (items);
    if (items > 2 && data != NULL){
      slope = leastSqrRegression(data, items);
      avgY = this->AVGy;
      res = 1;
    }
    delete data;
    return res;
  }
  /**
   * Function that transforms vector values into array and get the size of the array
   */
  struct Point* LR::prepareData (int &dataSize){
    dataSize = data.size();
    struct Point* d = new struct Point [dataSize];
    int idx = 0;
    for (std::vector<Point>::iterator it = data.begin() ; it != data.end(); ++it,idx++){
      d[idx].x = ((Point)*it).x;
      d[idx].y = ((Point)*it).y;
    }
    return d;
  }

  /**
   * return 0 if error, return 1 if ok
   */
  float LR::leastSqrRegression(struct Point* xyCollection, int dataSize) {

     double SUMx = 0;     //sum of x values
     double SUMy = 0;     //sum of y values
     double SUMxy = 0;    //sum of x * y
     double SUMxx = 0;    //sum of x^2
     double SUMres = 0;   //sum of squared residue
     double res = 0;      //residue squared

     //calculate various sums
     for (int i = 0; i < dataSize; i++){
        SUMx = SUMx + (xyCollection + i)->x; //sum of x
        SUMy = SUMy + (xyCollection + i)->y; //sum of y
        SUMxy = SUMxy + (xyCollection + i)->x * (xyCollection + i)->y; //sum of squared x*y
        SUMxx = SUMxx + (xyCollection + i)->x * (xyCollection + i)->x; //sum of squared x
     }

     AVGy = SUMy / dataSize; //calculate the means of x and y
     AVGx = SUMx / dataSize;

     slope = (dataSize * SUMxy - SUMx * SUMy) / (dataSize * SUMxx - SUMx*SUMx);   //slope or a1
     y_intercept = AVGy - slope * AVGx;                                           //y itercept or a0


     //calculate squared residues, their sum etc.
     for (int i = 0; i < dataSize; i++) {
        Yres = pow(((xyCollection + i)->y - y_intercept - (slope * (xyCollection + i)->x)), 2); //current (y_i - a0 - a1 * x_i)^2
        SUM_Yres += Yres;                           //sum of (y_i - a0 - a1 * x_i)^2
        res = pow((xyCollection + i)->y - AVGy, 2); //current residue squared (y_i - AVGy)^2
        SUMres += res;                              //sum of squared residues

//        log.debug("%s: %s ",__FILE__,"   (%0.2f %0.2f)      %0.5E         %0.5E\n",
//         (xyCollection + i)->x, (xyCollection + i)->y, res, Yres);
     }
     //calculate r^2 coefficient of determination
     Rsqr = (SUMres - SUM_Yres) / SUMres;
     Serr = sqrt(SUM_Yres / (dataSize-2));
     Sdev = sqrt(SUMres / (dataSize - 1));

//     log.debug("%s: %s ",__FILE__,"x mean(AVGx) = %0.5E\n", AVGx);
//     log.debug("%s: %s ",__FILE__,"y mean(AVGy) = %0.5E\n", AVGy);
//     log.debug("%s: %s ",__FILE__,"The linear equation that best fits the given data");
//     log.debug("%s: %s ",__FILE__,"       y = %2.8lfx + %2.8f\n", slope, y_intercept);
//     log.debug("%s: %s ",__FILE__,"------------------------------------------------------------\n");
//     log.debug("%s: %s ",__FILE__,"   Original (x,y)   (y_i - y_avg)^2     (y_i - a_o - a_1*x_i)^2\n");
//     printf("Sum of (y_i - y_avg)^2 = %0.5E\t\n", SUMres);
//     printf("Sum of (y_i - a_o - a_1*x_i)^2 = %0.5E\t\n", SUM_Yres);
//     printf("Standard deviation(St) = %0.5E\n", sqrt(SUMres / (dataSize - 1)));
//     printf("Standard error of the estimate(Sr) = %0.5E\t\n", sqrt(SUM_Yres / (dataSize-2)));
//     printf("Coefficent of determination(r^2) = %0.5E\t\n", (SUMres - SUM_Yres)/SUMres);
//     printf("Correlation coefficient(r) = %0.5E\t\n", sqrt(Rsqr));

     return slope;
  }

} /* namespace container */
