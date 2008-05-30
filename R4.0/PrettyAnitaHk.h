//////////////////////////////////////////////////////////////////////////////
/////  PrettyAnitaHk.h        Pretty ANITA hk class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ANITA hks in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef PRETTYANITAHK_H
#define PRETTYANITAHK_H

//Includes
#include <TObject.h>

class PrettyAnitaHk: public TObject
{
 public:
  PrettyAnitaHk();		 
  ~PrettyAnitaHk();

  PrettyAnitaHk(Int_t           trun,
		UInt_t          trealTime,
		UInt_t          tpayloadTime,
		UInt_t          tpayloadTimeUs,
		Float_t         *tintTemps,
		Float_t         *textTemps,
		Float_t         *tvoltages,
		Float_t         *tcurrents,
		Float_t         *tmagnetometer,
		Float_t         *tpressures,
		Float_t         *taccelerometer[],
		Float_t         *trawSunSensor[],
		Float_t         *tssMag[],
		Float_t         *tssElevation,
		Float_t         *tssAzimuth,
		Float_t         *tssAzimuthAdu5,
		Int_t           *tssGoodFlag,
		Int_t           tintFlag);

//Declaration of leaves types
   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UInt_t          payloadTimeUs;
   Float_t         intTemps[17];
   Float_t         extTemps[25];
   Float_t         voltages[11];
   Float_t         currents[12];
   Float_t         magnetometer[3];
   Float_t         pressures[2];
   Float_t         accelerometer[2][4];
   Float_t         rawSunSensor[4][5];
   Float_t         ssMag[4][2];
   Float_t         ssElevation[4];
   Float_t         ssAzimuth[4];
   Float_t         ssAzimuthAdu5[4];
   Int_t           ssGoodFlag[4];   
   Int_t           intFlag;
   
  ClassDef(PrettyAnitaHk,1);
};


#endif //PRETTYANITAHK_H
