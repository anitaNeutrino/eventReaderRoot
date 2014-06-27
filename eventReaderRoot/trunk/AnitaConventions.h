//////////////////////////////////////////////////////////////////////////////
/////  AnitaConventions.h        ANITA Convetnions                       /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A handy file full of enumerations and the like identifying     /////
/////     the conventions we try and use in the ANITA offline code.      /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

/*! \mainpage ANITA-II Event Reader
 *
 * \section intro_sec Introduction
 *
 * This is the somewhat sketchy documentation for the ANITA-II event reader.
 *
 * \section prereq_sec Prerequisites
 *
 *  -# <A HREF="http://root.cern.ch">ROOT</A>
 *  
 * \section optional_sec Optional Extras 
 * -# <A HREF="http://www.fftw.org/">FFTW 3 -- Fastest Fourier Transform in the West</a>
 * -# <A HREF="http://www.hep.ucl.ac.uk/uhen/anita/libRootFftwWrapper">libRootFftwWrapper -- a ROOT wrapper for FFTW 3</a>
 * 
 * \section install_sec Installation
 * -# Checkout the code from the SVN repository, eg.: <BR><PRE>svn co https://delos.mps.ohio-state.edu/anitaGround/eventReaderRoot/trunk myEventReaderDir</PRE>
 * -# Define the ANITA_UTIL_INSTALL_DIR to point to the location you want the library installed (the library files will end up in (ANITA_UTIL_INSTALL_DIR)/lib and the header files in (ANITA_UTIL_INSTALL_DIR)/include).
 * -# Do <PRE>make</PRE><PRE>make install</PRE>
 * \section manual_sec Manual
 * If you are averse to reading web pages (and who wouldn't be) you can download a <a href="manual/anitaEventReader.pdf">pdf copy of the reference material</a> but be warned it won't be a thrilling read as it was written by a computer program.
 * \section troubleshooting_sec Troubleshooting
 * There are a myriad of ways that one can run into problems with the event reader, the most common of which are listed here
 * - Path problems -- the bane of poorly organised code. By far the easiest way to use the ANITA offline code is to just set the ANITA_UTIL_INSTALL_DIR and have it point to the location you want to install all the packages. If things are set up correctly you will end up with a ANITA_UTIL_INSTALL_DIR/include and ANITA_UTIL_INSTALL_DIR/lib and ANITA_UTIL_INSTALL_DIR/share/anitaCalib all populated with essential headers, libraries and calibration constants. A quick round of <pre>make clean all install</pre> in libRootFftwWrapper and eventReader (and magicDisplay, etc.) can solve most such difficulties.
 * - Calibration data -- AnitaEventCalibrator looks for calibration data in the following order (make sure it finds some).
 * \section examples_sec Examples
 * The first example of the eventReaderRoot library in use is the exampleDumpHk program which  can be made by typing <PRE>make exampleDumpHk</PRE> in the main eventReaderRoot directory. This program shows the way to access hk data in the ROOT files, similar programs could be written for all the other data types. The main components of the program are:
 * - Opening a TFile of the .root file
 * - Getting the TTree from the TFile (in this case hkTree)
 * - Setting the tree branch address to be the address of a pointer to the class type (in this case CalibratedHk)
 * - A loop over all the "events" in the TTree
 * - The GetEntry function which effectively copies the data for this "event" from the TTree such that it can be accessed from the class pointer
 * - At this point all methods of the class (CalibratedHk) can be accessed.
 * 
 * There are a number of examples of eventReaderRoot in use in the macros directory. In general these are simply run at the command prompt by typing something like: <PRE>root macroName.C</PRE>
 * 
 * For further examples of using the waveform data in the event files, see the <a href="http://www.hep.ucl.ac.uk/uhen/anita/magicDisplay/">Magic Display</a> and <a href="http://www.hep.ucl.ac.uk/uhen/anita/eventCorrelator/">Event Correlator</a> libraries.
 *
 */

#ifndef ANITACONVENTIONS_H
#define ANITACONVENTIONS_H

#ifndef ROOT_Rtypes
#if !defined(__CINT__) || defined(__MAKECINT__)
#include "Rtypes.h"
#endif
#endif

#define ANITA_FLIGHT_H

