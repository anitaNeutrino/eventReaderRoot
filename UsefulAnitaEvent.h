//////////////////////////////////////////////////////////////////////////////
/////  UsefulAnitaEvent.h        Useful ANITA event class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ANITA events in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef USEFULANITAEVENT_H
#define USEFULANITAEVENT_H

//Includes
#include <TObject.h>
#include "RawAnitaEvent.h"
#include "AnitaConventions.h"

class TGraph;
class AnitaEventCalibrator;
class PrettyAnitaHk;
class RawAnitaHeader;

//!  UsefulAnitaEvent -- The Calibrated Useful Anita Event object
/*!
  The ROOT implementation of the calibrated useful anita event
  \ingroup rootclasses
*/
class UsefulAnitaEvent: public RawAnitaEvent
{

 public:
   UsefulAnitaEvent(); ///< Default constructor
   UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, PrettyAnitaHk *theHk=0); ///< Assignment constructor using PrettyAnitaHk for the temperature calibration. See WaveCalType for calibration options
   UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, RawAnitaHeader *theHd); ///< Assignment constructor using RawAnitaHeader for the c3p0Num to do the temperature calibration using the 125MHz clock. See WaveCalType for calibration options
   UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, Double_t surfTemp); ///< Assignment constructor using surfTemp for the temperature calibration.  See WaveCalType for calibration options
   ~UsefulAnitaEvent(); ///< Destructor
   int calibrateEvent(WaveCalType::WaveCalType_t calType); ///< Work horse member function called by constructor

  //Generally useful function
  static int getChanIndex(int surf, int chan)
     {return chan+(CHANNELS_PER_SURF*surf);} ///< Utility fucntion


  TGraph *getGraphFromSurfAndChan(int surf, int chan); ///< Returns a voltage-time waveform for given SURF and channel
  TGraph *getGraph(int chanIndex); ///< Returns a voltage-time waveform for given channel index
  TGraph *getGraph(int ant, AnitaPol::AnitaPol_t pol); ///< Returns a voltage time waveform for given ant-pol
  TGraph *getGraph(AnitaRing::AnitaRing_t ring,
		   int phi,
		   AnitaPol::AnitaPol_t pol); ///< Returns a voltage-time waveform for given ring-phi-pol

  Int_t guessRco(int chanIndex); ///< Looks at clock channel to try and guess which RCO phase we are in.
  Double_t getTempCorrectionFactor(); ///< Returns the temperature correction factor (or guesses at it using clock periods if necessary)
  void analyseClocksForGuesses();


  //The calibrated numbers
  UInt_t fC3poNum; ///< Used to monitor the clock period.
  Int_t gotCalibTemp; ///< Flag to check if we are using the temperature calibration
  Double_t calibTemp; ///< Value for the temperature calibration
  WaveCalType::WaveCalType_t fCalType; ///< The calibration type requested
  int fNumPoints[NUM_DIGITZED_CHANNELS]; ///< Number of poins per channel
  double fVolts[NUM_DIGITZED_CHANNELS][NUM_SAMP]; ///< Array of unwrapped (unless kNoCalib) voltages for each channel
  double fTimes[NUM_DIGITZED_CHANNELS][NUM_SAMP]; ///< Array of unwrapped (unless kNoCalib) times for each channel
  int fCapacitorNum[NUM_DIGITZED_CHANNELS][NUM_SAMP]; ///< Array of capacitor numbers
  UInt_t fLastEventGuessed; ///< Internal variable to see if we've already tried to guess the RCO and temp correction factors for this event
  Int_t fRcoArray[NUM_SURF];   ///< An array to store the guessed at RCO values;
  Double_t fTempFactorGuess;  ///< A holder variable to cling on to the temperature correction factor that we are guessing at.


  AnitaEventCalibrator *fCalibrator; ///< Pointer to the AnitaEventCalibrator

  ClassDef(UsefulAnitaEvent,2);
};


#endif //USEFULANITAEVENT_H
