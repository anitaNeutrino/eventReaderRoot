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
#include "TMath.h"
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


UShort_t RawAnitaHeader::getL3TrigPattern(AnitaPol::AnitaPol_t pol){
  return pol == AnitaPol::kHorizontal ? l3TrigPatternH : l3TrigPattern;
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



int RawAnitaHeader::isInPhiMaskOffline(int phi, AnitaPol::AnitaPol_t pol)
{
  if(phi<0 || phi>=PHI_SECTORS) return -1;
  switch(pol) {
  case AnitaPol::kVertical:
    return  ((phiTrigMaskOffline&(1<<(phi))) ? 1 :0);
  case AnitaPol::kHorizontal:
    return  ((phiTrigMaskHOffline&(1<<phi)) ? 1 : 0);
  default:
    return -1;
  }
  return -1;

}
int RawAnitaHeader::isInL1MaskOffline(int phi, AnitaPol::AnitaPol_t pol)
{
  if(phi<0 || phi>=PHI_SECTORS) return -1;
  switch(pol) {
  case AnitaPol::kVertical:
    return  ((l1TrigMaskOffline&(1<<(phi))) ? 1 :0);
  case AnitaPol::kHorizontal:
    return  ((l1TrigMaskHOffline&(1<<phi)) ? 1 : 0);
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

Float_t RawAnitaHeader::getPeakThetaDeg()
{
#define THETA_RANGE 150
#define NUM_BINS_THETA 256
  return -1*THETA_RANGE*((Float_t)peakThetaBin/NUM_BINS_THETA - 0.5);

}

Float_t RawAnitaHeader::getPeakThetaRad()
{
  return getPeakThetaDeg()*TMath::DegToRad();
}

Float_t RawAnitaHeader::getPeakPhiDeg()
{
  Int_t phiInd2=(prioritizerStuff&0x7ff)>>1;
  Int_t phiInd=(phiInd2&0x3f);
  Int_t phiSector=(phiInd2>>6);
  const Float_t phiArrayDeg[NUM_SEAVEYS]={0,22.5,45,67.5,90,112.5,135,157.5,180,
					   202.5,225,247.5,270,292.5,315,337.5,
					   0,22.5,45,67.5,90,112.5,135,157.5,180,
					   202.5,225,247.5,270,292.5,315,337.5,
					   0,22.5,45,67.5,90,112.5,135,157.5,180,
					   202.5,225,247.5,270,292.5,315,337.5};
  // const Float_t phiArray[NUM_SEAVEYS]={0.454076, 0.848837, 1.21946, 1.5909, 1.99256, 2.34755,
  // 				 2.70865, 3.069, 3.47166, 3.87134, 4.26593, 4.67822, 5.10608,
  // 				 5.51456, 5.89388, 0.044416,
  // 				 0.387236, 0.778985, 1.17058, 1.56246, 1.965, 2.36015,
  // 				 2.74449, 3.13392, 3.5209, 3.92214, 4.31697, 4.71495,
  // 				 5.10387, 5.4971, 5.88069, 6.28301,
  // 				 0.341474, 0.785398, 1.15377, 1.5708, 1.95866, 2.3562,
  // 				 2.75883, 3.14159, 3.52916, 3.92699, 4.34619, 4.71239,
  // 					      5.1258, 5.49779, 5.87493, 2.30671e-06};
#define NUM_BINS_PHI 64
#define PHI_RANGE 22.5
  return phiArrayDeg[phiSector] + PHI_RANGE*((Float_t)phiInd/NUM_BINS_PHI - 0.5);
}


Float_t RawAnitaHeader::getPeakPhiRad()
{
  return getPeakPhiDeg()*TMath::DegToRad();
}

Float_t RawAnitaHeader::getImagePeak()
{
  return Float_t(imagePeak)/65535.;

}

Float_t RawAnitaHeader::getCoherentSumPeak()
{
  return coherentSumPeak;
}

AnitaPol::AnitaPol_t RawAnitaHeader::getPeakPol()
{
  if(prioritizerStuff&0x1)
    return AnitaPol::kVertical;
  return AnitaPol::kHorizontal;
}

Int_t RawAnitaHeader::getAboveThresholdFlag()
{
  return ((prioritizerStuff & 0x1000)>>12);
}
Int_t RawAnitaHeader::getBinToBinIncreaseFlag()
{
  return ((prioritizerStuff & 0x2000)>>13);
}
Int_t RawAnitaHeader::getSaturationFlag()
{
  return ((prioritizerStuff & 0x4000)>>14);
}


Int_t RawAnitaHeader::getTriggerBitRF() const{
  ///< Bit 0 is RF, 1 is ADU5, 2 is G12, 3 is software/external
  return (trigType & (1 << 0));
}

Int_t RawAnitaHeader::getTriggerBitADU5() const{
  ///< Bit 0 is RF, 1 is ADU5, 2 is G12, 3 is software/external
  return (trigType & (1 << 1));
}

Int_t RawAnitaHeader::getTriggerBitG12() const{
  ///< Bit 0 is RF, 1 is ADU5, 2 is G12, 3 is software/external
  return (trigType & (1 << 2));
}

Int_t RawAnitaHeader::getTriggerBitSoftExt() const{
  ///< Bit 0 is RF, 1 is ADU5, 2 is G12, 3 is software/external
  return (trigType & (1 << 3));
}
