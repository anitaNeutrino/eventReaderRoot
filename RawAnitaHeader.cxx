//////////////////////////////////////////////////////////////////////////////
/////  RawAnitaHeader.cxx        ANITA header reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ANITA headers and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawAnitaHeader.h"
#include "AnitaPacketUtil.h"
#include <iostream>
#include <fstream>
#include <cstring>
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
 if(hdPtr->gHdr.code!=PACKET_HD ||
     hdPtr->gHdr.verId!=VER_EVENT_HEADER ||
     hdPtr->gHdr.numBytes!=sizeof(AnitaEventHeader_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_HD) << "\n" 
	      << "code:\t" << hdPtr->gHdr.code << "\t" << PACKET_HD 
	      << "\nversion:\t" << (int)hdPtr->gHdr.verId 
	      << "\t" << (int)VER_EVENT_HEADER 
	      << "\nsize:\t" << hdPtr->gHdr.numBytes << "\t"
	      << sizeof(AnitaEventHeader_t) << std::endl;
  }

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
   //   nadirAntTrigMask=hdPtr->nadirAntTrigMask;
   l1TrigMask=hdPtr->l1TrigMask;
   l1TrigMaskH=hdPtr->l1TrigMaskH;
   phiTrigMask=hdPtr->phiTrigMask;
   phiTrigMaskH=hdPtr->phiTrigMaskH;
   trigType=hdPtr->turfio.trigType;
   l3Type1Count=hdPtr->turfio.l3Type1Count;
   trigNum=hdPtr->turfio.trigNum;
   trigTime=hdPtr->turfio.trigTime;
   c3poNum=hdPtr->turfio.c3poNum;
   ppsNum=hdPtr->turfio.ppsNum;
   deadTime=hdPtr->turfio.deadTime;
   bufferDepth=hdPtr->turfio.bufferDepth;
   turfioReserved=hdPtr->turfio.reserved[0];
   l3TrigPattern=hdPtr->turfio.l3TrigPattern;
   l3TrigPatternH=hdPtr->turfio.l3TrigPatternH;
   //   memcpy(reserved,hdPtr->reserved,2*sizeof(UChar_t));
   run=trun;
   realTime=trealTime;
   triggerTime=ttriggerTime;
   triggerTimeNs=ttriggerTimeNs;
   goodTimeFlag=tgoodTimeFlag;


  //Prioritizer stuff
  peakThetaBin=hdPtr->peakThetaBin;
  imagePeak=hdPtr->imagePeak;
  coherentSumPeak=hdPtr->coherentSumPeak;
  prioritizerStuff=hdPtr->prioritizerStuff;
}



RawAnitaHeader::RawAnitaHeader(AnitaEventHeaderVer30_t *hdPtr, Int_t trun, UInt_t trealTime,
			       UInt_t ttriggerTime, UInt_t ttriggerTimeNs, Int_t tgoodTimeFlag)
{
 if(hdPtr->gHdr.code!=PACKET_HD ||
     hdPtr->gHdr.verId!=VER_EVENT_HEADER ||
     hdPtr->gHdr.numBytes!=sizeof(AnitaEventHeader_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_HD) << "\n" 
	      << "code:\t" << hdPtr->gHdr.code << "\t" << PACKET_HD 
	      << "\nversion:\t" << (int)hdPtr->gHdr.verId 
	      << "\t" << (int)VER_EVENT_HEADER 
	      << "\nsize:\t" << hdPtr->gHdr.numBytes << "\t"
	      << sizeof(AnitaEventHeader_t) << std::endl;
  }

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
   phiTrigMask=hdPtr->phiTrigMask;
   trigType=hdPtr->turfio.trigType;
   l3Type1Count=hdPtr->turfio.l3Type1Count;
   trigNum=hdPtr->turfio.trigNum;
   trigTime=hdPtr->turfio.trigTime;
   c3poNum=hdPtr->turfio.c3poNum;
   ppsNum=hdPtr->turfio.ppsNum;
   deadTime=hdPtr->turfio.deadTime;
   bufferDepth=hdPtr->turfio.bufferDepth;
   turfioReserved=hdPtr->turfio.reserved;
   upperL1TrigPattern=hdPtr->turfio.upperL1TrigPattern;
   lowerL1TrigPattern=hdPtr->turfio.lowerL1TrigPattern;
   upperL2TrigPattern=hdPtr->turfio.upperL2TrigPattern;
   lowerL2TrigPattern=hdPtr->turfio.lowerL2TrigPattern;
   l3TrigPattern=hdPtr->turfio.l3TrigPattern;
   memcpy(reserved,hdPtr->reserved,2*sizeof(UChar_t));
   memcpy(otherTrigPattern,hdPtr->turfio.otherTrigPattern,3*sizeof(UShort_t));
   nadirL1TrigPattern=hdPtr->turfio.nadirL1TrigPattern;
   nadirL2TrigPattern=hdPtr->turfio.nadirL2TrigPattern;
   run=trun;
   realTime=trealTime;
   triggerTime=ttriggerTime;
   triggerTimeNs=ttriggerTimeNs;
   goodTimeFlag=tgoodTimeFlag;
}




