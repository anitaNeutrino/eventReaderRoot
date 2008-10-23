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



//!  GpsGga -- GPS Geoid Data
/*!
  The ROOT implementation of the GPS Geoid data
  \ingroup rootclasses
*/
class GpsGga: public TObject
{
 public:
   GpsGga(); ///<Default constructor
   ~GpsGga(); ///<Destructor

  GpsGga(Int_t           trun,
	 UInt_t          trealTime,
	 GpsGgaStruct_t *gpsStruct); ///< Assignment constructor

  Int_t           run; ///< run number, assigned offline
  UInt_t          realTime; ///< time in unixTime (from CPU)
  UInt_t          payloadTime; ///< time in unixTime (from CPU)
  UInt_t          payloadTimeUs; ///< sub second timing in us (from CPU)
  UInt_t          timeOfDay; ///< time of day in ms (from GPS)
  UChar_t          numSats; ///< Number of satellites visible
  Float_t         latitude; ///< Latitude in degrees
  Float_t         longitude; ///< Longitude in degrees
  Float_t         altitude; ///< Altitude in degrees
  Float_t         hdop; ///< Horizontal dilution of precision
  Float_t         geoidSeparation; ///< Geoidal separation value in metres
  Float_t         ageOfCalc; ///< Age of differential calculation (in seconds)
  UChar_t         posFixType; ///< Position fix tupe (0-invalid,1-autonoumous, 2-differential)
  UChar_t         baseStationId; ///< Differential base station id number -- no idea what it is
  Int_t           intFlag; ///< Interpolation flag will be zero for raw data

   
  ClassDef(GpsGga,10);
};


#endif //GPSGGA_H
