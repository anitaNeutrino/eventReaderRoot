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

#include <map>

class TGraph;
class AnitaEventCalibrator;
class PrettyAnitaHk;
class RawAnitaHeader;
class CalibratedAnitaEvent;

//!  UsefulAnitaEvent -- The Calibrated Useful Anita Event object
/*!
  The ROOT implementation of the calibrated useful anita event
  \ingroup rootclasses
*/
class UsefulAnitaEvent: public RawAnitaEvent
{

 public:
  UsefulAnitaEvent(); ///< Default constructor
  UsefulAnitaEvent(CalibratedAnitaEvent *calibratedPtr, WaveCalType::WaveCalType_t calType=WaveCalType::kDefault); ///< Assignment constructor from a CalibratedAnitaEvent
  //  UsefulAnitaEvent(CalibratedAnitaEvent *calibratedPtr, WaveCalType::WaveCalType_t calType, RawAnitaHeader* theHd); ///< Assignment constructor from a CalibratedAnitaEvent
  UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, PrettyAnitaHk *theHk=0); ///< Assignment constructor using PrettyAnitaHk for the temperature calibration. See WaveCalType for calibration options
  UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, RawAnitaHeader *theHd); ///< Assignment constructor using RawAnitaHeader for the c3p0Num to do the temperature calibration using the 125MHz clock. See WaveCalType for calibration options
  UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, Double_t surfTemp); ///< Assignment constructor using surfTemp for the temperature calibration.  See WaveCalType for calibration options
  virtual ~UsefulAnitaEvent(); ///< Destructor
  int calibrateEvent(WaveCalType::WaveCalType_t calType); ///< Work horse member function called by constructor

  //Generally useful function
  static int getChanIndex(int surf, int chan)
     {return chan+(CHANNELS_PER_SURF*surf);} ///< Utility function


  TGraph *getGraphFromSurfAndChan(int surf, int chan) const; ///< Returns a voltage-time waveform for given SURF and channel
  TGraph *getGraph(int chanIndex) const; ///< Returns a voltage-time waveform for given channel index
  TGraph *getGraph(int ant, AnitaPol::AnitaPol_t pol) const; ///< Returns a voltage time waveform for given ant-pol
  TGraph *getGraph(AnitaRing::AnitaRing_t ring,
		   int phi,
		   AnitaPol::AnitaPol_t pol) const; ///< Returns a voltage-time waveform for given ring-phi-pol
  TGraph *getDeconvolvedALFA(); //
  Int_t guessRco(int chanIndex); ///< Looks at clock channel to try and guess which RCO phase we are in.
  Int_t getRcoCorrected(int chanIndex); ///< Returns firmware RCO after correcting for latch delay (and factor of -1 for different definitions of which phase getRCO and guessRCO return)
  // Double_t getTempCorrectionFactor(); ///< Returns the temperature correction factor (or guesses at it using clock periods if necessary)
  // void analyseClocksForGuesses();
  // void analyseClocksForTempGuessBen();


  // std::vector<std::vector<Double_t> > getNeighbouringClockCorrelations(Double_t lowPassClockFilterFreq, Int_t giveGraphsToInputArrays, TGraph* grInterpClocksArray[NUM_SURF], TGraph* grCorClocksArray[NUM_SURF][2]);
  // std::vector<std::vector<Double_t> > getNeighbouringClockCorrelations2(bool writeGraphs);

  std::vector<Double_t> getClockAlignment(std::vector<Int_t> listOfClockNums); ///< For calibration
  Int_t checkIfTreatingCalibratedEventAsRawEvent(RawAnitaEvent* rawEventPtr, const char* funcName); ///< Prints a verbose error message if you're calling the wrong constructor type



  //The calibrated numbers
  Int_t fFromCalibratedAnitaEvent; ///< Flag used in determining whether the event came from a CalibratedAnitaEvent
  UInt_t fC3poNum; ///< Used to monitor the clock period.
  Int_t gotCalibTemp; ///< Flag to check if we are using the temperature calibration
  Double_t calibTemp; ///< Value for the temperature calibration
  WaveCalType::WaveCalType_t fCalType; ///< The calibration type requested
  int fNumPoints[NUM_DIGITZED_CHANNELS]; ///< Number of poins per channel
  double fVolts[NUM_DIGITZED_CHANNELS][NUM_SAMP]; ///< Array of unwrapped (unless kNoCalib) voltages for each channel
  double fTimes[NUM_DIGITZED_CHANNELS][NUM_SAMP]; ///< Array of unwrapped (unless kNoCalib) times for each channel
  int fCapacitorNum[NUM_DIGITZED_CHANNELS][NUM_SAMP]; ///< Array of capacitor numbers
  UInt_t fLastEventGuessed; ///< Internal variable to see if we've already tried to guess the RCO and temp correction factors for this event
  Int_t fRcoArray[NUM_SURF]; ///< An array to store the guessed at RCO values
  Double_t fTempFactorGuesses[NUM_SURF]; ///< A holder variable to cling on to the temperature correction factor that we are guessing at
  Int_t fClockProblem; ///< Flag raised if more than 4 upgoing zero crossings in clock, won't update temp correction. RCO guessing may also be negatively affected by this.
  Int_t fClockSpike; ///<Flag raised if the ADC value is too large or small in clock. 
  Int_t fRFSpike; ///<Flag raised if the ADC value is too large or small in RF. 
  Double_t fClockPhiArray[NUM_SURF]; ///< An array to store the derived clock calibration numbers (from aligning the clocks)
  std::vector<Int_t> SpikeyRFChannelList; /// a list to store the spikey RF channel.

  Bool_t getAlfaFilterFlag();
  Bool_t setAlfaFilterFlag(Bool_t newBoolianFlag);

 private:
  Bool_t fFilterAlfaChannel;

  ClassDef(UsefulAnitaEvent,7);
};


#endif //USEFULANITAEVENT_H
