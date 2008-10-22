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

#include "CalibratedHk.h"

//!  PrettyAnitaHk -- The prettified ANITA Hk
/*!
  The ROOT implementation of the Anita Hk
  \ingroup rootclasses
*/
class PrettyAnitaHk: public TObject
{
 public:
   PrettyAnitaHk(); ///< Default constructor		 
   ~PrettyAnitaHk(); ///< Destructor

   PrettyAnitaHk(CalibratedHk *calHk, Int_t tintFlag); ///<New assignment constructor
   
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
		 Int_t           tintFlag); ///< Old assignment constructor

//Declaration of leaves types
  Int_t           run; ///< Run number, assigned offline
  UInt_t          realTime; ///< Time in unixTime
  UInt_t          payloadTime; ///< Time in unixTime
  UInt_t          payloadTimeUs; ///< Subsecond time
  Float_t         intTemps[19]; ///< Internal temperatures, includes extra sbsTemps
  Float_t         extTemps[25]; ///< External temperatures
  Float_t         voltages[11]; ///< voltages
  Float_t         currents[12]; ///< currents
  Float_t         magnetometer[3]; ///< magnetometer
  Float_t         pressures[2]; ///< pressures
  Float_t         accelerometer[2][4]; ///< accelerometer data
  Float_t         rawSunSensor[4][5]; ///< raw SS data
  Float_t         ssMag[4][2]; ///< Sunsensor magnitude data
  Float_t         ssElevation[4]; ///< Sunsensor elevation data
  Float_t         ssAzimuth[4]; ///< Sunsensor azimuth data
  Float_t         ssAzimuthAdu5[4]; ///< Sunsensor azimuth relative to ADU5
  Int_t           ssGoodFlag[4]; ///< Sunsensor goodness flag
  Int_t           intFlag; ///< Interpolation flag -- zero for raw data
   
  ClassDef(PrettyAnitaHk,2);
};


#endif //PRETTYANITAHK_H
