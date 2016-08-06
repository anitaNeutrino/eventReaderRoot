//////////////////////////////////////////////////////////////////////////////
/////  GpuPowerSpectra.h        GPU Power Spectra                         /////
/////                                                                     /////
/////  Description:                                                       /////
/////     A simple class for storing GPU Power Spectra structs in a tree  /////
/////  Author: Ryan Nichol (r.nichol@ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "GpuPowerSpectra.h"
#include "AnitaPacketUtil.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(GpuPowerSpectra);

GpuPowerSpectra::GpuPowerSpectra() 
{
   //Default Constructor
}

GpuPowerSpectra::~GpuPowerSpectra() {
   //Default Destructor
}



// typedef struct {
//   short bins[99];
// } GpuAnitaBandPowerSpectrumStruct_t;

// typedef struct {
//   GenericHeader_t gHdr;
//   unsigned int unixTimeFirstEvent;
//   unsigned int unixTimeLastEvent;
//   unsigned int numEventsAveraged;
//   unsigned int firstEventInAverage;
//   unsigned char phiSector;
//   unsigned char nothing;
//   GpuAnitaBandPowerSpectrumStruct_t powSpectra[NUM_ANTENNA_RINGS][2];
// } GpuPhiSectorPowerSpectrumStruct_t;

GpuPowerSpectra::GpuPowerSpectra(Int_t trun, Int_t trealTime, GpuPhiSectorPowerSpectrumStruct_t *gpuPtr)
{
  simplePacketCheck(&(gpuPtr->gHdr),PACKET_GPU_AVE_POW_SPEC);

  run=trun;
  realTime=trealTime;
  for(int ring=0;ring<NUM_ANTENNA_RINGS;ring++) {
    for(int pol=0;pol<2;pol++) {
      memcpy(&powerSpectra[ring][pol][0],&gpuPtr->powSpectra[ring][pol].bins[0],99*sizeof(Short_t));
    }
  }
  unixTimeFirstEvent=gpuPtr->unixTimeFirstEvent;
  unixTimeLastEvent=gpuPtr->unixTimeLastEvent;
  numEventsAveraged=gpuPtr->numEventsAveraged;
  firstEventInAverage=gpuPtr->firstEventInAverage;
  phiSector=gpuPtr->phiSector;
  nothing=gpuPtr->nothing;
}

