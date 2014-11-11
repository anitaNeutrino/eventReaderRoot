//
// Body for AnitaConventions class so that CINT recognizes its existence
//
#include "AnitaConventions.h"
#include <iostream>
#include <cstring>

const char *WaveCalType::calTypeAsString(WaveCalType::WaveCalType_t calType)
{
   switch(calType) {     
   case kNoCalib: return "kNoCalib -- No Calibration"; break;
   case kJustUnwrap: return "kJustUnwrap -- Unwrap Data, No Calibration"; break;
   case kADC: return "kADC -- No Calibration"; break;       
   case kVoltageTime: return "kVoltageTime -- Default 2GSa/s 1mv/ADC"; break;
   case kJustTimeNoUnwrap: return "kJustTimeNoUnwrap -- Only applies the bin-by-bin time calibration" ; break;
   case kVTLabAG: return "kVTLabAG --- Applies Abby's bin-by-bin and epsilon corrections"; break;
   case kVTLabAGFastClock: return "kVTLabAGFastClock -- Applies Abby's bin-by-bin and epsilon corrections and tries to align the clocks"; break;
   case kVTLabAGCrossCorClock: return "kVTLabAGCrossCorClock -- Applies Abby's bin-by-bin and epsilon corrections and tries to align the clocks using cross-correlations"; break;
   case kVTFullAGFastClock: return "kVTFullAGFastClock -- Applies Abby's bin-by-bin and epsilon corrections and tries to align the clocks + delays"; break;
   case kVTFullAGCrossCorClock: return "kVTFullAGCrossCorClock -- Applies Abby's bin-by-bin and epsilon corrections and tries to align the clocks using cross-correlations+ delays"; break;
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
