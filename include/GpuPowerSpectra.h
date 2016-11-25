//////////////////////////////////////////////////////////////////////////////
/////  GpuPowerSpectra.h      GPU Power Spectra                          /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing GPU Power Spectra structs in a tree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef GPUPOWERSPECTRA_H
#define GPUPOWERSPECTRA_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"
#include "TGraph.h"

#define NUM_BINS_GPU_POW_SPEC 99

//!  GpuPowerSpectra -- The GPU Power Specta Data
/*!
  The ROOT implementation of the GPU Power Spectra data
  \ingroup rootclasses
*/
class GpuPowerSpectra: public TObject
{
 public:
   GpuPowerSpectra(); ///<Default constructor
   ~GpuPowerSpectra(); ///< Destructor

   GpuPowerSpectra(Int_t run, Int_t trealTime, GpuPhiSectorPowerSpectrumStruct_t *gpuPtr); ///< Assignment constructor
  TGraph* getGraph(Int_t phi);


   Int_t           run; ///< Run number from offline
   UInt_t          realTime; ///< Time in unixTime
   Short_t powerSpectra[NUM_PHI][NUM_BINS_GPU_POW_SPEC];
   unsigned int unixTimeFirstEvent;
   unsigned int unixTimeLastEvent;
   unsigned int numEventsAveraged;
   unsigned int firstEventInAverage;
  unsigned char pol;
  unsigned char ring;



  ClassDef(GpuPowerSpectra,11);
};


#endif //GPUPOWERSPECTRA_H
