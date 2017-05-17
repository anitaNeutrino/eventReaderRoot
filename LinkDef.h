#ifdef __ROOTCLING__
#define ROOT_PARSER
#elif __CINT__
#define ROOT_PARSER
#endif

#ifdef ROOT_PARSER

#pragma link off all globals;
#pragma link off all classes;




#pragma link C++ namespace     WaveCalType;
#pragma link C++ enum          WaveCalType::EWaveCalType;
#pragma link C++ nestedtypedef WaveCalType::WaveCalType_t;

#pragma link C++ namespace     AnitaRing;
#pragma link C++ enum          AnitaRing::EAnitaRing;
#pragma link C++ nestedtypedef AnitaRing::AnitaRing_t;

#pragma link C++ namespace     AnitaBand;
#pragma link C++ enum          AnitaBand::EAnitaBand;
#pragma link C++ nestedtypedef AnitaBand::AnitaBand_t;

#pragma link C++ namespace     AnitaPol;
#pragma link C++ enum          AnitaPol::EAnitaPol;
#pragma link C++ nestedtypedef AnitaPol::AnitaPol_t;

#pragma link C++ namespace     AnitaLocations;
#pragma link C++ namespace     AnitaCompress;

//#pragma link C++ nestedtypedef AnalysisBlinding::strategy_t;



#pragma link C++ class GenericHeader_t+;
#pragma link C++ class RawAnitaHeader+;
#pragma link C++ class AnitaGeomTool+;
#pragma link C++ class RawAnitaEvent+;
#pragma link C++ class TimedAnitaHeader+;
#pragma link C++ class UsefulAnitaEvent+;
#pragma link C++ class CalibratedAnitaEvent+;
#pragma link C++ class Adu5Pat+;
#pragma link C++ class Adu5Vtg+;
#pragma link C++ class G12Pos+;
#pragma link C++ class G12Sat+;
#pragma link C++ class GpsGga+;
#pragma link C++ class Adu5Sat+;
#pragma link C++ class AnitaEventCalibrator+;
#pragma link C++ class PrettyAnitaHk+;
#pragma link C++ class SurfHk+;
#pragma link C++ class AveragedSurfHk+;
#pragma link C++ class TurfRate+;
#pragma link C++ class SummedTurfRate+;
#pragma link C++ class AcqdStart+;
#pragma link C++ class GpsdStart+;
#pragma link C++ class MonitorHk+;
#pragma link C++ class LogWatchdStart+;
#pragma link C++ class RunStart+;
#pragma link C++ class CommandEcho+;
#pragma link C++ class OtherMonitorHk+;
#pragma link C++ class RawHk+;
#pragma link C++ class CalibratedHk+;
#pragma link C++ class RawSSHk+;
#pragma link C++ class CalibratedSSHk+;
#pragma link C++ class SlowRate+;
#pragma link C++ class GpuPowerSpectra+;
#pragma link C++ class AnitaDataset+;
#pragma link C++ class RTLSpectrum+;
#pragma link C++ class TruthAnitaEvent+;
#pragma link C++ class TuffNotchStatus+;
#pragma link C++ class TuffRawCmd+;
#pragma link C++ namespace AnitaVersion;
#pragma link C++ enum AnitaDataset::BlindingStrategy; 


// Change l1 to l2 for Header's.
#pragma read sourceClass="RawAnitaHeader" targetClass="RawAnitaHeader"  \
  source="UShort_t l1TrigMask; UShort_t l1TrigMaskH" version="[1-39]" target="l2TrigMask,l2TrigMaskH" \
  embed="true" code="{l2TrigMask=onfile.l1TrigMask;l2TrigMaskH=onfile.l1TrigMaskH;}"


// add offline stuff for older ROOTified headers
#pragma read sourceClass="RawAnitaHeader" targetClass="RawAnitaHeader"  \
  source="UShort_t phiTrigMask; UShort_t phiTrigMaskH;" version="[40-41]" target="phiTrigMaskOffline,phiTrigMaskHOffline, l1TrigMaskOffline, l1TrigMaskHOffline" \
  embed="true" code="{phiTrigMaskOffline=onfile.phiTrigMask;phiTrigMaskHOffline=onfile.phiTrigMaskH; l1TrigMaskOffline=0; l1TrigMaskHOffline=0;}"


/** Fills in A3 compatibility fields for older ROOTified Turf Stuff */
#pragma read sourceClass="TurfRate" targetClass="TurfRate" \
 source="UShort_t phiTrigMask; UShort_t l2TrigMask;"\
 target="l2TrigMaskH,phiTrigMaskH,l2RatesH,l3RatesH" version="[40-42]" \
 embed="true" code="{phiTrigMaskH=onfile.phiTrigMask; l2TrigMaskH=onfile.l2TrigMask; memset(l2RatesH,0,sizeof(l2RatesH)); memset(l3RatesH,0,sizeof(l3RatesH));}"

/** Fills in compatibility fields for A3 ROOTified Turf Stuff */
#pragma read sourceClass="TurfRate" targetClass="TurfRate" \
 source="UShort_t l1Rates[16][2]; UChar_t l3Rates[16][2]" \
 target="l2Rates,l3Rates,l2RatesH,l3RatesH" version="[30-34]" \
 embed="true"\
 code="{ for (int i = 0; i< 16; i++) \
         { \
           l2Rates[i]=onfile.l1Rates[i][0]; \
           l2RatesH[i]=onfile.l1Rates[i][1]; \
           l3Rates[i]=onfile.l3Rates[i][0]; \
           l3RatesH[i]=onfile.l3Rates[i][1];\
         }}"


/** Fills in A3 compatibility fields for older ROOTified Turf Stuff */
#pragma read sourceClass="SummedTurfRate" targetClass="SummedTurfRate" \
 source="UShort_t phiTrigMask; UShort_t l2TrigMask;"\
 target="l2TrigMaskH,phiTrigMaskH,l2RatesH,l3RatesH" version="[40-42]" \
 embed="true" code="{phiTrigMaskH=onfile.phiTrigMask; l2TrigMaskH=onfile.l2TrigMask; memset(l2RatesH,0,sizeof(l2RatesH)); memset(l3RatesH,0,sizeof(l3RatesH));}"

/** Fills in compatibility fields for A3 ROOTified Turf Stuff */
#pragma read sourceClass="SummedTurfRate" targetClass="SummedTurfRate" \
 source="UShort_t l1Rates[16][2]; UChar_t l3Rates[16][2]" \
 target="l2Rates,l3Rates,l2RatesH,l3RatesH" version="[30-34]" \
 embed="true"\
 code="{ for (int i = 0; i< 16; i++) \
         { \
           l2Rates[i]=onfile.l1Rates[i][0]; \
           l2RatesH[i]=onfile.l1Rates[i][1]; \
           l3Rates[i]=onfile.l3Rates[i][0]; \
           l3RatesH[i]=onfile.l3Rates[i][1];\
         }}"






#else
#error "for compilation"
#endif
