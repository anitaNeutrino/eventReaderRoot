#ifndef ANITA_FLIGHT_INFO_H 
#define ANITA_FLIGHT_INFO_H 


/* Information about flights (e.g. what channels are broken, etc.) */ 

#include "AnitaConventions.h" 
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
   ULong64_t getUsableAntennas(const RawAnitaHeader *h, AnitaPol::AnitaPol_t pol); 






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
