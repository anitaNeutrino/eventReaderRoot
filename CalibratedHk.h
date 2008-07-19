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
#define NUM_CURRENTS 12
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
#define NUM_POWERS 6
#endif

#define NUM_ATTITUDE 13

class RawHk;

class CalibratedHk: public TObject
{
 public:
  CalibratedHk();		 
  ~CalibratedHk();

  CalibratedHk(RawHk *hkPtr, RawHk *avzPtr, RawHk *calPtr);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime; 
   UInt_t          payloadTimeUs; 

   Float_t         voltage[NUM_IP320_BOARDS][CHANS_PER_IP320];
   Float_t         useful[NUM_IP320_BOARDS][CHANS_PER_IP320];
   Float_t         magX;
   Float_t         magY;
   Float_t         magZ;
   Short_t         sbsTemp[4];
   
   Float_t   getInternalTemp(int index);
   Float_t   getSBSTemp(int index);
   Float_t   getExternalTemp(int index);
   Float_t   getVoltage(int index);
   Float_t   getCurrent(int index);
   Float_t   getPower(int index);
   Float_t   getAttitude(int index);

   char *getPowerName(int index);
   char *getCurrentName(int index);
   char *getVoltageName(int index);
   char *getExternalTempName(int index);
   char *getInternalTempName(int index);
   char *getSBSTempName(int index);
   char *getAttitudeName(int index);

  ClassDef(CalibratedHk,10);
};


#endif //CALIBRATEDHK_H
