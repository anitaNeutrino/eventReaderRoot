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
  if(bdPtr->gHdr.code!=PACKET_PED_SUBBED_EVENT ||
     bdPtr->gHdr.verId!=VER_PEDSUBBED_EVENT_BODY ||
     bdPtr->gHdr.numBytes!=sizeof(PedSubbedEventBody_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << bdPtr->gHdr.code << "\t" << PACKET_PED_SUBBED_EVENT 
	      << "\nversion:\t" << bdPtr->gHdr.verId 
	      << "\t" << VER_PEDSUBBED_EVENT_BODY 
	      << "\nsize:\t" << bdPtr->gHdr.numBytes << "\t"
	      << sizeof(PedSubbedEventBody_t) << std::endl;
  }

   //Filling Constructor
   whichPeds=bdPtr->whichPeds;
   eventNumber=bdPtr->eventNumber;
   memcpy(surfEventId,bdPtr->surfEventId,ACTIVE_SURFS*sizeof(UInt_t));
   
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
