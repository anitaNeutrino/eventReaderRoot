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
#include "simpleStructs.h"

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

  //Ben Rotter's rfPower calibration :)
  Double_t convertRfPowTodBm(int surf, int chan, int adc);
  Double_t convertRfPowToKelvin(int surf, int chan, int adc);


  //Some flags that do things
  Int_t fApplyClockFudge; //Should we use my silly clock fudge factor


  // Variables for BS calib 
  Int_t rcoLatchStart[NUM_SURF][NUM_CHIP];
  Int_t rcoLatchEnd[NUM_SURF][NUM_CHIP];
  Int_t fReadRcoFromFirmware;
#define NUM_EVENTS_TO_AVERAGE_TEMP_OVER 1000
  Double_t fAllSurfAverageClockDeltaTs[NUM_EVENTS_TO_AVERAGE_TEMP_OVER]; ///< For rolling average
  Int_t fAllSurfAverageClockNumDeltaTs[NUM_EVENTS_TO_AVERAGE_TEMP_OVER]; ///< For rolling average
  Int_t fTempEventInd; ///< Current index of rolling average arrays
  UInt_t fLastEventNumber; ///< If doing a rolling average makes sense to check we are being sequential
  Int_t fNumEventsAveraged;


  //Variables for RG Calib
  Double_t mvCalibVals[NUM_SURF][NUM_CHAN][NUM_CHIP];
  Double_t timeBaseCalib[NUM_SURF][NUM_CHIP][NUM_RCO];
  int rcoLatchCalib[NUM_SURF][NUM_CHIP];
  Double_t epsilonCalib[NUM_SURF][NUM_CHIP][NUM_RCO]; //Note the rco is the end rco

  
  //Variables for clock-based trigger jitter correction
  Double_t clockJitterOffset[NUM_SURF][NUM_CHIP];
  Double_t fastClockPeakOffset[NUM_SURF][NUM_CHIP];
  Double_t fancyClockJitterOffset[NUM_SURF][NUM_CHIP];

  //Calibration constants for first pass bin-by-bin
  Double_t justBinByBin[NUM_SURF][NUM_CHIP][NUM_RCO][NUM_SAMP];
  Double_t epsilonFromAbby[NUM_SURF][NUM_CHIP][NUM_RCO]; ///< Note the rco here is the rco which the event reports (ie. 1-->0 goes in [0] and 0-->1 goes in [1]
  Double_t clockCrossCorr[NUM_SURF][NUM_CHIP];
  Double_t chipByChipDeltats[NUM_SURF][NUM_CHAN][NUM_CHIP]; ///< Cable + chip-to-chip delays

  //Cable Length Calib
  Double_t groupDelayCalib[NUM_SURF][NUM_CHAN];

  //Simon's deltat values
  Double_t simonsDeltaT[NUM_SURF][NUM_CHAN];

  //Ben's RF power calibration  
  Double_t RfPowYInt[NUM_SURF][NUM_CHAN];
  Double_t RfPowSlope[NUM_SURF][NUM_CHAN];



  //Temporary variables that don't really need to be part of the class
  int rawArray[NUM_SURF][NUM_CHAN][NUM_SAMP];
  int fLabChip[NUM_SURF][NUM_CHAN];
  int rcobit[NUM_SURF][NUM_CHAN][NUM_SAMP]; 
  int scaArray[NUM_SURF][NUM_CHAN][NUM_SAMP]; 
  int unwrappedArray[NUM_SURF][NUM_CHAN][NUM_SAMP];
  Double_t surfTimeArray[NUM_SURF][NUM_SAMP];
  Double_t mvArray[NUM_SURF][NUM_CHAN][NUM_SAMP];
  int numPointsArray[NUM_SURF][NUM_CHAN];

  Double_t clockPhiArray[NUM_SURF];
  Double_t timeArray[NUM_SURF][NUM_CHAN][NUM_SAMP]; 



  TF1 *fSquareWave;
  TF1 *fFakeTemp;

  TGraph *grCorClock[NUM_SURF-1];

  int justBinByBinTimebase(UsefulAnitaEvent *eventPtr);
  void zeroMean(); ///< Worker function for zero meaning the waveform
  void processClockJitter(UsefulAnitaEvent *eventPtr); ///< Worker function for applying the inter-SURF clock based trigger jitter calibration
  void processClockJitterFast(UsefulAnitaEvent *eventPtr); ///< Worker function for applying the inter-SURF clock based trigger jitter calibration -- fast method
  void processClockJitterCorrelation(UsefulAnitaEvent *eventPtr); ///< Worker function for applying the inter-SURF clock based trigger jitter calibration -- using cross-correlation
  void applyClockPhiCorrection(UsefulAnitaEvent *eventPtr); ///< Worker fucntion if we are dealing with CalibratedAnitaEvent
  //void processEventAG(UsefulAnitaEvent *eventPtr);
  void processEventAG(UsefulAnitaEvent *eventPtr, Int_t fGetTempFactorInProcessEventAG, Int_t fUseRolloingTempAverage, Int_t fReadRcoFromFirmware);
  void processEventBS(UsefulAnitaEvent* eventPtr);

  void updateRollingAverageClockDeltaT(UsefulAnitaEvent* eventPtr, Double_t allSurfMeanUpDt, Int_t allSurfNumUpDt);


  void processEventUnwrapFast(UsefulAnitaEvent *eventPtr);
  

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
  Double_t Get_Interpolation_X(Double_t x1, Double_t y1, Double_t x2, Double_t y2, Double_t y);
  void correlateClocks(TGraph *grClock[NUM_SURF], Double_t deltaT);
  void addPedestals();
  Int_t fClockUpSampleFactor;
  Double_t fEpsilonTempScale;

  PedestalStruct_t fPedStruct;


  ClassDef(AnitaEventCalibrator,2);
  
};


#endif //ANITAEVENTCALIBRATOR_H
