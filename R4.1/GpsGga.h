//////////////////////////////////////////////////////////////////////////////
/////  GpsGga.h        Pretty ANITA hk class                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 VTG objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef GPSGGA_H
#define GPSGGA_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"

class GpsGga: public TObject
{
 public:
  GpsGga();		 
  ~GpsGga();

  GpsGga(Int_t           trun,
	 UInt_t          trealTime,
	 GpsGgaStruct_t *gpsStruct);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UInt_t          payloadTimeUs;
   UInt_t          timeOfDay;
   UChar_t          numSats;
   Float_t         latitude;
   Float_t         longitude;
   Float_t         altitude;
   Float_t         hdop;
   Float_t         geoidSeparation;
   Float_t         ageOfCalc;
   UChar_t         posFixType;
   UChar_t         baseStationId;
   Int_t           intFlag;

   
  ClassDef(GpsGga,10);
};


#endif //GPSGGA_H
