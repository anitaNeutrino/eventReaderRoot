//////////////////////////////////////////////////////////////////////////////
/////  CalibratedHk.h        Raw Hk Data                                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing HkData structs in a tree            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef CALIBRATEDHK_H
#define CALIBRATEDHK_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"


#ifndef NUM_VOLTAGES
#define NUM_VOLTAGES 11
#endif
#ifndef NUM_CURRENTS
#define NUM_CURRENTS 7
#endif
#ifndef NUM_INT_TEMPS
#define NUM_INT_TEMPS 15
#endif
#ifndef NUM_SBS_TEMPS
#define NUM_SBS_TEMPS 4
#endif
#ifndef NUM_EXT_TEMPS
#define NUM_EXT_TEMPS 25
#endif
#ifndef NUM_POWERS
#define NUM_POWERS 5
#endif

#ifndef NUM_ATTITUDE
#define NUM_ATTITUDE 13
#endif
#ifndef NUM_PRESSURES
#define NUM_PRESSURES 2
#endif
#ifndef NUM_SUNSENSORS
#define NUM_SUNSENSORS 4
#endif
#ifndef NUM_ACCELEROMETERS
#define NUM_ACCELEROMETERS 2
#endif

class RawHk;


//!  CalibratedHk -- The calibrated housekeeping data
/*!
  The calibrated housekeeping data.
  \ingroup rootclasses
*/
class CalibratedHk: public TObject
{
 public:
   CalibratedHk(); ///< Default constructor		 
   ~CalibratedHk(); ///< Default destructor

   CalibratedHk(RawHk *hkPtr, RawHk *avzPtr, RawHk *calPtr); ///< Assignment constructor... takes the RawHk data along with the zero (avzPtr) and +5V (calPtr) calibration points

   Int_t           run; ///< Assigned offline
   UInt_t          realTime; ///< Time in unixTime
   UInt_t          payloadTime; ///< Time in unixTime
   UInt_t          payloadTimeUs; ///< Sub second time in us

   Float_t         voltage[NUM_IP320_BOARDS][CHANS_PER_IP320]; ///<Array of voltages
   Float_t         useful[NUM_IP320_BOARDS][CHANS_PER_IP320]; ///<Array of calibrated temperatures, currents, etc.
   Float_t         magX; ///< Magnetometer x direction
   Float_t         magY; ///< Magnetometer y direction
   Float_t         magZ; ///< Magnetometer z direction
   Short_t         sbsTemp[4]; ///< The four onboard temperature sensors
   
   Float_t   getInternalTemp(int index); ///< Returns internal temperature in degrees (0:14)
   Float_t   getSBSTemp(int index); ///< Returns SBS temperature in degrees (0:3)
   Float_t   getExternalTemp(int index); ///< Returns external temperature in degrees (0:24)
   Float_t   getVoltage(int index); ///< Returns voltage (0:10)
   Float_t   getCurrent(int index); ///< Returns current (0:11)
   Float_t   getPower(int index); ///< Returns power (0:5)
   Float_t   getAttitude(int index); ///< Returns attitude (0:12)
   Float_t   getPressure(int index); ///< Returns pressure (0:1)
   Float_t   getAccelerometer(int acInd, int type); ///< Returns accelerometer stuff
   Float_t   getRawSunsensor(int ssInd, int type); ///< Returns raw sunsensor stuff, here type goes x1,x2,y1,y2,T
   Float_t getNeobrickTemp() {return useful[1][38];} ///< Returns the neobrick temperature.
   Float_t getNeobrickPres() {return useful[1][39];} ///< Returns the neobrick pressure.

   //Sunsensor Stuff
   void getSSMagnitude(int ssInd, Float_t *magnitude,Float_t *magX, Float_t *magY); ///< Get sunsensor magnitude
   Float_t getSSTemp(int ssInd); ///< Get sunsensor temperature
   Int_t getSSXRatio(int ssInd, Float_t *xRatio); ///< Get sunsensor x-ratio
   Int_t getSSYRatio(int ssInd, Float_t *yRatio); ///< Get sunsensor y-ratio
   Int_t getFancySS(int ssInd, Float_t pos[3], Float_t *azimuth,
		    Float_t *elevation, Float_t *relAzimuth); ///< Convert sunsensor data to elevation and azimuth


   static const char *getPowerName(int index); ///< Return char string name of power
   static const char *getCurrentName(int index); ///< Return char string name of current
   static const char *getVoltageName(int index); ///< Return char string name of voltage
   static const char *getExternalTempName(int index); ///< Return char string name of external temperature sensor
   static const char *getInternalTempName(int index); ///< Return char string name of internal temperature sensor
   static const char *getSBSTempName(int index); ///< Return char string name of SBS temperature sensor
   static const char *getAttitudeName(int index); ///< Return char string name of attitude sensor

  ClassDef(CalibratedHk,10);
};


#endif //CALIBRATEDHK_H
