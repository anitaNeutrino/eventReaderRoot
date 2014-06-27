//
// Body for AnitaPacketUtil class so that CINT recognizes its existence
//
#include "AnitaPacketUtil.h"
#include "AnitaGeomTool.h"
#include "simpleStructs.h"
#include "compress/mulawTables.h"
#include <iostream>
#include <zlib.h>
#include <cstring>





unsigned int simpleIntCrc(unsigned int *p, unsigned int n)
{
  unsigned int sum = 0;
  unsigned int i;
  for (i=0L; i<n; i++) {
    //
    sum += *p++;
  }
  //  printf("%u %u\n",*p,sum);
  return ((0xffffffff - sum) + 1);
}




void fillGenericHeader(void *thePtr, PacketCode_t code, unsigned short numBytes) {
  //  printf("sizeof(GenericHeader_t):\t%d\n",sizeof(GenericHeader_t));
  //  printf("numBytes %d\n",numBytes);
  unsigned int *typedPtr = (unsigned int *)thePtr;
  unsigned int intBytes=(numBytes-sizeof(GenericHeader_t))/4;
  GenericHeader_t *gHdr= (GenericHeader_t*)typedPtr;
  unsigned long dodgyNum=(unsigned long)typedPtr;
  dodgyNum+=sizeof(GenericHeader_t);
  unsigned int *dataPtr=(unsigned int*)dodgyNum;
  //  unsigned int *dataPtr=(unsigned int*) (typedPtr+sizeof(GenericHeader_t));  
  //  printf("%u -- %u -- %u\n",thePtr,dataPtr,((unsigned int)(dataPtr)-(unsigned int)(thePtr)));
  gHdr->code=code;
  gHdr->numBytes=numBytes;
  gHdr->feByte=0xfe;
  switch(code&BASE_PACKET_MASK) {
  case PACKET_BD: gHdr->verId=VER_EVENT_BODY; break;
  case PACKET_HD: gHdr->verId=VER_EVENT_HEADER; break;
  case PACKET_HD_SLAC: gHdr->verId=SLAC_VER_EVENT_HEADER; break;
  case PACKET_WV: gHdr->verId=VER_WAVE_PACKET; break;
  case PACKET_SURF: gHdr->verId=VER_SURF_PACKET; break;
  case PACKET_SURF_HK: gHdr->verId=VER_SURF_HK; break;
  case PACKET_TURF_RATE: gHdr->verId=VER_TURF_RATE; break;
  case PACKET_AVG_SURF_HK: gHdr->verId=VER_AVG_SURF_HK; break;
  case PACKET_SUM_TURF_RATE: gHdr->verId=VER_SUM_TURF_RATE; break;
  case PACKET_TURF_REGISTER: gHdr->verId=VER_TURF_REG; break;
  case PACKET_LAB_PED: gHdr->verId=VER_LAB_PED; break;
  case PACKET_FULL_PED: gHdr->verId=VER_FULL_PED; break;
  case PACKET_ENC_WV_PEDSUB: gHdr->verId=VER_ENC_WAVE_PACKET; break;
  case PACKET_ENC_SURF: gHdr->verId=VER_ENC_SURF_PACKET; break;
  case PACKET_PED_SUBBED_EVENT: gHdr->verId=VER_PEDSUBBED_EVENT_BODY; break;
  case PACKET_GPS_ADU5_PAT: gHdr->verId=VER_ADU5_PAT; break;
  case PACKET_GPS_ADU5_SAT: gHdr->verId=VER_ADU5_SAT; break;
  case PACKET_GPS_ADU5_VTG: gHdr->verId=VER_ADU5_VTG; break;
  case PACKET_GPS_G12_POS: gHdr->verId=VER_G12_POS; break;
  case PACKET_GPS_G12_SAT: gHdr->verId=VER_G12_SAT; break;
  case PACKET_GPS_GGA: gHdr->verId=VER_GPS_GGA; break;
  case PACKET_HKD: gHdr->verId=VER_HK_FULL; break;  
  case PACKET_HKD_SS: gHdr->verId=VER_HK_SS; break;
  case PACKET_CMD_ECHO: gHdr->verId=VER_CMD_ECHO; break;
  case PACKET_MONITOR: gHdr->verId=VER_MONITOR; break;
  case PACKET_SLOW1: gHdr->verId=VER_SLOW_1; break;
  case PACKET_SLOW2: gHdr->verId=VER_SLOW_2; break;
  case PACKET_SLOW_FULL: gHdr->verId=VER_SLOW_FULL; break;
  case PACKET_ZIPPED_FILE: gHdr->verId=VER_ZIPPED_FILE; break;
  case PACKET_ZIPPED_PACKET: gHdr->verId=VER_ZIPPED_PACKET; break;
  case PACKET_RUN_START: gHdr->verId=VER_RUN_START; break;
  case PACKET_OTHER_MONITOR: gHdr->verId=VER_OTHER_MON; break;
  case PACKET_GPSD_START: gHdr->verId=VER_GPSD_START; break;
  case PACKET_LOGWATCHD_START: gHdr->verId=VER_LOGWATCHD_START; break;
  case PACKET_ACQD_START: gHdr->verId=VER_ACQD_START; break;
  default: 
    gHdr->verId=0; break;
  }
  gHdr->checksum=simpleIntCrc(dataPtr,intBytes);
  //  printf("Int bytes %u\t checksum %u\n",intBytes,gHdr->checksum);
}

#define PKT_E_CHECKSUM 0x1
#define PKT_E_CODE 0x2
#define PKT_E_FEBYTE 0x4
#define PKT_E_SIZE 0x8

int checkPacket(void *thePtr)
//0 means no errors
//1 means checksum mismatch
//2 means unknown packet code
//4 means missing feByte missing
//8 means packet size mismatch
{
  unsigned int *typedPtr=(unsigned int*) thePtr;
  int retVal=0,packetSize=0;
  GenericHeader_t *gHdr= (GenericHeader_t*)typedPtr;
  unsigned int numInts=(gHdr->numBytes-sizeof(GenericHeader_t))/4;
  unsigned long dodgyNum=(unsigned long)typedPtr;
  dodgyNum+=sizeof(GenericHeader_t);
  unsigned int *dataPtr=(unsigned int*)dodgyNum;
  //  unsigned int *dataPtr=(unsigned int*) (typedPtr+sizeof(GenericHeader_t)); 
  //  std::cout << thePtr << "\t" << dataPtr << std::endl;
  unsigned int checksum=0;
  if(numInts<4000)
    checksum=simpleIntCrc(dataPtr,numInts);
  PacketCode_t code=gHdr->code;
  if(checksum!=gHdr->checksum) {
    //    printf("Checksum Mismatch (possibly %s (%d)) (%u ints -- %d bytes) %u -- %u \n",
    //	   packetCodeAsString(code),code,numInts,gHdr->numBytes,checksum,gHdr->checksum);	
    retVal+=PKT_E_CHECKSUM;
  }
  switch(code&BASE_PACKET_MASK) {
  case PACKET_BD: packetSize=sizeof(AnitaEventBody_t); break;	    
  case PACKET_HD: packetSize=sizeof(AnitaEventHeader_t); break;	    
  case PACKET_WV: packetSize=sizeof(RawWaveformPacket_t); break;
  case PACKET_PEDSUB_WV: packetSize=sizeof(PedSubbedWaveformPacket_t); break;
  case PACKET_PEDSUB_SURF: packetSize=sizeof(PedSubbedSurfPacket_t); break;
  case PACKET_SURF: packetSize=sizeof(RawSurfPacket_t); break;
  case PACKET_SURF_HK: packetSize=sizeof(FullSurfHkStruct_t); break;
  case PACKET_TURF_RATE: packetSize=sizeof(TurfRateStruct_t); break;
  case PACKET_TURF_REGISTER: packetSize=sizeof(TurfRegisterContents_t); break;
  case PACKET_AVG_SURF_HK: packetSize=sizeof(AveragedSurfHkStruct_t); break;
  case PACKET_SUM_TURF_RATE: packetSize=sizeof(SummedTurfRateStruct_t); break;
  case PACKET_ENC_WV_PEDSUB: break;
  case PACKET_ENC_SURF: break;
  case PACKET_ENC_SURF_PEDSUB: break;
  case PACKET_PED_SUBBED_EVENT: packetSize=sizeof(PedSubbedEventBody_t); break;
  case PACKET_LAB_PED: packetSize=sizeof(FullLabChipPedStruct_t); break;
  case PACKET_FULL_PED: packetSize=sizeof(FullPedStruct_t); break;
  case PACKET_GPS_ADU5_PAT: packetSize=sizeof(GpsAdu5PatStruct_t); break;
  case PACKET_GPS_ADU5_SAT: packetSize=sizeof(GpsAdu5SatStruct_t); break;
  case PACKET_GPS_ADU5_VTG: packetSize=sizeof(GpsAdu5VtgStruct_t); break;
  case PACKET_GPS_G12_POS: packetSize=sizeof(GpsG12PosStruct_t); break;
  case PACKET_GPS_G12_SAT: packetSize=sizeof(GpsG12SatStruct_t); break;
  case PACKET_GPS_GGA: packetSize=sizeof(GpsGgaStruct_t); break;
  case PACKET_HKD: packetSize=sizeof(HkDataStruct_t); break;    
  case PACKET_HKD_SS: packetSize=sizeof(SSHkDataStruct_t); break;    
  case PACKET_CMD_ECHO: packetSize=sizeof(CommandEcho_t); break;
  case PACKET_MONITOR: packetSize=sizeof(MonitorStruct_t); break;
  case PACKET_WAKEUP_LOS: packetSize=WAKEUP_LOS_BUFFER_SIZE; break;
  case PACKET_WAKEUP_HIGHRATE: packetSize=WAKEUP_TDRSS_BUFFER_SIZE; 
    break;
  case PACKET_WAKEUP_COMM1: packetSize=WAKEUP_LOW_RATE_BUFFER_SIZE; 
    break;
  case PACKET_WAKEUP_COMM2: packetSize=WAKEUP_LOW_RATE_BUFFER_SIZE; 
    break;
  case PACKET_SLOW1: packetSize=sizeof(SlowRateType1_t); 
    break;
  case PACKET_SLOW2: packetSize=sizeof(SlowRateType1_t); 
    break;
  case PACKET_SLOW_FULL: packetSize=sizeof(SlowRateFull_t); 
    break;
  case PACKET_RUN_START: packetSize=sizeof(RunStart_t); 
    break;
  case PACKET_OTHER_MONITOR: packetSize=sizeof(OtherMonitorStruct_t); 
    break;
  case PACKET_ZIPPED_PACKET: break;
  case PACKET_ZIPPED_FILE: break;
  case PACKET_GPSD_START: packetSize=sizeof(GpsdStartStruct_t); break;
  case PACKET_LOGWATCHD_START: packetSize=sizeof(LogWatchdStart_t); break;
  case PACKET_ACQD_START: packetSize=sizeof(AcqdStartStruct_t); break;
    //  default: 
    //    retVal+=PKT_E_CODE; break;
  }
  if(packetSize && (packetSize!=gHdr->numBytes))
    retVal+=PKT_E_SIZE;
  if(gHdr->feByte!=0xfe) retVal+=PKT_E_FEBYTE;
  return retVal;

}

