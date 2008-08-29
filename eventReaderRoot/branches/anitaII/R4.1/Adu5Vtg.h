//////////////////////////////////////////////////////////////////////////////
/////  Adu5Vtg.h        Pretty ANITA hk class                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 VTG objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ADU5VTG_H
#define ADU5VTG_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"

class Adu5Vtg: public TObject
{
 public:
  Adu5Vtg();		 
  ~Adu5Vtg();

  Adu5Vtg(Int_t           trun,
	  UInt_t          trealTime,
	  GpsAdu5VtgStruct_t *gpsStruct);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UInt_t          payloadTimeUs;
   Float_t         trueCourse;
   Float_t         magneticCourse;
   Float_t         speedInKnots;
   Float_t         speedInKPH;
   Int_t           intFlag;

   
  ClassDef(Adu5Vtg,10);
};


#endif //ADU5VTG_H
