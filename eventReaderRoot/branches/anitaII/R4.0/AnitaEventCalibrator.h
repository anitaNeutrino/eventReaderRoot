//////////////////////////////////////////////////////////////////////////////
/////  AnitaEventCalibrator.h        ANITA Event Calibrator              /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for calibrating UsefulAnitaEvents               /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ANITAEVENTCALIBRATOR_H
#define ANITAEVENTCALIBRATOR_H

//Includes
#include <TObject.h>
#include <TF1.h>
#include <TH1.h>
#include "AnitaConventions.h"


class UsefulAnitaEvent;

class AnitaEventCalibrator : public TObject
{
 public:
   
  AnitaEventCalibrator();
  ~AnitaEventCalibrator();

  //Instance generator
  static AnitaEventCalibrator*  Instance();
  
  //Calibration Routine
  int calibrateUsefulEvent(UsefulAnitaEvent *eventPtr,
			   WaveCalType::WaveCalType_t calType);

  //Generally useful function
  static int getChanIndex(int surf, int chan)
     {return chan+(9*surf);}

  //Some flags that do things
  Int_t fApplyClockFudge; //Should we use my silly clock fudge factor
  


  //Variables for RG Calib
  float mvCalibVals[NUM_SURF][NUM_CHAN][NUM_CHIP];
  float timeBaseCalib[NUM_SURF][NUM_CHIP][NUM_RCO];
  int rcoLatchCalib[NUM_SURF][NUM_CHIP];
  float epsilonCalib[NUM_SURF][NUM_CHIP][NUM_RCO]; //Note the rco is the end rco

  //Variables Jiwoo Calib
  float tcalTBin[NUM_SURF][NUM_CHIP][NUM_RCO][NUM_SAMP];
  float tcalEpsilon[NUM_SURF][NUM_CHIP][NUM_RCO];
  float tcalRcoDelayBin[NUM_SURF][NUM_CHIP][NUM_RCO];
  float tcalFudgeFactor[NUM_SURF][NUM_CHIP][NUM_RCO];
  
  //Variables for clock-based trigger jitter correction
  float clockJitterOffset[NUM_SURF][NUM_CHIP];
  float fastClockJitterOffset[NUM_SURF][NUM_CHIP];
  float fancyClockJitterOffset[NUM_SURF][NUM_CHIP];

  //Cable Length Calib
  float groupDelayCalib[NUM_SURF][NUM_CHAN];

  //Temporary variables that don't really need to be part of the class
  int rawArray[NUM_SURF][NUM_CHAN][NUM_SAMP];
  int fLabChip[NUM_SURF][NUM_CHAN];
  int rcobit[NUM_SURF][NUM_CHAN][NUM_SAMP]; 
  int scaArray[NUM_SURF][NUM_CHAN][NUM_SAMP]; 
  int unwrappedArray[NUM_SURF][NUM_CHAN][NUM_SAMP];
  double surfTimeArray[NUM_SURF][NUM_SAMP];
  double mvArray[NUM_SURF][NUM_CHAN][NUM_SAMP];
  int numPointsArray[NUM_SURF][NUM_CHAN];

  double clockPhiArray[NUM_SURF];
  double timeArray[NUM_SURF][NUM_CHAN][NUM_SAMP]; 

  TF1 *fSquareWave;

  void processEventRG(UsefulAnitaEvent *eventPtr);
  void processEventJW(UsefulAnitaEvent *eventPtr,float temp);
  void zeroMean();
  void processClockJitter();
  void processClockJitterFast();
  void processClockJitterCorrelation();

 protected:
   static AnitaEventCalibrator *fgInstance;  
   // protect against multiple instances

 private:
  void loadCalib();
  float Get_Interpolation_X(float x1, float y1, float x2, float y2, float y);
   
  

  ClassDef(AnitaEventCalibrator,1);
  
};


#endif //ANITAEVENTCALIBRATOR_H
