//////////////////////////////////////////////////////////////////////////////
/////  CalibratedHk.h        Raw Hk Data                                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing HkData structs in a tree            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef CALIBRATEDHK_H
#define CALIBRATEDHK_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"

class RawHk;

class CalibratedHk: public TObject
{
 public:
  CalibratedHk();		 
  ~CalibratedHk();

  CalibratedHk(RawHk *hkPtr, RawHk *avzPtr, RawHk *calPtr);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime; 
   UInt_t          payloadTimeUs; 

   Float_t         voltage[NUM_IP320_BOARDS][CHANS_PER_IP320];
   Float_t         useful[NUM_IP320_BOARDS][CHANS_PER_IP320];
   Float_t         magX;
   Float_t         magY;
   Float_t         magZ;
   Short_t         sbsTemp[4];
   
   Float_t   getInternalTemp(int index);
   Float_t   getSBSTemp(int index);
   Float_t   getExternalTemp(int index);

  ClassDef(CalibratedHk,10);
};


#endif //CALIBRATEDHK_H
