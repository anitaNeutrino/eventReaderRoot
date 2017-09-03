#include "AnitaFlightInfo.h" 
#include "RawAnitaHeader.h" 
#include "AnitaVersion.h" 





static int  nagged = 0; 

ULong64_t AnitaFlightInfo::getUsableAntennas(const RawAnitaHeader * h, const RawAnitaEvent * ev, AnitaPol::AnitaPol_t pol) 
{

  int v = AnitaVersion::getVersionFromUnixTime(h->realTime); 


  if (v == 3) 
  {
    return pol == AnitaPol::kHorizontal ? ~(0ul) : ~( 1ul << 8); 
  }
  if (v == 4) 
  {
    ULong64_t theAntennas = (pol == AnitaPol::kHorizontal) ? (0ul) : ( 1ul << 45);
		//after this time, lab A had a weird clock problem
		if(h->realTime >= 1482447082 && !(ev->chipIdFlag[0]&0x3))
		{
			theAntennas |= (1ul << 34);
			theAntennas |= (1ul << 38);
			theAntennas |= (1ul << 42);
			theAntennas |= (1ul << 46);
		}
		return ~(theAntennas);
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

UChar_t AnitaFlightInfo::getUsableLabs(const RawAnitaHeader * h, AnitaPol::AnitaPol_t pol) 
{
	int v = AnitaVersion::getVersionFromUnixTime(h->realTime);

	if (v == 3)
	{
		return ~(0);
	}
	if (v == 4)
	{
		if(h->realTime >= 1482447082) return ~(1);
		return ~(0);
	}

	fprintf(stderr, "I don't know that Anita%d\n", v);
	return ~(0);
}