RawAnitaHeader::RawAnitaHeader(AnitaEventHeaderVer13_t *hdPtr, Int_t trun, UInt_t trealTime,
			       UInt_t ttriggerTime, UInt_t ttriggerTimeNs, Int_t tgoodTimeFlag)
{
 if(hdPtr->gHdr.code!=PACKET_HD ||
     hdPtr->gHdr.verId!=13 ||
     hdPtr->gHdr.numBytes!=sizeof(AnitaEventHeaderVer13_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_HD) << "\n" 
	      << "code:\t" << hdPtr->gHdr.code << "\t" << PACKET_HD 
	      << "\nversion:\t" << hdPtr->gHdr.verId 
	      << "\t" << 13
	      << "\nsize:\t" << hdPtr->gHdr.numBytes << "\t"
	      << sizeof(AnitaEventHeaderVer13_t) << std::endl;
  }

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
   phiTrigMask=hdPtr->phiTrigMask;
   phiTrigMaskH=0;//hdPtr->phiTrigMaskH;
   trigType=hdPtr->turfio.trigType;
   l3Type1Count=hdPtr->turfio.l3Type1Count;
   trigNum=hdPtr->turfio.trigNum;
   trigTime=hdPtr->turfio.trigTime;
   c3poNum=hdPtr->turfio.c3poNum;
   ppsNum=hdPtr->turfio.ppsNum;
   deadTime=hdPtr->turfio.deadTime;
   bufferDepth=hdPtr->turfio.bufferDepth;
     turfioReserved=hdPtr->turfio.reserved;
   upperL1TrigPattern=hdPtr->turfio.upperL1TrigPattern;
   lowerL1TrigPattern=hdPtr->turfio.lowerL1TrigPattern;
   upperL2TrigPattern=hdPtr->turfio.upperL2TrigPattern;
   lowerL2TrigPattern=hdPtr->turfio.lowerL2TrigPattern;
   l3TrigPattern=hdPtr->turfio.l3TrigPattern;
   memcpy(reserved,hdPtr->reserved,2*sizeof(UChar_t));
     memcpy(otherTrigPattern,hdPtr->turfio.otherTrigPattern,3*sizeof(UShort_t));
     nadirL1TrigPattern=hdPtr->turfio.nadirL1TrigPattern;
     nadirL2TrigPattern=hdPtr->turfio.nadirL2TrigPattern;
   run=trun;
   realTime=trealTime;
   triggerTime=ttriggerTime;
   triggerTimeNs=ttriggerTimeNs;
   goodTimeFlag=tgoodTimeFlag;
}


RawAnitaHeader::RawAnitaHeader(AnitaEventHeaderVer12_t *hdPtr, Int_t trun, UInt_t trealTime,
			       UInt_t ttriggerTime, UInt_t ttriggerTimeNs, Int_t tgoodTimeFlag)
{
 if(hdPtr->gHdr.code!=PACKET_HD ||
     hdPtr->gHdr.verId!=12 ||
     hdPtr->gHdr.numBytes!=sizeof(AnitaEventHeaderVer12_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_HD) << "\n" 
	      << "code:\t" << hdPtr->gHdr.code << "\t" << PACKET_HD 
	      << "\nversion:\t" << hdPtr->gHdr.verId 
	      << "\t" << 12
	      << "\nsize:\t" << hdPtr->gHdr.numBytes << "\t"
	      << sizeof(AnitaEventHeaderVer12_t) << std::endl;
  }

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
   phiTrigMask=0;
   phiTrigMaskH=0;
   trigType=hdPtr->turfio.trigType;
   l3Type1Count=hdPtr->turfio.l3Type1Count;
   trigNum=hdPtr->turfio.trigNum;
   trigTime=hdPtr->turfio.trigTime;
   c3poNum=hdPtr->turfio.c3poNum;
   ppsNum=hdPtr->turfio.ppsNum;
   deadTime=hdPtr->turfio.deadTime;
   bufferDepth=hdPtr->turfio.bufferDepth;
     turfioReserved=hdPtr->turfio.reserved;
   upperL1TrigPattern=hdPtr->turfio.upperL1TrigPattern;
   lowerL1TrigPattern=hdPtr->turfio.lowerL1TrigPattern;
   upperL2TrigPattern=hdPtr->turfio.upperL2TrigPattern;
   lowerL2TrigPattern=hdPtr->turfio.lowerL2TrigPattern;
   l3TrigPattern=hdPtr->turfio.l3TrigPattern;
   memset(reserved,0,2*sizeof(UChar_t));
     memcpy(otherTrigPattern,hdPtr->turfio.otherTrigPattern,3*sizeof(UShort_t));
     nadirL1TrigPattern=hdPtr->turfio.nadirL1TrigPattern;
     nadirL2TrigPattern=hdPtr->turfio.nadirL2TrigPattern;
   run=trun;
   realTime=trealTime;
   triggerTime=ttriggerTime;
   triggerTimeNs=ttriggerTimeNs;
   goodTimeFlag=tgoodTimeFlag;
}



