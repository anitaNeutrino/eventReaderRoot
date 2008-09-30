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

class RawAnitaEvent: public TObject
{
 public:
  RawAnitaEvent();
  RawAnitaEvent(PedSubbedEventBody_t *psBody);
  RawAnitaEvent(PedSubbedEventBodyVer11_t *psBody);
  RawAnitaEvent(PedSubbedEventBodyVer10_t *psBody);
  ~RawAnitaEvent();

  //Important Stuff
  UInt_t whichPeds;
  UInt_t eventNumber;
  
  //SURF Event Numbers
  UInt_t surfEventId[ACTIVE_SURFS];

  //Header stuff
  UChar_t chanId[NUM_DIGITZED_CHANNELS];
  UChar_t chipIdFlag[NUM_DIGITZED_CHANNELS];
  UChar_t firstHitbus[NUM_DIGITZED_CHANNELS];
  UChar_t lastHitbus[NUM_DIGITZED_CHANNELS];
  
  Short_t xMax[NUM_DIGITZED_CHANNELS];
  Short_t xMin[NUM_DIGITZED_CHANNELS];
  Float_t mean[NUM_DIGITZED_CHANNELS];
  Float_t rms[NUM_DIGITZED_CHANNELS];

  //Waveform Data
  Short_t data[NUM_DIGITZED_CHANNELS][MAX_NUMBER_SAMPLES];

  Int_t getLabChip(Int_t chanIndex)
     {return chipIdFlag[chanIndex]&0x3;}
  Int_t getRCO(Int_t chanIndex)
    {return (chipIdFlag[chanIndex]&0x4)>>2;}
  Int_t getFirstHitBus(Int_t chanIndex) {
     return firstHitbus[chanIndex];
  }
  Int_t getLastHitBus(Int_t chanIndex) {
     if(lastHitbus[chanIndex]<255) return lastHitbus[chanIndex];
     return Int_t(lastHitbus[chanIndex]) + Int_t((chipIdFlag[chanIndex])>>4);
  }
  Int_t getWrappedHitBus(Int_t chanIndex) {
     return ((chipIdFlag[chanIndex])&0x8)>>3;
  }

  Int_t getLatestSample(Int_t chanIndex);
  Int_t getEarliestSample(Int_t chanIndex);

  ClassDef(RawAnitaEvent,11);
};


#endif //RAWANITAEVENT_H
