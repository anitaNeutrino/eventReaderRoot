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
   case kVTLabRG: return "kVTLabRG -- Voltage + Timing from Willy Field"; break;    
   case kVTFullRG: return "kVTFullRG -- V+T + CableDelay"; break;   
   case kVTLabJW: return "kVTLabJW -- Jiwoo timing nums - No Voltage Calib"; break;    
   case kVTFullJW: return "kVTFullJW -- Jiwoo timing nums + CableDelay - No Voltage Calib"; break;   
   case kVTLabJWPlus: return "kVTLabJWPlus -- Jiwoo  With Voltage"; break;
   case kVTFullJWPlus: return "kVTFullJWPlus -- Jiwoo + Cable Delay - With Voltage"; break;
   case kVTLabClockRG: return "kVTLabClockRG -- kVTLabRG + Trigger Jitter (from clock)"; break; 
   case kVTFullClockRG: return "kVTFullClockRG -- kVTFullRG + TJ"; break; 
   case kVTLabJWPlusClock: return "kVTLabJWPlusClock -- kVTLabJW + TJ"; break; 
   case kVTFullJWPlusClock: return "kVTFullJWPlusClock -- kVTFullJW + TJ"; break;
   case kVTLabClockZeroRG: return "kVTLabClockZeroRG -- kVTLabClockRG + Zero Mean"; break;  
   case kVTFullClockZeroRG: return "kVTFullClockZeroRG -- kVTFullClockRG + Zero Mean"; break;
   case kVTLabJWPlusClockZero: return "kVTLabJWPlusClockZero -- kVTLabJWPlusClock + Zero Mean"; break;
   case kVTFullJWPlusClockZero: return "kVTFullJWPlusClockZero -- kVTFullJWPlusClock + Zero Mean"; break;
   case kVTLabJWPlusFastClockZero: return "kVTLabJWPlusFastClockZero -- kVTLabJWPlusClock (no fit)+ Zero Mean"; break;
   case kVTFullJWPlusFastClockZero: return "kVTFullJWPlusFastClockZero -- kVTFullJWPlusClock (no fit)+ Zero Mean"; break;
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

char *AnitaRing::ringAsString(AnitaRing::AnitaRing_t ring)
{
  switch(ring) {
  case kUpperRing: return "Upper";
  case kLowerRing: return "Lower";
  case kNadirRing: return "Nadir";
  default:
    return "Unknown";
  }
  return "Unknown";
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

char *AnitaBand::bandAsString(AnitaBand::AnitaBand_t band)
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
