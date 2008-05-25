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
   eventNumber=hdPtr->eventNumber;
   calibStatus=hdPtr->calibStatus;
   priority=hdPtr->priority;
   turfUpperWord=hdPtr->turfUpperWord;
   otherFlag=hdPtr->otherFlag;
   errorFlag=hdPtr->errorFlag;
   otherFlag3=hdPtr->otherFlag3;
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
   upperL1TrigPattern=hdPtr->turfio.upperL1TrigPattern;
   lowerL1TrigPattern=hdPtr->turfio.lowerL1TrigPattern;
   upperL2TrigPattern=hdPtr->turfio.upperL2TrigPattern;
   lowerL2TrigPattern=hdPtr->turfio.lowerL2TrigPattern;
   l3TrigPattern=hdPtr->turfio.l3TrigPattern;
   run=trun;
   realTime=trealTime;
   triggerTime=ttriggerTime;
   triggerTimeNs=ttriggerTimeNs;
   goodTimeFlag=tgoodTimeFlag;
}

RawAnitaHeader::RawAnitaHeader(Int_t    trun,
			       UInt_t   trealTime,
			       UInt_t   tpayloadTime,
			       UInt_t   tpayloadTimeUs,
			       UInt_t   tgpsSubTime,
			       UInt_t   teventNumber,
			       UShort_t tcalibStatus,
			       UChar_t  tpriority,
			       UChar_t  tturfUpperWord,
			       UChar_t  totherFlag,
			       UChar_t  terrorFlag,
			       UChar_t  totherFlag3,
			       UChar_t  tnadirAntMask,
			       UInt_t   tantTrigMask,
			       UChar_t  ttrigType,
			       UChar_t  tl3Type1Count,
			       UShort_t ttrigNum,
			       UInt_t   ttrigTime,
			       UInt_t   tc3poNum,
			       UShort_t tppsNum,
			       UShort_t tdeadTime,
			       UChar_t  tbufferDepth,
			       UShort_t tupperL1TrigPattern,
			       UShort_t tlowerL1TrigPattern,
			       UShort_t tupperL2TrigPattern,
			       UShort_t tlowerL2TrigPattern,
			       UShort_t tl3TrigPattern,
			       UInt_t   ttriggerTime,
			       UInt_t   ttriggerTimeNs,
			       Int_t    tgoodTimeFlag)
{
   payloadTime=tpayloadTime;
   payloadTimeUs=tpayloadTimeUs;
   gpsSubTime=tgpsSubTime;
   eventNumber=teventNumber;
   calibStatus=tcalibStatus;
   priority=tpriority;
   turfUpperWord=tturfUpperWord;
   otherFlag=totherFlag;
   errorFlag=terrorFlag;
   otherFlag3=totherFlag3;
   nadirAntTrigMask=tnadirAntMask;
   antTrigMask=tantTrigMask;
   trigType=ttrigType;
   l3Type1Count=tl3Type1Count;
   trigNum=ttrigNum;
   trigTime=ttrigTime;
   c3poNum=tc3poNum;
   ppsNum=tppsNum;
   deadTime=tdeadTime;
   bufferDepth=tbufferDepth;
   upperL1TrigPattern=tupperL1TrigPattern;
   lowerL1TrigPattern=tlowerL1TrigPattern;
   upperL2TrigPattern=tupperL2TrigPattern;
   lowerL2TrigPattern=tlowerL2TrigPattern;
   l3TrigPattern=tl3TrigPattern;
   run=trun;
   realTime=trealTime;
   triggerTime=ttriggerTime;
   triggerTimeNs=ttriggerTimeNs;
   goodTimeFlag=tgoodTimeFlag;
} 
