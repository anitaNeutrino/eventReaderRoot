//////////////////////////////////////////////////////////////////////////////
/////  AnitaConventions.h        ANITA Convetnions                       /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A handy file full of enumerations and the like identifying     /////
/////     the conventions we try and use in the ANITA offline code.      /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////


#ifndef ANITACONVENTIONS_H
#define ANITACONVENTIONS_H

#ifndef ROOT_Rtypes
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Rtypes.h"
#endif
#endif

#define ANITA_FLIGHT_H

#include "AnitaVersion.h"

//!  WaveCalType -- The Calibration Type
/*!
  There are a number of calibration options available to create a UsefulAnitaEvent.
  \ingroup rootclasses
*/
namespace WaveCalType {
  typedef enum EWaveCalType {

    kNoCalib = 0x00, ///< The 260 samples straight from raw data
    kJustUnwrap = 0x01, ///< The X good samples from raw data (260-hitbus)
    kNominal = 0x02, ///< Using mV/ADC = 1 and all dts = 1./2.6 ns
    kVoltageTime = 0x02, ///< Same as kNominal
    kJustTimeNoUnwrap  = 0x03, ///< For calibration: sample-to-sample dts without unwrapping (or voltage calibs)
    kNoTriggerJitterNoZeroMean = 0x05, ///< No inter-SURF timing (or zero meaning)
    kNoChannelToChannelDelays  = 0x06, ///< Inter-SURF timing (trigger jitter) without cable delay
    kNoTriggerJitterNoZeroMeanFlipRco = 0x07, ///< For calib: opposite RCO from software algorithm
    kNoTriggerJitterNoZeroMeanFirmwareRco = 0x08, ///< For calib: applies RCO from firmware (no latch delay)
    kNoTriggerJitterNoZeroMeanFirmwareRcoFlipped = 0x09,///< For calib: 1-firmware RCO (no latch delay)
    kFull = 0x0a, ///< deltaTs, voltage, unwrap, trigger jitter, cable delay. The full monty.
    kDefault = 0x0a, ///< What you should call for analysis work
    kVTFast = 0x1b, ///< Faster, but no algorithm for it
    kAddPeds = 0x1c, ///< New thing
    kOnlyTiming = 0x1d, ///< All the timing of kFull, but none of the voltage calibration (Ped corrected ADC counts)
    kOnlyDTs = 0x1e, ///< Only the dT calibration and unwrapping, none of the voltage calibration (Ped corrected ADC counts), for noise analysis
    kNotACalib ///< Useful for looping over all calibrations 
  } WaveCalType_t; ///< The calibration enumeration type

  const char *calTypeAsString(WaveCalType::WaveCalType_t calType); ///< Returns the calibration type as a string
  void listAllCalTypes(); ///< Prints a list of all available calibration types
}

///First up we'll add some definitions of the raw data
#define NUM_SEAVEYS_ANITA1 32 ///< The number of Seavey antennas.
#define NUM_SEAVEYS_ANITA2 40 ///< The number of Seavey antennas.
#define NUM_SEAVEYS_ANITA3 48 ///< The number of Seavey antennas.
#define NUM_POLS 2 ///< The number of polarisations per Seavey
#ifdef ANITA_2_DATA
#define ACTIVE_SURFS 10 ///< The number of SURF digitizer cards.
#define NUM_SEAVEYS NUM_SEAVEYS_ANITA2 ///< The number of Seavey antennas.
#else
#define ACTIVE_SURFS 12 ///< The number of SURF digitizer cards.
#define NUM_SEAVEYS NUM_SEAVEYS_ANITA3 ///< The number of Seavey antennas.
#endif

