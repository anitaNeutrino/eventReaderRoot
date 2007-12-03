//////////////////////////////////////////////////////////////////////////////
/////  AnitaConventions.h        ANITA Convetnions                       /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A handy file full of enumerations and the like identifying     /////
/////     the conventions we try and use in the ANITA offline code.      /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////


#ifndef ANITACONVENTIONS_H
#define ANITACONVENTIONS_H

#ifndef ROOT_Rtypes
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Rtypes.h"
#endif
#endif


namespace WaveCalType {
  typedef enum EWaveCalType {
    kNoCalib                        = 0x00, //The 260 samples straight from raw data
    kJustUnwrap                     = 0x01, //The X good samples from raw data (260-hitbus)
    kADC                            = 0x02, //Same as kNoCalib -- i.e. useless
    kVoltageTime                    = 0x03, //Using 1 and 2.6
    kVTLabRG                        = 0x04, //Using all the Ryan/Gary numbers from Antarctica
    kVTFullRG                       = 0x05, //Same but also including cable delays
    kVTLabJW                        = 0x06, //Using Jiwoos differential numbers but no voltage calib yet
    kVTFullJW                       = 0x07, //Same but also including cable delays
    kVTLabJWPlus                    = 0x08, // kVTLabJW + Voltage Correction
    kVTFullJWPlus                   = 0x09, // kVTFullJW + Voltage Correction
    kVTLabClockRG                   = 0x0a, // kVTLabRG + Clock Jitter Correction 
    kVTFullClockRG                  = 0x0b, // kVTFullRG + Clock Jitter Correction 
    kVTLabJWPlusClock               = 0x0c, // kVTLabJWPlus + Clock Jitter Correction 
    kVTFullJWPlusClock              = 0x0d, // kVTFullJWPlus + Clock Jitter Correction 
    kVTLabClockZeroRG               = 0x0e, // kVTLabClockRG + Zero Mean
    kVTFullClockZeroRG              = 0x0f, // kVTFullClockRG + Zero Mean
    kVTLabJWPlusClockZero           = 0x10, // kVTLabJWPlusClock + Zero Mean
    kVTFullJWPlusClockZero          = 0x11,  // kVTFullJWPlusClock + Zero Mean
    kVTLabJWPlusFastClockZero       = 0x12, // kVTLabJWPlusClock (but faster and worse) + Zero Mean
    kVTFullJWPlusFastClockZero      = 0x13,  // kVTFullJWPlusClock (but faster and worse)  + Zero Mean
    kNotACalib
  } WaveCalType_t;

  const char *calTypeAsString(WaveCalType::WaveCalType_t calType);
  void listAllCalTypes();
}

///First up we'll add some definitions of the raw data

#define ACTIVE_SURFS 9
#define SCALERS_PER_SURF 32
#define RFCHAN_PER_SURF 8
#define CHANNELS_PER_SURF 9
#define LABRADORS_PER_SURF 4 /*jjb 2006-04-19 */
#define RCO_PER_LAB 2
#define NUM_DIGITZED_CHANNELS ACTIVE_SURFS*CHANNELS_PER_SURF
#define MAX_NUMBER_SAMPLES 260
#define EFFECTIVE_SAMPLES 256
#define NUM_PHI 16
#define NUM_SEAVEYS 32
#define NUM_BICONES 4
#define NUM_DISCONES 4

#define NUM_SURF ACTIVE_SURFS
#define NUM_CHAN CHANNELS_PER_SURF
#define NUM_CHIP LABRADORS_PER_SURF
#define NUM_RCO RCO_PER_LAB
#define NUM_SAMP MAX_NUMBER_SAMPLES
#define NUM_EFF_SAMP EFFECTIVE_SAMPLES

//Trigger Stuff
#define ANTS_PER_SURF 4
#define TRIGGER_SURFS 8
#define PHI_SECTORS 16

//Now some geometry and polarisation considerations
namespace AnitaRing {
   typedef enum EAnitaRing {
      kLowerRing  = 0,
      kUpperRing  = 1,
      kDisconeRing = 2,
      kBiconeRing =3
   } AnitaRing_t;
}

namespace AnitaPol {
   typedef enum EAnitaPol {
      kHorizontal = 0,
      kVertical   = 1
   } AnitaPol_t;
}

namespace AnitaLocations { 
   const double LONGITUDE_SURF_SEAVEY=167.06405555; // longitude, latitude and altitude of surface seavey
   const double LATITUDE_SURF_SEAVEY=-77.86177777;
   const double ALTITUDE_SURF_SEAVEY=-13.0;
   
   const double LONGITUDE_BH=167.06679444;
   const double LATITUDE_BH=-77.861936111;
   const double ALTITUDE_BH=-33.67;
}


#endif //ANITACONVENTIONS_H
