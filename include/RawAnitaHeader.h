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

   RawAnitaHeader(AnitaEventHeaderVer40_t *hdPtr, Int_t trun, UInt_t trealTime,
		  UInt_t ttriggerTime, UInt_t ttriggerTimeNs, Int_t tgoodTimeFlag);
  RawAnitaHeader(AnitaEventHeaderVer33_t *hdPtr, Int_t run, UInt_t realTime,
  		 UInt_t triggerTime, UInt_t triggerTimeNs,Int_t tgoodTimeFlag); ///< Version 33 constructor
  RawAnitaHeader(AnitaEventHeaderVer30_t *hdPtr, Int_t run, UInt_t realTime,
  		 UInt_t triggerTime, UInt_t triggerTimeNs,Int_t tgoodTimeFlag); ///< Version 30 constructor
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
    -  "Bits 5-8" reserved (currently zero)
  */
  UChar_t         errorFlag;
  UChar_t         surfSlipFlag; ///< Sync Slip between SURF 2-9 and SURF 1
  UChar_t         nadirAntTrigMask; ///< 8-bit nadir phi mask (from TURF)
  UInt_t          antTrigMask; ///< Deprecated

  UShort_t        l2TrigMask;
  UShort_t        l2TrigMaskH; ///<Deprecated


  //these are l1's because there is no need for L2 offline trig masks in A4 (since the scaler worked properly)
  // andl l1=l2 in A3, and this way we don't have to write a #pragma read rule for these
  UShort_t        l1TrigMaskOffline;
  UShort_t        l1TrigMaskHOffline; ///<Deprecated

  UShort_t        phiTrigMask; ///< 16-bit phi mask (from TURF)
  UShort_t        phiTrigMaskH; ///< 16-bit phi mask (from TURF)
  UShort_t        phiTrigMaskOffline; ///< 16-bit phi mask (from TURF)
  UShort_t        phiTrigMaskHOffline; ///< 16-bit phi mask (from TURF)

  //Prioritizer stuff
  UChar_t peakThetaBin;
  UShort_t imagePeak;
  UShort_t coherentSumPeak;

