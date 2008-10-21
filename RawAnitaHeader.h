//////////////////////////////////////////////////////////////////////////////
/////  RawAnitaHeader.h        Raw ANITA header class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ANITA headers in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWANITAHEADER_H
#define RAWANITAHEADER_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"

//!  RawAnitaHeader -- The Raw ANITA Event Header
/*!
  The ROOT implementation of the raw ANITA event header
  \ingroup rootclasses
*/
class RawAnitaHeader: public TObject
{
 public:
   RawAnitaHeader(); ///< Default constructor
   RawAnitaHeader(AnitaEventHeader_t *hdPtr, Int_t run, UInt_t realTime,
		  UInt_t triggerTime, UInt_t triggerTimeNs,Int_t tgoodTimeFlag); ///< Assignment constructor
  RawAnitaHeader(AnitaEventHeaderVer13_t *hdPtr, Int_t run, UInt_t realTime,
		 UInt_t triggerTime, UInt_t triggerTimeNs,Int_t tgoodTimeFlag); ///< Version 13 constructor
  RawAnitaHeader(AnitaEventHeaderVer12_t *hdPtr, Int_t run, UInt_t realTime,
		 UInt_t triggerTime, UInt_t triggerTimeNs,Int_t tgoodTimeFlag); ///< Version 12 constructor
  RawAnitaHeader(AnitaEventHeaderVer11_t *hdPtr, Int_t run, UInt_t realTime,
		 UInt_t triggerTime, UInt_t triggerTimeNs,Int_t tgoodTimeFlag); ///< Version 11 constructor
  RawAnitaHeader(AnitaEventHeaderVer10_t *hdPtr, Int_t run, UInt_t realTime,
		 UInt_t triggerTime, UInt_t triggerTimeNs,Int_t tgoodTimeFlag); ///< Version 10 constructor
  ~RawAnitaHeader(); ///< Destructor

  Int_t           run; ///< Run number, assigned on ground
  UInt_t          realTime; ///< unixTime of readout
  UInt_t          payloadTime; ///< unixTime of readout
  UInt_t          payloadTimeUs; ///< sub second time of readout
  UInt_t          gpsSubTime; ///< sub second time from GPS (if matched)
  UInt_t          turfEventId; ///< TURF Event Id (12-bit run + 20-bit event)
  UInt_t          eventNumber; ///< Software event number
  //!  Calib/Relay Status
  /*!
Here we are coutning bits from 1 to 16
    -  "Bit 1" 1st RFCM relay on/off
    -  "Bit 2" 2nd RFCM relay on/off
    -  "Bit 3" 3rd RFCM relay on/off
    -  "Bit 4" 4th RFCM relay on/off
    -  "Bit 5" Naidr RFCM relay on/off
    -  "Bit 6" GPS relay on/off
    -  "Bit 7" Cal Pulser relay on/off
    -  "Bit 8" Unused
    -  "Bit 9" RFCM Switch Port D
    -  "Bit 10" RFCM Switch Port C
    -  "Bit 11" RFCM Switch Port B
    -  "Bit 12" RFCM Switch Port A
    -  "Bit 13:16" Attenuator Setting

For the attenuator setting take (calibStatus&0xf000)>>12 and:

    -  "Value 7" 0th Attenuator setting (0dB)
    -  "Value 3" 1st Attenuator setting (3dB)
    -  "Value 5" 2nd Attenuator setting (8dB)
    -  "Value 1" 3rd Attenuator setting (12dB)
    -  "Value 6" 4th Attenuator setting (18dB)
    -  "Value 2" 5th Attenuator setting (22dB)
    -  "Value 4" 6th Attenuator setting (28dB)
    -  "Value 0" 7th Attenuator setting (33dB)
    
  */
  UShort_t        calibStatus; 
  UChar_t         priority; ///< Queue (lower 4-bits) and priority (upper 4-bits)
  UChar_t         turfUpperWord; ///< Upper word from TURF, useful for debugging
  UChar_t         otherFlag; ///< Currently the first two surf evNums 
 //!  Error Flag
  /*!
    Here we are coutning bits from 1 to 8
    -  "Bit 1" means sync slip between TURF and software
    -  "Bit 2" is sync slip between SURF 1 and software
    -  "Bit 3" is sync slip between SURF 10 and SURF 1
    -  "Bit 4" is non matching TURF test pattern
  */
  UChar_t         errorFlag;
  UChar_t         surfSlipFlag; ///< Sync Slip between SURF 2-9 and SURF 1
  UChar_t         nadirAntTrigMask; ///< 8-bit nadir phi mask (from TURF)
  UInt_t          antTrigMask; ///< 2x 16-bit phi ant mask (from TURF)
  UShort_t        phiTrigMask; ///< 16-bit phi mask (from TURF)
  UChar_t         reserved[2]; ///< [0]&0xf is TURF hold for this trigger, [0]&0xf0 is active TURF holds
  UChar_t         trigType; ///< Bit 0 is RF, 1 is ADU5, 2 is G12, 3 is software/external
  UChar_t         l3Type1Count; ///< Count of l3 type 1 triggers
  UShort_t        trigNum; ///< Trigger number (since last clear all)
  UInt_t          trigTime; ///< Trigger time in TURF clock ticks
  UInt_t          c3poNum; ///< Number of TURF clock ticks between GPS pulse per seconds
  UShort_t        ppsNum; ///< Number of GPS PPS since last clear all
  UShort_t        deadTime; ///< Deadtime as measured as a fraction of a second that all buffers were full (65535 means 100% dead)
  UChar_t         bufferDepth; ///< Buffer depth 
  UChar_t         turfioReserved; ///< Reserved
  UShort_t        upperL1TrigPattern; ///< Bit mask for upper ring l1 antenna triggers
  UShort_t        lowerL1TrigPattern; ///< Bit mask for lower ring l1 antenna triggers
  UShort_t        upperL2TrigPattern; ///< Bit mask for upper ring l2 cluster triggers
  UShort_t        lowerL2TrigPattern; ///< Bit mask for lower ring l2 cluster triggers
  UShort_t        l3TrigPattern; ///< Bit mask for l3 global triggers
  UShort_t        otherTrigPattern[3]; ///< Other trig patterns -- currently reserved
  UChar_t         nadirL1TrigPattern; ///< 8-bit trigger mask for L1 nadir triggers
  UChar_t         nadirL2TrigPattern; ///< 8-bit trigger mask for L2 nadir triggers
  UInt_t          triggerTime; ///< Trigger time from TURF converted to unixTime
  UInt_t          triggerTimeNs; ///< Trigger time in ns from TURF
  Int_t           goodTimeFlag; ///< 1 is good trigger time, 0 is bad trigger time
   
  char *trigTypeAsString(); ///< Returns trigger type as string
  int isInL3Pattern(int phi); ///< Returns 1 if phi sector had l3 trigger
  int isInL2Pattern(int phi, AnitaRing::AnitaRing_t ring); ///< Returns 1 if given phi-ring had l2 trigger
  int isInL1Pattern(int phi, AnitaRing::AnitaRing_t ring); ///< Returns 1 if given phi-ring had l1 trigger
   
  ClassDef(RawAnitaHeader,13);
};


#endif //RAWANITAHEADER_H