//!  WaveCalType -- The Calibration Type
/*!
  There are a number of calibration options available to create a UsefulAnitaEvent.
  \ingroup rootclasses
*/
namespace WaveCalType {
  typedef enum EWaveCalType {
    kNoCalib                        = 0x00, ///<The 260 samples straight from raw data
    kJustUnwrap                     = 0x01, ///<The X good samples from raw data (260-hitbus)
    kADC                            = 0x02, ///<Same as kNoCalib -- i.e. useless
    kVoltageTime                    = 0x03, ///<Using 1 and 2.6
    kVTLabRG                        = 0x04, ///<Using all the Ryan/Gary numbers from Antarctica
    kVTFullRG                       = 0x05, ///<Same but also including cable delays
    kVTLabJW                        = 0x06, ///<Using Jiwoos differential numbers but no voltage calib yet
    kVTFullJW                       = 0x07, ///<Same but also including cable delays
    kVTLabJWPlus                    = 0x08, ///< kVTLabJW + Voltage Correction
    kVTFullJWPlus                   = 0x09, ///< kVTFullJW + Voltage Correction
    kVTLabClockRG                   = 0x0a, ///< kVTLabRG + Clock Jitter Correction 
    kVTFullClockRG                  = 0x0b, ///< kVTFullRG + Clock Jitter Correction 
    kVTLabJWPlusClock               = 0x0c, ///< kVTLabJWPlus + Clock Jitter Correction 
    kVTFullJWPlusClock              = 0x0d, ///< kVTFullJWPlus + Clock Jitter Correction 
    kVTLabClockZeroRG               = 0x0e, ///< kVTLabClockRG + Zero Mean
    kVTFullClockZeroRG              = 0x0f, ///< kVTFullClockRG + Zero Mean
    kVTLabJWPlusClockZero           = 0x10, ///< kVTLabJWPlusClock + Zero Mean
    kVTFullJWPlusClockZero          = 0x11,  ///< kVTFullJWPlusClock + Zero Mean
    kVTLabJWPlusFastClockZero       = 0x12, ///< kVTLabJWPlusClock (but faster and worse) + Zero Mean
    kVTFullJWPlusFastClockZero      = 0x13,  ///< kVTFullJWPlusClock (but faster and worse)  + Zero Mean
    kVTFullJWPlusFancyClockZero     = 0x14, ///< Switching to using soemthing like Andres correlation method
    kVTFullJWPlusFudge              = 0x15, ///< kVTFullJW + Voltage Correction +Fudge Factor
    kJustTimeNoUnwrap               = 0x16, ///< Only applies the timebase calibrations, for use in calibration testing.
    kVTLabAG                        = 0x17, ///< Applies the bin-by-bin and epsilon corrections
    kVTLabAGFastClock               = 0x18, ///< Applies the bin-by-bin and epsilon corrections and tries to do a clock calibration
    kVTLabAGCrossCorClock           = 0x19, ///< Applies the bin-by-bin and epsilon corrections and tries to do a clock calibration using corrections
    kVTFullAGFastClock               = 0x1a, ///< Applies the bin-by-bin and epsilon corrections and tries to do a clock calibration + chip and cable delays
    kVTFullAGCrossCorClock           = 0x1b, ///< Applies the bin-by-bin and epsilon corrections and tries to do a clock calibration using corrections + chip and cable delays
    kVTInCalibratedFile              = 0x1b, ///< For use particularly with the new CalibratedAnitaEvent files
    kVTCalFilePlusSimon              = 0x1c, ///< As above with Simon's deltaT's
    kDefault                         = 0x1c, ///< Is now the default calibration if you don't specify anything
    kNotACalib ///< Useful for looping over all calibrations 
  } WaveCalType_t; ///< The calibration enumeration type

  const char *calTypeAsString(WaveCalType::WaveCalType_t calType); ///< Returns the calibration type as a string
  void listAllCalTypes(); ///< Prints a list of all available calibration types
}

///First up we'll add some definitions of the raw data
#define NUM_SEAVEYS 40 ///< The number of Seavey antennas.
#define NUM_SEAVEYS_ANITA1 32 ///< The number of Seavey antennas.
#define NUM_POLS 2 ///< The number of polarisations per Seavey
#define ACTIVE_SURFS 12 ///< The number of SURF digitizer cards.
#define SCALERS_PER_SURF 16 ///< The number of active trigger channels per SURF.
#define RFCHAN_PER_SURF 8 ///< The number of RF input channels per SURF.
#define CHANNELS_PER_SURF 9 ///< The total number of channels per SURF (including the clock).
#define LABRADORS_PER_SURF 4 ///< The number of LABRADOR chips per SURF.
#define RCO_PER_LAB 2 ///< The number of RCO phases per SURF.
#define NUM_DIGITZED_CHANNELS ACTIVE_SURFS*CHANNELS_PER_SURF ///< The total number of digitised channels.
#define MAX_NUMBER_SAMPLES 260 ///< The number of samples per waveform.
#define EFFECTIVE_SAMPLES 256 ///< The number of capacitors in the main array.
#define NUM_PHI 16 ///< The number of azimuthal segments.
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
#define TRIGGER_SURFS 10 ///< Who knows
#define PHI_SECTORS 16 ///< Number of azimuthal phi sectors
#define BANDS_PER_ANT 4 ///< Number of frequency bands used in the trigger
#define TURF_BANK_SIZE 4 ///< Number of TURF register banks
#define NADIR_ANTS 8 ///< Number of nadir dropdown antennas

#define WRAPPED_HITBUS 0x8 ///< Bit 3 is the wrapped hitbus bit


//Acromag stuff
#define CHANS_PER_IP320 40 ///< Channels per IP320 analogue acromag board
#define NUM_IP320_BOARDS 3 ///< Number of IP320 analogue acromag boards in the system


//Process Stuff
#define NUM_PROCESSES 16 ///< The number of processes, actually this a couple more than we need

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


//Now some geometry and polarisation considerations

//!  AnitaRing -- Enumeration for the three rings
/*!
  Really that's all there is to it.
  \ingroup rootclasses
*/
namespace AnitaRing {
   typedef enum EAnitaRing {
     kUpperRing  = 0, ///< The Upper Ring.
     kLowerRing  = 1, ///< The Lower Ring.
     kNadirRing = 2, ///< The Nadir Ring.
     kNotARing ///< Useful in for loops.
   } AnitaRing_t; ///< Ring enumeration
   
   char *ringAsString(AnitaRing::AnitaRing_t ring); ///< Returns the ring as a character string
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
   char *bandAsString(AnitaBand::AnitaBand_t band); ///< Returns the band as a character string.
}

//!  AnitaLocations -- A selection of useful ANITA-I related locations
/*!
  Things like the calibration antennas, etc.
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
};


#define STATIC_ADU5_PITCH -0.29
#define STATIC_ADU5_ROLL 0.89
#define OFFSET_ADU5_HEADING -0.32

#endif //ANITACONVENTIONS_H
