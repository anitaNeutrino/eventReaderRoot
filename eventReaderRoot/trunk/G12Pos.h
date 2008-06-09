//////////////////////////////////////////////////////////////////////////////
/////  G12Pos.h        Pretty ANITA hk class                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 VTG objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef G12Pos_H
#define G12Pos_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"

class G12Pos: public TObject
{
 public:
  G12Pos();		 
  ~G12Pos();

  G12Pos(Int_t           trun,
	 UInt_t          trealTime,
	 GpsG12PosStruct_t *gpsStruct);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UInt_t          payloadTimeUs;
   UInt_t          timeOfDay;
   UInt_t          numSats;
   Float_t         latitude;
   Float_t         longitude;
   Float_t         altitude;
   Float_t         trueCourse;
   Float_t         verticalVelocity;
   Float_t         speedInKnots;
   Float_t         pdop;
   Float_t         hdop;
   Float_t         vdop;
   Float_t         tdop;
   Int_t           intFlag;

   
  ClassDef(G12Pos,10);
};


#endif //G12Pos_H
