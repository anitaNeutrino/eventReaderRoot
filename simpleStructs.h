//////////////////////////////////////////////////////////////////////////////
/////  simpleStructs.h        Minimalistic ANITA Event Structs           /////
/////                                                                    /////
/////  Description:                                                      /////
/////     The minimal definitions and structures needed for reading      ///// 
/////  ANITA event data.                                                  /////
//////////////////////////////////////////////////////////////////////////////

#ifndef SIMPLESTRUCTS_H
#define SIMPLESTRUCTS_H


///First up we'll add some definitions of the raw data
#include "AnitaConventions.h"

//Enumerations
typedef enum {
    PACKET_BD = 0xff, // AnitaEventBody_t -- No
    PACKET_HD = 0x100, //AnitaEventHeader_t --Yes
    PACKET_WV = 0x101, //RawWaveformPacket_t --Yes
    PACKET_SURF = 0x102, //RawSurfPacket_t -- Yes
    PACKET_HD_SLAC = 0x103,
    PACKET_SURF_HK = 0x110, //FullSurfHkStruct_t --Yes
    PACKET_TURF_RATE = 0x111, //TurfRateStruct_t -- Yes
    PACKET_PEDSUB_WV = 0x120, //PedSubbedWaveformPacket_t -- Yes
    PACKET_ENC_SURF = 0x121, //EncodedSurfPacketHeader_t -- Yes
    PACKET_ENC_SURF_PEDSUB = 0x122, //EncodedPedSubbedSurfPacketHeader_t -- Yes
    PACKET_ENC_EVENT_WRAPPER = 0x123, 
    PACKET_PED_SUBBED_EVENT = 0x124, //PedSubbedEventBody_t -- No too big
    PACKET_ENC_WV_PEDSUB = 0x125, // EncodedPedSubbedChannelPacketHeader_t -- Yes
    PACKET_ENC_PEDSUB_EVENT_WRAPPER = 0x126,
    PACKET_PEDSUB_SURF = 0x127, //PedSubbedSurfPacket_t -- Yes 
    PACKET_LAB_PED = 0x130, //
    PACKET_FULL_PED = 0x131, //Too big to telemeter
    PACKET_GPS_ADU5_PAT = 0x200,
    PACKET_GPS_ADU5_SAT = 0x201,
    PACKET_GPS_ADU5_VTG = 0x202,
    PACKET_GPS_G12_POS = 0x203,
    PACKET_GPS_G12_SAT = 0x204,
    PACKET_HKD = 0x300,
    PACKET_CMD_ECHO = 0x400,
    PACKET_MONITOR = 0x500,
    PACKET_WAKEUP_LOS = 0x600,
    PACKET_WAKEUP_HIGHRATE = 0x601,
    PACKET_WAKEUP_COMM1 = 0x602,
    PACKET_WAKEUP_COMM2 = 0x603,
    PACKET_SLOW1 = 0x700,
    PACKET_SLOW2 = 0x800,
    PACKET_SLOW_FULL = 0x801,
    PACKET_ZIPPED_PACKET = 0x900, // Is just a zipped version of another packet
    PACKET_ZIPPED_FILE = 0xa00, // Is a zipped file
    PACKET_RUN_START = 0xb00, 
    PACKET_OTHER_MONITOR = 0xb01 
} PacketCode_t;

typedef struct {
    PacketCode_t code;    
    unsigned long packetNumber; //Especially for Ped
    unsigned short numBytes;
    unsigned char feByte;
    unsigned char verId;
    unsigned long checksum;
} GenericHeader_t;

typedef struct {
    unsigned char trigType; //Trig type bit masks
    // 0=RF, 1=PPS1, 2=PPS2, 3=Soft/Ext, 4=L3Type1, 5,6 buffer depth at trig
    unsigned char l3Type1Count; //L3 counter
    unsigned short trigNum; //turf trigger counter
    unsigned long trigTime;
    unsigned short ppsNum;     // 1PPS
    unsigned short deadTime; // fraction = deadTime/64400
    unsigned long c3poNum;     // 1 number of trigger time ticks per PPS
    unsigned short upperL1TrigPattern;
    unsigned short lowerL1TrigPattern;
    unsigned short upperL2TrigPattern;
    unsigned short lowerL2TrigPattern;
    unsigned short l3TrigPattern;
    unsigned char bufferDepth; //bits 0,1 trigTime depth 2,3 current depth
    unsigned char reserved;
} TurfioStruct_t;
 
typedef struct {
    GenericHeader_t gHdr;
    unsigned long unixTime;       /* unix UTC sec*/
    unsigned long unixTimeUs;     /* unix UTC microsec */
    long gpsSubTime;     /* the GPS fraction of second (in ns) 
			   (for the X events per second that get 
			   tagged with it, note it now includes
			   second offset from unixTime)*/
    unsigned long eventNumber;    /* Global event number */
    unsigned short surfMask;
    unsigned short calibStatus;   /* Were we flashing the pulser? */
    unsigned char priority; // priority and other
    unsigned char turfUpperWord; // The upper 8 bits from the TURF
    unsigned char otherFlag; //Currently unused 
    unsigned char otherFlag2; //Currently unused 
    unsigned long antTrigMask; // What was the ant trigger mask
    TurfioStruct_t turfio; /*The X byte TURFIO data*/
} AnitaEventHeader_t;

typedef struct {
    unsigned char chanId;   // chan+9*surf
    unsigned char chipIdFlag; // Bits 0,1 chipNum; Bit 3 hitBus wrap; 4-7 hitBusOff
    unsigned char firstHitbus; // If wrappedHitbus=0 data runs, lastHitbus+1
    unsigned char lastHitbus; //to firstHitbus-1 inclusive
    //Otherwise it runs from firstHitbus+1 to lastHitbus-1 inclusive
} RawSurfChannelHeader_t;

typedef struct {
    RawSurfChannelHeader_t header;
    short xMax;
    short xMin;
    float mean; //Filled by pedestalLib
    float rms; //Filled by pedestalLib
    short data[MAX_NUMBER_SAMPLES]; //Pedestal subtracted and 11bit data
} SurfChannelPedSubbed_t;

typedef struct {
    GenericHeader_t gHdr;
    unsigned long eventNumber;    /* Global event number */
    unsigned long whichPeds; //whichPedestals did we subtract
    SurfChannelPedSubbed_t channel[NUM_DIGITZED_CHANNELS];
} PedSubbedEventBody_t;

typedef struct { 
    GenericHeader_t gHdr;
    unsigned long unixTime;
    unsigned long unixTimeUs;
    unsigned short globalThreshold; //set to zero if there isn't one
    unsigned short errorFlag; //Will define at some point    
    unsigned short scalerGoal; //What are we aiming for with the scaler rate
    unsigned short upperWords[ACTIVE_SURFS];
    unsigned short scaler[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short threshold[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short setThreshold[ACTIVE_SURFS][SCALERS_PER_SURF];
    unsigned short rfPower[ACTIVE_SURFS][RFCHAN_PER_SURF];
    unsigned short surfTrigBandMask[ACTIVE_SURFS][2];
} FullSurfHkStruct_t;

#endif //SIMPLESTRUCTS_H