#define SCALERS_PER_SURF 12 ///< The number of active trigger channels per SURF.
#define SCALERS_PER_SURF_V30 16 ///< The number of active trigger channels per SURF.
#define L1S_PER_SURF 6
#define L1S_PER_SURF_V30 4
#define L2S_PER_SURF 2
#define L3S_PER_SURF 2
#define RFCHAN_PER_SURF 8 ///< The number of RF input channels per SURF.
#define CHANNELS_PER_SURF 9 ///< The total number of channels per SURF (including the clock).
#define LABRADORS_PER_SURF 4 ///< The number of LABRADOR chips per SURF.
#define RCO_PER_LAB 2 ///< The number of RCO phases per SURF.
#define NUM_DIGITZED_CHANNELS ACTIVE_SURFS*CHANNELS_PER_SURF ///< The total number of digitised channels.
#define MAX_NUMBER_SAMPLES 260 ///< The number of samples per waveform.
#define EFFECTIVE_SAMPLES 256 ///< The number of capacitors in the main array.
#define NUM_PHI 16 ///< The number of azimuthal segments.
#define NUM_ANTENNA_RINGS 3


#define NUM_NADIRS 8 ///< The number of dropdown antennas.
#define NUM_BICONES 4 ///< The number of calibration bicone antennas.
#define NUM_DISCONES 4 ///< Defunct


#define BASE_PACKET_MASK 0xffff ///< Bit mask to select packet code in GenericHeader_t


#define NUM_SURF ACTIVE_SURFS ///< Shorthand for number of SURFs.
#define NUM_CHAN CHANNELS_PER_SURF ///< Shorthand for number of channels per SURF.
#define NUM_CHIP LABRADORS_PER_SURF ///< Shorthand for number of LABRADOR chips per SURF.
#define NUM_RCO RCO_PER_LAB ///< Shorthand for number of RCO phases.
#define NUM_SAMP MAX_NUMBER_SAMPLES ///< Shorthand for number of samples.
#define NUM_EFF_SAMP EFFECTIVE_SAMPLES ///< Shorthand for number of capacitors in main array.

//Trigger Stuff
#define ANTS_PER_SURF 4 ///< Number of trigger antennas per SURF.
#define TRIGGERS_PER_SURF 12  ///< Who knows
#define TRIGGER_SURFS 8 ///< Who knows
#define PHI_SECTORS 16 ///< Number of azimuthal phi sectors
#define BANDS_PER_ANT 4 ///< Number of frequency bands used in the trigger
#define TURF_BANK_SIZE 4 ///< Number of TURF register banks
#define TURF_EVENT_DATA_SIZE 256
#define NADIR_ANTS 8 ///< Number of nadir dropdown antennas


#define WRAPPED_HITBUS 0x8 ///< Bit 3 is the wrapped hitbus bit


//Acromag stuff
#define CHANS_PER_IP320 40 ///< Channels per IP320 analogue acromag board
#define NUM_IP320_BOARDS 3 ///< Number of IP320 analogue acromag boards in the system


//Process Stuff
#define NUM_PROCESSES 20 ///< The number of processes, actually this a couple more than we need
#define NUM_PROCESSES_V40 20 ///< The number of processes, actually this a couple more than we need

//GPS stuff
#define MAX_SATS 12 ///< The maximum number of channels our GPS units have

#define MAX_CMD_LENGTH 20 ///< The maximum command length in bytes


#define NUM_PRIORITIES 10 ///< The number of event priorities
#define NUM_SATABLADES 8 ///< The number of SATA blade disks
#define NUM_SATAMINIS 8 ///< The number of SATA mini disks
#define NUM_USBS 31 ///< Defunct

#define WAKEUP_LOS_BUFFER_SIZE 4000 ///< Size of initial LOS buffer
#define WAKEUP_TDRSS_BUFFER_SIZE 500 ///< Size of initial fast TDRSS buffer
#define WAKEUP_LOW_RATE_BUFFER_SIZE 100 ///< Size of initial slow TDRSS/IRIDIUM buffers

#define ACQD_ID_MASK 0x001 ///< Acqd mask
#define ARCHIVED_ID_MASK 0x002 ///< Archived Id Mask
#define CALIBD_ID_MASK 0x004 ///< Calibd Id Mask
#define CMDD_ID_MASK 0x008 ///< Cmdd Id Mask
#define EVENTD_ID_MASK 0x010 ///< Eventd Id Mask
#define GPSD_ID_MASK 0x020 ///< GPSd Id Mask
#define HKD_ID_MASK 0x040 ///< Hkd Id Mask
#define LOSD_ID_MASK 0x080 ///< LOSd Id Mask
#define PRIORITIZERD_ID_MASK 0x100 ///< Prioritizerd Id Mask
#define SIPD_ID_MASK 0x200 ///< SIPd Id Mask
#define MONITORD_ID_MASK 0x400 ///< Monitord Id Mask
#define PLAYBACKD_ID_MASK 0x800 ///< Playbackd Id Mask
#define LOGWATCHD_ID_MASK 0x1000 ///< Logwatchd Id Mask
#define NEOBRICKD_ID_MASK 0x2000 ///< Neobrickd Id Mask
#define ALL_ID_MASK 0xffff ///< Catch all Id Mask