RawAnitaHeader::RawAnitaHeader(AnitaEventHeaderVer11_t *hdPtr, Int_t trun, UInt_t trealTime,
			       UInt_t ttriggerTime, UInt_t ttriggerTimeNs, Int_t tgoodTimeFlag)
{
 if(hdPtr->gHdr.code!=PACKET_HD ||
     hdPtr->gHdr.verId!=11 ||
     hdPtr->gHdr.numBytes!=sizeof(AnitaEventHeaderVer11_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_HD) << "\n" 
	      << "code:\t" << hdPtr->gHdr.code << "\t" << PACKET_HD 
	      << "\nversion:\t" << hdPtr->gHdr.verId 
	      << "\t" << 11
	      << "\nsize:\t" << hdPtr->gHdr.numBytes << "\t"
	      << sizeof(AnitaEventHeaderVer11_t) << std::endl;
  }

   payloadTime=hdPtr->unixTime;
   payloadTimeUs=hdPtr->unixTimeUs;
   gpsSubTime=hdPtr->gpsSubTime;
   turfEventId=0;
   eventNumber=hdPtr->eventNumber;
   calibStatus=hdPtr->calibStatus;
   priority=hdPtr->priority;
   turfUpperWord=hdPtr->turfUpperWord;
   otherFlag=hdPtr->otherFlag;
   errorFlag=hdPtr->errorFlag;
   surfSlipFlag=0;
   nadirAntTrigMask=hdPtr->nadirAntTrigMask;
   antTrigMask=hdPtr->antTrigMask;
   phiTrigMask=0;
   phiTrigMaskH=0;
   trigType=hdPtr->turfio.trigType;
   l3Type1Count=hdPtr->turfio.l3Type1Count;
   trigNum=hdPtr->turfio.trigNum;
   trigTime=hdPtr->turfio.trigTime;
   c3poNum=hdPtr->turfio.c3poNum;
   ppsNum=hdPtr->turfio.ppsNum;
   deadTime=hdPtr->turfio.deadTime;
   bufferDepth=hdPtr->turfio.bufferDepth;
   turfioReserved=hdPtr->turfio.reserved;
   upperL1TrigPattern=hdPtr->turfio.upperL1TrigPattern;
   lowerL1TrigPattern=hdPtr->turfio.lowerL1TrigPattern;
   upperL2TrigPattern=hdPtr->turfio.upperL2TrigPattern;
   lowerL2TrigPattern=hdPtr->turfio.lowerL2TrigPattern;
   l3TrigPattern=hdPtr->turfio.l3TrigPattern;
   memset(reserved,0,2*sizeof(UChar_t));
   run=trun;
   realTime=trealTime;
   triggerTime=ttriggerTime;
   triggerTimeNs=ttriggerTimeNs;
   goodTimeFlag=tgoodTimeFlag;
}



