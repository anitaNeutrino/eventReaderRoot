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
  ~RawAnitaEvent();

  //Important Stuff
  UInt_t whichPeds;
  UInt_t eventNumber;
  
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
  
  ClassDef(RawAnitaEvent,1);
};


#endif //RAWANITAEVENT_H
