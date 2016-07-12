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


   Int_t           run; ///< Run number from offline
   UInt_t          realTime; ///< Time in unixTime
   Short_t powerSpectra[NUM_ANTENNA_RINGS][2][99];
   unsigned int unixTimeFirstEvent;
   unsigned int unixTimeLastEvent;
   unsigned int numEventsAveraged;
   unsigned int firstEventInAverage;
   unsigned char phiSector;
   unsigned char nothing;



  ClassDef(GpuPowerSpectra,10);
};


#endif //GPUPOWERSPECTRA_H
