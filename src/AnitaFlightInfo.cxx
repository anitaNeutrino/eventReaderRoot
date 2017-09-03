#include "AnitaFlightInfo.h" 
#include "RawAnitaHeader.h" 
#include "AnitaVersion.h" 


static const int a3_tv8_broken_times[][2]=  
{
 {1419680069,1419681327},
 {1419683250,1419684400},
 {1419689971,1419694403},
 {1419716418,1419718574},
 {1419721436,1419726872},
 {1419727792,1419728845},
 {1419729208,1419729855},
 {1419729992,1419731399},
 {1419731536,1419733713},
 {1419734645,1419737950},
 {1419741367,1419743271},
 {1419743711,1419744441},
 {1419745047,1419746926},
 {1419788511,1419854556},
 {1419855046,1419855771},
 {1419856172,1419857217},
 {1419857233,1419874871},
 {1419876947,1419878187},
 {1419881864,1419884272},
 {1419953739,1419973292},
 {1420145946,1420150617},
 {1420150987,1420160266},
 {1420160429,1420174145},
 {1420178247,1420183797},
 {1420185755,1420186493},
 {1420186608,1420187435},
 {1420193226,1420194091},
 {1420196126,1420196942},
 {1420198337,1420199076},
 {1420344711,1420360655},
 {1420427910,1420429019},
 {1420485865,1420488373},
 {1420488595,1420489488},
 {1420489808,1420492112},
 {1420492257,1420496303},
 {1420530908,1420538596},
 {1420616478,1420724913},
 {1420746416,1420751172},
 {1420751477,1420755468},
 {1420767659,1420768513},
 {1420778773,1420779376} 
}; 


static bool timeInBetweenAnyInterval(int t, int N,  const int (* intervals)[2]) 
{
  for (int i = 0; i < N; i++) 
  {
    if (t >= intervals[i][0] && t<= intervals[i][1]) 
    {
      return true; 
    }
  }

  return false; 
}


static int  nagged = 0; 

ULong64_t AnitaFlightInfo::getUsableAntennas(const RawAnitaHeader * h, const RawAnitaEvent * ev, AnitaPol::AnitaPol_t pol) 
{

  int v = AnitaVersion::getVersionFromUnixTime(h->realTime); 


  if (v == 3) 
  {
    if (pol == AnitaPol::kHorizontal) //nothing bad
    {
      return ~(0ul); 
    }
    else //intermittent failure on 8TV . Picked out times when rms below 10 for over 10 mins (but ignored small spikes. 
    {

      if (timeInBetweenAnyInterval(h->triggerTime, sizeof(a3_tv8_broken_times) / sizeof(a3_tv8_broken_times[0]), &a3_tv8_broken_times[0]))
      {
        return ~(1ul <<  7);
      }

      return ~(0ul); 
    }
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