const char *packetCodeAsString(PacketCode_t code) {
  const char* string ;
  switch(code) {
  case PACKET_BD: string="AnitaEventBody_t"; break;	    
  case PACKET_HD: string="AnitaEventHeader_t"; break;	    
  case PACKET_WV: string="RawWaveformPacket_t"; break;
  case PACKET_PEDSUB_WV: string="PedSubbedWaveformPacket_t"; break;
  case PACKET_SURF: string="RawSurfPacket_t"; break;
  case PACKET_PEDSUB_SURF: string="PedSubbedSurfPacket_t"; break;
  case PACKET_SURF_HK: string="FullSurfHkStruct_t"; break;
  case PACKET_TURF_RATE: string="TurfRateStruct_t"; break;
  case PACKET_LAB_PED: string="FullLabChipPedStruct_t"; break;
  case PACKET_FULL_PED: string="FullPedStruct_t"; break;
  case PACKET_ENC_WV_PEDSUB: string="EncodedPedSubbedChannelPacketHeader_t"; break;
  case PACKET_ENC_SURF: string="EncodedSurfPacketHeader_t"; break;
  case PACKET_ENC_SURF_PEDSUB: string="EncodedPedSubbedSurfPacketHeader_t"; break;
  case PACKET_GPS_ADU5_PAT: string="GpsAdu5PatStruct_t"; break;
  case PACKET_GPS_ADU5_SAT: string="GpsAdu5SatStruct_t"; break;
  case PACKET_GPS_ADU5_VTG: string="GpsAdu5VtgStruct_t"; break;
  case PACKET_GPS_G12_POS: string="GpsG12PosStruct_t"; break;
  case PACKET_GPS_G12_SAT: string="GpsG12SatStruct_t"; break;
  case PACKET_HKD: string="HkDataStruct_t"; break;
  case PACKET_HKD_SS: string="SSHkDataStruct_t"; break;
  case PACKET_CMD_ECHO: string="CommandEcho_t"; break;
  case PACKET_MONITOR: string="MonitorStruct_t"; break;
  case PACKET_WAKEUP_LOS: string="LosWakeUpPacket"; break;
  case PACKET_WAKEUP_HIGHRATE: string="TdrssWakeUpPacket"; break;
  case PACKET_WAKEUP_COMM1: string="Comm1WakeUpPacket"; break;
  case PACKET_WAKEUP_COMM2: string="Comm2WakeUpPacket"; break;
  case PACKET_SLOW1: string="SlowRateType1_t"; break;
  case PACKET_SLOW2: string="SlowRateType1_t"; break;
  case PACKET_ZIPPED_PACKET: string="ZippedPacket_t"; break;
  case PACKET_ZIPPED_FILE: string="ZippedFile_t"; break;
  case PACKET_RUN_START: string="RunStart_t"; break;
  case PACKET_OTHER_MONITOR: string="OtherMonitorStruct_t"; break;
  case PACKET_GPSD_START: string="GpsdStartStruct_t"; break;
  case PACKET_LOGWATCHD_START: string="LogWatchdStart_t"; break;
  case PACKET_ACQD_START: string="AcqdStartStruct_t"; break;
  case PACKET_GPS_GGA: string="GpsGgaStruct_t"; break;
  case PACKET_AVG_SURF_HK: string="AveragedSurfHkStruct_t"; break;
  case PACKET_SUM_TURF_RATE: string="SummedTurfRateStruct_t"; break;

  default: 
    string="Unknown Packet Code"; break;
  }
  return string;
}



int unzipZippedPacket(ZippedPacket_t *zipPacket, char *output, unsigned int numBytesOut) {
    char *input = (char*) zipPacket;
    unsigned int returnBytes=numBytesOut;
    int retVal=unzipBuffer(&input[sizeof(ZippedPacket_t)],output,zipPacket->gHdr.numBytes-sizeof(ZippedPacket_t),&returnBytes);
    if(retVal!=0) 
	return retVal;
    if(zipPacket->numUncompressedBytes!=returnBytes)
	return -1;
    return 0;
}



int zipBuffer(char *input, char *output, unsigned int inputBytes, unsigned int *outputBytes)
{
    static int errorCounter=0;
    int retVal=compress((unsigned char*)output,(unsigned long*)outputBytes,(unsigned char*)input,(unsigned long)inputBytes);
    if(retVal==Z_OK)
	return 0;
    else {
	if(errorCounter<100) {
	    fprintf(stderr,"zlib compress returned %d  (%d of 100)\n",retVal,errorCounter);
	    errorCounter++;
	}
	return -1;
    }	
}


int unzipBuffer(char *input, char *output, unsigned int inputBytes, unsigned int *outputBytes)
{

  unsigned long outputBytesLong;
  static int errorCounter=0;
  //    int retVal=uncompress((unsigned char*)output,(unsigned long*)outputBytes,(unsigned char*)input,(unsigned long)inputBytes);
  //    if(retVal==Z_OK)
  outputBytesLong = *outputBytes;
  int retVal=uncompress((unsigned char*)output,&outputBytesLong,(unsigned char*)input,(unsigned long)inputBytes);
  if(retVal==Z_OK) 
    {
      *outputBytes = outputBytesLong;
 	return 0;
	//    else {
    }
  else {
    if(errorCounter<100) {
      fprintf(stderr,"zlib compress returned %d  (%d of 100)\n",retVal,errorCounter);
      errorCounter++;
      
      
	    //    static int errorCounter=0;
	    //int retVal=uncompress((unsigned char*)output,(unsigned long*)outputBytes,(unsigned char*)input,(unsigned long)inputBytes);
	    //if(retVal==Z_OK)
	    //	return 0;
	    //else {
	    //if(errorCounter<100) {
	    // fprintf(stderr,"zlib compress returned %d  (%d of 100)\n",retVal,errorCounter);
	    // errorCounter++;
    }
    return -1;
  }	
}


CompressErrorCode_t AnitaCompress::packPedSubbedEvent(PedSubbedEventBody_t *bdPtr,
				       EncodeControlStruct_t *cntlPtr,
				       unsigned char *output,
				       int *numBytes)
{

    int count=0,surfStart=0;
    int surf=0;
    int chan=0;  
    EncodedPedSubbedSurfPacketHeader_t *surfHdPtr;
//    printf("Event %lu\n",bdPtr->eventNumber);

    EncodedEventWrapper_t *encEvWrap = (EncodedEventWrapper_t*) output;
    encEvWrap->eventNumber=bdPtr->eventNumber;
    count+=sizeof(EncodedEventWrapper_t);

    for(surf=0;surf<ACTIVE_SURFS;surf++) {
	surfHdPtr = (EncodedPedSubbedSurfPacketHeader_t*) &output[count];
	surfStart=count;
	surfHdPtr->whichPeds=bdPtr->whichPeds;
	surfHdPtr->eventNumber=bdPtr->eventNumber;
	count+=sizeof(EncodedPedSubbedSurfPacketHeader_t);
	for(chan=0;chan<CHANNELS_PER_SURF;chan++) {
	    count+=encodePSChannel(cntlPtr->encTypes[surf][chan],
				   &(bdPtr->channel[AnitaGeomTool::getChanIndex(surf,chan)]),
				   &output[count]);
	    if(count>MAX_WAVE_BUFFER)
		return COMPRESS_E_TOO_BIG;
	}
	//Fill Generic Header_t;
	fillGenericHeader(surfHdPtr,PACKET_ENC_SURF_PEDSUB,(count-surfStart));
							 
    }
    *numBytes=count;
    encEvWrap->numBytes=count-sizeof(EncodedEventWrapper_t);
    fillGenericHeader(encEvWrap,PACKET_ENC_PEDSUB_EVENT_WRAPPER,count);
    return COMPRESS_E_OK;
}




CompressErrorCode_t AnitaCompress::unpackToPedSubbedEvent(PedSubbedEventBody_t *bdPtr,
							  unsigned char *input,
							  int numBytes)

{
  
  return unpackToPedSubbedEventWithStats(bdPtr,input,numBytes,0,0);
    
}


CompressErrorCode_t 
AnitaCompress::unpackOneSurfToPedSubbedEvent(PedSubbedEventBody_t *bdPtr,
					     unsigned char *input,
					     int numBytes)
{
    int count=0;
    int chan=0;  
    int chanIndex;
    CompressErrorCode_t retVal=COMPRESS_E_OK;
    EncodedPedSubbedSurfPacketHeader_t *surfHdPtr;
    EncodedSurfChannelHeader_t *chanHdPtr;
    
    
    surfHdPtr = (EncodedPedSubbedSurfPacketHeader_t*) &input[count];
    bdPtr->whichPeds=surfHdPtr->whichPeds;
    bdPtr->eventNumber=surfHdPtr->eventNumber;
    count+=sizeof(EncodedPedSubbedSurfPacketHeader_t);
    for(chan=0;chan<CHANNELS_PER_SURF;chan++) {	
      chanHdPtr = (EncodedSurfChannelHeader_t*)&input[count];
      count+=sizeof(EncodedSurfChannelHeader_t);
      //      printf("chan %d, encType %d\n",
      //	     chan,chanHdPtr->encType);
      chanIndex=chanHdPtr->rawHdr.chanId;
      if(chanIndex%9 != chan) {
	printf("Error reading encoded Ped Subbed SURF packet, have chanId %d and chan %d\n",chanIndex,chan);
	return COMPRESS_E_UNPACK;
      }
      
      retVal=decodePSChannel(chanHdPtr,
			     &input[count],
			     &(bdPtr->channel[chanIndex]));

      //      std::cout << chanIndex << "\t" << bdPtr->channel[chanIndex].data[0]
      //		<< "\n";
		

      
      if(retVal!=COMPRESS_E_OK) return retVal;
      count+=chanHdPtr->numBytes;
      if(count>numBytes)
	return COMPRESS_E_BADSIZE;
    }
	//Fill Generic Header_t;
	
    fillGenericHeader(bdPtr,PACKET_PED_SUBBED_EVENT,count);
    return COMPRESS_E_OK;

}



CompressErrorCode_t AnitaCompress::unpackOneWaveToPedSubbedEvent(PedSubbedEventBody_t *bdPtr,
								 unsigned char *input,
								 int numBytes)
{
    int count=0;
    int chanIndex;
    CompressErrorCode_t retVal=COMPRESS_E_OK;
    EncodedPedSubbedChannelPacketHeader_t *waveHdPtr;
    EncodedSurfChannelHeader_t *chanHdPtr;
    
    
    waveHdPtr = (EncodedPedSubbedChannelPacketHeader_t*) &input[count];
    bdPtr->whichPeds=waveHdPtr->whichPeds;
    bdPtr->eventNumber=waveHdPtr->eventNumber;
    count+=sizeof(EncodedPedSubbedChannelPacketHeader_t);
	
    chanHdPtr = (EncodedSurfChannelHeader_t*)&input[count];
    count+=sizeof(EncodedSurfChannelHeader_t);
    chanIndex=chanHdPtr->rawHdr.chanId;
    retVal=decodePSChannel(chanHdPtr,
			   &input[count],
			   &(bdPtr->channel[chanIndex]));
    if(retVal!=COMPRESS_E_OK) return retVal;
    count+=chanHdPtr->numBytes;
    if(count>numBytes)
	return COMPRESS_E_BADSIZE;
    
	//Fill Generic Header_t;	
//    fillGenericHeader(bdPtr,PACKET_PED_SUBBED_EVENT,count);
    return COMPRESS_E_OK;

}


