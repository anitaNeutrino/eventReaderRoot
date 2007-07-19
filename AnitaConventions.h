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
    kVTLabClockZeroRG               = 0x10, // kVTLabClockRG + Zero Mean
    kVTFullClockZeroRG              = 0x11, // kVTFullClockRG + Zero Mean
    kVTLabJWPlusClockZero           = 0x12, // kVTLabJWPlusClock + Zero Mean
    kVTFullJWPlusClockZero          = 0x13  // kVTFullJWPlusClock + Zero Mean
  } WaveCalType_t;
}

///First up we'll add some definitions of the raw data

#define ACTIVE_SURFS 9
#define CHANNELS_PER_SURF 9
#define LABRADORS_PER_SURF 4 /*jjb 2006-04-19 */
#define RCO_PER_LAB 2
#define NUM_DIGITZED_CHANNELS ACTIVE_SURFS*CHANNELS_PER_SURF
#define MAX_NUMBER_SAMPLES 260
#define EFFECTIVE_SAMPLES 256
#define NUM_PHI 16

#define NUM_SURF ACTIVE_SURFS
#define NUM_CHAN CHANNELS_PER_SURF
#define NUM_CHIP LABRADORS_PER_SURF
#define NUM_RCO RCO_PER_LAB
#define NUM_SAMP MAX_NUMBER_SAMPLES
#define NUM_EFF_SAMP EFFECTIVE_SAMPLES


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



#endif //ANITACONVENTIONS_H
