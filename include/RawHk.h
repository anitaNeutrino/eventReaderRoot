//////////////////////////////////////////////////////////////////////////////
/////  RawHk.h        Raw Hk Data                                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing HkData structs in a tree            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWHK_H
#define RAWHK_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"


//!  RawHk -- The Raw ANITA Housekeeping
/*!
  The ROOT implementation of the raw ANITA housekeeping
  \ingroup rootclasses
*/
class RawHk: public TObject
{
 public:
   RawHk(); ///< Default constructor
   ~RawHk(); ///< Destructor

   RawHk(Int_t trun, Int_t trealTime, HkDataStruct_t *hkPtr); ///< Assignment constructor

   Int_t           run; ///< Run number, assigned offline
   UInt_t          realTime; ///< Time in unixTime
   UInt_t          payloadTime;  ///< Time in unixTime
   UInt_t          payloadTimeUs;  ///< Subsecond timing

   AnalogueCode_t  acromagCode; ///< Is it raw or calibration data
   UShort_t        acromagData[NUM_IP320_BOARDS][CHANS_PER_IP320]; ///< The array of ADC values
   Float_t         magX; ///< Magnetometer x value
   Float_t         magY; ///< Magnetometer y value
   Float_t         magZ; ///< Magnetometer z value
   Short_t         sbsTemp[3]; ///<SBS temperatures multiplied by 10
   Short_t         ntuTemp[3]; ///<NTU temperatures in some crazy scheme
   
  ClassDef(RawHk,11);
};


#endif //RAWHK_H
