//////////////////////////////////////////////////////////////////////////////
/////  TimedAnitaHeader.h        Timed ANITA header class                /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing ANITA headers with fixed times      /////
/////     (with various corrections)                                     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
/////  Author: Cosmin Deacnou (cozzyd@kicp.uchicago.edu)                 /////
//////////////////////////////////////////////////////////////////////////////
//
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



TimedAnitaHeader::TimedAnitaHeader(const RawAnitaHeader &old, UInt_t correctedTriggerTime, UInt_t correctedTriggerTimeNs, Double_t tttAlignment)
  : RawAnitaHeader(old)
{
  oldTriggerTime=triggerTime;
  oldTriggerTimeNs=triggerTimeNs;
  triggerTime=correctedTriggerTime;
  triggerTimeNs=correctedTriggerTimeNs;
  tttAlignmentFraction = tttAlignment; 
}
