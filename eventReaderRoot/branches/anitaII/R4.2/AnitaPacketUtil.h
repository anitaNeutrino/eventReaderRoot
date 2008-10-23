//////////////////////////////////////////////////////////////////////////////
/////  AnitaPacketUtil.h        ANITA Packet Util                        /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A handy file containing a set of useful functions for          /////
/////   understanding packetised ANITA-II data                           /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////


#ifndef ANITAPACKETUTIL_H
#define ANITAPACKETUTIL_H

#ifndef ROOT_Rtypes
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Rtypes.h"
#endif
#endif

#include "simpleStructs.h"

/** @defgroup packetutil The Packet Utilities
 * These are utilities for analysing the packetised telemetry data.
 */

//Generic Header Stuff
//!  fillGenericHeader -- utility function
/*!
  Utility function useful when reconstituting raw data.
  \ingroup packetutil
*/
void fillGenericHeader(void *thePtr, PacketCode_t code, unsigned short numBytes);

//!  checkPacket -- utility function
/*!
  Utility function useful when looking at telemetry data
  \ingroup packetutil
*/
int checkPacket(void *thePtr);
//!  simpleIntCrc -- utility function
/*!
  Utility function useful when looking at telemetry data
  \ingroup packetutil
*/
unsigned int simpleIntCrc(unsigned int *p, unsigned int n);
//!  packetCodeAsString -- utility function
/*!
  Converts packet code enumeration to char string
  \ingroup packetutil
*/
char *packetCodeAsString(PacketCode_t code);
//!  unzipZippedPacket -- utility function
/*!
  Utility function useful when looking at zipped telemetry data
  \ingroup packetutil
*/
int unzipZippedPacket(ZippedPacket_t *zipPacket, char *output, unsigned int numBytesOut);
//!  zipBuffer -- utility function
/*!
  Utility function useful when looking at telemetry data
  \ingroup packetutil
*/
int zipBuffer(char *input, char *output, unsigned int inputBytes, unsigned int *outputBytes);
//!  unzipBuffer -- utility function
/*!
  Utility function useful when looking at telemetry data
  \ingroup packetutil
*/
int unzipBuffer(char *input, char *output, unsigned int inputBytes, unsigned int *outputBytes);
//Compressed Waveform Stuff


//!  CompressErrorCode_t -- Enumeration for compression errors
/*!
  Enumeration for compression errors
  \ingroup packetutil
*/
typedef enum {
    COMPRESS_E_OK = 0,
    COMPRESS_E_PACK = 0x100,
    COMPRESS_E_BADSIZE,
    COMPRESS_E_UNPACK,
    COMPRESS_E_NO_PEDS,
    COMPRESS_E_BAD_PEDS,
    COMPRESS_E_TOO_BIG,
    COMPRESS_E_BAD_CRC,
    COMPRESS_E_BAD_CODE
} CompressErrorCode_t ;

//!  AnitaCompress -- Tools to unpack compressed waveform data
/*!
  Tools to uncompress compressed waveform data. Can't be bothered to comment on all the functions. Mainly becasue I don't really remember how most of them work.
  \ingroup packetutil
*/
namespace AnitaCompress {

#define MAX_WAVE_BUFFER NUM_DIGITZED_CHANNELS*MAX_NUMBER_SAMPLES*4


    unsigned short bifurcate(short input);
    short unbifurcate(unsigned short input);
    unsigned char charbifurcate(char input);
    char charunbifurcate(unsigned char input);
    unsigned short bitpack(unsigned short bits,unsigned short nwords, 
			   unsigned short *in, unsigned char *out);
    void bitstrip(unsigned short nbits,unsigned short nwords,unsigned short *data);
    int codepack(int n, unsigned int *in, unsigned char *out);
    int codeunpack(int m, unsigned char *in, unsigned int *out);
    unsigned int fibonacci(unsigned short input);
    unsigned int encodeFibonacci(unsigned short input,int *numBits);
    
    unsigned short unfibonacci(unsigned int input);
  
    
    //Now we'll add the wrapper routines that will actually be called