CompressErrorCode_t 
AnitaCompress::unpackToPedSubbedEventWithStats(PedSubbedEventBody_t *bdPtr,
					       unsigned char *input,
					       int numBytes,
					       EncodeControlStruct_t *cntlPtr,
					       int *sizeArray)
{

    int count=0;
    int surf=0;
    int chan=0;  
    CompressErrorCode_t retVal=COMPRESS_E_OK;
    EncodedPedSubbedSurfPacketHeader_t *surfHdPtr;
    EncodedSurfChannelHeader_t *chanHdPtr;

    GenericHeader_t *gHdr=(GenericHeader_t*) input;
    if(gHdr->code==PACKET_ENC_PEDSUB_EVENT_WRAPPER && numBytes>(int)sizeof(EncodedEventWrapper_t)) { 
	return unpackToPedSubbedEventWithStats(bdPtr,&input[sizeof(EncodedEventWrapper_t)],numBytes-sizeof(EncodedEventWrapper_t),cntlPtr,sizeArray);
    }

    //    printf("unpackToPedSubbedEvent\n");
    for(surf=0;surf<ACTIVE_SURFS;surf++) {
      surfHdPtr = (EncodedPedSubbedSurfPacketHeader_t*) &input[count];
      bdPtr->whichPeds=surfHdPtr->whichPeds;
      bdPtr->eventNumber=surfHdPtr->eventNumber;
      count+=sizeof(EncodedPedSubbedSurfPacketHeader_t);
      for(chan=0;chan<CHANNELS_PER_SURF;chan++) {
	    
	    chanHdPtr = (EncodedSurfChannelHeader_t*)&input[count];
	    count+=sizeof(EncodedSurfChannelHeader_t);
//	    printf("surf %d, chan %d, encType %d\n",
//		   surf,chan,chanHdPtr->encType);


	    retVal=decodePSChannel(chanHdPtr,
				   &input[count],
				   &(bdPtr->channel[AnitaGeomTool::getChanIndex(surf,chan)]));
	    if(retVal!=COMPRESS_E_OK) return retVal;
	    count+=chanHdPtr->numBytes;	
	    if(cntlPtr) {
		cntlPtr->encTypes[surf][chan]=chanHdPtr->encType;;
	    }
	    if(sizeArray) {
		sizeArray[AnitaGeomTool::getChanIndex(surf,chan)]=chanHdPtr->numBytes;
	    }
	    if(count>numBytes)
		return COMPRESS_E_BADSIZE;
	}
	//Fill Generic Header_t;
    }
    fillGenericHeader(bdPtr,PACKET_PED_SUBBED_EVENT,count);
    return COMPRESS_E_OK;

}


void AnitaCompress::fillMinMaxMeanRMS(SurfChannelPedSubbed_t *chanPtr) {
    int samp;
    int wrappedHitbus=chanPtr->header.chipIdFlag&WRAPPED_HITBUS;
    int firstSamp=0,lastSamp=MAX_NUMBER_SAMPLES;
    int index=0;
    float mean=0,meanSq=0,nsamps=0;
    chanPtr->xMax=-4095;
    chanPtr->xMin=4095;
	
    if(wrappedHitbus) {
	firstSamp=chanPtr->header.firstHitbus+1;
	lastSamp=chanPtr->header.lastHitbus;
    }
    else {
	firstSamp=chanPtr->header.lastHitbus+1;
	lastSamp=MAX_NUMBER_SAMPLES+chanPtr->header.firstHitbus;
    }    
    for(samp=firstSamp;samp<lastSamp;samp++) {
	index=samp;
	if(index>=MAX_NUMBER_SAMPLES) index-=MAX_NUMBER_SAMPLES;
	mean+=chanPtr->data[index];
	meanSq+=(chanPtr->data[index]*chanPtr->data[index]);
	nsamps++;

	if(chanPtr->data[index]>chanPtr->xMax)
	    chanPtr->xMax=chanPtr->data[index];
	if(chanPtr->data[index]<chanPtr->xMin)
	    chanPtr->xMin=chanPtr->data[index];

    }
    if(nsamps>0) {
	mean/=nsamps;
	meanSq/=nsamps;
	chanPtr->mean=mean;
	if(meanSq>mean*mean)
	    chanPtr->rms=sqrt(meanSq-mean*mean);
    }
    

}


int AnitaCompress::encodeChannel(ChannelEncodingType_t encType, SurfChannelFull_t *chanPtr, unsigned char *buffer) 
{
//    printf("Event %lu, ChanId %d\n",theHead.eventNumber,
//	   chanPtr->header.chanId);



    EncodedSurfChannelHeader_t *chanHdPtr
	=(EncodedSurfChannelHeader_t*) &buffer[0];
    int count=0;    
    int encSize=0;
    chanHdPtr->rawHdr=chanPtr->header;
    chanHdPtr->encType=encType;
    count+=sizeof(EncodedSurfChannelHeader_t);
    
    switch(encType) {
	case ENCODE_NONE:
	    encSize=encodeWaveNone(&buffer[count],chanPtr);
	    break;
	default:
	    chanHdPtr->encType=ENCODE_NONE;
	    encSize=encodeWaveNone(&buffer[count],chanPtr);
	    break;	    
    }    
    chanHdPtr->numBytes=encSize;
    chanHdPtr->crc=simpleCrcShort((unsigned short*)&buffer[count],
				  encSize/2);
    return (count+encSize);        
}


CompressErrorCode_t AnitaCompress::decodeChannel(EncodedSurfChannelHeader_t *encChanHdPtr,unsigned char *input, SurfChannelFull_t *chanPtr)
{
    CompressErrorCode_t retVal;
    int numBytes=encChanHdPtr->numBytes;
    int newCrc=simpleCrcShort((unsigned short*)input,numBytes/2);
    if(encChanHdPtr->crc!=newCrc) return COMPRESS_E_BAD_CRC;
    

//    printf("decodePSChannel encType==%d\n",encChanHdPtr->encType);
    chanPtr->header=encChanHdPtr->rawHdr;
    switch(encChanHdPtr->encType) {
	case ENCODE_NONE:
	    retVal=decodeWaveNone(input,numBytes,chanPtr);
	    break;
	default:
	    //Try ENCODE_NONE
	    retVal=decodeWaveNone(input,numBytes,chanPtr);
	    retVal=COMPRESS_E_BAD_CODE;
	    break;
    }
    return retVal;
}


int AnitaCompress::encodeWaveNone(unsigned char *buffer,SurfChannelFull_t *chanPtr) {    
    int encSize=MAX_NUMBER_SAMPLES*sizeof(unsigned short);
    memcpy(buffer,chanPtr->data,encSize);
    return encSize;
}

CompressErrorCode_t AnitaCompress::decodeWaveNone(unsigned char *input,int numBytes,SurfChannelFull_t *chanPtr){    
    int encSize=MAX_NUMBER_SAMPLES*sizeof(unsigned short);
    //Check that numBytes is correct
    if(numBytes!=encSize) return COMPRESS_E_BADSIZE;
    memcpy(chanPtr->data,input,encSize);
    return COMPRESS_E_OK;
}


int AnitaCompress::encodePSChannel(ChannelEncodingType_t encType, SurfChannelPedSubbed_t *chanPtr, unsigned char *buffer) 
{
//    printf("Event %lu, ChanId %d\n",theHead.eventNumber,
//	   chanPtr->header.chanId);

    EncodedSurfChannelHeader_t *chanHdPtr
	=(EncodedSurfChannelHeader_t*) &buffer[0];
    int count=0;    
    int encSize=0;
    chanHdPtr->rawHdr=chanPtr->header;
    chanHdPtr->encType=encType;
    count+=sizeof(EncodedSurfChannelHeader_t);
    
    switch(encType) {
	case ENCODE_NONE:
	    encSize=encodePSWaveNone(&buffer[count],chanPtr);
	    break;
	case ENCODE_LOSSLESS_12BIT:
	    encSize=encodePSWave12bitBinary(&buffer[count],chanPtr);
	    break;
	case ENCODE_LOSSLESS_BINARY:
	    encSize=encodePSWaveLosslessBinary(&buffer[count],chanPtr,
					       &(chanHdPtr->encType));
	    break;
	case ENCODE_LOSSLESS_FIBONACCI:
	    encSize=encodePSWaveLosslessFibonacci(&buffer[count],chanPtr);
	    break;
	case ENCODE_LOSSLESS_BINFIB_COMBO:
	    encSize=encodePSWaveLosslessBinFibCombo(&buffer[count],chanPtr,
						    &(chanHdPtr->encType));
	    break;
	case ENCODE_LOSSY_MULAW:
	case ENCODE_LOSSY_MULAW_8BIT:
	    encSize=encodePSWaveLossyMulawOptimally(&buffer[count],chanPtr,8,
						    &(chanHdPtr->encType));
	    break;
	case ENCODE_LOSSY_MULAW_7BIT:
	    encSize=encodePSWaveLossyMulawOptimally(&buffer[count],chanPtr,7,
						    &(chanHdPtr->encType));
	    break;
	case ENCODE_LOSSY_MULAW_6BIT:
	    encSize=encodePSWaveLossyMulawOptimally(&buffer[count],chanPtr,6,
						    &(chanHdPtr->encType));
	    break;
	case ENCODE_LOSSY_MULAW_5BIT:
	    encSize=encodePSWaveLossyMulawOptimally(&buffer[count],chanPtr,5,
						    &(chanHdPtr->encType));
	    break;
	case ENCODE_LOSSY_MULAW_4BIT:
	    encSize=encodePSWaveLossyMulawOptimally(&buffer[count],chanPtr,4,
						    &(chanHdPtr->encType));
	    break;
	case ENCODE_LOSSY_MULAW_3BIT:
	    encSize=encodePSWaveLossyMulawOptimally(&buffer[count],chanPtr,3,
						    &(chanHdPtr->encType));
	    break;
	default:
	    chanHdPtr->encType=ENCODE_NONE;
	    encSize=encodePSWaveNone(&buffer[count],chanPtr);
	    break;	    
    }    
    chanHdPtr->numBytes=encSize;
    chanHdPtr->crc=simpleCrcShort((unsigned short*)&buffer[count],
				  encSize/2);
    return (count+encSize);        
}

