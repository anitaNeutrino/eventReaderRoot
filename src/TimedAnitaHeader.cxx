//////////////////////////////////////////////////////////////////////////////
/////  TimedAnitaHeader.cxx        ANITA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing ANITA headers with fixed times      /////
/////     (from matching to the ADU5 data)                               /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "TimedAnitaHeader.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(TimedAnitaHeader);

TimedAnitaHeader::TimedAnitaHeader() 
{
   //Default Constructor
}

TimedAnitaHeader::~TimedAnitaHeader() {
   //Default Destructor
}


TimedAnitaHeader::TimedAnitaHeader(AnitaEventHeader_t *hdPtr, Int_t trun, UInt_t trealTime, UInt_t ttriggerTime, UInt_t ttriggerTimeNs, Int_t tgoodTimeFlag, Int_t tsecQualFlag, UInt_t toldTriggerTime)
  : RawAnitaHeader(hdPtr,trun,trealTime,ttriggerTime,ttriggerTimeNs,tgoodTimeFlag)
{
  secQualFlag=tsecQualFlag;
  oldTriggerTime=toldTriggerTime;
}


TimedAnitaHeader::TimedAnitaHeader(RawAnitaHeader &old, UInt_t newTriggerTime, Int_t tsecQualFlag)
  :RawAnitaHeader(old)
{
  oldTriggerTime=triggerTime;
  triggerTime=newTriggerTime;
  secQualFlag=tsecQualFlag;
}
