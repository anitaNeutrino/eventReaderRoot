//////////////////////////////////////////////////////////////////////////////
/////  TimedAnitaHeader.h        Timed ANITA header class                /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing ANITA headers with fixed times      /////
/////     (with various corrections)                                     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
/////  Author: Cosmin Deacnou (cozzyd@kicp.uchicago.edu)                 /////
//////////////////////////////////////////////////////////////////////////////

#ifndef TIMEDANITAHEADER_H
#define TIMEDANITAHEADER_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "RawAnitaHeader.h"
#include "simpleStructs.h"

//!  TimedAnitaHeader -- Anita Header withe new time
/*!
  ANITA-I class after fixing timing. Deprecated in ANITA-II. Updated again for ANITA-IV. 
  \ingroup rootclasses
*/
class TimedAnitaHeader: public RawAnitaHeader
{
 public:
   TimedAnitaHeader(); ///<Default constructor
   TimedAnitaHeader(const RawAnitaHeader &old, UInt_t correctedTriggerTime, UInt_t correctedTriggerTimeNs, Double_t tttAlignmentFraction); ///< Assignment constructor
		 
  ~TimedAnitaHeader(); ///< Destructor

  UInt_t oldTriggerTime; ///< oldTriggerTime from before second-stage time matching
  UInt_t oldTriggerTimeNs; ///< oldTriggerTime from before second-stage time matching

  Double_t tttAlignmentFraction; ///< fraction of the time we match the GpsTTT stuff 
   
  ClassDef(TimedAnitaHeader,40);
};


#endif //TIMEDANITAHEADER_H