CompressErrorCode_t AnitaCompress::decodePSChannel(EncodedSurfChannelHeader_t *encChanHdPtr,unsigned char *input, SurfChannelPedSubbed_t *chanPtr)
{
    CompressErrorCode_t retVal;
    int numBytes=encChanHdPtr->numBytes;
    int newCrc=simpleCrcShort((unsigned short*)input,numBytes/2);
    if(encChanHdPtr->crc!=newCrc) return COMPRESS_E_BAD_CRC;
    

//    printf("decodePSChannel encType==%d\n",encChanHdPtr->encType);
    chanPtr->header=encChanHdPtr->rawHdr;
    switch(encChanHdPtr->encType) {
	case ENCODE_NONE:
	    retVal=decodePSWaveNone(input,numBytes,chanPtr);
	    break;
	case ENCODE_LOSSLESS_12BIT:
	    retVal=decodePSWave12bitBinary(input,numBytes,chanPtr);
	    break;
	case ENCODE_LOSSLESS_11BIT:
	case ENCODE_LOSSLESS_10BIT:
	case ENCODE_LOSSLESS_9BIT:
	case ENCODE_LOSSLESS_8BIT:
	case ENCODE_LOSSLESS_7BIT:
	case ENCODE_LOSSLESS_6BIT:
	case ENCODE_LOSSLESS_5BIT:
	case ENCODE_LOSSLESS_4BIT:
	case ENCODE_LOSSLESS_3BIT:
	case ENCODE_LOSSLESS_2BIT:
	case ENCODE_LOSSLESS_1BIT:
	    retVal=decodePSWaveLosslessBinary(input,numBytes,chanPtr,encChanHdPtr->encType);
	    break;
	case ENCODE_LOSSLESS_FIBONACCI:
	    retVal=decodePSWaveLosslessFibonacci(input,numBytes,chanPtr);
	    break;
	case ENCODE_LOSSLESS_BINFIB_COMBO:
	case ENCODE_LOSSLESS_BINFIB_10BIT:
	case ENCODE_LOSSLESS_BINFIB_9BIT:
	case ENCODE_LOSSLESS_BINFIB_8BIT:
	case ENCODE_LOSSLESS_BINFIB_7BIT:
	case ENCODE_LOSSLESS_BINFIB_6BIT:
	case ENCODE_LOSSLESS_BINFIB_5BIT:
	case ENCODE_LOSSLESS_BINFIB_4BIT:
	case ENCODE_LOSSLESS_BINFIB_3BIT:
	case ENCODE_LOSSLESS_BINFIB_2BIT:
	case ENCODE_LOSSLESS_BINFIB_1BIT:
	    retVal=decodePSWaveLosslessBinFibCombo(input,numBytes,chanPtr,
						   encChanHdPtr->encType);
	    break;
	case ENCODE_LOSSY_MULAW_11_8:
	case ENCODE_LOSSY_MULAW_11_7:
	case ENCODE_LOSSY_MULAW_11_6:
	case ENCODE_LOSSY_MULAW_11_5:
	case ENCODE_LOSSY_MULAW_11_4:
	case ENCODE_LOSSY_MULAW_11_3:
	case ENCODE_LOSSY_MULAW_10_8:
	case ENCODE_LOSSY_MULAW_10_7:
	case ENCODE_LOSSY_MULAW_10_6:
	case ENCODE_LOSSY_MULAW_10_5:
	case ENCODE_LOSSY_MULAW_10_4:
	case ENCODE_LOSSY_MULAW_10_3:
	case ENCODE_LOSSY_MULAW_9_7:
	case ENCODE_LOSSY_MULAW_9_6:
	case ENCODE_LOSSY_MULAW_9_5:
	case ENCODE_LOSSY_MULAW_9_4:
	case ENCODE_LOSSY_MULAW_9_3:
	case ENCODE_LOSSY_MULAW_8_6:
	case ENCODE_LOSSY_MULAW_8_5:
	case ENCODE_LOSSY_MULAW_8_4:
	case ENCODE_LOSSY_MULAW_8_3:
	case ENCODE_LOSSY_MULAW_7_5:
	case ENCODE_LOSSY_MULAW_7_4:
	case ENCODE_LOSSY_MULAW_7_3:
	case ENCODE_LOSSY_MULAW_6_4:
	case ENCODE_LOSSY_MULAW_6_3:
	    retVal=decodePSWaveLossyMuLaw(input,numBytes,chanPtr,
					  encChanHdPtr->encType);
	    break;

	default:
	    //Try ENCODE_NONE
	    retVal=decodePSWaveNone(input,numBytes,chanPtr);
	    retVal=COMPRESS_E_BAD_CODE;
	    break;
    }
    if(retVal==COMPRESS_E_OK) fillMinMaxMeanRMS(chanPtr);
    return retVal;
}


int AnitaCompress::encodePSWaveNone(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr) {    
//    int samp;
    int encSize=MAX_NUMBER_SAMPLES*sizeof(short);
    memcpy(buffer,chanPtr->data,encSize);
//    for(samp=0;samp<MAX_NUMBER_SAMPLES;samp+=4) {
//	printf("samp %d: value %d\n",samp,chanPtr->data[samp]);
//    }
    return encSize;
}

CompressErrorCode_t AnitaCompress::decodePSWaveNone(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr){    
//    int samp;
    int encSize=MAX_NUMBER_SAMPLES*sizeof(unsigned short);
    //Check that numBytes is correct
    if(numBytes!=encSize) return COMPRESS_E_BADSIZE;
//    printf("Here\n");
    memcpy(chanPtr->data,input,encSize);
//    for(samp=0;samp<MAX_NUMBER_SAMPLES;samp++) {
//	printf("samp %d: value %d\n",samp,chanPtr->data[samp]);
//    }
    return COMPRESS_E_OK;
}

#define OFFSET_VALUE 2048

int AnitaCompress::encodePSWave12bitBinary(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr) {    
    int encSize=3*(MAX_NUMBER_SAMPLES/4)*sizeof(unsigned short);
    int samp=0,charNum=0,valNum=0;
    unsigned short value[4];
    for(samp=0;samp<MAX_NUMBER_SAMPLES;samp+=4) {
	charNum=(samp*3)/2;
	for(valNum=0;valNum<4;valNum++)
	    value[valNum]=chanPtr->data[samp+valNum]+OFFSET_VALUE;
    
	pack12bit(value,&buffer[charNum]);
    }
    return encSize;
}

CompressErrorCode_t AnitaCompress::decodePSWave12bitBinary(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr){    
    int encSize=3*(MAX_NUMBER_SAMPLES/4)*sizeof(unsigned short);
    if(numBytes!=encSize) return COMPRESS_E_BADSIZE;
    unsigned short value[4];
    int samp=0,charNum=0,valNum=0;
    for(charNum=0;charNum<6*(MAX_NUMBER_SAMPLES/4);charNum+=6) {
	samp=2*(charNum/3);	
	unpack12bit(value,&input[charNum]);
	for(valNum=0;valNum<4;valNum++)
	    chanPtr->data[samp+valNum]=((short)value[valNum])-OFFSET_VALUE;
    }
    return COMPRESS_E_OK;
}

int AnitaCompress::encodePSWaveLosslessBinary(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr, ChannelEncodingType_t *encTypePtr) {    
    //Remember this function works an array of pedestal subtracted shorts
    //Which is what SurfChannelPedSubbed_t contains
    int wordNum=0;
    int numBitsLeft;
    int bitMask;
    int bitNum;
    unsigned char *currentChar=buffer;
    int currentBit=0;
    unsigned short newVal=0;
    int mean=0;
    short xMax=chanPtr->xMax; //Filled by pedestalLib
    short xMin=chanPtr->xMin; //Filled by pedestalLib   
    short *input = chanPtr->data;

    //Find min and max
//    for(wordNum=0;wordNum<numInputs;wordNum++) {
//	mean+=input[wordNum];
//	if(input[wordNum]>xMax) xMax=input[wordNum];
//	if(input[wordNum]<xMin) xMin=input[wordNum];
//    }
//    mean/=numInputs;
    //Don't actual use min we just use the median value

    int rangeTotal=xMax-xMin;
    float logValue=(float)(log(rangeTotal+1)/log(2));
    int bitSize=(int)logValue;
    if((logValue-bitSize)>0) bitSize++;
    
    *encTypePtr=getBinaryEncType(bitSize);

    mean=xMin+rangeTotal/2;
//    printf("mean %d\txMin %d\txMax %d\tbitSize %d\n",mean,xMin,xMax,bitSize);
//    if(mean<-20 || mean>20)
//	printf("mean %d, xMin %d xMax %d\n",mean,xMin,xMax);
    short *meanPtr=(short*)currentChar;
    (*meanPtr)=mean;
    currentChar+=2;
/*     *currentChar = (unsigned char) bitSize; */
/*     currentChar++; */

    for(wordNum=0;wordNum<MAX_NUMBER_SAMPLES;wordNum++) {
	newVal=bifurcate(input[wordNum]-mean);
 	numBitsLeft=bitSize;
//	printf("input: %d (%d)\t newVal: %#x\t: bitsize %d\n",input[wordNum],
//	       input[wordNum]-mean,newVal,bitSize);
//	printf("cc %u cbit %d:\n",(int)(currentChar-buffer),currentBit);
	while(numBitsLeft) {
//	    if(wordNum<5) cout << wordNum << "\t" << numBitsLeft << endl;
	    if(numBitsLeft>(8-currentBit)) {			
		bitMask=0;
		for(bitNum=0;bitNum<(8-currentBit);bitNum++)
		    bitMask|=(1<<bitNum);	   
		(*currentChar)|= (newVal&bitMask)<<currentBit;
		newVal=(newVal>>(8-currentBit));
		numBitsLeft-=(8-currentBit);
		currentChar++;
		currentBit=0;
	    }
	    else {			
		bitMask=0;
		for(bitNum=0;bitNum<numBitsLeft;bitNum++)
		    bitMask|=(1<<bitNum);
		(*currentChar)|= (newVal&bitMask)<<currentBit;
		currentBit+=numBitsLeft;
		if(currentBit==8) {
		    currentBit=0;
		    currentChar++;
		}
		numBitsLeft=0;
	    }
	}
    }
//    for(int i=0;i<int(currentChar-buffer);i++) {
//	cout << i << "\t"  << (int)buffer[i] << endl;
//    }
    if(currentBit) currentChar++;
    return (int)(currentChar-buffer);
}

CompressErrorCode_t AnitaCompress::decodePSWaveLosslessBinary(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr, ChannelEncodingType_t encType)
{
    int sampNum=0;
    short *meanPtr=(short*)input;
    short mean=(short)(*meanPtr);
//    unsigned char flag =  input[1];
    int bitSize=getBinaryBitSize(encType);
    int bitNum;
//    if(mean<-1100 || mean>100)
//    printf("binary mean %d %#x\n",mean,mean);
//    printf("mean %d\tbitSize %d\n",mean,bitSize);

    unsigned char *currentChar=&input[2];
    int currentBit=0;
    unsigned short tempNum;
    while(sampNum<MAX_NUMBER_SAMPLES) {
	int fred = *( (int*) currentChar);
//	printf("cc %u cbit %d: \t %#x\n",(int)(currentChar-input),currentBit,fred);
//	bin_prnt_int(fred);
	tempNum=0;	    
	for(bitNum=currentBit;bitNum<currentBit+bitSize;bitNum++) {
	    tempNum|= (((fred>>bitNum)&0x1)<<(bitNum-currentBit));
//	    bin_prnt_short(tempNum);
//	    cout << (((fred>>bitNum)&0x1)<<(bitNum-currentBit));
	}
//	cout << endl;
//	bin_prnt_short(tempNum);

//	cout << sampNum << "\t" << tempNum << endl;

	chanPtr->data[sampNum]=mean+unbifurcate(tempNum);
//	printf("samp %d:\t %#x \t %d\n",sampNum,tempNum,chanPtr->data[sampNum]);
	
	sampNum++;
	currentBit+=bitSize;
	while(currentBit>=8) {
	    currentBit-=8;
	    currentChar++;
	}
	if(((int)(currentChar-input))>numBytes) return COMPRESS_E_BADSIZE;
    }
    return COMPRESS_E_OK;

}

