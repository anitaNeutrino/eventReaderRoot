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
  for(int phi=0;phi<NUM_PHI;phi++) {
    memcpy(&powerSpectra[phi][0],&gpuPtr->powSpectra[phi].bins[0],NUM_BINS_GPU_POW_SPEC*sizeof(UChar_t));
  }
  unixTimeFirstEvent=gpuPtr->unixTimeFirstEvent;
  unixTimeLastEvent=gpuPtr->unixTimeLastEvent;
  numEventsAveraged=gpuPtr->numEventsAveraged;
  firstEventInAverage=gpuPtr->firstEventInAverage;
  pol=gpuPtr->pol;
  ring=gpuPtr->ring;
}


TGraph* GpuPowerSpectra::getGraph(Int_t phi){


  const double floatToUCharConversionForPacket = 255./60;

  // 256 samples per channel on the GPU at the time of writing (ANITA-4)
  const double deltaF_MHz = 2.6e3/256;
  std::vector<double> freqs(NUM_BINS_GPU_POW_SPEC);
  std::vector<double> power(NUM_BINS_GPU_POW_SPEC);
  const int freqBinOffset = 20;
  for(int freqInd = 0; freqInd < NUM_BINS_GPU_POW_SPEC; freqInd++){
    freqs.at(freqInd) = (freqInd+freqBinOffset)*deltaF_MHz;
    power.at(freqInd) = float(powerSpectra[phi][freqInd])/floatToUCharConversionForPacket;
    // power.at(freqInd)
  }

  return new TGraph(NUM_BINS_GPU_POW_SPEC, &freqs[0], &power[0]);

}
