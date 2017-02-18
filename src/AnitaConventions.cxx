//
// Body for AnitaConventions class so that CINT recognizes its existence
//
#include "AnitaConventions.h"
#include <iostream>
#include <cstring>

const char *WaveCalType::calTypeAsString(WaveCalType::WaveCalType_t calType)
{
   switch(calType) {     
   case kNoCalib: 
     return "The 260 samples straight from raw data";
     break;
   case kJustUnwrap:
     return "The X good samples from raw data (260-hitbus)";
     break;
   case kNominal:
     return "Using mV/ADC = 1 and all dts = 1./2.6 ns";
     break;
   case kJustTimeNoUnwrap :
     return "For calibration: sample-to-sample dts without unwrapping (or voltage calibs)";
     break;
   case kNoTriggerJitterNoZeroMean:
     return "No inter-SURF timing (or zero meaning)";
     break;
   case kNoChannelToChannelDelays :
     return "Inter-SURF timing (trigger jitter) without cable delay ";
     break;
   case kNoTriggerJitterNoZeroMeanFlipRco:
     return "For calib: opposite RCO from software algorithm";
     break;
   case kNoTriggerJitterNoZeroMeanFirmwareRco:
     return "For calib: applies RCO from firmware (no latch delay)";
     break;
   case kNoTriggerJitterNoZeroMeanFirmwareRcoFlipped:
     return "For calib: 1-firmware RCO (no latch delay)";
     break;
   case kFull:
     return "deltaTs; voltage; unwrap; trigger jitter; cable delay. The full monty.";
     break;
   case kAddPeds:
     return "Same as full calibration but add pedestal voltages back into waveforms.";
     break;

   // case kDefault:
   //   return "Default timing calibration: what you should call for analysis work";
   //   break;
   case kNotACalib: 
     return "Useful for looping over all calibrations";
     break;

   default:
     return "Unknown Calibration Type";
     break;
   }
   return "Unknown Calibration Type";
}

const char *AnitaRing::ringAsString(AnitaRing::AnitaRing_t ring)
{
  switch(ring) {
  case kTopRing: return "Top";
  case kMiddleRing: return "Middle";
  case kBottomRing: return "Bottom";
  default:
    return "Unknown";
  }
  return "Unknown";
}


char AnitaRing::ringAsChar(AnitaRing::AnitaRing_t ring)
{
  switch(ring) {
  case kTopRing: return 'T';
  case kMiddleRing: return 'M';
  case kBottomRing: return 'B';
  default:
    return 'U';
  }
  return 'U';
}

char AnitaPol::polAsChar(AnitaPol::AnitaPol_t pol)
{
  switch(pol) {
  case kVertical: return 'V';
  case kHorizontal: return 'H';
      default:
    return 'U';
  }
  return 'U';
}

char AnitaTrigPol::polAsChar(AnitaTrigPol::AnitaTrigPol_t pol)
{
  switch(pol) {
  case kLCP: return 'L';
  case kRCP: return 'R';
      default:
    return 'U';
  }
  return 'U';
}


const char *AnitaBand::bandAsString(AnitaBand::AnitaBand_t band)
{
  switch(band) {
  case kLow: return "Low";
  case kMid: return "Mid";
  case kHigh: return "High";
  case kFull: return "Full";
  default:
    return "Unknown";
  }
  return "Unknown";
}

void WaveCalType::listAllCalTypes()
{
   std::cout << "Calibration Options" << std::endl;
   std::cout << "==============================================================================\n\n";
   for(int i=(int)kNoCalib;i<(int)kNotACalib;i++) {
      std::cout << i << "\t" << WaveCalType::calTypeAsString((WaveCalType::WaveCalType_t)i) << std::endl;
   }
   std::cout << std::endl;
}

  
const Double_t AnitaLocations::getWaisLatitude() {return AnitaVersion::get() == 3 ? LATITUDE_WAIS : LATITUDE_WAIS_A4;};
const Double_t AnitaLocations::getWaisLongitude() {return AnitaVersion::get() == 3 ? LONGITUDE_WAIS : LONGITUDE_WAIS_A4;};
const Double_t AnitaLocations::getWaisAltitude() {return AnitaVersion::get() == 3 ? ALTITUDE_WAIS : ALTITUDE_WAIS_A4;};
  