int AnitaCompress::encodePSWaveLosslessFibonacci(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr) {    
    //Remember this function works an array of pedestal subtracted shorts
    //Which is what SurfChannelPedSubbed_t contains
    int wordNum=0;
    int numBitsLeft;
    int bitMask;
    int bitNum;
    unsigned char *currentChar=buffer;
    int currentBit=0;
    unsigned short newVal=0;
    int mean=(int)chanPtr->mean;
    if(mean>127) mean=127;
    if(mean<-127) mean=-127;
//    short xMax=chanPtr->xMax; //Filled by pedestalLib
//    short xMin=chanPtr->xMin; //Filled by pedestalLib   
    short *input = chanPtr->data;

//    int rangeTotal=xMax-xMin;
//    printf("mean %d\txMin %d\txMax %d\n",mean,xMin,xMax);
    
    char *meanPtr=(char*)currentChar;
    (*meanPtr)=(char)(mean);
    currentChar++;


    for(wordNum=0;wordNum<MAX_NUMBER_SAMPLES;wordNum++) {
	newVal=bifurcate(input[wordNum]-mean);
	newVal=encodeFibonacci(newVal,&numBitsLeft);
	while(numBitsLeft) {
//	    if(wordNum<5) cout << wordNum << "\t" << numBitsLeft << endl;
	    if(numBitsLeft>(8-currentBit)) {			
		bitMask=0;
		for(bitNum=0;bitNum<(8-currentBit);bitNum++)
		    bitMask|=(1<<bitNum);	   
		(*currentChar)|= (newVal&bitMask)<<currentBit;
		newVal=(newVal>>(8-currentBit));
		numBitsLeft-=(8-currentBit);
		currentChar++;
		currentBit=0;
	    }
	    else {			
		bitMask=0;
		for(bitNum=0;bitNum<numBitsLeft;bitNum++)
		    bitMask|=(1<<bitNum);
		(*currentChar)|= (newVal&bitMask)<<currentBit;
		currentBit+=numBitsLeft;
		if(currentBit==8) {
		    currentBit=0;
		    currentChar++;
		}
		numBitsLeft=0;
	    }
	}
    }
//    for(int i=0;i<int(currentChar-buffer);i++) {
//	cout << i << "\t"  << (int)buffer[i] << endl;
//    }
    if(currentBit) currentChar++;
    return (int)(currentChar-buffer);
}

CompressErrorCode_t AnitaCompress::decodePSWaveLosslessFibonacci(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr)
{
    int sampNum=0;
    char *meanPtr=(char*)input;
    short mean=(short)(*meanPtr);

    unsigned char *currentChar=&input[1];
    int currentBit=0;
    unsigned short fibNum;
    unsigned short unfibNum;
    int tempBit,bitNum;
    while(sampNum<MAX_NUMBER_SAMPLES) {
	tempBit=currentBit;
	int fred = *( (int*) currentChar);
	if(sampNum==259)
	    printf("samp %d, fred %#x\n",sampNum,fred);
	while(tempBit<32) {
	    if( ((fred>>tempBit)&0x1) && ((fred>>(tempBit+1))&0x1)) {
		//Got two ones;
		fibNum=0;
		for(bitNum=currentBit;bitNum<=tempBit+1;bitNum++) {
		    fibNum|= (((fred>>bitNum)&0x1)<<(bitNum-currentBit));
		}
		unfibNum=unfibonacci(fibNum);
		chanPtr->data[sampNum]=mean+unbifurcate(unfibNum);
	
		

		tempBit+=2;
		currentChar+=(tempBit/8);
		currentBit=(tempBit%8);
//		cout << tempBit << "\t" << currentChar << "\t" << currentBit << endl;
		break;
	    }
//	    cout << ((fred>>tempBit)&0x1) << " ";
	    tempBit++;
	}
	sampNum++;
	if(((int)(currentChar-input))>numBytes) return COMPRESS_E_BADSIZE;
    }

    return COMPRESS_E_OK;

}

int AnitaCompress::encodePSWaveLosslessBinFibCombo(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr, ChannelEncodingType_t *encTypePtr) {    
    //Remember this function works an array of pedestal subtracted shorts
    //Which is what SurfChannelPedSubbed_t contains
    float numSigma=3; //Will hard code for now
    int wordNum=0;
    int numBitsLeft;
    int bitMask;
    int bitNum;
    unsigned short fibVal=0;
    int asFib=0;
    int fibBits;
    unsigned char *currentChar=buffer;
    int currentBit=0;
    unsigned char *overflowChar;
    int overflowBit=0;
    unsigned short newVal=0;
    int mean=(int)chanPtr->mean;
    if(mean>127) mean=127;
    if(mean<-127) mean=-127;
    float sigma=chanPtr->rms;
    short xMax=chanPtr->xMax; //Filled by pedestalLib
    short xMin=chanPtr->xMin; //Filled by pedestalLib   
    short *input = chanPtr->data;
    int rangeTotal=(int)(numSigma*sigma);
    float logValue=(float)(log(rangeTotal+1)/log(2));
    int bitSize=(int)logValue;
    if((logValue-bitSize)>0) bitSize++;
    if(bitSize==1) bitSize++;


    //Reset xMin and xMax to be the binary/fibonacci cut off point
    xMin=mean-(1<<(bitSize-1));
    xMax=mean+(1<<(bitSize-1));
    xMax-=1;
//    printf("mean %d\txMin %d\txMax %d\tbitSize %d\n",mean,xMin,xMax,bitSize);
    
    char *meanPtr=(char*)currentChar;
    (*meanPtr)=(char)(mean);
    currentChar++;
    *encTypePtr=getBinFibEncType(bitSize);
//    *currentChar = (unsigned char) bitSize;
    //   currentChar++;


    overflowChar=currentChar+(MAX_NUMBER_SAMPLES*bitSize)/8;
    overflowBit=(MAX_NUMBER_SAMPLES*bitSize)%8;

    unsigned short flagVal=(1<<bitSize)-1;
    int countOverflows=0;

    for(wordNum=0;wordNum<MAX_NUMBER_SAMPLES;wordNum++) {
	//Check if value is in the range xMin to xMax
	if(input[wordNum]>xMin && input[wordNum]<xMax) {
	    newVal=bifurcate(input[wordNum]-mean);
	}
	else {
	    //If not encode number as fibonacci in the overflow
	    countOverflows++;
	    newVal=flagVal;
	    if(input[wordNum]>=xMax) {
		fibVal=((input[wordNum]-xMax)<<1)+1; //Odd numbers
	    }
	    else {
		fibVal=((xMin-input[wordNum])<<1)+2; //Even numbers
	    }
	    asFib=encodeFibonacci(fibVal,&fibBits);

	    
//	    cout << "Fib:\t" << fibVal << "\t" << asFib << "\t" << fibBits << endl;
//	    bin_prnt_int(asFib);
//	    cout <<

	    numBitsLeft=fibBits;		
	    while(numBitsLeft) {
//	    if(wordNum<3) cout << wordNum << "\t" << numBitsLeft << endl;
		if(numBitsLeft>(8-overflowBit)) {			
		    bitMask=0;
		    for(bitNum=0;bitNum<(8-overflowBit);bitNum++)
			bitMask|=(1<<bitNum);	   
		    (*overflowChar)|= (asFib&bitMask)<<overflowBit;
		    asFib=(asFib>>(8-overflowBit));
		    numBitsLeft-=(8-overflowBit);
		    overflowChar++;
		    overflowBit=0;
		}
		else {			
		    bitMask=0;
		    for(bitNum=0;bitNum<numBitsLeft;bitNum++)
			bitMask|=(1<<bitNum);
		    (*overflowChar)|= (asFib&bitMask)<<overflowBit;
		    overflowBit+=numBitsLeft;
		    if(overflowBit==8) {
			overflowBit=0;
			overflowChar++;
		    }
		    numBitsLeft=0;
		}
	    }
	    
	}
//	cout << wordNum << "\t" << newVal << endl;
 	numBitsLeft=bitSize;		
	while(numBitsLeft) {
//	    if(wordNum<3) cout << wordNum << "\t" << numBitsLeft << endl;
	    if(numBitsLeft>(8-currentBit)) {			
		bitMask=0;
		for(bitNum=0;bitNum<(8-currentBit);bitNum++)
		    bitMask|=(1<<bitNum);	   
		(*currentChar)|= (newVal&bitMask)<<currentBit;
		newVal=(newVal>>(8-currentBit));
		numBitsLeft-=(8-currentBit);
		currentChar++;
		currentBit=0;
	    }
	    else {			
		bitMask=0;
		for(bitNum=0;bitNum<numBitsLeft;bitNum++)
		    bitMask|=(1<<bitNum);
		(*currentChar)|= (newVal&bitMask)<<currentBit;
		currentBit+=numBitsLeft;
		if(currentBit==8) {
		    currentBit=0;
		    currentChar++;
		}
		numBitsLeft=0;
	    }
	}
    }
//    cout << "countOverflows: " << countOverflows << endl;
    if(overflowBit) overflowChar++;
//    cout << int(overflowChar-buffer) << "\t" << overflowBit << endl;
    return (int)(overflowChar-buffer);

}
 
CompressErrorCode_t AnitaCompress::decodePSWaveLosslessBinFibCombo(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr, ChannelEncodingType_t encType) {

    char *meanPtr=(char*)input;
    short mean=(*meanPtr);

//    if(mean<-1100 || mean>100)
//	printf("binfib mean %d %#x\n",mean,mean);
//    unsigned char flag =  input[1];
    int bitSize=getBinFibBitSize(encType);   
    short overFlowVals[256];
    short xMin=mean-(1<<(bitSize-1));
    short xMax=mean+(1<<(bitSize-1));
    xMax-=1;
    unsigned short flagVal=(1<<bitSize)-1;

    unsigned char *currentChar=&input[1];
    int currentBit=0;
    unsigned char *overflowChar;
    int overflowBit=0;
    overflowChar=currentChar+(MAX_NUMBER_SAMPLES*bitSize)/8;
    overflowBit=(MAX_NUMBER_SAMPLES*bitSize)%8;
    int numOverflows=numBytes-(int)(overflowChar-input);
//    cout << "numOverflows:  " << numOverflows << endl;

    int overflowNum=0;
    unsigned short tempNum;
    int fibNum=0;
    int sampNum=0;
    int tempBit,bitNum;

   //Loop through overflow
    while(overflowNum<260) {
	tempBit=overflowBit;
	int fred = *( (int*) overflowChar);	
//	bin_prnt_int(fred);
	while(tempBit<32) {
	    //    bin_prnt_int((fred>>tempBit));
	    if( ((fred>>tempBit)&0x1) && ((fred>>(tempBit+1))&0x1)) {
		//Got two ones;
		fibNum=0;
		for(bitNum=overflowBit;bitNum<=tempBit+1;bitNum++) {
		    fibNum|= (((fred>>bitNum)&0x1)<<(bitNum-overflowBit));
		}
//		bin_prnt_int(fibNum);
//		cout << fibNum << endl;
//		tempNum=decodeFibonacci(fibNum,tempBit+2-overflowBit);
		tempNum=unfibonacci(fibNum);
//		cout << fibNum << "\t" << tempBit+2-overflowBit 
//		     <<"\t" << tempNum << endl;
		if(tempNum&0x1) {
		    //Positive
//		    tempNum-=1;
		    overFlowVals[overflowNum]=xMax+((tempNum-1)>>1);
		}
		else {
		    //Negative
		    overFlowVals[overflowNum]=xMin-((tempNum-2)>>1);
		}
//		if(overflowNum<2) cout << buffer[overflowNum] << "\t" << tempNum << endl;
//		cout << overflowNum << "\t" <<   overFlowVals[overflowNum] << endl;
		tempBit+=2;
		overflowChar+=(tempBit/8);
		overflowBit=(tempBit%8);
//		cout << tempBit << "\t" << overflowChar << "\t" << overflowBit << endl;

		break;
	    }
//	    cout << ((fred>>tempBit)&0x1) << " ";
	    tempBit++;
	}
	if(tempBit==32) break;
//	cout << endl;
//	cout << overflowNum << "\t" << buffer[overflowNum] << endl;
	overflowNum++;
//	if(overflowNum>4) break;
    }
    numOverflows=overflowNum;
//    cout << "numOverflows:  " << numOverflows << endl;
    overflowNum=0;
    //Loop through binary

    while(sampNum<MAX_NUMBER_SAMPLES) {
	int fred = *( (int*) currentChar);
//	bin_prnt_int(fred);
	tempNum=0;	    
	for(bitNum=currentBit;bitNum<currentBit+bitSize;bitNum++) {
	    tempNum|= (((fred>>bitNum)&0x1)<<(bitNum-currentBit));
//	    bin_prnt_short(tempNum);
//	    cout << (((fred>>bitNum)&0x1)<<(bitNum-currentBit));
	}
//	cout << endl;
//	bin_prnt_short(tempNum);
//	cout << sampNum << "\t" << tempNum << endl;
	if(tempNum==flagVal) {
	    chanPtr->data[sampNum]=overFlowVals[overflowNum];
	    overflowNum++;
	}
	else {
	    chanPtr->data[sampNum]=mean+unbifurcate(tempNum);
	}
	sampNum++;
	currentBit+=bitSize;
	while(currentBit>=8) {
	    currentBit-=8;
	    currentChar++;
	}
    }
    return COMPRESS_E_OK;
}   



