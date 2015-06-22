//////////////////////////////////////////////////////////////////////////////
/////  CalibratedAnitaEvent.h        Calibrated ANITA event class        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing calibrated ANITA events in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef CALIBRATEDANITAEVENT_H
#define CALIBRATEDANITAEVENT_H

//Includes
#include <TObject.h>
#include "RawAnitaEvent.h"
#include "AnitaConventions.h"

class TGraph;
class AnitaEventCalibrator;
class UsefulAnitaEvent;
class PrettyAnitaHk;
class RawAnitaHeader;

//!  CalibratedAnitaEvent -- The Calibrated Calibrated Anita Event object
/*!
  The ROOT implementation of the calibrated anita event, this is just a CPU and disk space saving exercise
  \ingroup rootclasses
*/
class CalibratedAnitaEvent: public RawAnitaEvent
{

 public:
   CalibratedAnitaEvent(); ///< Default constructor not used   
   CalibratedAnitaEvent(UsefulAnitaEvent *usefulPtr); ///< Assignment constructor from a UsefulAnitaEvent
   virtual ~CalibratedAnitaEvent(); ///< Destructor

  //The calibrated numbers
  // UInt_t fC3poNum; ///< Used to monitor the clock period... not read out in ANITA-3 :(
  WaveCalType::WaveCalType_t fCalType; ///< The calibration type used
  Int_t fRcoArray[NUM_SURF];   ///< An array to store the guessed at RCO values;
  Double_t fTempFactorGuess;  ///< A holder variable to cling on to the temperature correction factor that we are guessing at.
  Double_t fClockPhiArray[NUM_SURF];   ///< An array to store the clock offsets


  ClassDef(CalibratedAnitaEvent,3);
};


#endif //CALIBRATEDANITAEVENT_H
