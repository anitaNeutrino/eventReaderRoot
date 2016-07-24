
#ifndef OLDSTRUCTS_H
#define OLDSTRUCTS_H


//////////////////////////////////////////////////////////////////////////////
///// Old versions of structs                                            /////
/////////////////////////////////////////////////////////////////////////////


/// Everything below here is for legacy support
#ifndef DOXYGEN_SHOULD_SKIP_THIS


//!  The TURF I/O struct
/*!
  Is part of the AnitaEventHeader_t and contains all sorts of useful info
  about trigger patterns, deadTime, trigger time and trigger type.
*/
typedef struct {
  //!  The trigger type
  /*!
    0=RF, 1=PPS1, 2=PPS2, 3=Soft/Ext, 4=L3Type1, 5,6 buffer depth at trig
  */
  unsigned char trigType; ///<Trig type bit masks
  unsigned char l3Type1Count; ///<L3 counter
  unsigned short trigNum; ///<turf trigger counter
  unsigned int trigTime;
  unsigned short ppsNum;     ///< 1PPS
  unsigned short deadTime; ///< fraction = deadTime/64400
  unsigned int c3poNum;     ///< 1 number of trigger time ticks per PPS
  unsigned short upperL1TrigPattern;
  unsigned short lowerL1TrigPattern;
  unsigned short upperL2TrigPattern;
  unsigned short lowerL2TrigPattern;
  unsigned short l3TrigPattern;
  unsigned short otherTrigPattern[3];
  unsigned char nadirL1TrigPattern;
  unsigned char nadirL2TrigPattern; ///<Might just be the same thing
  unsigned char bufferDepth; ///<bits 0,1 trigTime depth 2,3 current depth
  unsigned char reserved;
} TurfioStructVer30_t;


//!  ANITA Event Header -- Telemetered
/*!
  ANITA Event Header, contains all kinds of fun information about the event
  including times, trigger patterns, event numbers and error words
*/
typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime;       ///< unix UTC sec
  unsigned int unixTimeUs;     ///< unix UTC microsec 

  //!  GPS timestamp
  /*!
     the GPS fraction of second (in ns) 
     (for the X events per second that get 
     tagged with it, note it now includes
     second offset from unixTime)
  */
  int gpsSubTime;    
  unsigned int turfEventId; ///<Turf event id that doesn't roll
  unsigned int eventNumber;    ///< Global event number 
  unsigned short calibStatus;   ///< Were we flashing the pulser? 
  unsigned char priority; ///< priority and other
  unsigned char turfUpperWord; ///< The upper 8 bits from the TURF
  unsigned char otherFlag; ///< Currently the first two surf evNums 
  //!  Error Flag
  /*!
    Bit 1 means sync slip between TURF and software
    Bit 2 is sync slip between SURF 1 and software
    Bit 3 is sync slip between SURF 10 and SURF 1
    Bit 4 is non matching TURF test pattern
    Bit 5 is startBitGood (1 is good, 0 is bad);
    Bit 6 is stopBitGood (1 is good, 0 is bad);
    Bit 7-8 TURFIO photo shutter output
  */
  unsigned char errorFlag; 
  unsigned char surfSlipFlag; ///< Sync Slip between SURF 2-9 and SURF 1
  unsigned char nadirAntTrigMask; ///< 8-bit nadir phi mask (from TURF)
  unsigned int antTrigMask; ///< 2x 16-bit phi ant mask (from TURF)
  unsigned short phiTrigMask; ///< 16-bit phi mask (from TURF)
  unsigned char reserved[2]; ///< reserved[0] is 
  TurfioStructVer30_t turfio; ///<The X byte TURFIO data
} AnitaEventHeaderVer30_t;

