//////////////////////////////////////////////////////////////////////////////
/////  RawAnitaHeader.cxx        ANITA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ANITA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAnitaHeader.h"
#include <iostream>
#include <fstream>

ClassImp(RawAnitaHeader);

RawAnitaHeader::RawAnitaHeader() 
{
   //Default Constructor
}

RawAnitaHeader::~RawAnitaHeader() {
   //Default Destructor
}


RawAnitaHeader::RawAnitaHeader(AnitaEventHeader_t *hdPtr, Int_t trun, UInt_t trealTime,
			       UInt_t ttriggerTime, UInt_t ttriggerTimeNs, Int_t tgoodTimeFlag)
{
   payloadTime=hdPtr->unixTime;
   payloadTimeUs=hdPtr->unixTimeUs;
   gpsSubTime=hdPtr->gpsSubTime;
   turfEventId=hdPtr->turfEventId;
   eventNumber=hdPtr->eventNumber;
   calibStatus=hdPtr->calibStatus;
   priority=hdPtr->priority;
   turfUpperWord=hdPtr->turfUpperWord;
   otherFlag=hdPtr->otherFlag;
   errorFlag=hdPtr->errorFlag;
   surfSlipFlag=hdPtr->surfSlipFlag;
   nadirAntTrigMask=hdPtr->nadirAntTrigMask;
   antTrigMask=hdPtr->antTrigMask;
   trigType=hdPtr->turfio.trigType;
   l3Type1Count=hdPtr->turfio.l3Type1Count;
   trigNum=hdPtr->turfio.trigNum;
   trigTime=hdPtr->turfio.trigTime;
   c3poNum=hdPtr->turfio.c3poNum;
   ppsNum=hdPtr->turfio.ppsNum;
   deadTime=hdPtr->turfio.deadTime;
   bufferDepth=hdPtr->turfio.bufferDepth;
   reserved=hdPtr->turfio.reserved;
   upperL1TrigPattern=hdPtr->turfio.upperL1TrigPattern;
   lowerL1TrigPattern=hdPtr->turfio.lowerL1TrigPattern;
   upperL2TrigPattern=hdPtr->turfio.upperL2TrigPattern;
   lowerL2TrigPattern=hdPtr->turfio.lowerL2TrigPattern;
   l3TrigPattern=hdPtr->turfio.l3TrigPattern;
   memcpy(otherTrigPattern,hdPtr->turfio.otherTrigPattern,3*sizeof(UShort_t));
   nadirL1TrigPattern=hdPtr->turfio.nadirL1TrigPattern;
   nadirL2TrigPattern=hdPtr->turfio.nadirL2TrigPattern;
   run=trun;
   realTime=trealTime;
   triggerTime=ttriggerTime;
   triggerTimeNs=ttriggerTimeNs;
   goodTimeFlag=tgoodTimeFlag;
}


char *RawAnitaHeader::trigTypeAsString()
{
   static char theString[20];
   int count=0;
   
   char *trigTypes[4]={"RF","PPS1","PPS2","Soft"};
   
   for(int i=0;i<4;i++) {
      if(trigType & (1<<i)) {
	 if(count==0) {
	    strcpy(theString,trigTypes[i]);
	    count++;
	 }
	 else {
	    sprintf(theString,"%s + %s",theString,trigTypes[i]);
	 }
      }
   }
   return theString;
}
