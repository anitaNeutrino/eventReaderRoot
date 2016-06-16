//////////////////////////////////////////////////////////////////////////////
/////  RawSSHk.h        Raw Hk Data                                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing HkData structs in a tree            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWSSHK_H
#define RAWSSHK_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"


//!  RawSSHk -- The Raw ANITA Housekeeping
/*!
  The ROOT implementation of the raw ANITA housekeeping
  \ingroup rootclasses
*/
class RawSSHk: public TObject
{
 public:
   RawSSHk(); ///< Default constructor
   ~RawSSHk(); ///< Destructor

   RawSSHk(Int_t trun, Int_t trealTime, SSHkDataStruct_t *hkPtr); ///< Assignment constructor

   Int_t           run; ///< Run number, assigned offline
   UInt_t          realTime; ///< Time in unixTime
   UInt_t          payloadTime;  ///< Time in unixTime
   UInt_t          payloadTimeUs;  ///< Subsecond timing

   AnalogueCode_t  acromagCode; ///< Is it raw or calibration data
   UShort_t        acromagData[CHANS_PER_IP320]; ///< The array of ADC values
  ClassDef(RawSSHk,10);
};


#endif //RAWSSHK_H
