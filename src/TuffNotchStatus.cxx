#include "TuffNotchStatus.h" 

#include <stdlib.h>


ClassImp(TuffNotchStatus); 

TuffNotchStatus::TuffNotchStatus(Int_t run, const TuffNotchStatus_t * status )
  : run(run) 
{


  notchSetTime = status->notchSetTime; 
  unixTime = status->unixTime; 
  memcpy(startSectors, status->startSectors, sizeof(startSectors)); 
  memcpy(endSectors, status->endSectors, sizeof(endSectors)); 
  for (int i = 0; i < NUM_RFCM; i++) 
  {
    temperatures[i] = status->temperatures[i];  
  }


}


unsigned short TuffNotchStatus::getNotchedSectors(int i) const
{

  if (startSectors[i] == 16 && endSectors[i] == 16) return 0; 

  unsigned short answer = 0; 
  if (startSectors[i] < endSectors[i])
  {
    for (int j = startSectors[i]; j <= endSectors[i]; j++)
    {
      answer |= (1 << j); 
    }
  }
  else
  {
    for (int j = endSectors[i]; j <= startSectors[i] + 16; j++)
    {
      answer |= (1 <<  (j % 16)); 
    }
  }

  return answer;

}
