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


class RawHk: public TObject
{
 public:
  RawHk();		 
  ~RawHk();

  RawHk(Int_t trun, Int_t trealTime, HkDataStruct_t *hkPtr);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime; 
   UInt_t          payloadTimeUs; 

   AnalogueCode_t  acromagCode;
   UShort_t        acromagData[NUM_IP320_BOARDS][CHANS_PER_IP320];
   Float_t         magX;
   Float_t         magY;
   Float_t         magZ;
   Short_t         sbsTemp[4];
   
  ClassDef(RawHk,10);
};


#endif //RAWHK_H
