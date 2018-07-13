//////////////////////////////////////////////////////////////////////////////
/////  Adu5Sat.h        Adu5 Satellite Info class                          /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty Adu5 SAT  objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ADU5SAT_H
#define ADU5SAT_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"

class TPad;

//!  Adu5Sat -- The ADU5 Satellite Information
/*!
  This is a class to hold the satellite visibilty information for the ADU5's
  \ingroup rootclasses
*/
class Adu5Sat: public TObject
{
 public:
  Adu5Sat();		 
  ~Adu5Sat();

  Adu5Sat(Int_t           trun,
	  UInt_t          trealTime,
	  GpsAdu5SatStruct_t *gpsStruct);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime;
   UInt_t          numSats[4]; ///< The number of satellites each of the antennas can see
   UChar_t         prn[4][MAX_SATS]; ///< The PRN of each visible satellite
   UChar_t         elevation[4][MAX_SATS];  ///< The elevation of each visible satellite
   UChar_t         snr[4][MAX_SATS]; ///< The SNR of each visible satellite
   UChar_t         flag[4][MAX_SATS]; ///< The usability flag of each visible satellite
   UShort_t        azimuth[4][MAX_SATS]; ///< The azimuth of each visible satellite
   
   void getCirclePlot(TPad *padSat,const char *title=0); ///< Creates a 2D elevation-azimuth plot
   int getNumSats(int whichAnt);
   int getPRN(int whichAnt,int whichSat);
   int getSNR(int whichAnt,int whichSat);
   int getElevation(int whichAnt,int whichSat);
   int getAzimuth(int whichAnt,int whichSat);
   int getFlag(int whichAnt,int whichSat);
   
   

  ClassDef(Adu5Sat,10);
};


#endif //ADU5SAT_H
