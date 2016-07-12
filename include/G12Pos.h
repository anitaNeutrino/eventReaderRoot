//////////////////////////////////////////////////////////////////////////////
/////  G12Pos.h        Pretty ANITA hk class                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 VTG objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef G12Pos_H
#define G12Pos_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"



//!  G12Pos -- The position data from the G12
/*!
  The ROOT implementation of the G12 position data
  \ingroup rootclasses
*/
class G12Pos: public TObject
{
 public:
   G12Pos(); ///< Default constructor		 
   ~G12Pos(); ///< Destructor

  G12Pos(Int_t           trun,
	 UInt_t          trealTime,
	 GpsG12PosStruct_t *gpsStruct); ///< Assignment constructor

  Int_t           run; ///< Run number assigned offline
  UInt_t          realTime; ///< Time in unixtime from CPU
  UInt_t          payloadTime; ///< Time in unixtime from CPU
  UInt_t          payloadTimeUs; ///< Sun second time from CPU
  UInt_t          timeOfDay; ///< Time in ms since the start of the day (from G12)
  UInt_t          numSats; ///< Number of visible satellites
  Float_t         latitude; ///< Latitude in degrees
  Float_t         longitude; ///< Longitude in degrees
  Float_t         altitude; ///< Altitude in metres
  Float_t         trueCourse; ///< Course relative to true north
  Float_t         verticalVelocity; ///< Vertical velocity in m/s (I think)
  Float_t         speedInKnots; ///< Speed of ground in knots
  Float_t         pdop; ///< Position dilution of precision (smaller is better)
  Float_t         hdop; ///< Horizontal dilution of precision
  Float_t         vdop; ///< Vertical dilution of precision
  Float_t         tdop; ///< Time dilution of precision
  Int_t           intFlag; ///< Interpolation flag (zero for raw uninterpolated data)

   
  ClassDef(G12Pos,10);
};


#endif //G12Pos_H
