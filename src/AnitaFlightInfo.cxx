#include "AnitaFlightInfo.h" 
#include "RawAnitaHeader.h" 
#include "AnitaVersion.h" 





static int  nagged = 0; 

ULong64_t AnitaFlightInfo::getUsableAntennas(const RawAnitaHeader * h, AnitaPol::AnitaPol_t pol) 
{

  int v = AnitaVersion::getVersionFromUnixTime(h->realTime); 


  if (v == 3) 
  {
    return pol == AnitaPol::kHorizontal ? ~(0ul) : ~( 1ul << 8); 
  }
  if (v == 4) 
  {
    return pol == AnitaPol::kHorizontal ? ~(0ul) : ~( 1ul << 45); 
  }

  if (nagged++ < 10) 
  {
      fprintf(stderr,"getUsableAntennas not implemented yet for ANITA %d, returning all usable\n", v); 
      if (nagged == 10) 
      {
      fprintf(stderr,"  (printed out enough of these warnings, so will stop now)\n"); 

      }
  }

  return ~0ull; 

}