//Old FullSurfHkStruct_t
typedef struct { 
    GenericHeader_t gHdr;
    unsigned int unixTime;
    unsigned int unixTimeUs;
    unsigned short globalThreshold; //set to zero if there isn't one
    unsigned short errorFlag; //Will define at some point    
    unsigned short scalerGoal; //What are we aiming for with the scaler rate
    unsigned short upperWords[ACTIVE_SURFS];
    unsigned short scaler[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short threshold[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short setThreshold[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short rfPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
    unsigned short surfTrigBandMask[ACTIVE_SURFS];
} FullSurfHkStructVer12_t;



typedef struct { 
    GenericHeader_t gHdr;
    unsigned int unixTime;
    unsigned int unixTimeUs;
    unsigned short globalThreshold; //set to zero if there isn't one
    unsigned short errorFlag; //Will define at some point    
    unsigned short scalerGoals[BANDS_PER_ANT]; //What are we aiming for with the scaler rate
    unsigned short upperWords[ACTIVE_SURFS];
    unsigned short scaler[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short threshold[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short setThreshold[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short rfPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
    unsigned short surfTrigBandMask[ACTIVE_SURFS];
} FullSurfHkStructVer13_t;

typedef struct { 
    GenericHeader_t gHdr;
    unsigned int unixTime;
    unsigned int unixTimeUs;
    unsigned short globalThreshold; ///<set to zero if there isn't one
    unsigned short errorFlag; ///<Will define at some point    
    unsigned short scalerGoals[BANDS_PER_ANT]; ///<What are we aiming for with the scaler rate
    unsigned short scalerGoalsNadir[BANDS_PER_ANT]; ///<What are we aiming for with the scaler rate
    unsigned short upperWords[ACTIVE_SURFS];
    unsigned short scaler[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short threshold[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short setThreshold[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short rfPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
    unsigned short surfTrigBandMask[ACTIVE_SURFS];
} FullSurfHkStructVer14_t;

//! SURF Hk -- Telemetered
/*!
  SURF Hk, contains thresholds, band rates (scalers) and rf power
*/
typedef struct { 
    GenericHeader_t gHdr;
    unsigned int unixTime;
    unsigned int unixTimeUs;
    unsigned short globalThreshold; ///<set to zero if there isn't one
    unsigned short errorFlag; ///<Will define at some point    
    unsigned short scalerGoals[BANDS_PER_ANT]; ///<What are we aiming for with the scaler rate
    unsigned short scalerGoalsNadir[BANDS_PER_ANT]; ///<What are we aiming for with the scaler rate
    unsigned short upperWords[ACTIVE_SURFS];
    unsigned short scaler[ACTIVE_SURFS][SCALERS_PER_SURF_V30];
    unsigned short threshold[ACTIVE_SURFS][SCALERS_PER_SURF_V30];
    unsigned short setThreshold[ACTIVE_SURFS][SCALERS_PER_SURF_V30];
    unsigned short rfPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
    unsigned short surfTrigBandMask[ACTIVE_SURFS];
} FullSurfHkStructVer30_t;


//Old AveragedSurfHkStruct_t

typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime; ///<Time of first hk
  unsigned short numHks; ///<Number of hks in average
  unsigned short deltaT; ///<Difference in time between first and last 
  unsigned int hadError; ///<Bit mask to be defined
  unsigned short globalThreshold;
  unsigned short reserved; 
  unsigned short scalerGoals[BANDS_PER_ANT];
  unsigned short scalerGoalsNadir[BANDS_PER_ANT]; ///<What are we aiming for with the scaler rate
  unsigned short avgScaler[ACTIVE_SURFS][SCALERS_PER_SURF_V30];
  unsigned short rmsScaler[ACTIVE_SURFS][SCALERS_PER_SURF_V30];
  unsigned short avgThresh[ACTIVE_SURFS][SCALERS_PER_SURF_V30];
  unsigned short rmsThresh[ACTIVE_SURFS][SCALERS_PER_SURF_V30];
  unsigned short avgRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
  unsigned short rmsRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
  unsigned short surfTrigBandMask[ACTIVE_SURFS];
} AveragedSurfHkStructVer30_t;


typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime; ///<Time of first hk
  unsigned short numHks; ///<Number of hks in average
  unsigned short deltaT; ///<Difference in time between first and last 
  unsigned int hadError; ///<Bit mask to be defined
  unsigned short globalThreshold;
  unsigned short reserved; 
    unsigned short scalerGoals[BANDS_PER_ANT];
    unsigned short scalerGoalsNadir[BANDS_PER_ANT]; ///<What are we aiming for with the scaler rate
  unsigned short avgScaler[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short rmsScaler[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short avgThresh[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short rmsThresh[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short avgRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
  unsigned short rmsRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
  unsigned short surfTrigBandMask[ACTIVE_SURFS];
} AveragedSurfHkStructVer14_t;

typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime; //Time of first hk
  unsigned short numHks; //Number of hks in average
  unsigned short deltaT; //Difference in time between first and last 
  unsigned int hadError; //Bit mask to be defined
  unsigned short globalThreshold;
  unsigned short scalerGoals[BANDS_PER_ANT];
  unsigned short avgScaler[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short rmsScaler[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short avgThresh[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short rmsThresh[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short avgRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
  unsigned short rmsRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
  unsigned short surfTrigBandMask[ACTIVE_SURFS];
} AveragedSurfHkStructVer13_t;

typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime; //Time of first hk
  unsigned short numHks; //Number of hks in average
  unsigned short deltaT; //Difference in time between first and last 
  unsigned int hadError; //Bit mask to be defined
  unsigned short globalThreshold;
  unsigned short scalerGoal;
  unsigned short avgScaler[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short rmsScaler[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short avgThresh[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short rmsThresh[ACTIVE_SURFS][SCALERS_PER_SURF];
  unsigned short avgRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
  unsigned short rmsRFPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
  unsigned short surfTrigBandMask[ACTIVE_SURFS];
} AveragedSurfHkStructVer12_t;

//Old TurfRateStruct_t

typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime;
  unsigned short ppsNum; ///<It's only updated every second so no need for sub-second timing
  unsigned short deadTime; ///<How much were we dead??
  unsigned char l3Rates[PHI_SECTORS][2]; /// to get Hz
  unsigned short l1TrigMask; ///< As read from TURF (16-bit upper phi, lower phi)
  unsigned short l1TrigMaskH; ///< As read from TURF (16-bit upper phi, lower phi)
  unsigned short phiTrigMask; ///< 16 bit phi-sector mask
  unsigned short phiTrigMaskH; ///< 16 bit phi-sector mask
  unsigned char errorFlag;///<Bit 1-4 bufferdepth, Bits 5,6,7 are for upper,lower,nadir trig mask match
  unsigned char reserved[3];
  unsigned int c3poNum;
} TurfRateStructVer34_t;


typedef struct {
   GenericHeader_t gHdr;
   unsigned int unixTime;
   unsigned short ppsNum; ///<It's only updated every second so no need for sub-second timing
   unsigned short deadTime; ///<How much were we dead??
   unsigned short l1Rates[PHI_SECTORS][2]; ///<x16 to get Hz
   unsigned char upperL2Rates[PHI_SECTORS]; ///<x64 to get Hz
   unsigned char lowerL2Rates[PHI_SECTORS]; ///<x64 to get Hz
   unsigned char l3Rates[PHI_SECTORS]; ///<Hz
   unsigned short nadirL1Rates[NADIR_ANTS]; ///<x16 to get Hz
   unsigned char nadirL2Rates[NADIR_ANTS]; ///<x64 to get Hz
   unsigned int antTrigMask; ///< As read from TURF (16-bit upper phi, lower phi)
   unsigned short phiTrigMask; ///< 16 bit phi-sector mask
   unsigned char nadirAntTrigMask; ///< 8-bit nadir phi mask
   unsigned char errorFlag;///<Bit 1-4 bufferdepth, Bits 5,6,7 are for upper,lower,nadir trig mask match
} TurfRateStructVer16_t;

typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime;
  unsigned int ppsNum; ///<It's only updated every second so no need for sub-second timing
  unsigned short l1Rates[PHI_SECTORS][2]; ///<x16 to get Hz
  unsigned char upperL2Rates[PHI_SECTORS]; ///<x64 to get Hz
  unsigned char lowerL2Rates[PHI_SECTORS]; ///<x64 to get Hz
  unsigned char l3Rates[PHI_SECTORS]; ///<Hz
  unsigned short nadirL1Rates[NADIR_ANTS]; ///<x16 to get Hz
  unsigned char nadirL2Rates[NADIR_ANTS]; ///<x64 to get Hz
  unsigned int antTrigMask; ///< As read from TURF (16-bit upper phi, lower phi)
  unsigned short phiTrigMask; ///< 16 bit phi-sector mask
  unsigned char nadirAntTrigMask; ///< 8-bit nadir phi mask
  unsigned char errorFlag;///<Bit 1,2,3 are for upper,lower,nadir trig mask match
} TurfRateStructVer15_t;

typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime;
  unsigned int ppsNum; //It's only updated every second so no need for sub-second timing
  unsigned short l1Rates[PHI_SECTORS][2]; //x16 to get Hz
  unsigned char upperL2Rates[PHI_SECTORS]; //x64 to get Hz
  unsigned char lowerL2Rates[PHI_SECTORS]; //x64 to get Hz
  unsigned char l3Rates[PHI_SECTORS]; //Hz
  unsigned char nadirL1Rates[PHI_SECTORS]; //?? to get Hz
  unsigned char nadirL2Rates[PHI_SECTORS]; //?? to get Hz
  unsigned int antTrigMask;
  unsigned char nadirAntTrigMask;
  unsigned char reserved[3];
} TurfRateStructVer14_t;


typedef struct {
   GenericHeader_t gHdr;
   unsigned int unixTime;
   unsigned int ppsNum; //It's only updated every second so no need for sub-second timing
   unsigned short l1Rates[PHI_SECTORS][2]; // up and down counts
   unsigned char upperL2Rates[PHI_SECTORS];
   unsigned char lowerL2Rates[PHI_SECTORS];
   unsigned char l3Rates[PHI_SECTORS];
   unsigned int antTrigMask;
   unsigned char nadirAntTrigMask; //Will need to do pad three bytes
} TurfRateStructVer13_t;

typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime;
  unsigned int ppsNum; //It's only updated every second so no need for sub-second timing
  unsigned short l1Rates[PHI_SECTORS][2]; // up and down counts
  unsigned char upperL2Rates[PHI_SECTORS];
  unsigned char lowerL2Rates[PHI_SECTORS];
  unsigned char l3Rates[PHI_SECTORS];
} TurfRateStructVer12_t;

typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime;
  unsigned int ppsNum; //It's only updated every second so no need for sub-second timing
  unsigned short l1Rates[TRIGGER_SURFS][ANTS_PER_SURF]; // 3 of 8 counters
  unsigned char upperL2Rates[PHI_SECTORS];
  unsigned char lowerL2Rates[PHI_SECTORS];
  unsigned char l3Rates[PHI_SECTORS];
} TurfRateStructVer11_t;


//Old SummedTurfRateStruct_t
typedef struct {
    GenericHeader_t gHdr;
    unsigned int unixTime; ///<Time of first hk
    unsigned short numRates; ///<Number of rates in average
    unsigned short deltaT; ///<Difference in time between first and last 
    unsigned int deadTime; ///<Summed dead time between first and last
    unsigned char bufferCount[4]; ///<Counting filled buffers
    unsigned int l1Rates[PHI_SECTORS][2]; ///<x16/numRates to get Hz 
    unsigned short upperL2Rates[PHI_SECTORS]; ///<x64/numRates to get Hz
    unsigned short lowerL2Rates[PHI_SECTORS]; ///<x64/numRates to get Hz
    unsigned short l3Rates[PHI_SECTORS]; ///< /numRates to get Hz
    unsigned int nadirL1Rates[NADIR_ANTS]; ///<x16/numRates to get Hz
    unsigned short nadirL2Rates[NADIR_ANTS]; ///<x64/numRates to get Hz  
    unsigned int antTrigMask; ///<As read from TURF (16-bit upper phi, lower phi)
    unsigned short phiTrigMask; ///<16-bit phi-sector mask
    unsigned char nadirAntTrigMask; ///< 8-bit nadir phi mask
    unsigned char errorFlag;///<Bit 1-4 bufferdepth, Bits 5,6,7 are for upper,lower,nadir trig mask match
} SummedTurfRateStructVer16_t;

typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime; ///<Time of first hk
  unsigned short numRates; ///<Number of rates in average
  unsigned short deltaT; ///<Difference in time between first and last 
  unsigned int l1Rates[PHI_SECTORS][2]; ///<x16 to get Hz 
  unsigned short upperL2Rates[PHI_SECTORS]; ///<x64 to get Hz
  unsigned short lowerL2Rates[PHI_SECTORS]; ///<x64 to get Hz
  unsigned short l3Rates[PHI_SECTORS]; ///<Hz
  unsigned short nadirL1Rates[NADIR_ANTS]; ///<x16 to get Hz
  unsigned char nadirL2Rates[NADIR_ANTS]; ///<x64 to get Hz  
  unsigned int antTrigMask; ///<As read from TURF (16-bit upper phi, lower phi)
  unsigned short phiTrigMask; ///<16-bit phi-sector mask
  unsigned char nadirAntTrigMask; ///< 8-bit nadir phi mask
  unsigned char errorFlag;///<Bit 1,2,3 are for upper,lower,nadir trig mask match
} SummedTurfRateStructVer15_t;

typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime; //Time of first hk
  unsigned short numRates; //Number of rates in average
  unsigned short deltaT; //Difference in time between first and last 
  unsigned int l1Rates[PHI_SECTORS][2]; //x16 to get Hz 
  unsigned short upperL2Rates[PHI_SECTORS]; //x64 to get Hz
  unsigned short lowerL2Rates[PHI_SECTORS]; //x64 to get Hz
  unsigned short l3Rates[PHI_SECTORS]; //Hz
  unsigned int antTrigMask;
  unsigned char nadirAntTrigMask; //Maybe need to pad three bytes
  unsigned char reserved[3];
} SummedTurfRateStructVer14_t;



typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime; //Time of first hk
  unsigned short numRates; //Number of rates in average
  unsigned short deltaT; //Difference in time between first and last 
  unsigned int l1Rates[PHI_SECTORS][2]; //upper and lower rings only
  unsigned short upperL2Rates[PHI_SECTORS];
  unsigned short lowerL2Rates[PHI_SECTORS];
  unsigned short l3Rates[PHI_SECTORS];
  unsigned int antTrigMask;
  unsigned char nadirAntTrigMask; //Maybe need to pad three bytes
} SummedTurfRateStructVer11_t;


typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime; //Time of first hk
  unsigned short numRates; //Number of rates in average
  unsigned short deltaT; //Difference in time between first and last 
  unsigned int l1Rates[TRIGGER_SURFS][ANTS_PER_SURF]; // 3 of 8 counters
  unsigned short upperL2Rates[PHI_SECTORS];
  unsigned short lowerL2Rates[PHI_SECTORS];
  unsigned short l3Rates[PHI_SECTORS];
} SummedTurfRateStructVer10_t;


//Old Headers
typedef struct {
  //!  The trigger type
  /*!
    0=RF, 1=PPS1, 2=PPS2, 3=Soft/Ext, 4=L3Type1, 5,6 buffer depth at trig
  */
  unsigned char trigType; ///<Trig type bit masks
  unsigned char l3Type1Count; ///<L3 counter
  unsigned short trigNum; ///<turf trigger counter
  unsigned int trigTime;
  unsigned short ppsNum;     ///< 1PPS
  unsigned short deadTime; ///< fraction = deadTime/64400
  unsigned int c3poNum;     ///< 1 number of trigger time ticks per PPS
  unsigned short upperL1TrigPattern;
  unsigned short lowerL1TrigPattern;
  unsigned short upperL2TrigPattern;
  unsigned short lowerL2TrigPattern;
  unsigned short l3TrigPattern;
  unsigned short otherTrigPattern[3];
  unsigned char nadirL1TrigPattern;
  unsigned char nadirL2TrigPattern; ///<Might just be the same thing
  unsigned char bufferDepth; ///<bits 0,1 trigTime depth 2,3 current depth
  unsigned char reserved;
} TurfioStructVer13_t;

typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime;       ///< unix UTC sec
  unsigned int unixTimeUs;     ///< unix UTC microsec 

  //!  GPS timestamp
  /*!
     the GPS fraction of second (in ns) 
     (for the X events per second that get 
     tagged with it, note it now includes
     second offset from unixTime)
  */
  int gpsSubTime;    
  unsigned int turfEventId; ///<Turf event id that doesn't roll
  unsigned int eventNumber;    ///< Global event number 
  unsigned short calibStatus;   ///< Were we flashing the pulser? 
  unsigned char priority; ///< priority and other
  unsigned char turfUpperWord; ///< The upper 8 bits from the TURF
  unsigned char otherFlag; ///< Currently the first two surf evNums 
  //!  Error Flag
  /*!
    Bit 1 means sync slip between TURF and software
    Bit 2 is sync slip between SURF 1 and software
    Bit 3 is sync slip between SURF 10 and SURF 1
    Bit 4 is non matching TURF test pattern
  */
  unsigned char errorFlag; 
  unsigned char surfSlipFlag; ///< Sync Slip between SURF 2-9 and SURF 1
  unsigned char nadirAntTrigMask; ///< 8-bit nadir phi mask (from TURF)
  unsigned int antTrigMask; ///< 2x 16-bit phi ant mask (from TURF)
  unsigned short phiTrigMask; ///< 16-bit phi mask (from TURF)
  unsigned char reserved[2];
  TurfioStructVer13_t turfio; ///<The X byte TURFIO data
} AnitaEventHeaderVer13_t;

typedef struct {
  unsigned char trigType; //Trig type bit masks
  // 0=RF, 1=PPS1, 2=PPS2, 3=Soft/Ext, 4=L3Type1, 5,6 buffer depth at trig
  unsigned char l3Type1Count; //L3 counter
  unsigned short trigNum; //turf trigger counter
  unsigned int trigTime;
  unsigned short ppsNum;     // 1PPS
  unsigned short deadTime; // fraction = deadTime/64400
  unsigned int c3poNum;     // 1 number of trigger time ticks per PPS
  unsigned short upperL1TrigPattern;
  unsigned short lowerL1TrigPattern;
  unsigned short upperL2TrigPattern;
  unsigned short lowerL2TrigPattern;
  unsigned short l3TrigPattern;
  unsigned short otherTrigPattern[3];
  unsigned char nadirL1TrigPattern;
  unsigned char nadirL2TrigPattern; //Might just be the same thing
  unsigned char bufferDepth; //bits 0,1 trigTime depth 2,3 current depth
  unsigned char reserved;
} TurfioStructVer12_t;


typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime;       /* unix UTC sec*/
  unsigned int unixTimeUs;     /* unix UTC microsec */
  int gpsSubTime;     /* the GPS fraction of second (in ns) 
                         (for the X events per second that get 
                           tagged with it, note it now includes
                           second offset from unixTime)*/
  unsigned int turfEventId; //Turf event id that doesn't roll
  unsigned int eventNumber;    /* Global event number */
  unsigned short calibStatus;   /* Were we flashing the pulser? */
  unsigned char priority; // priority and other
  unsigned char turfUpperWord; // The upper 8 bits from the TURF
  unsigned char otherFlag; // Currently the first two surf evNums 
  unsigned char errorFlag; /*Bit 1 means sync slip between TURF and software
                             Bit 2 is sync slip between SURF 1 and software
                             Bit 3 is sync slip between SURF 10 and SURF 1
                             Bit 4 is non matching TURF test pattern*/
  unsigned char surfSlipFlag; /* Sync Slip between SURF 2-9 and SURF 1*/
  unsigned char nadirAntTrigMask; //
  unsigned int antTrigMask; // What was the ant trigger mask
  TurfioStructVer12_t turfio; /*The X byte TURFIO data*/
} AnitaEventHeaderVer12_t;

typedef struct {
    unsigned char trigType; //Trig type bit masks
    // 0=RF, 1=PPS1, 2=PPS2, 3=Soft/Ext, 4=L3Type1, 5,6 buffer depth at trig
    unsigned char l3Type1Count; //L3 counter
    unsigned short trigNum; //turf trigger counter
    unsigned int trigTime;
    unsigned short ppsNum;     // 1PPS
    unsigned short deadTime; // fraction = deadTime/64400
    unsigned int c3poNum;     // 1 number of trigger time ticks per PPS
    unsigned short upperL1TrigPattern;
    unsigned short lowerL1TrigPattern;
    unsigned short upperL2TrigPattern;
    unsigned short lowerL2TrigPattern;
    unsigned short l3TrigPattern;
    unsigned char bufferDepth; //bits 0,1 trigTime depth 2,3 current depth
    unsigned char reserved;
} TurfioStructVer11_t;



typedef struct {
  GenericHeader_t gHdr;
  unsigned int unixTime;       /* unix UTC sec*/
  unsigned int unixTimeUs;     /* unix UTC microsec */
  int gpsSubTime;     /* the GPS fraction of second (in ns) 
                         (for the X events per second that get 
                           tagged with it, note it now includes
                           second offset from unixTime)*/
  unsigned int eventNumber;    /* Global event number */
  unsigned short calibStatus;   /* Were we flashing the pulser? */
  unsigned char priority; // priority and other
  unsigned char turfUpperWord; // The upper 8 bits from the TURF
  unsigned char otherFlag; //Currently unused 
  unsigned char errorFlag; //Bit 1 means sync slip
  unsigned char otherFlag3;
  unsigned char nadirAntTrigMask; //
  unsigned int antTrigMask; // What was the ant trigger mask
  TurfioStructVer11_t turfio; /*The X byte TURFIO data*/
} AnitaEventHeaderVer11_t;

typedef struct {
    unsigned char trigType; //Trig type bit masks
    // 0=RF, 1=PPS1, 2=PPS2, 3=Soft/Ext, 4=L3Type1, 5,6 buffer depth at trig
    unsigned char l3Type1Count; //L3 counter
    unsigned short trigNum; //turf trigger counter
    unsigned int trigTime;
    unsigned short ppsNum;     // 1PPS
    unsigned short deadTime; // fraction = deadTime/64400
    unsigned int c3poNum;     // 1 number of trigger time ticks per PPS
    unsigned short upperL1TrigPattern;
    unsigned short lowerL1TrigPattern;
    unsigned short upperL2TrigPattern;
    unsigned short lowerL2TrigPattern;
    unsigned short l3TrigPattern;
    unsigned char bufferDepth; //bits 0,1 trigTime depth 2,3 current depth
    unsigned char reserved;
} TurfioStructVer10_t;

typedef struct {
    GenericHeader_t gHdr;
    unsigned int unixTime;       /* unix UTC sec*/
    unsigned int unixTimeUs;     /* unix UTC microsec */
    int gpsSubTime;     /* the GPS fraction of second (in ns) 
                           (for the X events per second that get 
                           tagged with it, note it now includes
                           second offset from unixTime)*/
    unsigned int eventNumber;    /* Global event number */
    unsigned short surfMask;
    unsigned short calibStatus;   /* Were we flashing the pulser? */
    unsigned char priority; // priority and other
    unsigned char turfUpperWord; // The upper 8 bits from the TURF
    unsigned char otherFlag; //Currently unused 
    unsigned char otherFlag2; //Currently unused 
    unsigned int antTrigMask; // What was the ant trigger mask
    TurfioStructVer10_t turfio; /*The X byte TURFIO data*/
} AnitaEventHeaderVer10_t;


//Old Event Structures
typedef struct {
  GenericHeader_t gHdr;
  unsigned int eventNumber;    /* Global event number */
  unsigned int surfEventId[ACTIVE_SURFS];
  unsigned int whichPeds; ///<whichPedestals did we subtract
  SurfChannelPedSubbed_t channel[NUM_DIGITZED_CHANNELS];
} PedSubbedEventBodyVer11_t;

typedef struct {
    GenericHeader_t gHdr;
    unsigned int eventNumber;    /* Global event number */
    unsigned int whichPeds; //whichPedestals did we subtract
    SurfChannelPedSubbed_t channel[NUM_DIGITZED_CHANNELS];
} PedSubbedEventBodyVer10_t;



#endif //DOXYGEN_SHOULD_SKIP_THIS
/*\@}*/


#endif //OLDSTRUCTS_H
