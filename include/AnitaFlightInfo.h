#ifndef ANITA_FLIGHT_INFO_H 
#define ANITA_FLIGHT_INFO_H 


/* Information about flights (e.g. what channels are broken, etc.) */ 

#include "AnitaConventions.h" 
#include "RawAnitaEvent.h"
#include "Rtypes.h" 

class RawAnitaHeader; 

namespace AnitaFlightInfo
{

   /**
    * Returns a bitmask of the usable antennas for the given event 
    *
    * Unusable can mean things like: 
    *    - Broken amplifier
    *    - Crazy clock 
    *
    **/
   ULong64_t getUsableAntennas(const RawAnitaHeader * h, const RawAnitaEvent * ev, AnitaPol::AnitaPol_t pol); 

   /**
    * Returns a bitmask of the usable LABs for the given event 
    * Bit 1 corresponds to LAB A, 2 is LAB B, 3 is C, 4 is D
    * Bits 5-8 are currently unused.
    **/
   UChar_t getUsableLabs(const RawAnitaHeader * h, AnitaPol::AnitaPol_t pol);





   /* TODO: 
    *
    *  Some other things that could go in here include: 
    *
    *   - First / last run of a flight
    *   - notch information 
    *   - ALFA info? 
    *   - ??? 
    *
    */ 


}
#endif
