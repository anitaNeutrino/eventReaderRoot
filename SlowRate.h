//////////////////////////////////////////////////////////////////////////////
/////  SlowRate.h        Slow Rate data structure                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing slow rate data                      /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef SLOWRATE_H
#define SLOWRATE_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"

//!  SlowRate -- The Slow Rate data
/*!
  The ROOT implementation of the SLOW rate data
  \ingroup rootclasses
*/
class SlowRate: public TObject
{
 public:
   SlowRate(); ///< Default constructor
   ~SlowRate(); ///< Destructor
   
   SlowRate(Int_t trun, UInt_t trealTime, SlowRateFull_t *slowPtr);

   Int_t           run; ///< Run number, assigned offline
   UInt_t          realTime; ///< Time in unixTime
   UInt_t          payloadTime; ///< Time in unixTime
  
   
   Float_t latitude; ///< Payload latitude
   Float_t longitude; ///< Payload longitude
   Short_t altitude; ///< Payload altitude
   UInt_t eventNumber; ///< Latest event number
   UChar_t rfPwrAvg[ACTIVE_SURFS][RFCHAN_PER_SURF]; ///< The average rf power (in ADCs)
   UChar_t avgScalerRates[TRIGGER_SURFS][ANTS_PER_SURF]; ///< The average scaler rate per antenna (* 2^7 to get Hz)
   UChar_t rmsScalerRates[TRIGGER_SURFS][ANTS_PER_SURF]; ///< The rms of the scaler rates (also * 2^7 )
   UChar_t avgL1Rates[TRIGGER_SURFS]; ///< 3 of 8 counters --fix later
   UChar_t avgL2Rates[PHI_SECTORS]; ///< average of upper and lower
   UChar_t avgL3Rates[PHI_SECTORS]; ///< Average L3 rate 
   UChar_t eventRate1Min; ///<Multiplied by 8
   UChar_t eventRate10Min; ///<Multiplied by 8

   UChar_t temps[4];  ///<{SBS,SURF,TURF,RAD}
   UChar_t powers[4]; ///<{PV V, +24V, BAT I, 24 I}

   Double_t getRFPowerInK(int surf, int chan); ///< Returns the pseudo-calibrated RF power in K.
   Double_t getMeasuredRFPowerInK(int surf, int chan); ///< Returns the pseudo-calibrated RF power in K.
   Int_t getAvgScaler(int surf, int ant); ///< Returns the averaged scaler value
   Int_t getRmsScaler(int surf, int ant); ///< Returns the rms scaler value
   Int_t getL1Rate(int surf); ///< Returns the average L1 rate in the SURF
   Int_t getL2Rate(int phi); ///< Returns the average L2 rate for the sector.
   Float_t getL3Rate(int phi); ///< Returns the average L3 rate for the sector.

   Float_t getLatitude() {return latitude;} ///< Returns the latitude
   Float_t getLongitude() {return longitude;} ///< Returns the longitude
   Float_t getAltitude(); ///< Returns the altitude correct for negative crazziness
   Float_t getPower(int powerInd); ///< Returns the actual power value
   Float_t getTemp(int tempInd); ///< Returns the actual temp

   const char *getPowerName(int powerInd); ///< Returns the name of the power reading
   const char *getTempName(int tempInd); ///< Returns the name of temp sensor
   ClassDef(SlowRate,10);
};


#endif //SLOWRATE_H
