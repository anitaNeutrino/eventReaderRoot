//////////////////////////////////////////////////////////////////////////////
/////  Adu5Sat.h        Adu5 Satellite Info class                          /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty Adu5 SAT  objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ADU5SAT_H
#define ADU5SAT_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"

class Adu5Sat: public TObject
{
 public:
  Adu5Sat();		 
  ~Adu5Sat();

  Adu5Sat(Int_t           trun,
	  UInt_t          trealTime,
	  GpsAdu5SatStruct_t *gpsStruct);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UInt_t          numSats[4];
   UChar_t         prn[4][MAX_SATS];
   UChar_t         elevation[4][MAX_SATS];
   UChar_t         snr[4][MAX_SATS];
   UChar_t         flag[4][MAX_SATS];
   UShort_t        azimuth[4][MAX_SATS];
   
  ClassDef(Adu5Sat,10);
};


#endif //ADU5SAT_H