//!  Prioritizer stuff
  /*!
    Here we are coutning bits from 1 to 8
    -  "LSB" Polarisation 1 is VPol
    -  "Bit 2-11" is peak phi bin
    -  "Bit 12 unused"
    -  "Bit 13 is filtering flag - above max power"
    -  "Bit 14 is filtering flag - bin-to-bin increase"
    -  "Bit 15 is saturation flag"
    -  "Bit 16 unused"
  */
  UShort_t prioritizerStuff;




  //!  Reserved bytes --- Deprecated
  /*!
    The lower four bits of the first byte (reserved[0]&0xf) show the TURF hold issued for this trigger. There should be only one buffer held per trigger and it should match the SURF labrador readout for the event.
    Here we are coutning bits from 1 to 4
    -  "Bit 1" is hold 1 (buffer A)
    -  "Bit 2" is hold 2 (buffer B)
    -  "Bit 3" is hold 3 (buffer C)
    -  "Bit 4" is hold 4 (buffer D)

The upper four bits of the first byte (reserved[0]&0xf0)>>4 show which TURF holds were acitve when the trigger was formed. The bits have the same meaning as above. It is possible to have all holds active (in which case there are no free buffers and the trigger is dead until a buffer becomes available).

The second byte (reserved[1]) is currently reserved.
  */
  UChar_t         reserved[2]; // [0]&0xf is TURF hold for this trigger, [0]&0xf0 is active TURF holds
  UChar_t         trigType; ///< Bit 0 is RF, 1 is ADU5, 2 is G12, 3 is software/external
  UChar_t         l3Type1Count; ///< Count of l3 type 1 triggers
  UShort_t        trigNum; ///< Trigger number (since last clear all)
  UInt_t          trigTime; ///< Trigger time in TURF clock ticks
  UInt_t          c3poNum; ///< Number of TURF clock ticks between GPS pulse per seconds
  UShort_t        ppsNum; ///< Number of GPS PPS since last clear all
  //!  Dead Time
  /*!
    The number of of 16.384 us clock ticks in the current second, upto triggerTimeNs ns, which all four buffers were full. A more consistent definition of deadTime is available in the TurfRate class.
  */
  UShort_t        deadTime;
  //!  Buffer depth
  /*!
    The lowest two bits (bufferDepth&0x3) are a two-bit number (with range 0-3) that count the number of held buffers at the time of the trigger.

    The next lowest two bits (bufferDepth&0xc)>>2 are a two-bit number (with range 0-3) that count the number of held buffers at the time of readout.
  */
  UChar_t         bufferDepth; // Buffer depth
  UChar_t         turfioReserved; ///< Reserved
  UShort_t        upperL1TrigPattern; ///< Bit mask for upper ring l1 antenna triggers. eg. if the bit 1 (the lowest bit) is active it means the upper ring antenna in phi sector 1 contributes an L1 trigger to the event.
  UShort_t        lowerL1TrigPattern; ///< Bit mask for lower ring l1 antenna triggers. eg. if the bit 1 (the lowest bit) is active it means the lower ring antenna in phi sector 1 contributes an L1 trigger to the event.
  UShort_t        upperL2TrigPattern; ///< Bit mask for upper ring l2 cluster triggers. eg. if the bit 1 (the lowest bit) is active it means the three antenna cluster centred on the upper ring antenna in phi sector 1 contributes an L2 trigger to the event.
  UShort_t        lowerL2TrigPattern; ///< Bit mask for lower ring l2 cluster triggers. eg. if the bit 1 (the lowest bit) is active it means the three antenna cluster centred on the lower ring antenna in phi sector 1 contributes an L2 trigger to the event.
  UShort_t        l3TrigPattern; ///< Bit mask for l3 global triggers. eg. if the bit 1 (the lowest bit) is active it means that phi sector 1 contributed an L3 trigger to the event.
  UShort_t        l3TrigPatternH; ///< Bit mask for l3 global triggers. eg. if the bit 1 (the lowest bit) is active it means that phi sector 1 contributed an L3 trigger to the event.
  UShort_t        otherTrigPattern[3]; ///< Other trig patterns -- currently reserved
  UChar_t         nadirL1TrigPattern; ///< 8-bit trigger mask for L1 nadir triggers. Here bit 1 is antenna 33 (phi 1), bit 2 is antenna 34 (phi 3), bit 3 is antenna 35 (phi 5), bit 4 is antenna 36 (phi 7), bit 5 is antenna 37 (phi 9), bit 6 is antenna 38 (phi 11), bit 7 is antenna 39 (phi 13) and bit 8 is antenna 40 (phi 15).
  UChar_t         nadirL2TrigPattern; ///< 8-bit trigger mask for L2 nadir triggers. Nadir L2 triggers are for the even phi sectors and are just the OR of the neighbouring antennas. So bit 1 is phi sector 2 (the OR of phi 1 and phi 3), through to bit 8 is phi sector 16 (the OR of phi's 15 and 1).
  UInt_t          triggerTime; ///< Trigger time from TURF converted to unixTime
  UInt_t          triggerTimeNs; ///< Trigger time in ns from TURF
  Int_t           goodTimeFlag; ///< 1 is good trigger time, 0 is bad trigger time


  UInt_t          rawtrigTime; ///< Trigger time in TURF clock ticks before corrections
  UInt_t          rawc3poNum; ///< Number of TURF clock ticks between GPS pulse per seconds before corrections
  UShort_t        rawppsNum; ///< Number of GPS PPS since last clear all before corrections

  const char *trigTypeAsString() const; ///< Returns trigger type as string
  UShort_t getL3TrigPattern(AnitaPol::AnitaPol_t pol) const;

  int isInL3Pattern(int phi, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical) const; ///< Returns 1 if given phi-ring had l3 trigger. pol does nothing for A4
  int isInPhiMask(int phi, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical) const; ///< Returns 1 if given phi-pol is in mask
  int isInL1Mask(int phi, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical) const; ///< Returns 1 if given phi-pol is in mask
  int getL1Mask( AnitaPol::AnitaPol_t pol) const; ///< Get's the l1 Mask. In A4 this is the same as l2 mask
  int getPhiMask(AnitaPol::AnitaPol_t pol = AnitaPol::kVertical) const; ///<pol does nothing for A4
  int isInL2Mask(int phi) const; ///< Returns 1 if given phi-ring had l1 trigger
  int getL2Mask() const { return l2TrigMask; }
  int isInPhiMaskOffline(int phi, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical) const; // pol does nothing in A4
  int isInL1MaskOffline(int phi, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical) const; //A3 only

  int getPhiMaskOffline(AnitaPol::AnitaPol_t pol = AnitaPol::kHorizontal) const; //pol does nothing in A4
  int getL1MaskOffline( AnitaPol::AnitaPol_t pol) const;  //a3 only
  int getCurrentTurfBuffer() const; ///< Returns the current TURF buffer number (0, 1, 2 or 3);
  unsigned int getCurrentTurfHolds() const; ///< Returns a 4-bit bitmask corresponding to the currently held buffers.
  int getNumberOfCurrentTurfHolds() const; ///< Returns the number of currently held TURF buffers (0-4)
  int getTurfRunNumber() const
  { return (((turfEventId&0xfff00000)>>20));} ///< Returns the run number portion of the TURF event id
  int getTurfEventNumber() const
  { return (turfEventId&0xfffff);} ///< Returns the event number portion of the TURF event id.

  Int_t getAboveThresholdFlag() const;
  Int_t getBinToBinIncreaseFlag() const;
  Int_t getSaturationFlag() const;
  Float_t getPeakThetaRad() const;
  Float_t getPeakPhiRad() const;
  Float_t getPeakThetaDeg() const;
  Float_t getPeakPhiDeg() const;
  Float_t getImagePeak() const;
  Float_t getCoherentSumPeak() const;
  AnitaPol::AnitaPol_t getPeakPol() const;

  Int_t getTriggerBitRF() const;
  Int_t getTriggerBitADU5() const;
  Int_t getTriggerBitG12() const;
  Int_t getTriggerBitSoftExt() const;

  Int_t setMask (UShort_t newL2Mask, UShort_t newPhiMask, AnitaPol::AnitaPol_t pol); // sets phi and l1 masking (used in icemc)
  Int_t setTrigPattern (UShort_t newTrigPattern, AnitaPol::AnitaPol_t pol);          // sets l3 trigger (used in icemc)

  ClassDef(RawAnitaHeader,42);

};


#endif //RAWANITAHEADER_H