//RTLd stuff
#define NUM_RTLSDR 6  /// The number of devices
#define RTLSDR_MAX_SPECTRUM_BINS 4096  // The maximum number of bins we can save in a packet.


// TUFF stuff
#define NUM_TUFF_NOTCHES 3
#define NUM_RFCM 4



//Now some geometry and polarisation considerations

//!  AnitaRing -- Enumeration for the three rings
/*!
  Really that's all there is to it.
  \ingroup rootclasses
*/
namespace AnitaRing {
   typedef enum EAnitaRing {
     kTopRing  = 0, ///< The Top Ring.
     kUpperRing = kTopRing,
     kMiddleRing  = 1, ///< The Middle Ring.
     kLowerRing = kMiddleRing,
     kBottomRing = 2, ///< The Bottom Ring.
     kNadirRing=kBottomRing,
     kNotARing ///< Useful in for loops.
   } AnitaRing_t; ///< Ring enumeration

   const char *ringAsString(AnitaRing::AnitaRing_t ring); ///< Returns the ring as a character string
   char ringAsChar(AnitaRing::AnitaRing_t ring); ///< Returns the ring as a character string
}

//!  AnitaPol -- Enumeration for the two polarisations
/*!
  Really that's all there is to it.
  \ingroup rootclasses
*/
namespace AnitaPol {
   typedef enum EAnitaPol {
     kHorizontal = 0, ///< Horizontal Polarisation
     kVertical   = 1, ///< Vertical Polarisation
     kNotAPol ///< USeful in for loops.
   } AnitaPol_t; ///< Polarisation enumeration.
   char polAsChar(AnitaPol::AnitaPol_t pol); ///< Returns the polarisation as a character string.
}

//!  AnitaTrigPol -- Enumeration for the two trigger polarisations
/*!
  Really that's all there is to it.
  \ingroup rootclasses
*/
namespace AnitaTrigPol {
   typedef enum EAnitaTrigPol {
     kLCP = 0, ///< Left-circular polarisation (e.g. A4)
     kRCP = 1, ///< Right-circular polarisation (e.g. A4)
     kHorizontal = 2, ///< Horizontal Polarisation (e.g. A3)
     kVertical = 3, ///< Vertical Polarisation (e.g. A3)
     kNotATrigPol ///< USeful in for loops.
   } AnitaTrigPol_t; ///< Polarisation enumeration.
   char polAsChar(AnitaTrigPol::AnitaTrigPol_t pol); ///< Returns the polarisation as a character string.

   /** Conversion from AnitaPol_t to AnitaTrigPol_t */
   AnitaTrigPol::AnitaTrigPol_t fromAnitaPol(AnitaPol::AnitaPol_t pol);

}




//!  AnitaBand -- Enumeration for the four frequency bands
/*!
  Really that's all there is to it.
  \ingroup rootclasses
*/
namespace AnitaBand {
   typedef enum EAnitaBand {
     kLow =0, ///< The low band.
     kMid =1, ///< The middle band.
     kHigh =2, ///< The high band.
     kFull =3 ///< The full band.x
   } AnitaBand_t; ///< Band enumeration.
   const char *bandAsString(AnitaBand::AnitaBand_t band); ///< Returns the band as a character string.
}

//!  AnitaLocations -- A selection of useful ANITA-I related locations, now updated for ANITA-3.
//
// TODO this needs to be updated properly to support multiple ANITA's
//
/*!
  Things like the calibration antennas and pulsers etc.
  \ingroup rootclasses
*/
namespace AnitaLocations {

