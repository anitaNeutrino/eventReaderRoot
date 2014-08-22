//////////////////////////////////////////////////////////////////////////////
/////  CalibratedSSHk.h        Raw SSHk Data                                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing SSHkData structs in a tree            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef CALIBRATEDSSHK_H
#define CALIBRATEDSSHK_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"


class RawSSHk;


//!  CalibratedSSHk -- The calibrated housekeeping data
/*!
  The calibrated housekeeping data.
  \ingroup rootclasses
*/
class CalibratedSSHk: public TObject
{
 public:
   CalibratedSSHk(); ///< Default constructor		 
   ~CalibratedSSHk(); ///< Default destructor

   CalibratedSSHk(RawSSHk *hkPtr, RawSSHk *avzPtr, RawSSHk *calPtr); ///< Assignment constructor... takes the RawSSHk data along with the zero (avzPtr) and +5V (calPtr) calibration points

   Int_t           run; ///< Assigned offline
   UInt_t          realTime; ///< Time in unixTime
   UInt_t          payloadTime; ///< Time in unixTime
   UInt_t          payloadTimeUs; ///< Sub second time in us

   Float_t         voltage[CHANS_PER_IP320]; ///<Array of voltages
   Float_t         useful[CHANS_PER_IP320]; ///<Array of calibrated temperatures, currents, etc.

   //Sunsensor Stuff
   void getSSMagnitude(int ssInd, Float_t *magnitude,Float_t *magX, Float_t *magY); ///< Get sunsensor magnitude
   Float_t getSSTemp(int ssInd); ///< Get sunsensor temperature
   Int_t getSSXRatio(int ssInd, Float_t *xRatio); ///< Get sunsensor x-ratio
   Int_t getSSYRatio(int ssInd, Float_t *yRatio); ///< Get sunsensor y-ratio
   Int_t getFancySS(int ssInd, Float_t pos[3], Float_t *azimuth,
		    Float_t *elevation, Float_t *relAzimuth); ///< Convert sunsensor data to elevation and azimuth

   Float_t   getRawSunsensor(int ssInd, int type); ///< Returns raw sunsensor stuff, here type goes x1,x2,y1,y2,T

  ClassDef(CalibratedSSHk,10);
};


#endif //CALIBRATEDSSHK_H
