//////////////////////////////////////////////////////////////////////////////
/////  AcqdStart.h        Acqd Start                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Acqd Start structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ACQDSTART_H
#define ACQDSTART_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"

//!  AcqdStart -- The Acqd Start Struct 
/*!
 This is a class to hold the object created when Acqd starts. Contained in the AcqdStart block is information regarding the channel means and RMS's. These objects inherit from TObject and are written to TTree's.
*/
class AcqdStart: public TObject
{
 public:
  AcqdStart();		 
  ~AcqdStart();

  //! The assignment constructor
  /*!
    \param trun is the run number
    \param trealTime is the time -- should be the same as payloadTime.
    \param startPtr is a pointer to an AcqdStartStruct_t object which contains the raw data from the payload
  */
  AcqdStart(Int_t trun, Int_t trealTime, AcqdStartStruct_t *startPtr);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime; 
   UInt_t          numEvents;

   UChar_t turfIdBytes[4];
   UInt_t turfIdVersion;
   UChar_t turfioIdBytes[4];
   UInt_t turfioIdVersion;
   
   UChar_t         testBytes[8];
   Float_t         chanMean[ACTIVE_SURFS][CHANNELS_PER_SURF]; ///<Ped subtracted
   Float_t         chanRMS[ACTIVE_SURFS][CHANNELS_PER_SURF]; ///<Ped subtracted
   UShort_t        threshVals[10]; ///<Threshold values for mini-scan
   UShort_t        scalerVals[ACTIVE_SURFS][SCALERS_PER_SURF][10]; ///<Scaler values for mini-scan
   
   void printTurfioVersion();
   void printTurfVersion();


  ClassDef(AcqdStart,11);
};


#endif //ACQDSTART_H
