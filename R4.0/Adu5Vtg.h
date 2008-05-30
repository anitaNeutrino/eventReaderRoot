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

class Adu5Vtg: public TObject
{
 public:
  Adu5Vtg();		 
  ~Adu5Vtg();

  Adu5Vtg(Int_t           trun,
	  UInt_t          trealTime,
	  UInt_t          tpayloadTime,
	  UInt_t          tpayloadTimeUs,
	  Float_t         ttrueCourse,
	  Float_t         tmagneticCourse,
	  Float_t         tspeedInKnots,
	  Float_t         tspeedInKPH,
	  Int_t           tintFlag);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UInt_t          payloadTimeUs;
   Float_t         trueCourse;
   Float_t         magneticCourse;
   Float_t         speedInKnots;
   Float_t         speedInKPH;
   Int_t           intFlag;

   
  ClassDef(Adu5Vtg,1);
};


#endif //ADU5VTG_H