RawAnitaHeader::RawAnitaHeader(AnitaEventHeaderVer10_t *hdPtr, Int_t trun, UInt_t trealTime,
			       UInt_t ttriggerTime, UInt_t ttriggerTimeNs, Int_t tgoodTimeFlag)
{
 if(hdPtr->gHdr.code!=PACKET_HD ||
     hdPtr->gHdr.verId!=10 ||
     hdPtr->gHdr.numBytes!=sizeof(AnitaEventHeaderVer10_t)) {
    std::cerr << "Mismatched packet:\t" << packetCodeAsString(PACKET_HD) << "\n" 
	      << "code:\t" << hdPtr->gHdr.code << "\t" << PACKET_HD 
	      << "\nversion:\t" << hdPtr->gHdr.verId 
	      << "\t" << 10
	      << "\nsize:\t" << hdPtr->gHdr.numBytes << "\t"
	      << sizeof(AnitaEventHeaderVer10_t) << std::endl;
  }

   payloadTime=hdPtr->unixTime;
   payloadTimeUs=hdPtr->unixTimeUs;
   gpsSubTime=hdPtr->gpsSubTime;
   turfEventId=0;
   eventNumber=hdPtr->eventNumber;
   calibStatus=hdPtr->calibStatus;
   priority=hdPtr->priority;
   turfUpperWord=hdPtr->turfUpperWord;
   otherFlag=hdPtr->otherFlag;
   errorFlag=0;
   surfSlipFlag=0;
   nadirAntTrigMask=0;
   antTrigMask=hdPtr->antTrigMask;
   phiTrigMask=0;
   phiTrigMaskH=0;
   trigType=hdPtr->turfio.trigType;
   l3Type1Count=hdPtr->turfio.l3Type1Count;
   trigNum=hdPtr->turfio.trigNum;
   trigTime=hdPtr->turfio.trigTime;
   c3poNum=hdPtr->turfio.c3poNum;
   ppsNum=hdPtr->turfio.ppsNum;
   deadTime=hdPtr->turfio.deadTime;
   bufferDepth=hdPtr->turfio.bufferDepth;
   turfioReserved=hdPtr->turfio.reserved;
   upperL1TrigPattern=hdPtr->turfio.upperL1TrigPattern;
   lowerL1TrigPattern=hdPtr->turfio.lowerL1TrigPattern;
   upperL2TrigPattern=hdPtr->turfio.upperL2TrigPattern;
   lowerL2TrigPattern=hdPtr->turfio.lowerL2TrigPattern;
   l3TrigPattern=hdPtr->turfio.l3TrigPattern;
   memset(reserved,0,2*sizeof(UChar_t));
   run=trun;
   realTime=trealTime;
   triggerTime=ttriggerTime;
   triggerTimeNs=ttriggerTimeNs;
   goodTimeFlag=tgoodTimeFlag;
}


const char *RawAnitaHeader::trigTypeAsString()
{
   static char theString[20];
   int count=0;
   sprintf(theString,"None");
   
   const char *trigTypes[4]={"RF","PPS1","PPS2","Soft"};
   
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


 
int RawAnitaHeader::isInL3Pattern(int phi, AnitaPol::AnitaPol_t pol)
{ 
  if(phi<0 || phi>=PHI_SECTORS) return -1;
  switch(pol) {
  case AnitaPol::kVertical:
    return  ((l3TrigPattern&(1<<(phi))) ? 1 :0);
  case AnitaPol::kHorizontal:
    return  ((l3TrigPatternH&(1<<phi)) ? 1 : 0);
  default:
    return -1;
  }      
  return -1;
  
}
int RawAnitaHeader::isInPhiMask(int phi, AnitaPol::AnitaPol_t pol)
{ 
  if(phi<0 || phi>=PHI_SECTORS) return -1;
  switch(pol) {
  case AnitaPol::kVertical:
    return  ((phiTrigMask&(1<<(phi))) ? 1 :0);
  case AnitaPol::kHorizontal:
    return  ((phiTrigMaskH&(1<<phi)) ? 1 : 0);
  default:
    return -1;
  }      
  return -1;
  
}
int RawAnitaHeader::isInL1Mask(int phi, AnitaPol::AnitaPol_t pol)
{ 
  if(phi<0 || phi>=PHI_SECTORS) return -1;
  switch(pol) {
  case AnitaPol::kVertical:
    return  ((l1TrigMask&(1<<(phi))) ? 1 :0);
  case AnitaPol::kHorizontal:
    return  ((l1TrigMaskH&(1<<phi)) ? 1 : 0);
  default:
    return -1;
  }      
  return -1;
  
}

int RawAnitaHeader::getCurrentTurfBuffer()
///< Returns the current TURF buffer number (0, 1, 2 or 3);
{
  int curBuf=reserved[0]&0xf;
  switch(curBuf) {
  case 1: return 0;
  case 2: return 1;
  case 4: return 2;
  case 8: return 3;
  default: return -1;
  }
}

unsigned int RawAnitaHeader::getCurrentTurfHolds()
///< Returns a 4-bit bitmask corresponding to the currently held buffers.
{

  unsigned int curHolds=(reserved[0]&0xf)>>4;
  return curHolds;
}

int RawAnitaHeader::getNumberOfCurrentTurfHolds()
///< Returns the number of currently held TURF buffers (0-4)
{
  int countHolds=0;
  unsigned int curHolds=(reserved[0]&0xf)>>4;
  for(int buffer=0;buffer<4;buffer++) {
    if(curHolds & (1<<buffer))
      countHolds++;
  }
  return countHolds;  
}
