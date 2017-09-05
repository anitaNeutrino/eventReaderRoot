//////////////////////////////////////////////////////////////////////////////
/////  RawAnitaEvent.h        Raw ANITA event class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ANITA events in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWANITAEVENT_H
#define RAWANITAEVENT_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"

//!  RawAnitaEvent -- The Raw ANITA Event Data
/*!
  The ROOT implementation of the raw ANITA event data
  \ingroup rootclasses
*/
class RawAnitaEvent: public TObject
{
 public:
   RawAnitaEvent(); ///< Default constructor
   RawAnitaEvent(PedSubbedEventBody_t *psBody); ///< Assignment constructor
   RawAnitaEvent(PedSubbedEventBodyVer11_t *psBody); ///< Ver11 Assignment constructor
   RawAnitaEvent(PedSubbedEventBodyVer10_t *psBody); ///< Ver10 Assignment constructor
   ~RawAnitaEvent(); ///< Destructor

   //Important Stuff
   UInt_t whichPeds; ///< Timestamp of pedestals used in subtraction
   UInt_t eventNumber; ///< Event number from software
  
  //SURF Event Numbers
   UInt_t surfEventId[ACTIVE_SURFS]; ///< SURF Event Id's

  //Header stuff
   UChar_t chanId[NUM_DIGITZED_CHANNELS]; ///< Chan Id -- a bit gratuitous
   //!  chipIdFlag
   /*!
     0:1  LABRADOR chip
     2 RCO Value
     3 HITBUS wrap flag -- 1 if HITBUS wraps around end of sample array
     4-7 HITBUS offset
   */
   UChar_t chipIdFlag[NUM_DIGITZED_CHANNELS];
   //!  First sample of the hitbus 
   /*!
     The last sample in the waveform is [firstHitbus-1] --unless wrapped.
     Otherwise it runs from firstHitbus+1 to lastHitbus-1 inclusive
   */ 
   UChar_t firstHitbus[NUM_DIGITZED_CHANNELS];
   //!  Last sample of the hitbus
   /*!
     The first sample in the waveform is [lastHitbus+1] -- unless wrapped.
     Otherwise it runs from firstHitbus+1 to lastHitbus-1 inclusive
   */
   UChar_t lastHitbus[NUM_DIGITZED_CHANNELS];
  
   Short_t xMax[NUM_DIGITZED_CHANNELS]; ///< Maximum value in ADCs???
   Short_t xMin[NUM_DIGITZED_CHANNELS]; ///< Minimum value in ADCs???
   Float_t mean[NUM_DIGITZED_CHANNELS]; ///< Mean of the waveform
   Float_t rms[NUM_DIGITZED_CHANNELS]; ///< RMS of the waveform
   
   //Waveform Data
   Short_t data[NUM_DIGITZED_CHANNELS][MAX_NUMBER_SAMPLES]; ///< The pedestal subtracted waveform data. Note that these arrays must be unwrapped and calibrated to become UsefulAnitaEvent objects

  Int_t getLabChip(Int_t chanIndex) const
     {return chipIdFlag[chanIndex]&0x3;} ///< Returns the LABRADOR number
  Int_t getRCO(Int_t chanIndex) const
     {return (chipIdFlag[chanIndex]&0x4)>>2;} ///< Returns the RCO phase
  Int_t getFirstHitBus(Int_t chanIndex) const {
     return firstHitbus[chanIndex];
  } ///< Returns the firstHitbus value for the channel
  Int_t getLastHitBus(Int_t chanIndex) const {
     if(lastHitbus[chanIndex]<255) return lastHitbus[chanIndex];
     return Int_t(lastHitbus[chanIndex]) + Int_t((chipIdFlag[chanIndex])>>4);
  } ///< Returns the lastHitbus value for the channel
  Int_t getWrappedHitBus(Int_t chanIndex) const {
     return ((chipIdFlag[chanIndex])&0x8)>>3;
  } ///< Return the wrapped hitbus flag for the channel. When the HITBUS is wrapped the waveform runs from firstHitbus+1 to lastHitbus-1, otherwise it runs from lastHitbus+1 to firstHitbus-1 (crossing the 259-->0 boudnary).

  Int_t getLatestSample(Int_t chanIndex); ///<Returns the latest sample in the waveform
  Int_t getEarliestSample(Int_t chanIndex); ///< Returns the earliest sample in the waveform

  ClassDef(RawAnitaEvent,11);
};


#endif //RAWANITAEVENT_H
