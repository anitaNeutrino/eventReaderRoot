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

class TGraph;
class UsefulAnitaEvent;

//!  AnitaEventCalibrator -- The ANITA Event Calibrator
/*!
  Used to calibrate the raw ANITA data into voltage-time waveforms
  \ingroup rootclasses
*/
class AnitaEventCalibrator : public TObject
{
 public:
   
  AnitaEventCalibrator();
  ~AnitaEventCalibrator();

  //Instance generator
  static AnitaEventCalibrator*  Instance();
  
  
  /*! \brief Calibration Routine
   *         The routine that is used to calibrate the voltage time waveforms in a UsefulAnitaEvent
   *
   *  The routines to calibrate a RawAnitaEvent into an UsefulAnitaEvent. The exact type of calibration applied depends on the WaveCalType::WaveCalType_t selected.
   */
  int calibrateUsefulEvent(UsefulAnitaEvent *eventPtr,
			   WaveCalType::WaveCalType_t calType);


  static int getChanIndex(int surf, int chan)
     {return chan+(9*surf);}   ///<Generally useful function

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
  float fastClockPeakOffset[NUM_SURF][NUM_CHIP];
  float fancyClockJitterOffset[NUM_SURF][NUM_CHIP];

  //Calibration constants for first pass bin-by-bin
  float justBinByBin[NUM_SURF][NUM_CHIP][NUM_RCO][NUM_SAMP];
  float epsilonFromAbby[NUM_SURF][NUM_CHIP][NUM_RCO]; ///< Note the rco here is the rco which the event reports (ie. 1-->0 goes in [0] and 0-->1 goes in [1]
  float clockCrossCorr[NUM_SURF][NUM_CHIP];

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
  TF1 *fFakeTemp;

  TGraph *grCorClock[NUM_SURF-1];

  int justBinByBinTimebase(UsefulAnitaEvent *eventPtr);
  void processEventRG(UsefulAnitaEvent *eventPtr); ///< Worker function for the RG (defunct) calibrations
  void processEventJW(UsefulAnitaEvent *eventPtr); ///< Worker function for the JW calibrations
  void zeroMean(); ///< Worker function for zero meaning the waveform
  void processClockJitter(); ///< Worker function for applying the inter-SURF clock based trigger jitter calibration
  void processClockJitterFast(); ///< Worker function for applying the inter-SURF clock based trigger jitter calibration -- fast method
  void processClockJitterCorrelation(); ///< Worker function for applying the inter-SURF clock based trigger jitter calibration -- using cross-correlation
  void processEventAG(UsefulAnitaEvent *eventPtr);
  

  Int_t getClockUpSampleFactor() 
  { return fClockUpSampleFactor;} ///< Returns the factor by which the clock is upsampled in the correlation calibration.
  void setClockUpSampleFactor(Int_t factor)
  { fClockUpSampleFactor=factor;} ///< Sets the factor by which the clock is upsampled in the correlation calibration.
  
  Double_t getEpsilonTempScale() 
  { return fEpsilonTempScale;} ///< Returns the factor by which we multiply the temperature scale for the epsilon part
  void setEpsilonTempScale(Double_t scale)
  { fEpsilonTempScale=scale;} ///< Sets the factor by which we multiply the temperature scale for the epsilon part


 protected:
   static AnitaEventCalibrator *fgInstance;  
   // protect against multiple instances

 private:
  void loadCalib();
  float Get_Interpolation_X(float x1, float y1, float x2, float y2, float y);
  void correlateTenClocks(TGraph *grClock[NUM_SURF], Double_t deltaT);
  Int_t fClockUpSampleFactor;
  Double_t fEpsilonTempScale;

  ClassDef(AnitaEventCalibrator,1);
  
};


#endif //ANITAEVENTCALIBRATOR_H
