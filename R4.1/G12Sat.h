//////////////////////////////////////////////////////////////////////////////
/////  G12Sat.h        G12 Satellite Info class                          /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty G12 SAT  objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef G12SAT_H
#define G12SAT_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"

class TPad;

class G12Sat: public TObject
{
 public:
  G12Sat();		 
  ~G12Sat();

  G12Sat(Int_t           trun,
	  UInt_t          trealTime,
	  GpsG12SatStruct_t *gpsStruct);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UInt_t          numSats;
   UChar_t         prn[MAX_SATS];
   UChar_t         elevation[MAX_SATS];
   UChar_t         snr[MAX_SATS];
   UChar_t         flag[MAX_SATS];
   UShort_t        azimuth[MAX_SATS];
   
   void getCirclePlot(TPad *padSat);

  ClassDef(G12Sat,10);
};


#endif //G12SAT_H