    CompressErrorCode_t packPedSubbedEvent(PedSubbedEventBody_t *bdPtr,
					   EncodeControlStruct_t *cntlPtr,
					   unsigned char *output,
					   int *numBytes);
    CompressErrorCode_t unpackToPedSubbedEvent(PedSubbedEventBody_t *bdPtr,
					       unsigned char *input,
					       int numBytes);
    CompressErrorCode_t unpackOneSurfToPedSubbedEvent(PedSubbedEventBody_t *bdPtr,
						      unsigned char *input,
						      int numBytes);
    CompressErrorCode_t unpackOneWaveToPedSubbedEvent(PedSubbedEventBody_t *bdPtr,
						      unsigned char *input,
						      int numBytes);
    CompressErrorCode_t 
    unpackToPedSubbedEventWithStats(PedSubbedEventBody_t *bdPtr,
				    unsigned char *input,
				    int numBytes,
				    EncodeControlStruct_t *cntlPtr,
				    int *sizeArray);
				  
	
//And here are it's worker routines
    void fillMinMaxMeanRMS(SurfChannelPedSubbed_t *chanPtr);
    unsigned short simpleCrcShort(unsigned short *p, unsigned long n);
    int encodeChannel(ChannelEncodingType_t encType, SurfChannelFull_t *chanPtr, unsigned char *buffer);
    CompressErrorCode_t decodeChannel(EncodedSurfChannelHeader_t *encChanHdPtr, unsigned char *input, SurfChannelFull_t *chanPtr);
    int encodeWaveNone(unsigned char *buffer,SurfChannelFull_t *chanPtr);
    CompressErrorCode_t decodeWaveNone(unsigned char *input,int numBytes,SurfChannelFull_t *chanPtr);

    int encodePSChannel(ChannelEncodingType_t encType, SurfChannelPedSubbed_t *chanPtr, unsigned char *buffer) ;
    CompressErrorCode_t decodePSChannel(EncodedSurfChannelHeader_t *encChanHdPtr,unsigned char *input, SurfChannelPedSubbed_t *chanPtr);


    int encodePSWaveNone(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr);
    CompressErrorCode_t decodePSWaveNone(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr);

    int encodePSWave12bitBinary(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr);
    CompressErrorCode_t decodePSWave12bitBinary(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr);

    int encodePSWaveLosslessBinary(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr,ChannelEncodingType_t *encTypePtr );
    CompressErrorCode_t decodePSWaveLosslessBinary(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr, ChannelEncodingType_t encType);

    int encodePSWaveLosslessFibonacci(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr);
    CompressErrorCode_t decodePSWaveLosslessFibonacci(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr);

    int encodePSWaveLosslessBinFibCombo(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr, ChannelEncodingType_t *encTypePtr);
    CompressErrorCode_t decodePSWaveLosslessBinFibCombo(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr, ChannelEncodingType_t encType);


  int encodePSWaveLossyMulawOptimally(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr,int mulawBits, ChannelEncodingType_t *encTypePtr);
  int encodePSWaveLossyMuLaw(unsigned char *buffer,SurfChannelPedSubbed_t *chanPtr,ChannelEncodingType_t encType);
  CompressErrorCode_t decodePSWaveLossyMuLaw(unsigned char *input,int numBytes,SurfChannelPedSubbed_t *chanPtr,ChannelEncodingType_t encType);



  void getInputAndMuLawBits(ChannelEncodingType_t encType, int *inputPtr,
			    int *mulawPtr) ;
  
  
  ChannelEncodingType_t getEncodingTypeFromInputAndMuLawBits(int inputBits,
							     int mulawBits);


    ChannelEncodingType_t getBinaryEncType(int bitSize);
    int getBinaryBitSize(ChannelEncodingType_t encType);
    ChannelEncodingType_t getBinFibEncType(int bitSize);
    int getBinFibBitSize(ChannelEncodingType_t encType);

    char *compressErrorCodeAsString(CompressErrorCode_t code);
    
#define PACKVALS 4      // no. of 16-bit values packed into each unit
#define PACKWDS  3      // no. of 16-bit words the values are packed into
    void unpack12bit(unsigned short *w, void *srcp);
    void pack12bit(unsigned short *w, unsigned char *destp);


    char convertToMuLaw(short input, int inputBits, int mulawBits);
    unsigned char convertToMuLawUC(short input, int inputBits,int mulawBits);
    short convertFromMuLawUC(unsigned char input, int outputBits, int mulawBits);
    short convertFromMuLaw(char input, int outputBits, int mulawBits);

    
    unsigned int fibonacci(unsigned short input);
    unsigned int encodeFibonacci(unsigned short input,int *numBits);
    unsigned short unfibonacci(unsigned int input);
}

#endif //ANITAPACKETUTIL_H
