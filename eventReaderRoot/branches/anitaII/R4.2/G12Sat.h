//////////////////////////////////////////////////////////////////////////////
/////  G12Sat.h        G12 Satellite Info class                          /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty G12 SAT  objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef G12SAT_H
#define G12SAT_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"

class TPad;


//!  G12Sat -- The satellite data from the G12
/*!
  The ROOT implementation of the G12 satellite data
  \ingroup rootclasses
*/
class G12Sat: public TObject
{
 public:
   G12Sat(); ///< Default constructor
   ~G12Sat(); ///< Destructor

  G12Sat(Int_t           trun,
	  UInt_t          trealTime,
	 GpsG12SatStruct_t *gpsStruct); ///< Assignment constructor

  Int_t           run; ///< Run number assigned offline
  UInt_t          realTime; ///< Time in unixtime
  UInt_t          payloadTime; ///< Time in unixtime
  UInt_t          numSats; ///< Number of satellites visible
  UChar_t         prn[MAX_SATS]; ///< PRN of visible satellites
  UChar_t         elevation[MAX_SATS]; ///< Elevation of visible satellites
  UChar_t         snr[MAX_SATS]; ///< SNR of visible satellites
  UChar_t         flag[MAX_SATS]; ///< Usability flag of satellites
   UShort_t        azimuth[MAX_SATS]; ///< Azimuth of visible satellites
   
   void getCirclePlot(TPad *padSat); ///< Fills the TPad with a 2D azimuth-elevation plot

  ClassDef(G12Sat,10);
};


#endif //G12SAT_H