int AnitaCompress::encodePSWaveLossyMulawOptimally(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr,int mulawBits, ChannelEncodingType_t *encTypePtr) {
  short xMax=chanPtr->xMax; //Filled by pedestalLib
  short xMin=chanPtr->xMin; //Filled by pedestalLib
  short rangeTotal=xMax-xMin;
  int numBits=1;
  while(rangeTotal>(1<<numBits)) numBits++;
  int inputBits=numBits+1;
  if(inputBits<=mulawBits) return encodePSWaveLosslessBinary(buffer,chanPtr,encTypePtr);
  if(inputBits<6) inputBits=6;
  if(inputBits<mulawBits+2) inputBits=mulawBits+2;
  *encTypePtr=getEncodingTypeFromInputAndMuLawBits(inputBits,mulawBits);
  return encodePSWaveLossyMuLaw(buffer,chanPtr,*encTypePtr);
}

int AnitaCompress::encodePSWaveLossyMuLaw(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr,ChannelEncodingType_t encType)
{
    //Remember this function works an array of pedestal subtracted shorts
    //Which is what SurfChannelPedSubbed_t contains
    int wordNum=0;
    int numBitsLeft;
    int bitMask;
    int bitNum;
    unsigned char *currentChar=buffer;
    int currentBit=0;
    unsigned char newVal=0;
    int inputBits=0,mulawBits=0;
    getInputAndMuLawBits(encType,&inputBits,&mulawBits);
    short maxVal=(1<<(inputBits-1));
    short minVal=-1*maxVal;

    short xMax=chanPtr->xMax; //Filled by pedestalLib
    short xMin=chanPtr->xMin; //Filled by pedestalLib  
    short rangeTotal=xMax-xMin; 
    short *input = chanPtr->data;
    int mean=xMin+(int)rangeTotal/2;
    short testVal;
    int bitSize=mulawBits;

//    printf("mean %d\txMin %d\txMax %d\tbitSize %d\n",mean,xMin,xMax,bitSize);
//    printf("minVal %d, maxVal %d\n",minVal,maxVal);
    char *meanPtr=(char*)currentChar;
    (*meanPtr)=(char)(mean);
    currentChar++;

    for(wordNum=0;wordNum<MAX_NUMBER_SAMPLES;wordNum++) {
      testVal=input[wordNum]-mean;
      if(testVal>maxVal) testVal=maxVal;
      if(testVal<minVal) testVal=minVal;
      newVal=convertToMuLawUC(testVal,inputBits,mulawBits);
//      printf("input %d, testVal %d, newVal %d\n",input[wordNum],testVal,newVal);
      numBitsLeft=bitSize;		
      while(numBitsLeft) {
	if(numBitsLeft>(8-currentBit)) {			
	  bitMask=0;
	  for(bitNum=0;bitNum<(8-currentBit);bitNum++)
	    bitMask|=(1<<bitNum);	   
	  (*currentChar)|= (newVal&bitMask)<<currentBit;
	  newVal=(newVal>>(8-currentBit));
	  numBitsLeft-=(8-currentBit);
	  currentChar++;
	  currentBit=0;
	    }
	else {			
	  bitMask=0;
	  for(bitNum=0;bitNum<numBitsLeft;bitNum++)
	    bitMask|=(1<<bitNum);
	  (*currentChar)|= (newVal&bitMask)<<currentBit;
	  currentBit+=numBitsLeft;
	  if(currentBit==8) {
	    currentBit=0;
	    currentChar++;
	  }
	  numBitsLeft=0;
	}
      }
    }
    //    for(int i=0;i<int(currentChar-buffer);i++) {
    //	cout << i << "\t"  << (int)buffer[i] << endl;
    //    }
    if(currentBit) currentChar++;
    return (int)(currentChar-buffer);
    
    
}



CompressErrorCode_t AnitaCompress::decodePSWaveLossyMuLaw(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr,ChannelEncodingType_t encType)
{
    int sampNum=0;
    char *meanPtr=(char*)input;
    short mean=(short)(*meanPtr);
    int inputBits=0,mulawBits=0;
    getInputAndMuLawBits(encType,&inputBits,&mulawBits);
    int bitSize=mulawBits;
    int bitNum;

    unsigned char *currentChar=&input[1];
    int currentBit=0;
    unsigned char tempNum;
    short newVal;

//    printf("mean %d, mulawBits %d, inputBits %d\n",mean,mulawBits,inputBits);

    while(sampNum<MAX_NUMBER_SAMPLES) {
	int fred = *( (int*) currentChar);
//	bin_prnt_int(fred);
	tempNum=0;	    
	for(bitNum=currentBit;bitNum<currentBit+bitSize;bitNum++) {
	    tempNum|= (((fred>>bitNum)&0x1)<<(bitNum-currentBit));
//	    bin_prnt_short(tempNum);
//	    cout << (((fred>>bitNum)&0x1)<<(bitNum-currentBit));
	}
//	cout << endl;
//	bin_prnt_short(tempNum);
//	cout << sampNum << "\t" << tempNum << endl;
	newVal=convertFromMuLawUC(tempNum,inputBits,mulawBits);
	chanPtr->data[sampNum]=mean+newVal;
//	printf("output %d, newVal %d, tempNum %d\n",chanPtr->data[sampNum],
//	       newVal,tempNum);
	
	sampNum++;
	currentBit+=bitSize;
	while(currentBit>=8) {
	    currentBit-=8;
	    currentChar++;
	}
	if(((int)(currentChar-input))>numBytes) return COMPRESS_E_BADSIZE;
    }
    return COMPRESS_E_OK;


}

void AnitaCompress::getInputAndMuLawBits(ChannelEncodingType_t encType, int *inputPtr,
			  int *mulawPtr) 
{
  switch(encType) {    
    case ENCODE_LOSSY_MULAW_11_8:
      *inputPtr=11;
      *mulawPtr=8;
      return;
    case ENCODE_LOSSY_MULAW_11_7:
      *inputPtr=11;
      *mulawPtr=7;
      return;
    case ENCODE_LOSSY_MULAW_11_6:
      *inputPtr=11;
      *mulawPtr=6;
      return;
    case ENCODE_LOSSY_MULAW_11_5:
      *inputPtr=11;
      *mulawPtr=5;
      return;
    case ENCODE_LOSSY_MULAW_11_4:
      *inputPtr=11;
      *mulawPtr=4;
      return;
    case ENCODE_LOSSY_MULAW_11_3:
      *inputPtr=11;
      *mulawPtr=3;
      return;
    case ENCODE_LOSSY_MULAW_10_8:
      *inputPtr=10;
      *mulawPtr=8;
      return;
    case ENCODE_LOSSY_MULAW_10_7:
      *inputPtr=10;
      *mulawPtr=7;
      return;
    case ENCODE_LOSSY_MULAW_10_6:
      *inputPtr=10;
      *mulawPtr=6;
      return;
    case ENCODE_LOSSY_MULAW_10_5:
      *inputPtr=10;
      *mulawPtr=5;
      return;
    case ENCODE_LOSSY_MULAW_10_4:
      *inputPtr=10;
      *mulawPtr=4;
      return;
    case ENCODE_LOSSY_MULAW_10_3:
      *inputPtr=10;
      *mulawPtr=3;
      return;
    case ENCODE_LOSSY_MULAW_9_7:
      *inputPtr=9;
      *mulawPtr=7;
      return;
    case ENCODE_LOSSY_MULAW_9_6:
      *inputPtr=9;
      *mulawPtr=6;
      return;
    case ENCODE_LOSSY_MULAW_9_5:
      *inputPtr=9;
      *mulawPtr=5;
      return;
    case ENCODE_LOSSY_MULAW_9_4:
      *inputPtr=9;
      *mulawPtr=4;
      return;
    case ENCODE_LOSSY_MULAW_9_3:
      *inputPtr=9;
      *mulawPtr=3;
      return;
    case ENCODE_LOSSY_MULAW_8_6:
      *inputPtr=8;
      *mulawPtr=6;
      return;
    case ENCODE_LOSSY_MULAW_8_5:
      *inputPtr=8;
      *mulawPtr=5;
      return;
    case ENCODE_LOSSY_MULAW_8_4:
      *inputPtr=8;
      *mulawPtr=4;
      return;
    case ENCODE_LOSSY_MULAW_8_3:
      *inputPtr=8;
      *mulawPtr=3;
      return;
    case ENCODE_LOSSY_MULAW_7_5:
      *inputPtr=7;
      *mulawPtr=5;
      return;
    case ENCODE_LOSSY_MULAW_7_4:
      *inputPtr=7;
      *mulawPtr=4;
      return;
    case ENCODE_LOSSY_MULAW_7_3:
      *inputPtr=7;
      *mulawPtr=3;
      return;
    case ENCODE_LOSSY_MULAW_6_4:
      *inputPtr=6;
      *mulawPtr=4;
      return;
  case ENCODE_LOSSY_MULAW_6_3:
      *inputPtr=6;
      *mulawPtr=3;
      return;
  default:
      *inputPtr=11;
      *mulawPtr=8;
      return;
  }
  *inputPtr=11;
  *mulawPtr=11;
  return;
      
}

