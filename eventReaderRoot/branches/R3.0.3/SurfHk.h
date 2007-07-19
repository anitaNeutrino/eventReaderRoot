//////////////////////////////////////////////////////////////////////////////
/////  SurfHk.h        Pretty ANITA hk class                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 VTG objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef SURFHK_H
#define SURFHK_H

//Includes
#include <TObject.h>

class SurfHk: public TObject
{
 public:
  SurfHk();		 
  ~SurfHk();



  SurfHk(Int_t           trun,
	 UInt_t          trealTime,
	 UInt_t          tpayloadTime,
	 UInt_t          tpayloadTimeUs,
	 UShort_t        tglobalThreshold,
	 UShort_t        terrorFlag,
	 UShort_t        tscalerGoal,
	 UShort_t        tupperWords[9],
	 UShort_t        tscaler[9][32],
	 UShort_t        tthreshold[9][32],
	 UShort_t        tsetThreshold[9][32],
	 UShort_t        trfPower[9][8],
	 UShort_t        tsurfTrigBandMask[9][2],
	 Int_t           tintFlag);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UInt_t          payloadTimeUs;
   UShort_t        globalThreshold;
   UShort_t        errorFlag;
   UShort_t        scalerGoal;
   UShort_t        upperWords[9];
   UShort_t        scaler[9][32];
   UShort_t        threshold[9][32];
   UShort_t        setThreshold[9][32];
   UShort_t        rfPower[9][8];
   UShort_t        surfTrigBandMask[9][2];
   Int_t           intFlag;

   
  ClassDef(SurfHk,1);
};


#endif //SURFHK_H
