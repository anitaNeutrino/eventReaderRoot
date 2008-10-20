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

//!  Adu5Vtg -- The ADU5 Velocity and Course Information
/*!
  This is a class to hold the velocity and course information for the ADU5's
  \ingroup rootclasses
*/
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
   Float_t         trueCourse; ///< Course relative to true north
   Float_t         magneticCourse; ///< Course relative to magnetic north
   Float_t         speedInKnots; ///< Speed over ground in knots
   Float_t         speedInKPH; ///< Speed over ground in kph
   Int_t           intFlag;

   
  ClassDef(Adu5Vtg,10);
};


#endif //ADU5VTG_H
