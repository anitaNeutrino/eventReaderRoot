//////////////////////////////////////////////////////////////////////////////
/////  AnitaEventCalibrator.h        ANITA Event Calibrator              /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for calibrating UsefulAnitaEvents               /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ANITAEVENTCALIBRATOR_H
#define ANITAEVENTCALIBRATOR_H

// ROOT 
#include <TObject.h>
#include <TF1.h>
#include <TH1.h>
#include <TMath.h>
#include <TGraph.h>

#ifdef USE_FFT_TOOLS
#include "FFTtools.h"
#endif

// std lib
#include <fstream>
#include <map>
#include <bitset>

// Anita
#include "AnitaConventions.h"
#include "simpleStructs.h"
#include "AnitaClock.h"
#include "AnitaGeomTool.h"
#include "RingBuffer.h"
#include "RawAnitaHeader.h"

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

  static AnitaEventCalibrator* Instance(); ///< Instance generator

  /*! \brief Calibration Routine
   *         The routine that is used to calibrate the voltage time waveforms in a UsefulAnitaEvent
   *
   *  The routines to calibrate a RawAnitaEvent into an UsefulAnitaEvent. The exact type of calibration applied depends on the WaveCalType::WaveCalType_t selected.
   */

  
  Int_t calibrateUsefulEvent(UsefulAnitaEvent *eventPtr, 
			     WaveCalType::WaveCalType_t calType);///< Workhorse

  void guessRco(UsefulAnitaEvent* eventPtr); ///< Guess RCO from clock
  void getTempFactors(UsefulAnitaEvent* eventPtr); ///< Interface to RingBuffer of clock periods for temperature correction
  void updateTemperatureCorrection(UsefulAnitaEvent* eventPtr); ///< Update RingBuffer for this event
  Int_t unwrapChannel(UsefulAnitaEvent* eventPtr, Int_t surf, Int_t chan, Int_t rco, 
		      Bool_t fApplyTempCorrection, Bool_t fAddPedestal, 
		      Double_t* voltsArray, Double_t* timeArray, Int_t* scaArray);
  void applyVoltageCalibration(UsefulAnitaEvent* eventPtr);
  void keepOnlySomeTimeAfterClockUptick(TGraph* grClock, Double_t deltaClockKeepNs);

  std::vector<Double_t> getClockAlignment(UsefulAnitaEvent* eventPtr,
					  Int_t numPoints[NUM_SURF],
					  Double_t volts[NUM_SURF][NUM_CHAN][NUM_SAMP],
					  Double_t times[NUM_SURF][NUM_SAMP]);
  std::vector<Double_t> getClockAlignment(UsefulAnitaEvent* eventPtr,
					  Int_t numPoints[NUM_SURF],
					  Double_t volts[NUM_SURF][NUM_CHAN][NUM_SAMP],
					  Double_t times[NUM_SURF][NUM_SAMP],
					  std::vector<Int_t> listOfClockNums);
  void deleteClockAlignmentTGraphs();
  void zeroMeanNonClockChannels();

  Double_t getTimeOfZeroCrossing(Double_t x1, Double_t y1, Double_t x2, Double_t y2);
  Int_t getTimeOfUpwardsClockTicksCrossingZero(Int_t numPoints, Int_t surf, Double_t* times, 
					       Double_t* volts, 
					       std::vector<Double_t>& timeZeroCrossings, 
					       std::vector<Int_t>& sampZeroCrossings, 
					       bool raiseFlagIfClocksAreWeird);

  void findExtremaSamples(Int_t length, Double_t* volts,
			  std::vector<Int_t>& maximaSamps, 
			  std::vector<Int_t>& minimaSamps);


  void initializeVectors();
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  ///// Member variables
  //////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Primary event data: voltage and time arrays
  Double_t voltsArray[NUM_SURF][CHANNELS_PER_SURF][NUM_SAMP]; ///< Channel volts in mV
  Double_t timeArray[NUM_SURF][NUM_SAMP]; ///< Channel times in ns 
  Int_t numPointsArray[NUM_SURF]; ///< Number of samples in each channel
  Int_t scaArray[NUM_SURF][NUM_SAMP]; ///< Capacitor numbers for each sample

  // Secondary event data
  std::vector<std::vector<RingBuffer> > clockPeriodRingBuffers; ///< Holds rolling average of temperature correction
  // RingBuffer* clockPeriodRingBuffer; ///< Holds rolling average of temperature correction
  std::vector<Double_t> tempFactors; ///< Multiplicative factor for deltaTs & epsilons accounting for temperature


  // Int_t rcoArray[NUM_SURF]; ///< The output of AnitaEvenCalibrator::guessRco() goes here.
  // Double_t measuredClockPeriods[NUM_SURF][NUM_RCO][AnitaClock::maxNumZcs]; ///< For guessRco and getTempFactor
  std::vector<Int_t> rcoVector; ///< The output of AnitaEventCalibrator::guessRco() goes here.
  std::vector<std::vector<std::vector<Double_t> > > measuredClockPeriods;

  // Constants for timing/voltage calibration
  Double_t relativeChannelDelays[NUM_SURF][NUM_CHAN]; ///< Cable + other delays
  Double_t deltaTs[NUM_SURF][NUM_CHIP][NUM_RCO][NUM_SAMP]; ///< Calib constants: The time for the write pointer to travel between successove capacitors.
  Double_t epsilons[NUM_SURF][NUM_CHIP][NUM_RCO]; ///< Calib constants: time (ns) for write pointer to go from sample 255 -> 0 (in ANITA-2 and ANITA-3 with the LAB-3 digitizers)
  Double_t mvCalibVals[NUM_SURF][NUM_CHAN][NUM_CHIP]; ///< Calib constants: converts from ADC counts to millivolts

  // Constants for calibration algorithms
  Double_t clockKeepTime[NUM_SURF][NUM_CHIP]; ///< How much time after clock up-tick to keep, calib food
  Int_t firstHitBusRcoLatchLimit;


  std::vector<Int_t> defaultClocksToAlign; ///< List of SURFs for getClockAlignment() (for calibration progs)
  std::vector<TGraph*> grClockInterps; ///< Interpolated clocks
  std::map<Double_t, TGraph*> grClock0s; ///< Need to process clock 0 depending on needs of other SURF
  std::vector<TGraph*> grCorClock;
  std::vector<TGraph*> grClocks;


  std::vector<Double_t> clockAlignment;
  Double_t dtInterp; ///< Interpolating clock for alignment step
  Double_t nominalDeltaT; ///< If we don't want bin-to-bin deltaTs
  Int_t fClockProblem; ///< Flag raised if more than 4 or less than 2 upgoing zero crossings in a SURF clock


  //Ben Rotter's rfPower calibration :)
  Double_t convertRfPowTodBm(int surf, int chan, int adc);
  Double_t convertRfPowToKelvin(int surf, int chan, int adc);

  Double_t RfPowYInt[NUM_SURF][NUM_CHAN];
  Double_t RfPowSlope[NUM_SURF][NUM_CHAN];







  


 protected:
  static AnitaEventCalibrator *fgInstance; ///< Just one instance

 private:
  void loadCalib(); ///< Reads calibration constants from text files into arrays
  PedestalStruct_t fPedStruct; ///< For adding pedestals back onto voltage samples
  ClassDef(AnitaEventCalibrator,0);
  
};


#endif //ANITAEVENTCALIBRATOR_H
