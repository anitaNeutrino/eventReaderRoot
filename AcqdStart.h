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


class AcqdStart: public TObject
{
 public:
  AcqdStart();		 
  ~AcqdStart();

  AcqdStart(Int_t trun, Int_t trealTime, AcqdStartStruct_t *startPtr);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime; 
   UInt_t          numEvents;
   UChar_t         testBytes[8];
   Float_t         chanMean[ACTIVE_SURFS][CHANNELS_PER_SURF]; ///<Ped subtracted
   Float_t         chanRMS[ACTIVE_SURFS][CHANNELS_PER_SURF]; ///<Ped subtracted
   UShort_t        threshVals[10];
   UShort_t        scalerVals[ACTIVE_SURFS][SCALERS_PER_SURF][10];

  ClassDef(AcqdStart,10);
};


#endif //ACQDSTART_H