ChannelEncodingType_t AnitaCompress::getEncodingTypeFromInputAndMuLawBits(int inputBits,
							     int mulawBits)
{
  switch (inputBits) {
  case 11:
    switch (mulawBits) {
    case 8:
      return ENCODE_LOSSY_MULAW_11_8;
    case 7:
      return ENCODE_LOSSY_MULAW_11_7;
    case 6:
      return ENCODE_LOSSY_MULAW_11_6;
    case 5:
      return ENCODE_LOSSY_MULAW_11_5;
    case 4:
      return ENCODE_LOSSY_MULAW_11_4;
    case 3:
      return ENCODE_LOSSY_MULAW_11_3;
    default:
      return ENCODE_LOSSY_MULAW_11_8;
    }
  case 10:
    switch (mulawBits) {
    case 8:
      return ENCODE_LOSSY_MULAW_10_8;
    case 7:
      return ENCODE_LOSSY_MULAW_10_7;
    case 6:
      return ENCODE_LOSSY_MULAW_10_6;
    case 5:
      return ENCODE_LOSSY_MULAW_10_5;
    case 4:
      return ENCODE_LOSSY_MULAW_10_4;
    case 3:
      return ENCODE_LOSSY_MULAW_10_3;
    default:
      return ENCODE_LOSSY_MULAW_10_8;
    }
  case 9:
    switch (mulawBits) {
    case 7:
      return ENCODE_LOSSY_MULAW_9_7;
    case 6:
      return ENCODE_LOSSY_MULAW_9_6;
    case 5:
      return ENCODE_LOSSY_MULAW_9_5;
    case 4:
      return ENCODE_LOSSY_MULAW_9_4;
    case 3:
      return ENCODE_LOSSY_MULAW_9_3;
    default:
      return ENCODE_LOSSY_MULAW_9_7;
    }
  case 8:
    switch (mulawBits) {
    case 6:
      return ENCODE_LOSSY_MULAW_8_6;
    case 5:
      return ENCODE_LOSSY_MULAW_8_5;
    case 4:
      return ENCODE_LOSSY_MULAW_8_4;
    case 3:
      return ENCODE_LOSSY_MULAW_8_3;
    default:
      return ENCODE_LOSSY_MULAW_8_6;
    }
  case 7:
    switch (mulawBits) {
    case 5:
      return ENCODE_LOSSY_MULAW_7_5;
    case 4:
      return ENCODE_LOSSY_MULAW_7_4;
    case 3:
      return ENCODE_LOSSY_MULAW_7_3;
    default:
      return ENCODE_LOSSY_MULAW_7_5;
    }
  case 6:
    switch (mulawBits) {
    case 4:
      return ENCODE_LOSSY_MULAW_6_4;
    case 3:
      return ENCODE_LOSSY_MULAW_6_3;
    default:
      return ENCODE_LOSSY_MULAW_6_4;
    }
  default:
    switch (mulawBits) {
    case 8:
      return ENCODE_LOSSY_MULAW_11_8;
    case 7:
      return ENCODE_LOSSY_MULAW_11_7;
    case 6:
      return ENCODE_LOSSY_MULAW_11_6;
    case 5:
      return ENCODE_LOSSY_MULAW_11_5;
    case 4:
      return ENCODE_LOSSY_MULAW_11_4;
    case 3:
      return ENCODE_LOSSY_MULAW_11_3;
    default:
      return ENCODE_LOSSY_MULAW_11_8;
    }
  }
  return ENCODE_NONE;
}

ChannelEncodingType_t AnitaCompress::getBinaryEncType(int bitSize) {
    switch(bitSize) {
	case 12: return ENCODE_LOSSLESS_12BIT;
	case 11: return ENCODE_LOSSLESS_11BIT;
	case 10: return ENCODE_LOSSLESS_10BIT;
	case 9: return ENCODE_LOSSLESS_9BIT;
	case 8: return ENCODE_LOSSLESS_8BIT;
	case 7: return ENCODE_LOSSLESS_7BIT;
	case 6: return ENCODE_LOSSLESS_6BIT;
	case 5: return ENCODE_LOSSLESS_5BIT;
	case 4: return ENCODE_LOSSLESS_4BIT;
	case 3: return ENCODE_LOSSLESS_3BIT;
	case 2: return ENCODE_LOSSLESS_2BIT;
	case 1: return ENCODE_LOSSLESS_1BIT;
	default:
	    return ENCODE_LOSSLESS_12BIT;
    }
    return ENCODE_LOSSLESS_12BIT;
}
	    
    
int AnitaCompress::getBinaryBitSize(ChannelEncodingType_t encType) {
    switch(encType) {
	case ENCODE_LOSSLESS_12BIT: return 12;
	case ENCODE_LOSSLESS_11BIT: return 11;
	case ENCODE_LOSSLESS_10BIT: return 10;	   
	case ENCODE_LOSSLESS_9BIT: return 9;
	case ENCODE_LOSSLESS_8BIT: return 8;
	case ENCODE_LOSSLESS_7BIT: return 7;
	case ENCODE_LOSSLESS_6BIT: return 6;
	case ENCODE_LOSSLESS_5BIT: return 5;
	case ENCODE_LOSSLESS_4BIT: return 4;
	case ENCODE_LOSSLESS_3BIT: return 3;
	case ENCODE_LOSSLESS_2BIT: return 2;
	case ENCODE_LOSSLESS_1BIT: return 1;
	default:
	    return 12;
    }
    return 12;
}


ChannelEncodingType_t AnitaCompress::getBinFibEncType(int bitSize) {
    switch(bitSize) {
	case 10: return ENCODE_LOSSLESS_BINFIB_10BIT;
	case 9: return ENCODE_LOSSLESS_BINFIB_9BIT;
	case 8: return ENCODE_LOSSLESS_BINFIB_8BIT;
	case 7: return ENCODE_LOSSLESS_BINFIB_7BIT;
	case 6: return ENCODE_LOSSLESS_BINFIB_6BIT;
	case 5: return ENCODE_LOSSLESS_BINFIB_5BIT;
	case 4: return ENCODE_LOSSLESS_BINFIB_4BIT;
	case 3: return ENCODE_LOSSLESS_BINFIB_3BIT;
	case 2: return ENCODE_LOSSLESS_BINFIB_2BIT;
	case 1: return ENCODE_LOSSLESS_BINFIB_1BIT;
	default:
	    return ENCODE_LOSSLESS_BINFIB_10BIT;
    }
    return ENCODE_LOSSLESS_BINFIB_10BIT;
}
	    
    
int AnitaCompress::getBinFibBitSize(ChannelEncodingType_t encType) {
    switch(encType) {
	case ENCODE_LOSSLESS_BINFIB_10BIT: return 10;	   
	case ENCODE_LOSSLESS_BINFIB_9BIT: return 9;
	case ENCODE_LOSSLESS_BINFIB_8BIT: return 8;
	case ENCODE_LOSSLESS_BINFIB_7BIT: return 7;
	case ENCODE_LOSSLESS_BINFIB_6BIT: return 6;
	case ENCODE_LOSSLESS_BINFIB_5BIT: return 5;
	case ENCODE_LOSSLESS_BINFIB_4BIT: return 4;
	case ENCODE_LOSSLESS_BINFIB_3BIT: return 3;
	case ENCODE_LOSSLESS_BINFIB_2BIT: return 2;
	case ENCODE_LOSSLESS_BINFIB_1BIT: return 1;
	default:
	    return 10;
    }
    return 10;
}
    





unsigned short AnitaCompress::simpleCrcShort(unsigned short *p, unsigned long n)
{

    unsigned short sum = 0;
    unsigned long i;
    for (i=0L; i<n; i++) {
	sum += *p++;
    }
    return ((0xffff - sum) + 1);
}

const char *AnitaCompress::compressErrorCodeAsString(CompressErrorCode_t code) 
{
    const char *string;
    switch(code) {
	case COMPRESS_E_OK:
	    string = "Success";
	    break;
	case COMPRESS_E_PACK:
	    string = "Error packing data";
	    break;
	case COMPRESS_E_BADSIZE:
	    string = "Enocded data size wrong";
	    break;
	case COMPRESS_E_UNPACK:
	    string = "Error unpacking data";
	    break;
	case COMPRESS_E_NO_PEDS:
	    string = "Pedestals not available";
	    break;
	case COMPRESS_E_BAD_PEDS:
	    string = "Pedestals malformed";
	    break;
	case COMPRESS_E_TOO_BIG:
	    string = "Data overflow";
	    break;
	case COMPRESS_E_BAD_CRC:
	    string = "Checksum mismatch";
	    break;
	default:
      string = "Unknown error code" ;
   }

   return (string) ;
}
	    
#ifndef MEMCPY
/* For unix, use the following: */
#define MEMCPY	memcpy
#endif

void AnitaCompress::unpack12bit(unsigned short *w, void *srcp)
{
    char *dumPtr = (char *) srcp;
    unsigned short val;
    w[3] = 0;

    MEMCPY((char *)&val, dumPtr, sizeof(short));
    w[0] = val & 0x0fff;
    w[3] += ((val & 0xf000) >> 12);

    MEMCPY((char *)&val, dumPtr + sizeof(short), sizeof(short));
    w[1] = val & 0x0fff;
    w[3] += ((val & 0xf000) >> 8);

    MEMCPY((char *)&val, dumPtr + (2 * sizeof(short)), sizeof(short));
    w[2] = val & 0x0fff;
    w[3] += ((val & 0xf000) >> 4);
}

void AnitaCompress::pack12bit(unsigned short *w, unsigned char *destp)
{
    unsigned short val;

    val = (w[0] & 0x0fff) + ((w[3] << 12) & 0xf000);
    MEMCPY(destp, (char *)&val, sizeof(short));

    val = (w[1] & 0x0fff) + ((w[3] <<  8) & 0xf000);
    MEMCPY(destp+2, (char *)&val, sizeof(short));

    val = (w[2] & 0x0fff) + ((w[3] <<  4) & 0xf000);
    MEMCPY(destp+4, (char *)&val, sizeof(short));
}


unsigned char AnitaCompress::convertToMuLawUC(short input, int inputBits,
			       int mulawBits) 
{
    char val=convertToMuLaw(input,inputBits,mulawBits);
//    printf("c %d, uc %d\n",val,charbifurcate(val));
    return charbifurcate(val);
}

char AnitaCompress::convertToMuLaw(short input, int inputBits, int mulawBits) 
{    
    //It is the responsibilty of the caller to ensure input is within the range specified.
    //Might add checks as I don't trust the user.
    int index=input+(1<<(inputBits-1));
    switch (inputBits) {
	case 11:
	    switch (mulawBits) {
		case 8:
		    return linear11toMuLaw8[index];
		case 7:
		    return linear11toMuLaw7[index];
		case 6:
		    return linear11toMuLaw6[index];
		case 5:
		    return linear11toMuLaw5[index];
		case 4:
		    return linear11toMuLaw4[index];
		case 3:
		    return linear11toMuLaw3[index];
		default:
		    return linear11toMuLaw8[index];
	    }
	case 10:
	    switch (mulawBits) {
		case 8:
		    return linear10toMuLaw8[index];
		case 7:
		    return linear10toMuLaw7[index];
		case 6:
		    return linear10toMuLaw6[index];
		case 5:
		    return linear10toMuLaw5[index];
		case 4:
		    return linear10toMuLaw4[index];
		case 3:
		    return linear10toMuLaw3[index];
		default:
		    return linear10toMuLaw8[index];
	    }
	case 9:
	    switch (mulawBits) {
		case 7:
		    return linear9toMuLaw7[index];
		case 6:
		    return linear9toMuLaw6[index];
		case 5:
		    return linear9toMuLaw5[index];
		case 4:
		    return linear9toMuLaw4[index];
		case 3:
		    return linear9toMuLaw3[index];
		default:
		    return linear9toMuLaw7[index];
	    }
	case 8:
	    switch (mulawBits) {
		case 6:
		    return linear8toMuLaw6[index];
		case 5:
		    return linear8toMuLaw5[index];
		case 4:
		    return linear8toMuLaw4[index];
		case 3:
		    return linear8toMuLaw3[index];
		default:
		    return linear8toMuLaw6[index];
	    }
	case 7:
	    switch (mulawBits) {
		case 5:
		    return linear7toMuLaw5[index];
		case 4:
		    return linear7toMuLaw4[index];
		case 3:
		    return linear7toMuLaw3[index];
		default:
		    return linear7toMuLaw5[index];
	    }
	case 6:
	    switch (mulawBits) {
		case 4:
		    return linear6toMuLaw4[index];
		case 3:
		    return linear6toMuLaw3[index];
		default:
		    return linear6toMuLaw4[index];
	    }
	default:
	    switch (mulawBits) {
		case 8:
		    return linear11toMuLaw8[index];
		case 7:
		    return linear11toMuLaw7[index];
		case 6:
		    return linear11toMuLaw6[index];
		case 5:
		    return linear11toMuLaw5[index];
		case 4:
		    return linear11toMuLaw4[index];
		case 3:
		    return linear11toMuLaw3[index];
		default:
		    return linear11toMuLaw8[index];
	    }
    }
    return 0;
}