  const double LONGITUDE_SURF_SEAVEY=167.0564667; ///< Longitude of surface seavey.
  const double LATITUDE_SURF_SEAVEY=-77.86185; ///< Latitude of surface seavey.
  const double ALTITUDE_SURF_SEAVEY=15.0; ///< Altitude of surface seavey.

  const double LONGITUDE_BH=167.06679444; ///< Longitude of borehole antenna.
  const double LATITUDE_BH=-77.861936111; ///< Latitude if borehole antenna.
  const double ALTITUDE_BH=-33.67; ///< Altitude of borehole antenna.

  //   const double LONGITUDE_TD=158.4564333333; ///< Longitude of Taylor Dome antenna.
  //   const double LATITUDE_TD=77.88116666666; ///< Latitude of Taylor Dome antenna.
  //   const double ALTITUDE_TD=2712.72; ///< Altitude of Taylor Dome antenna.
  const double LONGITUDE_TD=158.45925;
  const double LATITUDE_TD=-77.8803;
  const double ALTITUDE_TD=2260-97.;

  //Anita3 WAIS pulser location
  // WAIS divide position taken from Steph's e-log
  // https://www.phys.hawaii.edu/elog/anita_notes/595
  const Double_t LATITUDE_WAIS_A3 = - (79 + (27.93728/60)); ///< Latitude of WAIS divide pulser
  const Double_t LONGITUDE_WAIS_A3 = -(112 + (6.74974/60)); ///< Longitude of WAIS divide pulser
  const Double_t ALTITUDE_WAIS_A3 = 1813.42;///< Altitude of WAIS divide pulser

  //keeping this around until we can inform people they should use getWais() type calls instead.
  const Double_t LATITUDE_WAIS __attribute__((deprecated)) = - (79 + (27.93728/60));///< Latitude of WAIS divide pulser
  const Double_t LONGITUDE_WAIS __attribute__((deprecated)) = -(112 + (6.74974/60));///< Longitude of WAIS divide pulser
  const Double_t ALTITUDE_WAIS __attribute__((deprecated)) = 1813.42;///< Altitude of WAIS divide pulser


  //Anita 4 location (from Ben Strutt on Slack)
  // And now with an elog note https://www.phys.hawaii.edu/elog/anita_notes/699
  const Double_t LATITUDE_WAIS_A4 = - (-79.468116); ///< Latitude of WAIS divide pulser
  const Double_t LONGITUDE_WAIS_A4 = -(112.059258); ///< Longitude of WAIS divide pulser
  const Double_t ALTITUDE_WAIS_A4 = 1779.80;///< Altitude of WAIS divide pulser

  //Flight independent calls
  Double_t getWaisLatitude();
  Double_t getWaisLongitude();
  Double_t getWaisAltitude();

  // Siple Dome position taken from Steph's e-log
  // https://www.phys.hawaii.edu/elog/anita_notes/595
  const Double_t LATITUDE_SIPLE = - (81 + (39.139/60)); ///< Latitude of Siple dome pulser
  const Double_t LONGITUDE_SIPLE = -(149 + (0.01/60)); ///< Longitude of Siple dome pulser
  // bvv: The next constant is the best I could find at the moment. elog
  // 595 doens't have any information on altitude:
  const Double_t ALTITUDE_SIPLE = 601;///< Altitude of Siple dome pulser according to http://mapcarta.com/25623620

  // LDB position taken from Steph's e-log
  // https://www.phys.hawaii.edu/elog/anita_notes/617
  const Double_t LATITUDE_LDB  = - (77 + (51.23017/60)); ///< Latitude at LDB
  const Double_t LONGITUDE_LDB = + (167 + (12.16908/60)); ///< Longitude at LDB
  const Double_t ALTITUDE_LDB  = 0.; ///< Altitude at LDB

};


// Please note that in order to avoid unnecessarily dependencies during compiling
// I have moved the static adu5 into UsefulAdu5Pat.h. Go look for them there.

// // ANITA-2 values in degrees
// #define STATIC_ADU5_PITCH -0.29
// #define STATIC_ADU5_ROLL 0.89
// #define OFFSET_ADU5_HEADING -0.32


// Number of ANITA payloads.
#define NUM_ANITAS 4


#endif //ANITACONVENTIONS_H
