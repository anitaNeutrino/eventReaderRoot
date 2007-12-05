//////////////////////////////////////////////////////////////////////////////
/////  TurfRate.h        Pretty ANITA hk class                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 VTG objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef TURFRATE_H
#define TURFRATE_H

//Includes
#include <TObject.h>

class TurfRate: public TObject
{
 public:
  TurfRate();		 
  ~TurfRate();



  TurfRate(Int_t           trun,
	   UInt_t          trealTime,
	   UInt_t          tpayloadTime,
	   UInt_t          tpayloadTimeUs,
	   UShort_t        tl1Rates[8][4],
	   UChar_t         tupperL2Rates[16],
	   UChar_t         tlowerL2Rates[16],
	   UChar_t         tl3Rates[16],
	   Int_t           tintFlag);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UInt_t          payloadTimeUs;
   UShort_t        l1Rates[8][4];
   UChar_t         upperL2Rates[16];
   UChar_t         lowerL2Rates[16];
   UChar_t         l3Rates[16];
   Int_t           intFlag;

   
  ClassDef(TurfRate,1);
};


#endif //TURFRATE_H