short AnitaCompress::convertFromMuLawUC(unsigned char input, int outputBits, int mulawBits) 
{
//    printf("uc %d, c %d\n",input,charunbifurcate(input));
    return convertFromMuLaw(charunbifurcate(input),outputBits,mulawBits);
}

short AnitaCompress::convertFromMuLaw(char input, int outputBits, int mulawBits) 
{    
    int index=input+(1<<(mulawBits-1));
    switch (outputBits) {
	case 11:
	    switch (mulawBits) {
		case 8:
		    return mulaw8toLinear11[index];
		case 7:
		    return mulaw7toLinear11[index];
		case 6:
		    return mulaw6toLinear11[index];
		case 5:
		    return mulaw5toLinear11[index];
		case 4:
		    return mulaw4toLinear11[index];
		case 3:
		    return mulaw3toLinear11[index];
		default:
		    return mulaw8toLinear11[index];
	    }
	case 10:
	    switch (mulawBits) {
		case 8:
		    return mulaw8toLinear10[index];
		case 7:
		    return mulaw7toLinear10[index];
		case 6:
		    return mulaw6toLinear10[index];
		case 5:
		    return mulaw5toLinear10[index];
		case 4:
		    return mulaw4toLinear10[index];
		case 3:
		    return mulaw3toLinear10[index];
		default:
		    return mulaw8toLinear10[index];
	    }
	case 9:
	    switch (mulawBits) {
		case 7:
		    return mulaw7toLinear9[index];
		case 6:
		    return mulaw6toLinear9[index];
		case 5:
		    return mulaw5toLinear9[index];
		case 4:
		    return mulaw4toLinear9[index];
		case 3:
		    return mulaw3toLinear9[index];
		default:
		    return mulaw7toLinear9[index];
	    }
	case 8:
	    switch (mulawBits) {
		case 6:
		    return mulaw6toLinear8[index];
		case 5:
		    return mulaw5toLinear8[index];
		case 4:
		    return mulaw4toLinear8[index];
		case 3:
		    return mulaw3toLinear8[index];
		default:
		    return mulaw6toLinear8[index];
	    }
	case 7:
	    switch (mulawBits) {
		case 5:
		    return mulaw5toLinear7[index];
		case 4:
		    return mulaw4toLinear7[index];
		case 3:
		    return mulaw3toLinear7[index];
		default:
		    return mulaw5toLinear7[index];
	    }
	case 6:
	    switch (mulawBits) {
		case 4:
		    return mulaw4toLinear6[index];
		case 3:
		    return mulaw3toLinear6[index];
		default:
		    return mulaw4toLinear6[index];
	    }
	default:
	    switch (mulawBits) {
		case 8:
		    return mulaw8toLinear11[index];
		case 7:
		    return mulaw7toLinear11[index];
		case 6:
		    return mulaw6toLinear11[index];
		case 5:
		    return mulaw5toLinear11[index];
		case 4:
		    return mulaw4toLinear11[index];
		case 3:
		    return mulaw3toLinear11[index];
		default:
		    return mulaw8toLinear11[index];
	    }
    }
    return 0;
}


unsigned int fFibNums[24]={1,2,3,5,8,13,21,34,55,89,
		     144,233,377,610,987,1597,2584,4181,6765,10946,
		     17711,28657,46368,75025}; 
//these are enough to encode unsigned short

//convert a short to its fibonacci representation with a 11 prefix
unsigned int AnitaCompress::fibonacci(unsigned short input){
    int numBits;
    return encodeFibonacci(input,&numBits);
}

unsigned int AnitaCompress::encodeFibonacci(unsigned short input,int *numBits){
     unsigned int output;
     int i;
     i=0;
     // find the first fibonacci number greater than input
     while(input>=fFibNums[i]){
	  i++;
     }
     // set the prefix bit
     (*numBits)=i+1;
     output=1<<(i);
     i--; // we are now at the largest fibonacci number less than input
     //now set the bits for the minimal fibonacci representation
     while(i>=0){
	  if (input>=fFibNums[i]){
	       output += (1<<i);
	       input -= fFibNums[i];
	  }
	  i--;
     }  
     return output;
}

// reverse the above operation
unsigned short AnitaCompress::unfibonacci(unsigned int input)
{
     unsigned int i;
     unsigned short output;
     unsigned int curbit, lastbit;
     output=0;
     lastbit=0;
     for (i=0; i<23; i++){
	  curbit=(input>>i) & 0x0001;
	  if (curbit==1 && lastbit==1) break;  // done--found the prefix bit
	  else if (curbit==1) output+=fFibNums[i]; 
	  lastbit=curbit;
     }
     return output;
}



// perform the bifurcation mapping from integers to the positive
// integers as a prelude to applying prefix codes (e.g. fibonacci)
//
// chosen mapping is 0,-1,1,-2,2... => 1,2,3,4,5
// 
// posi integers map to 2*n+1
// negative integers map 2*abs(n)
//

unsigned short AnitaCompress::bifurcate(short input)
{
     unsigned short output;
     if (input>=0){
	  output=(2*(unsigned short) input + 1);
     }
     else{
	  output=(2* (unsigned short)abs(input));
     }
     return output;
}

short AnitaCompress::unbifurcate(unsigned short input)
{
     // note zero is not a valid input
     short output;
     if ((input % 2) == 1){//odd => non-negative
	  output=((input-1)/2);
     }
     else{ //even => negative
	  output=-(input/2);
     }
     return output;
}

unsigned char AnitaCompress::charbifurcate(char input)
{
     unsigned char output;
     if (input>=0){
	  output=(2*(unsigned char) input + 1);
     }
     else{
	 input*=-1;
	 output=(2* (unsigned char)input);
     }
     return output;
}

char AnitaCompress::charunbifurcate(unsigned char input)
{
     // note zero is not a valid input
     char output;
     if ((input % 2) == 1){//odd => non-negative
	  output=((input-1)/2);
     }
     else{ //even => negative
	  output=-(input/2);
     }
     return output;
}
	  
	  
	  
// discard (in place) the last nbits bits of the data, shifting right
// number of words is nwords
void AnitaCompress::bitstrip(unsigned short nbits, unsigned short nwords, unsigned short *data)
{
     unsigned short i;
     for (i=0; i<nwords; i++){
	  data[i]=data[i]>>nbits;
     }
}


// pack the nbits least-significant bits of the data
// into a byte stream.  Returns number of bytes of buffer used.
// The input data are left unchanged.
unsigned short AnitaCompress::bitpack(unsigned short nbits, unsigned short nwords,
	     unsigned short *in, unsigned char *out)
{
     unsigned short i;
     unsigned short scratch, mask1,mask2, byte, bit, nbitsleft;
     byte=0; //current output byte
     bit=0; //current output bit
     out[0]=0;
     mask1= ~(0xffff << (unsigned short) nbits); //mask to yield nbits lsbs
     for (i=0; i<nwords; i++){
	  scratch= in[i] & mask1;
	  nbitsleft=nbits;
	  while (nbitsleft>0){
	       if (nbitsleft>(8-bit)){ //won't all fit
		    mask2 = ~((0xffff)<<(8-bit));
		    out[byte] |= (scratch & mask2) << bit;
		    mask2 = ~mask2;
		    scratch = ((scratch & mask2) >> (8-bit));
		    nbitsleft -= (8-bit);
		    byte++; bit=0; out[byte]=0;
	       }
	       else { //everything fits in current byte
		    out[byte] |= scratch << bit;
		    bit = bit + nbitsleft;
		    if (bit==8){ byte++; bit=0; out[byte]=0;}
		    nbitsleft=0;
	       }
	  }
     }
     if (bit==0) return byte;
     else return byte+1;
}

//pack fibonacci code into bytes

int AnitaCompress::codepack(int n, unsigned int *in, unsigned char *out)
{
     //need to add checks on m!!!
     int byte,i;
     unsigned char bit, nbits;
     unsigned int sbit,scratch,mask;
     // in[n] is packed into out with leading zeros removed.
     // it must be filled with nonzero data in a prefix code
     byte=0;//current output byte
     bit=0;//next bit to be written
     out[0]=0;
     for (i=0; i<n; i++){ //i indexes input array
	  //how many bits in in[i]?
	  nbits=0;sbit=0x1;
	  while (sbit<=in[i]) {sbit=sbit<<1; nbits++;}
	  //there are nbit bits before there are leading zeros
	  //last 1 is in bit (nbit-1)
	  scratch=in[i];
	  while (nbits>0){
	       if (nbits>((unsigned char)8-bit)){
               //pack the least significant bits into the current byte
		    mask= ~((unsigned int)(0xffffffff)<<(8-bit));
		    out[byte] |= (scratch & mask) << bit;
		    mask = ~mask;
		    scratch=(scratch & mask) >> (8-bit);
		    nbits -= (8-bit);
		    byte++; bit=0; out[byte]=0;
	       }
	       else{//everything fits
		    out[byte] |= scratch << bit;
		    bit= bit + nbits;
		    if (bit==8){ byte++; bit=0; out[byte]=0;}
		    nbits=0;
	       }
	  }
     }
     if (bit==0) return byte; 
     else return byte+1;
}

//unpack fibonacci code 

int AnitaCompress::codeunpack(int m, unsigned char *in, unsigned int *out)
{
     int inbyte, inbit;
     int outword, outbit;
     unsigned int curbit, lastbit;
/* proceed one bit at a time.  When two ones are seen, advance
	to the next output word */
     inbyte=0; inbit=0;
     outword=0,outbit=0;
     lastbit=0;
     out[outword]=0;
     while (inbyte<m){
	  curbit=(in[inbyte]>>inbit) & 0x1;
	  out[outword] |= curbit << outbit;
	  inbit++; outbit++; 
	  if (inbit==8){ inbyte++; inbit=0;}
	  if ((curbit==1 && lastbit==1)|| outbit==32){
	       outword++; outbit=0; out[outword]=0;
	       //printf("%i %hx\n",outword,out[outword-1]);
	  }
	  if (outbit!=0) lastbit=curbit;
	  else lastbit=0;
     }
     return outword;
}


     
