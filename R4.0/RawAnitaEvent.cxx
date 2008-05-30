//////////////////////////////////////////////////////////////////////////////
/////  RawAnitaEvent.cxx        ANITA event reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ANITA events and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAnitaEvent.h"
#include <iostream>
#include <fstream>

ClassImp(RawAnitaEvent);

RawAnitaEvent::RawAnitaEvent() 
{
   //Default Constructor
}

RawAnitaEvent::~RawAnitaEvent() {
   //Default Destructor
}


RawAnitaEvent::RawAnitaEvent(PedSubbedEventBody_t *bdPtr) 
{
   //Filling Constructor
   whichPeds=bdPtr->whichPeds;
   eventNumber=bdPtr->eventNumber;
   
  for(int chan=0;chan<NUM_DIGITZED_CHANNELS;chan++) {
    chanId[chan]=bdPtr->channel[chan].header.chanId;
    chipIdFlag[chan]=bdPtr->channel[chan].header.chipIdFlag;
    firstHitbus[chan]=bdPtr->channel[chan].header.firstHitbus;
    lastHitbus[chan]=bdPtr->channel[chan].header.lastHitbus;
    
    xMin[chan]=bdPtr->channel[chan].xMin;
    xMax[chan]=bdPtr->channel[chan].xMax;
    mean[chan]=bdPtr->channel[chan].mean;
    rms[chan]=bdPtr->channel[chan].rms;

    memcpy(data[chan],bdPtr->channel[chan].data,sizeof(Short_t)*MAX_NUMBER_SAMPLES);    
  }
}
