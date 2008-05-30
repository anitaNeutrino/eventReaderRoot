//////////////////////////////////////////////////////////////////////////////
/////  Adu5Pat.h        Pretty ANITA hk class                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 PAT objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ADU5PAT_H
#define ADU5PAT_H

//Includes
#include <TObject.h>

class Adu5Pat: public TObject
{
 public:
  Adu5Pat();		 
  ~Adu5Pat();

  Adu5Pat(Int_t           trun,
	  UInt_t          trealTime,
	  UInt_t          treadTime,
	  UInt_t          tpayloadTime,
	  UInt_t          tpayloadTimeUs,
	  UInt_t          ttimeOfDay,
	  Float_t         tlatitude,
	  Float_t         tlongitude,
	  Float_t         taltitude,
	  Float_t         theading,
	  Float_t         tpitch,
	  Float_t         troll,
	  Float_t         tmrms,
	  Float_t         tbrms,
	  UInt_t          tattFlag,
	  Int_t           tintFlag);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          readTime;
   UInt_t          payloadTime;
   UInt_t          payloadTimeUs;
   UInt_t          timeOfDay;
   Float_t         latitude;
   Float_t         longitude;
   Float_t         altitude;
   Float_t         heading;
   Float_t         pitch;
   Float_t         roll;
   Float_t         mrms;
   Float_t         brms;
   UInt_t          attFlag;
   Int_t           intFlag;
   
  ClassDef(Adu5Pat,1);
};


#endif //ADU5PAT_H
