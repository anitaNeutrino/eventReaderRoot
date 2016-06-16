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
#include <cstring>

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

RawAnitaEvent::RawAnitaEvent(PedSubbedEventBodyVer11_t *bdPtr) 
{
  if(bdPtr->gHdr.code!=PACKET_PED_SUBBED_EVENT ||
     bdPtr->gHdr.verId!=11 ||
     bdPtr->gHdr.numBytes!=sizeof(PedSubbedEventBodyVer11_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << bdPtr->gHdr.code << "\t" << PACKET_PED_SUBBED_EVENT 
	      << "\nversion:\t" << bdPtr->gHdr.verId 
	      << "\t" << 11 
	      << "\nsize:\t" << bdPtr->gHdr.numBytes << "\t"
	      << sizeof(PedSubbedEventBodyVer11_t) << std::endl;
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

RawAnitaEvent::RawAnitaEvent(PedSubbedEventBodyVer10_t *bdPtr) 
{
  if(bdPtr->gHdr.code!=PACKET_PED_SUBBED_EVENT ||
     bdPtr->gHdr.verId!=10 ||
     bdPtr->gHdr.numBytes!=sizeof(PedSubbedEventBodyVer10_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << bdPtr->gHdr.code << "\t" << PACKET_PED_SUBBED_EVENT 
	      << "\nversion:\t" << bdPtr->gHdr.verId 
	      << "\t" << 10 
	      << "\nsize:\t" << bdPtr->gHdr.numBytes << "\t"
	      << sizeof(PedSubbedEventBodyVer10_t) << std::endl;
  }

   //Filling Constructor
   whichPeds=bdPtr->whichPeds;
   eventNumber=bdPtr->eventNumber;
   memset(surfEventId,0,ACTIVE_SURFS*sizeof(UInt_t));
   
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

Int_t RawAnitaEvent::getEarliestSample(Int_t chanIndex)
{
 Int_t lastHitBus=this->getLastHitBus(chanIndex);
 Int_t firstHitbus=this->getFirstHitBus(chanIndex);
 Int_t wrappedHitBus=this->getWrappedHitBus(chanIndex);
 Int_t earliestSample=0;
 if(!wrappedHitBus) {
   earliestSample=lastHitBus+1;
 }
 else {
   earliestSample=firstHitbus+1;
 }
 if(earliestSample==0) earliestSample=1;
 if(earliestSample<260) return earliestSample;
 return 1;
}

Int_t RawAnitaEvent::getLatestSample(Int_t chanIndex)
{
 Int_t lastHitBus=this->getLastHitBus(chanIndex);
 Int_t firstHitbus=this->getFirstHitBus(chanIndex);
 Int_t wrappedHitBus=this->getWrappedHitBus(chanIndex);
 Int_t latestSample=259;
 if(!wrappedHitBus) {
   latestSample=firstHitbus-1;
 }
 else {
   latestSample=lastHitBus-1;
 }
 if(latestSample>0) return latestSample;
 return 259;
}

