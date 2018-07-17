//////////////////////////////////////////////////////////////////////////////
/////  AnitaGeomTool.cxx        ANITA Geometry Tool                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for accessing ANITA II Geometry Data                     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AnitaGeomTool.h"
#include "AnitaVersion.h" 
#include <assert.h>
#include "TMutex.h" 

#define INCHTOMETER 0.0254

Double_t deltaRL = 0.0;
Double_t deltaUD = 0.0;


static AnitaGeomTool * instances[NUM_ANITAS+1] = {0,0,0,0,0}; 



//!  AnitaGeom -- Namespace used inside AnitaGeomTool
/*!
  \namespace AnitaGeom
  Contains all the mappings between antennas/polarization/phi-sectors/trigger channels/digitizer channels
*/
namespace AnitaGeom {
   
  ///< Map from antenna to SURF. Both polarizations from an antenna go to the same SURF.
  Int_t antToSurfMap[NUM_SEAVEYS]={11,5,10,4,11,4,10,5,11,5,10,4,11,4,10,5,
				   9,3,8,2,8,3,9,2,9,3,8,2,8,3,9,2,
				   6,0,7,1,6,1,7,0,6,0,7,1,6,1,7,0};
  
  ///< Map for VPOL channel of antenna to channel on SURF. (VPOL channels are 0-3)  
    Int_t vAntToChanAnita3[NUM_SEAVEYS]={3,1,3,5,1,3,1,3,2,0,2,0,0,2,0,2,
				 1,3,1,3,3,1,3,1,0,2,0,2,2,0,2,0,
				   3,1,3,1,1,3,1,3,2,0,2,0,0,2,0,2};
    Int_t vAntToChan[NUM_SEAVEYS]={3,1,3,5,1,3,1,3,2,0,2,0,0,2,0,2,
				 1,3,1,3,3,1,3,1,0,2,0,2,2,0,2,0,
				   3,1,3,1,1,3,1,3,2,0,2,0,0,2,0,2};
				  
  ///< Map for HPOL channel of antenna to channel on SURF. (HPOL channels are 4-7)
  Int_t hAntToChanAnita3[NUM_SEAVEYS]={7,5,7,1,5,7,5,7,6,4,6,4,4,6,4,6,
				 5,7,5,7,7,5,7,5,4,6,4,6,6,4,6,4,
				 7,5,7,5,5,7,5,7,6,4,6,4,4,6,4,6};
  Int_t hAntToChan[NUM_SEAVEYS]={7,5,7,1,5,7,5,7,6,4,6,4,4,6,4,6,
				 5,7,5,7,7,5,7,5,4,6,4,6,6,4,6,4,
				 7,5,7,5,5,7,5,7,6,4,6,4,4,6,4,6};
  
   
  ///< 1 is Normal orientation, -1 is 180 degree flip. (Top ring needs to be inverted in software when signals come through seaveys.)
  // Apparently -2 is a 90 degree flip.
    Int_t antOrientationMapAnita3[NUM_SEAVEYS] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
						   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
						   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    Int_t antOrientationMap[NUM_SEAVEYS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
					    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
					    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};


  ///< Map from SURF to antenna+polarization.
  ///< The numbers 1-48 indicate antenna as the negative sign is used to indicate polarization.
  ///< The negative sign indicates polarization (-ve is VPOL, +ve is HPOL)
  Int_t surfToAntMapAnita4[ACTIVE_SURFS][RFCHAN_PER_SURF]= {{-42, -34, -48, -40, 42, 34, 48, 40},
							    {-44, -36, -46, -38, 44, 36, 46, 38},
							    {-32, -24, -28, -20, 32, 24, 28, 20},
							    {-30, -22, -26, -18, 30, 22, 26, 18},
							    {-12,   4, -14,  -6, 12, -4, 14, 6 },  //ABL changed -/+4
							    {-10,  -2, -16,  -8, 10,  2, 16, 8 },
							    {-45, -37, -41, -33, 45, 37, 41, 33},
							    {-47, -39, -43, -35, 47, 39, 43, 35},
							    {-27, -19, -29, -21, 27, 19, 29, 21},
							    {-25, -17, -31, -23, 25, 17, 31, 23},
							    {-15,  -7, -11,  -3, 15,  7, 11, 3 },
							    {-13,  -5,  -9,  -1, 13,  5,  9, 1 }};
      
  Int_t surfToAntMapAnita3[ACTIVE_SURFS][RFCHAN_PER_SURF]= {{-42, -34, -48, -40, 42, 34, 48, 40},
							    {-44, -36, -46, -38, 44, 36, 46, 38},
							    {-32, -24, -28, -20, 32, 24, 28, 20},
							    {-30, -22, -26, -18, 30, 22, 26, 18},
							    {-12,   4, -14,  -6, 12, -4, 14,  6},
							    {-10,  -2, -16,  -8, 10,  2, 16,  8},
							    {-45, -37, -41, -33, 45, 37, 41, 33},
							    {-47, -39, -43, -35, 47, 39, 43, 35},
							    {-27, -19, -29, -21, 27, 19, 29, 21},
							    {-25, -17, -31, -23, 25, 17, 31, 23},
							    {-15,  -7, -11,  -3, 15,  7, 11,  3},
							    {-13,  -5,  -9,  -1, 13,  5, 9,   1}};
 
  ///< Map from phi-sector to antenna. Both start counting at zero.
  Int_t topAntNums[NUM_PHI]    = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  Int_t middleAntNums[NUM_PHI] = {16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
  Int_t bottomAntNums[NUM_PHI] = {32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47};
   
  ///< And the inverse, a map from antenna to phi-sector (using ANT-1 and ANT-17 and ANT-33 with the arrays).
  Int_t topPhiNums[NUM_PHI]    = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  Int_t middlePhiNums[NUM_PHI] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  Int_t bottomPhiNums[NUM_PHI] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};


  ///< Map from SURF and polarization to phi-sector.
  Int_t surfToPhiTriggerMapAnita3[ACTIVE_SURFS][2]={{-1,-1}, {-1,-1}, { 0, 4}, { 2, 6},
						    { 1, 5}, { 3, 7}, {15,11}, {13, 9},
						    {14,10}, {12, 8}, {-1,-1}, {-1,-1}};
  Int_t surfToPhiTriggerMapAnita4[ACTIVE_SURFS][2]={{-1,-1}, {-1,-1}, { 0, 1}, { 8, 9},
						    { 2, 3}, { 10,11}, {4,5}, {12, 13},
						    {6,7}, {14, 15}, {-1,-1}, {-1,-1}};

  
  using AnitaRing::kTopRing;
  using AnitaRing::kMiddleRing;
  using AnitaRing::kBottomRing;

  ///< Map from SURF trigger channel to ring  
  AnitaRing::AnitaRing_t surfTriggerChanToRing[SCALERS_PER_SURF]={kTopRing, kMiddleRing, kBottomRing,
								  kTopRing, kMiddleRing, kBottomRing,
								  kTopRing, kMiddleRing, kBottomRing,
								  kTopRing, kMiddleRing, kBottomRing};
  

  using AnitaTrigPol::kVertical;
  using AnitaTrigPol::kHorizontal;
  ///< Map from SURF trigger channel to polarization  
  AnitaTrigPol::AnitaTrigPol_t surfTriggerChanToPolAnita3[SCALERS_PER_SURF]={kVertical,   kVertical,   kVertical,
								     kVertical,   kVertical,   kVertical,
								     kHorizontal, kHorizontal, kHorizontal,
								     kHorizontal, kHorizontal, kHorizontal};

  using AnitaTrigPol::kRCP;
  using AnitaTrigPol::kLCP;
  AnitaTrigPol::AnitaTrigPol_t surfTriggerChanToPolAnita4[SCALERS_PER_SURF]={kRCP,kRCP,kRCP,
									     kRCP,kRCP,kRCP,
									     kLCP,kLCP,kLCP,
									     kLCP,kLCP,kLCP};
  
  Int_t phiToSurfTriggerMapAnita4[NUM_PHI] = {2,2,4,4,6,6,8,8,3,3,5,5,7,7,9,9};
  Int_t phiToSurfHalfAnita4[NUM_PHI]       = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};

  
  
  //These are from ANITA3
  Int_t phiToSurfTriggerMapAnita3[NUM_PHI] = {2,4,3,5,2,4,3,5,9,7,8,6,9,7,8,6};
  Int_t phiToSurfHalfAnita3[NUM_PHI]       = {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0};

}



//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Default constructor: defines antenna phase centre locations.
//////////////////////////////////////////////////////////////////////////////////////////////////////
AnitaGeomTool::AnitaGeomTool()
{
  int v = AnitaVersion::get(); 

  //Default constructor
  ringPhaseCentreOffset[0]=0.2-0.042685;
  ringPhaseCentreOffset[1]=0.2+0.00653;
  ringPhaseCentreOffset[2]=0.2+0.1927;

  //  readSimonsNumbers();
  //  readAnita2Photogrammetry();


  fUsePhotogrammetryNumbers=0;
  //TODO need to add phase-centers for A4 
  readPhaseCenterNumbers(v); // This one has to come before reading photogrammetry
  readPhotogrammetry(v);

  // Moved from fillAntPositionsFromPrioritizerdConfig
  // Who knows where else these get set in this crazy place...
  fHeadingRotationAxis.SetXYZ(0.,0.,1.);
  fPitchRotationAxis.SetXYZ(0.,1.,0.);
  fRollRotationAxis=fPitchRotationAxis.Cross(fHeadingRotationAxis);
  aftForeOffsetAngleVertical=TMath::DegToRad()*45;
  // for(int ant=0; ant<NUM_SEAVEYS; ant++){
  //   for(int pol=0; pol<2; pol++){
  //     std::cout << ant << "\t" << pol<< "\t"
  // 		<< rPhaseCentreFromVerticalHorn[ant][pol] << "\t"
  // 		<< rPhaseCentreFromVerticalHornPhotogrammetry[ant][pol] << "\t"
  // 		<< zPhaseCentreFromVerticalHorn[ant][pol] << "\t"
  // 		<< zPhaseCentreFromVerticalHornPhotogrammetry[ant][pol] << "\t"
  // 		<< azPhaseCentreFromVerticalHorn[ant][pol] << "\t"
  // 		<< azPhaseCentreFromVerticalHornPhotogrammetry[ant][pol] << "\t"
  // 		<< std::endl;
  //   }
  // }
  
  // fillAntPositionsFromPrioritizerdConfig();

  
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Default destructor
//////////////////////////////////////////////////////////////////////////////////////////////////////
AnitaGeomTool::~AnitaGeomTool()
{
  //Default destructor
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Use the geometrical information from he ANITA-3 Prioritizerd, from feed position numbers
//////////////////////////////////////////////////////////////////////////////////////////////////////
void AnitaGeomTool::fillAntPositionsFromPrioritizerdConfig() {

  Double_t phiArrayDeg[NUM_SEAVEYS]={0,22.5,45,67.5,90,112.5,135,157.5,180,
				     202.5,225,247.5,270,292.5,315,337.5,
				     0,22.5,45,67.5,90,112.5,135,157.5,180,
				     202.5,225,247.5,270,292.5,315,337.5,
				     0,22.5,45,67.5,90,112.5,135,157.5,180,
				     202.5,225,247.5,270,292.5,315,337.5};

  Double_t rArray[NUM_SEAVEYS]={0.9675,0.7402,0.9675,0.7402,0.9675,0.7402,0.9675,0.7402,
				0.9675,0.7402,0.9675,0.7402,0.9675,0.7402,0.9675,0.7402,
				2.0447,2.0447,2.0447,2.0447,2.0447,2.0447,2.0447,2.0447,
				2.0447,2.0447,2.0447,2.0447,2.0447,2.0447,2.0447,2.0447,
				2.0447,2.0447,2.0447,2.0447,2.0447,2.0447,2.0447,2.0447,
				2.0447,2.0447,2.0447,2.0447,2.0447,2.0447,2.0447,2.0447};
  Double_t zArray[NUM_SEAVEYS]={-1.4407,-2.4135,-1.4407,-2.4135,-1.4407,-2.4135,-1.4407,-2.4135,
				-1.4407,-2.4135,-1.4407,-2.4135,-1.4407,-2.4135,-1.4407,-2.4135,
				-5.1090,-5.1090,-5.1090,-5.1090,-5.1090,-5.1090,-5.1090,-5.1090,
				-5.1090,-5.1090,-5.1090,-5.1090,-5.1090,-5.1090,-5.1090,-5.1090,
				-6.1951,-6.1951,-6.1951,-6.1951,-6.1951,-6.1951,-6.1951,-6.1951,
				-6.1951,-6.1951,-6.1951,-6.1951,-6.1951,-6.1951,-6.1951,-6.1951};


  for(Int_t ant=0;ant<NUM_SEAVEYS;ant++) {
    for(Int_t pol=0;pol<2;pol++) {
      zPhaseCentreFromVerticalHorn[ant][pol]=zArray[ant];
      rPhaseCentreFromVerticalHorn[ant][pol]=rArray[ant];
      azPhaseCentreFromVerticalHorn[ant][pol]=phiArrayDeg[ant]*TMath::DegToRad();
      xPhaseCentreFromVerticalHorn[ant][pol]=rArray[ant]*TMath::Cos(azPhaseCentreFromVerticalHorn[ant][pol]);
      yPhaseCentreFromVerticalHorn[ant][pol]=rArray[ant]*TMath::Sin(azPhaseCentreFromVerticalHorn[ant][pol]);
    }
  }

  aftForeOffsetAngleVertical=TMath::DegToRad()*45;

  fHeadingRotationAxis.SetXYZ(0.,0.,1.);
  fPitchRotationAxis.SetXYZ(0.,1.,0.);
  fRollRotationAxis=fPitchRotationAxis.Cross(fHeadingRotationAxis);

}




//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Converts the SURF and channel numbers to phi-sector, ring and polarization
//////////////////////////////////////////////////////////////////////////////////////////////////////
Int_t AnitaGeomTool::getPhiRingPolFromSurfChanTrigger(Int_t surf,Int_t chan, Int_t &phi,
						      AnitaRing::AnitaRing_t &ring,AnitaTrigPol::AnitaTrigPol_t &pol)
{
  if(surf<0 || surf>=ACTIVE_SURFS) return -1;
  if(chan<0 || chan>=SCALERS_PER_SURF) return -1;

  Int_t surfHalf=0;				
  if((chan%6)>=3) surfHalf=1;

  Int_t v = AnitaVersion::get(); 

  phi = v == 4 ? AnitaGeom::surfToPhiTriggerMapAnita4[surf][surfHalf] : 
        v == 3 ? AnitaGeom::surfToPhiTriggerMapAnita3[surf][surfHalf] : 
        -1;

  ring=AnitaGeom::surfTriggerChanToRing[chan];

  pol = v == 4 ? AnitaGeom::surfTriggerChanToPolAnita4[chan] : 
        v == 3 ? AnitaGeom::surfTriggerChanToPolAnita3[chan] :
        AnitaTrigPol::kNotATrigPol  ;


  return phi;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Converts the SURF and L1 trigger channel to phi-sector and polarization.
//////////////////////////////////////////////////////////////////////////////////////////////////////
Int_t AnitaGeomTool::getPhiRingFromSurfL1Chan(Int_t surf, Int_t l1Chan, Int_t &phi, AnitaRing::AnitaRing_t &ring)
{
  if(phi<0 || phi>=NUM_PHI) return -1;
  Int_t surfHalf=l1Chan>2;   

  Int_t v = AnitaVersion::get(); 
  // this method only makes sends for A4
  assert(v==4); 

  phi = AnitaGeom::surfToPhiTriggerMapAnita4[surf][surfHalf] ; 

  if((l1Chan%3)==0) ring=AnitaRing::kTopRing;
  if((l1Chan%3)==1) ring=AnitaRing::kMiddleRing;
  if((l1Chan%3)==2) ring=AnitaRing::kBottomRing;
  return phi;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Converts the SURF and L1 trigger channel to phi-sector and polarization.
//////////////////////////////////////////////////////////////////////////////////////////////////////
Int_t AnitaGeomTool::getPhiPolFromSurfL1Chan(Int_t surf, Int_t l1Chan, Int_t &phi, AnitaPol::AnitaPol_t & pol)
{
  if(phi<0 || phi>=NUM_PHI) return -1;

  Int_t v = AnitaVersion::get(); 

  //this method only makes sense for A3
  assert(v == 3); 

  Int_t surfHalf=l1Chan%2;   
  phi=AnitaGeom::surfToPhiTriggerMapAnita3[surf][surfHalf];
  pol=AnitaPol::kVertical;
  if(l1Chan>=2) pol=AnitaPol::kHorizontal;
  return phi;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Converts the SURF and L1 trigger channel to phi-sector and polarization.
//////////////////////////////////////////////////////////////////////////////////////////////////////
Int_t AnitaGeomTool::getSurfChanTriggerFromPhiRingPol(Int_t phi,AnitaRing::AnitaRing_t ring, AnitaTrigPol::AnitaTrigPol_t pol ,Int_t &surf, Int_t &chan) {
  if(phi<0 || phi>=NUM_PHI) return -1;

  Int_t v = AnitaVersion::get(); 

  surf = v == 4 ? AnitaGeom::phiToSurfTriggerMapAnita4[phi] : 
         v == 3 ? AnitaGeom::phiToSurfTriggerMapAnita3[phi] : 
         -1 ;

  Int_t surfHalf = v == 4 ? AnitaGeom::phiToSurfHalfAnita4[phi] : 
                   v == 3 ? AnitaGeom::phiToSurfHalfAnita3[phi] : 
                  -1; 

  chan = (6-6*(pol % 2))+ ring + 3*surfHalf; 


  if(v == 3 && phi==3 && ring==AnitaRing::kTopRing) {     
    if(pol==AnitaTrigPol::kVertical) chan=6;
    if(pol==AnitaTrigPol::kHorizontal) chan=0;
  }
  return surf;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Convert phi-sector and polarization from phi-sector and polarization.
//////////////////////////////////////////////////////////////////////////////////////////////////////
Int_t AnitaGeomTool::getSurfL1TriggerChanFromPhiRing(Int_t phi, AnitaRing::AnitaRing_t ring, Int_t &surf, Int_t &l1Chan) 
{
  if(phi<0 || phi>=NUM_PHI) return -1;

  Int_t v = AnitaVersion::get(); 


  surf = v == 4 ? AnitaGeom::phiToSurfTriggerMapAnita4[phi] : 
         v == 3 ? AnitaGeom::phiToSurfTriggerMapAnita3[phi] : 
        -1 ;

  Int_t surfHalf = v == 4 ? AnitaGeom::phiToSurfHalfAnita4[phi] : 
                   v == 3 ? AnitaGeom::phiToSurfHalfAnita3[phi] : 
                   -1; 

  switch(ring) {
  case AnitaRing::kTopRing:
    l1Chan=3*surfHalf;
    break;
  case AnitaRing::kMiddleRing:
    l1Chan=1+(3*surfHalf);
    break;
  case AnitaRing::kBottomRing:
    l1Chan=2+(3*surfHalf);
    break;
  default:
    std::cerr << "Unknown ring " << ring << "\n";
  }       
  return surf;
}

Int_t AnitaGeomTool::getSurfL1TriggerChanFromPhiPol(Int_t phi, AnitaPol::AnitaPol_t pol, Int_t &surf, Int_t &l1Chan) 
{
  if(phi<0 || phi>=NUM_PHI) return -1;

  Int_t v = AnitaVersion::get(); 
  assert(v==3); 

  surf=AnitaGeom::phiToSurfTriggerMapAnita3[phi];
  Int_t surfHalf=AnitaGeom::phiToSurfHalfAnita3[phi];
  if(pol==AnitaPol::kVertical)
  {
    l1Chan=surfHalf;
  }
  else 
  {
    l1Chan=2+surfHalf;
  }

  return surf;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Converts phi-sector to surf and l2 trigger channel
//////////////////////////////////////////////////////////////////////////////////////////////////////
Int_t AnitaGeomTool::getSurfL2TriggerChanFromPhi(Int_t phi, Int_t &surf, Int_t &l2Chan)
{
  if(phi<0 || phi>=NUM_PHI) return -1;

  Int_t v = AnitaVersion::get(); 
  assert(v==4); 
  surf = AnitaGeom::phiToSurfTriggerMapAnita4[phi] ; 
  Int_t surfHalf=  AnitaGeom::phiToSurfHalfAnita4[phi] ; 

  l2Chan=surfHalf;
  return surf;
}




static TMutex instance_lock; 
//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Generates an instance of AnitaGeomTool, required for non-static functions.
//////////////////////////////////////////////////////////////////////////////////////////////////////
AnitaGeomTool*  AnitaGeomTool::Instance(int v )
{

  /**************************************************************************************************
   *                                                                                                *
   * Yeah, the classic singleton pattern isn't thread-safe.                                         *
   * We got away it before because we usually statically initialized AnitaGeomTool instances, but   *
   * with multiple versions, we can't necessarily do that.                                          *
   *                                                                                                *
   * If we required c++11, static initialization is guaranteed thread-safe, although                *
   * we'd have to use templates or something like that for this multiton pattern.                   *
   *                                                                                                *
   **************************************************************************************************/ 

  if (!v) v = AnitaVersion::get(); 

  AnitaGeomTool * tmp = instances[v]; 

  __asm__ __volatile__ ("" ::: "memory"); //memory fence! 
  if (!tmp) 
  {
    instance_lock.Lock(); 
    tmp = instances[v]; 
    if (!tmp) 
    {
      tmp = new AnitaGeomTool(); 
      __asm__ __volatile__ ("" ::: "memory");
      instances[v] = tmp; 
    }
    instance_lock.UnLock(); 
  }

  return instances[v];
}



Int_t AnitaGeomTool::getChanIndex(Int_t surf, Int_t chan){
  return chan+(CHANNELS_PER_SURF*surf);
}




Double_t AnitaGeomTool::getPhiDiff(Double_t firstPhi, Double_t secondPhi)
{
   Double_t phiDiff=firstPhi-secondPhi;
   if(TMath::Abs(phiDiff)>TMath::Abs(phiDiff+TMath::TwoPi()))
     phiDiff+=TMath::TwoPi();
   if(TMath::Abs(phiDiff)>TMath::Abs(phiDiff-TMath::TwoPi()))
     phiDiff-=TMath::TwoPi();
   return phiDiff;
}


Int_t AnitaGeomTool::getChanIndexFromRingPhiPol(AnitaRing::AnitaRing_t ring,
					      Int_t phi,
					      AnitaPol::AnitaPol_t pol)
{
  Int_t ant;
  if(phi<0 || phi>15) return -1;
  switch(ring) {
  case AnitaRing::kTopRing:
    ant=AnitaGeom::topAntNums[phi];
    break;
  case AnitaRing::kMiddleRing:
    ant=AnitaGeom::middleAntNums[phi];
    break;
  case AnitaRing::kBottomRing:
    ant=AnitaGeom::bottomAntNums[phi];
    break;
  default:
    return -1;
  }
  return getChanIndexFromAntPol(ant,pol);
}


Int_t AnitaGeomTool::getChanIndexFromAntPol(Int_t ant,
					  AnitaPol::AnitaPol_t pol)
{
  if(ant<0 || ant>(NUM_SEAVEYS-1)) return -1;
  Int_t surf,chan;
  surf=AnitaGeom::antToSurfMap[ant];
  if(pol==AnitaPol::kHorizontal) 
    chan=AnitaGeom::hAntToChan[ant];
  else if(pol==AnitaPol::kVertical)
    chan=AnitaGeom::vAntToChan[ant];
  else
    return -1;
  return getChanIndex(surf,chan);
}


// Int_t AnitaGeomTool::getRFCMFromAnt(Int_t ant)
// {
//   if(ant<0 || ant>(NUM_SEAVEYS-1)) return -1;
//   return AnitaGeom::antToRFCM[ant];
// }

// Int_t AnitaGeomTool::getRFCMChannelFromAntPol(Int_t ant,AnitaPol::AnitaPol_t pol)
// {
//   if(ant<0 || ant>(NUM_SEAVEYS-1)) return -1;
//   if(pol==AnitaPol::kHorizontal)
//     return AnitaGeom::hAntToRFCMChan[ant];
//   if(pol==AnitaPol::kVertical)
//     return AnitaGeom::vAntToRFCMChan[ant];
  
//   return -1;
// }


Int_t AnitaGeomTool::getSurfFromAnt(Int_t ant)
{
  if(ant<0 || ant>(NUM_SEAVEYS-1)) return -1;
  Int_t surf;
  surf=AnitaGeom::antToSurfMap[ant];
  return surf;
}


Int_t AnitaGeomTool::getChanFromAntPol(Int_t ant,AnitaPol::AnitaPol_t pol)
{
  Int_t chan;
  if(pol==AnitaPol::kHorizontal) 
      chan=AnitaGeom::hAntToChan[ant];
   else if(pol==AnitaPol::kVertical)
      chan=AnitaGeom::vAntToChan[ant];
   else
      return -1;
  return chan;
}


Int_t AnitaGeomTool::getAzimuthPartner(Int_t rx)
{
  //Need to update for bottom ring
  
  if (rx<16) 
    return AnitaGeom::middleAntNums[AnitaGeom::topPhiNums[rx]];
  else if(rx<32)
    return AnitaGeom::topAntNums[AnitaGeom::middlePhiNums[rx-16]]; 
  else
    return AnitaGeom::middleAntNums[AnitaGeom::bottomPhiNums[rx-32]]; 

  return -1;
}

void AnitaGeomTool::getNeighbors(Int_t rx,int& rxleft,int& rxright)
{

  /* for A3 and A4, for the top ring, the neighbors are offset by 2 and in the
   * bottom / mid ring they are offset by 1
   */

  int ring =  rx / 16; 

  if (ring == 0)
  {
    rxleft = (rx + 14)%16; 
    rxright = (rx + 2)%16; 
  }

  else
  {
    int ant = rx % 16; 
    rxleft = ring * 16 +   (ant +15) % 16; 
    rxright = ring * 16 +   (ant +1) % 16; 
  }


}

void AnitaGeomTool::getThetaPartners(Int_t rx,int& rxleft,int& rxright)
{
  if (rx<0 || rx>(NUM_SEAVEYS-1))    
    std::cerr << "Antenna number out of range!\n";     
  if (rx<16) {
    Int_t phi=AnitaGeom::topPhiNums[rx];
    Int_t phiLeft=phi-1;
    if(phiLeft<0) phiLeft=15;
    Int_t phiRight=phi+1;
    if(phiRight>15) phiRight=0;
    rxleft=AnitaGeom::topAntNums[phiLeft];
    rxright=AnitaGeom::topAntNums[phiRight];
  }
  else if (rx<32){
    Int_t phi=AnitaGeom::middlePhiNums[rx-16];
    Int_t phiLeft=phi-1;
    if(phiLeft<0) phiLeft=15;
    Int_t phiRight=phi+1;
    if(phiRight>15) phiRight=0;
    rxleft=AnitaGeom::middleAntNums[phiLeft];
    rxright=AnitaGeom::middleAntNums[phiRight];
  }
  else{
    Int_t phi=AnitaGeom::bottomPhiNums[rx-32];
    Int_t phiLeft=phi-1;
    if(phiLeft<0) phiLeft=15;
    Int_t phiRight=phi+1;
    if(phiRight>15) phiRight=0;
    rxleft=AnitaGeom::bottomAntNums[phiLeft];
    rxright=AnitaGeom::bottomAntNums[phiRight];
  }

}

Int_t AnitaGeomTool::getPhiSector(Int_t rx)
{
  if (rx<16)
    return AnitaGeom::topPhiNums[rx];
  else if(rx<32)
    return AnitaGeom::middlePhiNums[rx-16];
  else if(rx<48) {
    return AnitaGeom::bottomPhiNums[rx-32];
  }

  return 0;
}

Double_t AnitaGeomTool::getDirectionWrtNorth(Int_t phiSector, Double_t heading) {

  if(phiSector<0 || phiSector>=NUM_PHI) return -1.;

  //Copied this change from Amy don't (yet) know if it is correct
  // Float_t direction=(1.*360./NUM_PHI)+heading-(phiSector*360./NUM_PHI);

  ///< Heading increases clockwise but phi-sector numbers increase anticlockwise, hence -ve sign.
  Double_t direction = heading + aftForeOffsetAngleVertical*TMath::RadToDeg() - (phiSector*360./NUM_PHI);
  
  if(direction>=360){
    direction -= 360;
  }
  else if(direction < 0){
    direction += 360;
  }

  return direction;

}



Int_t AnitaGeomTool::getSurfChanFromChanIndex(Int_t chanIndex, // input channel index
					    Int_t &surf,Int_t &chan) // output surf and channel
{
  if(chanIndex<0 || chanIndex>=NUM_DIGITZED_CHANNELS)
    return 0;

  //  std::cout << "chanIndex is " << chanIndex << "\n";
  chan=chanIndex%CHANNELS_PER_SURF; // 0 to 8
  surf=(chanIndex-chan)/CHANNELS_PER_SURF; // 0 to 8

  return 1;

}

Int_t AnitaGeomTool::getAntPolFromSurfChan(Int_t surf,Int_t chan,Int_t &ant, AnitaPol::AnitaPol_t &pol) 
{

  int v = AnitaVersion::get(); 

  if (v >4 && v < 3) return 0; 

  ant = v == 4 ? AnitaGeom::surfToAntMapAnita4[surf][chan] : 
        v == 3 ? AnitaGeom::surfToAntMapAnita3[surf][chan] :
        0 ; 


  //  std::cout << "surf, chan, ant are " << surf << " " << chan << "\n";

  if (ant>0){
    pol = AnitaPol::kHorizontal;
  }
  else{
    pol = AnitaPol::kVertical;
  }
  
  ant=abs(ant)-1; // so that it's from 0 to 31

  return 1;
}

Int_t AnitaGeomTool::getAntOrientation(Int_t ant) {
  if(ant<0 || ant>=NUM_SEAVEYS)
    return 0;

  return AnitaVersion::get() == 3 ?  AnitaGeom::antOrientationMapAnita3[ant] : AnitaGeom::antOrientationMap[ant];
}

Int_t AnitaGeomTool::getLayer(Int_t irx) 
{
  if (irx<8)
    return 0;
  else if (irx>=8 && irx<16)
    return 1;
  else if (irx>=16)
    return 2;

  return 0;

}

AnitaRing::AnitaRing_t AnitaGeomTool::getRingFromAnt(Int_t ant) {
  if (ant<8)
    return AnitaRing::kTopRing;
  else if (ant>=8 && ant<16)
    return AnitaRing::kTopRing;
  else if (ant<32)
    return AnitaRing::kMiddleRing;
  else if (ant<48)
    return AnitaRing::kBottomRing;
  return AnitaRing::kNotARing;

}

void AnitaGeomTool::getRingAntPolPhiFromSurfChan(Int_t surf, Int_t chan,
						 AnitaRing::AnitaRing_t &ring,
						 Int_t &ant,
						 AnitaPol::AnitaPol_t &pol,
						 Int_t &phi)
{
  AnitaGeomTool::getAntPolFromSurfChan(surf,chan,ant,pol);
  ring=AnitaGeomTool::getRingFromAnt(ant);
  phi=AnitaGeomTool::getPhiFromAnt(ant);
}

void AnitaGeomTool::getSurfChanAntFromRingPhiPol(AnitaRing::AnitaRing_t ring,
						 Int_t phi,
						 AnitaPol::AnitaPol_t pol,
						 Int_t &surf, Int_t &chan, Int_t &ant)
{
  Int_t chanIndex=AnitaGeomTool::getChanIndexFromRingPhiPol(ring,phi,pol);
  AnitaGeomTool::getSurfChanFromChanIndex(chanIndex,surf,chan);
  ant=AnitaGeomTool::getAntFromPhiRing(phi,ring);
}


void AnitaGeomTool::readAnita2Photogrammetry()
{
  //  return; //Hack for now
  char calibDir[FILENAME_MAX];
  char fileName[FILENAME_MAX];
  char *calibEnv=getenv("ANITA_CALIB_DIR");
  if(!calibEnv) {
    char *utilEnv=getenv("ANITA_UTIL_INSTALL_DIR");
    if(!utilEnv)
      sprintf(calibDir,"calib");
    else
      sprintf(calibDir,"%s/share/anitaCalib",utilEnv);    
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }

  sprintf(fileName,"%s/antennaPositionTables.csv",calibDir);
  std::ifstream AntennaFile(fileName);
  if(!AntennaFile) {
    std::cerr << "Couldn't open:\t" << fileName << "\n";
    return;
  }


  //First up are the antenna positions
  TString line;
  for(Int_t i=0;i<4;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  
  for(Int_t ant=0;ant<32;ant++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Seavey:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<8;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	if (ant+1 != subString.Atoi()) {
	  std::cerr << "Antenna number mismatch\n";
	}
	break;
      case 1:	   
	xAntFromDeckHorn[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yAntFromDeckHorn[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zAntFromDeckHorn[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 4:	   
	rAntFromDeckHorn[ant]=subString.Atof()*INCHTOMETER; //m
	//	   std::cout << ant << "\t" << rAntFromDeckHorn[ant] << "\n";
	break;
      case 5:	   
	azCentreFromDeckHorn[ant]=subString.Atof()*TMath::DegToRad(); //radians
	break;
      case 6:	   
	apertureAzFromDeckHorn[ant]=subString.Atof()*TMath::DegToRad(); //radians
	break;
      case 7:	   
	apertureElFromDeckHorn[ant]=subString.Atof()*TMath::DegToRad(); //radians
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();

  }


  //Now we'll add a hack for the drop-down antennas
  for(Int_t ant=32;ant<48;ant++) {
    rAntFromDeckHorn[ant]=(101*INCHTOMETER)-0.38;
    zAntFromDeckHorn[ant]=(-21*INCHTOMETER)-1.8;
    azCentreFromDeckHorn[ant]=(-67.5 + 45*(ant-32))*TMath::DegToRad();
    apertureAzFromDeckHorn[ant]=azCentreFromDeckHorn[ant];
    apertureElFromDeckHorn[ant]=-10*TMath::DegToRad();
    xAntFromDeckHorn[ant]=rAntFromDeckHorn[ant]*TMath::Cos(azCentreFromDeckHorn[ant]);
    yAntFromDeckHorn[ant]=rAntFromDeckHorn[ant]*TMath::Sin(azCentreFromDeckHorn[ant]);
    //    std::cout << ant << "\t" << xAntFromDeckHorn[ant] << "\t" << yAntFromDeckHorn[ant]
    //	      << "\t" << zAntFromDeckHorn[ant] << "\t" << apertureAzFromDeckHorn[ant]*TMath::RadToDeg() << "\n";
  }
  for(Int_t pol=AnitaPol::kHorizontal;pol<=AnitaPol::kVertical;pol++) {
     for(Int_t ant=0;ant<NUM_SEAVEYS;ant++) {
	Double_t phaseCentreToAntFront=ringPhaseCentreOffset[Int_t(this->getRingFromAnt(ant))];
	
	
	Double_t   deltaXRL = -deltaRL*TMath::Sin(apertureAzFromDeckHorn[ant]);
	Double_t   deltaYRL = deltaRL*TMath::Cos(apertureAzFromDeckHorn[ant]);
	
	Double_t  deltaZUD = deltaUD*TMath::Cos(apertureElFromDeckHorn[ant]);
	Double_t  deltaXUD = deltaUD*TMath::Sin(apertureElFromDeckHorn[ant])*TMath::Cos(apertureAzFromDeckHorn[ant]);
	Double_t  deltaYUD = deltaUD*TMath::Sin(apertureElFromDeckHorn[ant])*TMath::Sin(apertureAzFromDeckHorn[ant]);
	
	deltaXRL = 0;
	deltaYRL = 0;
	
	deltaZUD = 0;
	deltaXUD = 0;
	deltaYUD = 0;
	
	// std::cout << deltaXRL << "  " << deltaXUD << "  "<< deltaYUD << "  "<< deltaYRL  <<  "  " <<  deltaZUD << std::endl;
	
	//     Now try to find the location of the antenna phaseCentre poInt_t     
	xPhaseCentreFromDeckHorn[ant][pol]=(xAntFromDeckHorn[ant] + deltaXRL + deltaXUD) -
	   phaseCentreToAntFront*TMath::Cos( apertureAzFromDeckHorn[ant])*TMath::Cos(apertureElFromDeckHorn[ant]); //m
	yPhaseCentreFromDeckHorn[ant][pol]=(yAntFromDeckHorn[ant] + deltaYRL + deltaYUD) -
	   phaseCentreToAntFront*TMath::Sin( apertureAzFromDeckHorn[ant])*TMath::Cos(apertureElFromDeckHorn[ant]); //m
	zPhaseCentreFromDeckHorn[ant][pol]=(zAntFromDeckHorn[ant] + deltaZUD) -phaseCentreToAntFront*TMath::Sin(apertureElFromDeckHorn[ant]); //m
	
    //  xPhaseCentreFromDeckHorn[ant]=xAntFromDeckHorn[ant] -
    //        phaseCentreToAntFront*TMath::Cos( apertureAzFromDeckHorn[ant])*TMath::Cos(apertureElFromDeckHorn[ant]); //m
    //      yPhaseCentreFromDeckHorn[ant]=yAntFromDeckHorn[ant] -
    //        phaseCentreToAntFront*TMath::Sin( apertureAzFromDeckHorn[ant])*TMath::Cos(apertureElFromDeckHorn[ant]); //m
    //      zPhaseCentreFromDeckHorn[ant]=zAntFromDeckHorn[ant] - phaseCentreToAntFront*TMath::Sin(apertureElFromDeckHorn[ant]); //m
	
	rPhaseCentreFromDeckHorn[ant][pol]=TMath::Sqrt(xPhaseCentreFromDeckHorn[ant][pol]*xPhaseCentreFromDeckHorn[ant][pol]+
						       yPhaseCentreFromDeckHorn[ant][pol]*yPhaseCentreFromDeckHorn[ant][pol]);//m
	azPhaseCentreFromDeckHorn[ant][pol]=TMath::ATan(yPhaseCentreFromDeckHorn[ant][pol]/xPhaseCentreFromDeckHorn[ant][pol]); //radians
	if(xPhaseCentreFromDeckHorn[ant][pol]<0)
	   azPhaseCentreFromDeckHorn[ant][pol]+=TMath::Pi();
	if(azPhaseCentreFromDeckHorn[ant][pol]<0)
	   azPhaseCentreFromDeckHorn[ant][pol]+=TMath::TwoPi();

    //      std::cout << "Face v PhaseCentre -- Ant " << ant  << std::endl;
    //      std::cout << apertureAzFromDeckHorn[ant] << "\t" << apertureElFromDeckHorn[ant] << "\n";
    //      std::cout << "x\t" << xAntFromDeckHorn[ant] << "\t" << xPhaseCentreFromDeckHorn[ant][pol] << std::endl;
    //      std::cout << "y\t" << yAntFromDeckHorn[ant] << "\t" << yPhaseCentreFromDeckHorn[ant][pol] << std::endl;
    //      std::cout << "z\t" << zAntFromDeckHorn[ant] << "\t" << zPhaseCentreFromDeckHorn[ant][pol] << std::endl;
    //      std::cout << "r\t" << rAntFromDeckHorn[ant] << "\t" << rPhaseCentreFromDeckHorn[ant][pol] << std::endl;
    //      std::cout << "phi\t" << azCentreFromDeckHorn[ant] << "\t" << azPhaseCentreFromDeckHorn[ant][pol] << std::endl;
     
     }
  }

  //Now bicones and discones  
  for(Int_t i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }


  for(Int_t ant=0;ant<4;ant++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Bicones:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	   
	xAntFromDeckBicone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yAntFromDeckBicone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zAntFromDeckBicone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();
  }

  for(Int_t ant=0;ant<4;ant++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Discones:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	   
	xAntFromDeckDiscone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yAntFromDeckDiscone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zAntFromDeckDiscone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      default:	   
	break;
      }
    }
    tokens->Delete();
  }

  //Now box enclosures and the like
  for(Int_t i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(Int_t corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Anita Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	   
	xAnitaBoxFromDeckCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yAnitaBoxFromDeckCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zAnitaBoxFromDeckCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();
  }
  for(Int_t i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(Int_t corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Battery Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	   
	xBatteryBoxFromDeckCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yBatteryBoxFromDeckCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zBatteryBoxFromDeckCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();
  }
  for(Int_t i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(Int_t corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Sip Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	   
	xSipBoxFromDeckCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	ySipBoxFromDeckCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zSipBoxFromDeckCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();
  }
  //Now gps stuff
  for(Int_t i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  {
    line.ReadLine(AntennaFile);
    //std::cout << "GPS Plane:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	 
      case 2:	
      case 3:	     
	gpsPlaneFromDeck[j-1]=subString.Atof(); //unit vector
	break;
      default:	   
	break;
      }
	
    }
    tokens->Delete();
  }  
  {
    line.ReadLine(AntennaFile);
    //std::cout << "GPS Heading:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	 
      case 2:	
      case 3:	     
	gpsHeadingFromDeck[j-1]=subString.Atof(); //unit vector
	break;
      default:	   
	break;
      }
	
    }
    tokens->Delete();
  }  

  aftForeOffsetAngleDeck=TMath::ATan(gpsHeadingFromDeck[1]/gpsHeadingFromDeck[0]);
  
  //Now switch to the (more useful) vertical measurements
  //First up are the antenna positions
  for(Int_t i=0;i<5;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  
  for(Int_t ant=0;ant<32;ant++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Seavey:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<8;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	//std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	if (ant+1 != subString.Atoi()) {
	  std::cerr << "Antenna number mismatch\n";
	}
	break;
      case 1:	   
	xAntFromVerticalHorn[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yAntFromVerticalHorn[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zAntFromVerticalHorn[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 4:	   
	rAntFromVerticalHorn[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 5:	   
	azCentreFromVerticalHorn[ant]=subString.Atof()*TMath::DegToRad(); //radians
	if(azCentreFromVerticalHorn[ant]<0)
	  azCentreFromVerticalHorn[ant]+=TMath::TwoPi();
	break;
      case 6:	   
	apertureAzFromVerticalHorn[ant]=subString.Atof()*TMath::DegToRad(); //radians
	break;
      case 7:	   
	apertureElFromVerticalHorn[ant]=subString.Atof()*TMath::DegToRad(); //radians
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();
  }
  for(Int_t pol=AnitaPol::kHorizontal;pol<=AnitaPol::kVertical;pol++) {
     for(Int_t ant=0;ant<32;ant++) {

    Double_t phaseCentreToAntFront=ringPhaseCentreOffset[Int_t(this->getRingFromAnt(ant))];

    Double_t   deltaXRL = -deltaRL*TMath::Sin(apertureAzFromVerticalHorn[ant]);
    Double_t   deltaYRL = deltaRL*TMath::Cos(apertureAzFromVerticalHorn[ant]);

    Double_t  deltaZUD = deltaUD*TMath::Cos(apertureElFromVerticalHorn[ant]);
    Double_t  deltaXUD = -deltaUD*TMath::Sin(apertureElFromVerticalHorn[ant])*TMath::Cos(apertureAzFromVerticalHorn[ant]);
    Double_t  deltaYUD = -deltaUD*TMath::Sin(apertureElFromVerticalHorn[ant])*TMath::Sin(apertureAzFromVerticalHorn[ant]);

    //std::cout << deltaXRL << "  " << deltaXUD << "  "<< deltaYUD << "  "<< deltaYRL  <<  "  " <<  deltaZUD << std::endl;

    //Now try to find the location of the antenna phaseCentre poInt_t     
    xPhaseCentreFromVerticalHorn[ant][pol]=(xAntFromVerticalHorn[ant] + deltaXRL + deltaXUD) -
      phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    yPhaseCentreFromVerticalHorn[ant][pol]=(yAntFromVerticalHorn[ant] + deltaYRL + deltaYUD) -
      phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    zPhaseCentreFromVerticalHorn[ant][pol]=(zAntFromVerticalHorn[ant] + deltaZUD) -phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m

    //      //Now try to find the location of the antenna phaseCentre poInt_t     
    //      xPhaseCentreFromVerticalHorn[ant]=xAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      yPhaseCentreFromVerticalHorn[ant]=yAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      zPhaseCentreFromVerticalHorn[ant]=zAntFromVerticalHorn[ant]-phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m
    rPhaseCentreFromVerticalHorn[ant][pol]=TMath::Sqrt(xPhaseCentreFromVerticalHorn[ant][pol]*xPhaseCentreFromVerticalHorn[ant][pol]+
						  yPhaseCentreFromVerticalHorn[ant][pol]*yPhaseCentreFromVerticalHorn[ant][pol]);//m
    azPhaseCentreFromVerticalHorn[ant][pol]=TMath::ATan(yPhaseCentreFromVerticalHorn[ant][pol]/xPhaseCentreFromVerticalHorn[ant][pol]); //radians
    if(xPhaseCentreFromVerticalHorn[ant][pol]<0)
      azPhaseCentreFromVerticalHorn[ant][pol]+=TMath::Pi();
    if(azPhaseCentreFromVerticalHorn[ant][pol]<0)
      azPhaseCentreFromVerticalHorn[ant][pol]+=TMath::TwoPi();

       
    //      std::cout << "Face v PhaseCentre -- Ant " << ant  << std::endl;
    //      std::cout << "x\t" << xAntFromVerticalHorn[ant] << "\t" << xPhaseCentreFromVerticalHorn[ant][pol] << std::endl;
    //      std::cout << "y\t" << yAntFromVerticalHorn[ant] << "\t" << yPhaseCentreFromVerticalHorn[ant][pol] << std::endl;
    //      std::cout << "z\t" << zAntFromVerticalHorn[ant] << "\t" << zPhaseCentreFromVerticalHorn[ant][pol] << std::endl;
    //      std::cout << "r\t" << rAntFromVerticalHorn[ant] << "\t" << rPhaseCentreFromVerticalHorn[ant][pol] << std::endl;
    //      std::cout << "phi\t" << azCentreFromVerticalHorn[ant] << "\t" << azPhaseCentreFromVerticalHorn[ant][pol] << std::endl;

  }
  }



  //Now we'll add a hack for the drop-down antennas
  for(Int_t pol=AnitaPol::kHorizontal;pol<=AnitaPol::kVertical;pol++) {
  for(Int_t ant=32;ant<NUM_SEAVEYS;ant++) {
    Double_t phaseCentreToAntFront=ringPhaseCentreOffset[Int_t(this->getRingFromAnt(ant))];
    //    std::cout << ant << "\t" << phaseCentreToAntFront << "\n";
    Double_t   deltaXRL = 0;
    Double_t   deltaYRL = 0;

    Double_t  deltaZUD = 0;
    Double_t  deltaXUD = 0;
    Double_t  deltaYUD = 0;

    // RJN changed the 0.38 number to 0.3 to match simons update function
    //Here's the little bastard that was causing all the problems
    rAntFromVerticalHorn[ant]=(101*INCHTOMETER)-0.3;
    //    rAntFromVerticalHorn[ant]=(101*INCHTOMETER)-0.38;
    zAntFromVerticalHorn[ant]=(-21*INCHTOMETER)-1.8;
    azCentreFromVerticalHorn[ant]=(-67.5 + 45*(ant-32))*TMath::DegToRad();
    
    apertureAzFromVerticalHorn[ant]=azCentreFromVerticalHorn[ant];
    apertureElFromVerticalHorn[ant]=-10*TMath::DegToRad();
    xAntFromVerticalHorn[ant]=rAntFromVerticalHorn[ant]*TMath::Cos(azCentreFromVerticalHorn[ant]);
    yAntFromVerticalHorn[ant]=rAntFromVerticalHorn[ant]*TMath::Sin(azCentreFromVerticalHorn[ant]);


    xPhaseCentreFromVerticalHorn[ant][pol]=(xAntFromVerticalHorn[ant] + deltaXRL + deltaXUD) -
      phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    yPhaseCentreFromVerticalHorn[ant][pol]=(yAntFromVerticalHorn[ant] + deltaYRL + deltaYUD) -
      phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    zPhaseCentreFromVerticalHorn[ant][pol]=(zAntFromVerticalHorn[ant] + deltaZUD) -phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m

    //      //Now try to find the location of the antenna phaseCentre poInt_t     
    //      xPhaseCentreFromVerticalHorn[ant][pol]=xAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      yPhaseCentreFromVerticalHorn[ant][pol]=yAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      zPhaseCentreFromVerticalHorn[ant][pol]=zAntFromVerticalHorn[ant]-phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m
    rPhaseCentreFromVerticalHorn[ant][pol]=TMath::Sqrt(xPhaseCentreFromVerticalHorn[ant][pol]*xPhaseCentreFromVerticalHorn[ant][pol]+
						  yPhaseCentreFromVerticalHorn[ant][pol]*yPhaseCentreFromVerticalHorn[ant][pol]);//m
    azPhaseCentreFromVerticalHorn[ant][pol]=TMath::ATan(yPhaseCentreFromVerticalHorn[ant][pol]/xPhaseCentreFromVerticalHorn[ant][pol]); //radians
    if(xPhaseCentreFromVerticalHorn[ant][pol]<0)
      azPhaseCentreFromVerticalHorn[ant][pol]+=TMath::Pi();
    if(azPhaseCentreFromVerticalHorn[ant][pol]<0)
      azPhaseCentreFromVerticalHorn[ant][pol]+=TMath::TwoPi();

    
  //   std::cout << "Ryan's:\t" << ant << " "  
// 	      << rPhaseCentreFromVerticalHorn[ant][pol] << " " 
// 	      << azPhaseCentreFromVerticalHorn[ant][pol] << " "
// 	      << zPhaseCentreFromVerticalHorn[ant][pol] << "\n";

  }
  }


  //Now bicones and discones  
  for(Int_t i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }


  for(Int_t ant=0;ant<4;ant++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Bicones:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	//std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	   
	xAntFromVerticalBicone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yAntFromVerticalBicone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zAntFromVerticalBicone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();
  }

  for(Int_t ant=0;ant<4;ant++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Discones:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	//std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	   
	xAntFromVerticalDiscone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yAntFromVerticalDiscone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zAntFromVerticalDiscone[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();
  }

  //Now box enclosures and the like
  for(Int_t i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(Int_t corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Anita Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	//std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	   
	xAnitaBoxFromVerticalCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yAnitaBoxFromVerticalCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zAnitaBoxFromVerticalCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();
  }
  for(Int_t i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(Int_t corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Battery Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	//std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	   
	xBatteryBoxFromVerticalCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yBatteryBoxFromVerticalCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zBatteryBoxFromVerticalCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();
  }
  for(Int_t i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(Int_t corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Sip Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	//std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	   
	xSipBoxFromVerticalCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	ySipBoxFromVerticalCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zSipBoxFromVerticalCorner[corner]=subString.Atof()*INCHTOMETER; //m
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();
  }
  //Now gps stuff
  for(Int_t i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  {
    line.ReadLine(AntennaFile);
    //std::cout << "GPS Plane:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	//std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	 
      case 2:	
      case 3:	     
	gpsPlaneFromVertical[j-1]=subString.Atof(); //unit vector
	break;
      default:	   
	break;
      }
	
    }
    tokens->Delete();
  }  
  {
    line.ReadLine(AntennaFile);
    //std::cout << "GPS Heading:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(Int_t j=0;j<4;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	//std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	break;
      case 1:	 
      case 2:	
      case 3:	     
	gpsHeadingFromVertical[j-1]=subString.Atof(); // unit vector
	break;
      default:	   
	break;
      }
	
    }
    tokens->Delete();
  }  
  aftForeOffsetAngleVertical=TMath::ATan(gpsHeadingFromVertical[1]/gpsHeadingFromVertical[0]);
  //  fHeadingRotationAxis.SetXYZ(gpsPlaneFromVertical[0],gpsPlaneFromVertical[1],gpsPlaneFromVertical[2]);
  //  fRollRotationAxis.SetXYZ(gpsHeadingFromVertical[0],gpsHeadingFromVertical[1],gpsHeadingFromVertical[2]);

  //Fix the heading and other axes to be 'ideal'

  //Now just for the sake of confusion we are going to redefine our definition
  //of phi equals zero to lie in the ADU5 fore direction.
  fHeadingRotationAxis.SetXYZ(0.,0.,1.);
  fPitchRotationAxis.SetXYZ(0.,1.,0.);
  fRollRotationAxis=fPitchRotationAxis.Cross(fHeadingRotationAxis);

  //ryans original pitch axis
  //fPitchRotationAxis=fRollRotationAxis.Cross(fHeadingRotationAxis);
  //reversed pitch axis
  //fPitchRotationAxis=fHeadingRotationAxis.Cross(fRollRotationAxis);

  //fPitchRotationAxis.SetXYZ(0.,1.,0.);
  //fRollRotationAxis=fPitchRotationAxis.Cross(fHeadingRotationAxis);

  //fitted gps axes
//   fPitchRotationAxis.SetXYZ(0,cos(-0.176*TMath::DegToRad()),sin(-0.176*TMath::DegToRad()));
//   fRollRotationAxis.SetXYZ(cos(0.460*TMath::DegToRad()),0,sin(0.460*TMath::DegToRad()));
//   fHeadingRotationAxis=fRollRotationAxis.Cross(fPitchRotationAxis);

  //2nd attempt fitted gps axes
//    fPitchRotationAxis.SetXYZ(0,cos(1.3*TMath::DegToRad()),sin(1.3*TMath::DegToRad()));
//    fRollRotationAxis.SetXYZ(cos(0.36*TMath::DegToRad()),0,sin(0.36*TMath::DegToRad()));
//    fHeadingRotationAxis=fRollRotationAxis.Cross(fPitchRotationAxis)
     ;


  //Now add in Simon's corrections
  for(Int_t pol=AnitaPol::kHorizontal;pol<=AnitaPol::kVertical;pol++) {
     for(Int_t ant=0;ant<NUM_SEAVEYS;ant++) {
	rPhaseCentreFromVerticalHorn[ant][pol]+=deltaRPhaseCentre[ant][pol];
	//    std::cout << rPhaseCentreFromVerticalHorn[ant][pol] << "\t" << deltaRPhaseCentre[ant][pol] << "\n";
	azPhaseCentreFromVerticalHorn[ant][pol]+=deltaPhiPhaseCentre[ant][pol];    
	if(azPhaseCentreFromVerticalHorn[ant][pol]<0)
	   azPhaseCentreFromVerticalHorn[ant][pol]+=TMath::TwoPi();
	if(azPhaseCentreFromVerticalHorn[ant][pol]>TMath::TwoPi())
	   azPhaseCentreFromVerticalHorn[ant][pol]-=TMath::TwoPi();    
	zPhaseCentreFromVerticalHorn[ant][pol]+=deltaZPhaseCentre[ant][pol];
	xPhaseCentreFromVerticalHorn[ant][pol]=rPhaseCentreFromVerticalHorn[ant][pol]*TMath::Cos(azPhaseCentreFromVerticalHorn[ant][pol]);
	yPhaseCentreFromVerticalHorn[ant][pol]=rPhaseCentreFromVerticalHorn[ant][pol]*TMath::Sin(azPhaseCentreFromVerticalHorn[ant][pol]);
     }
  }

//    std::cout << " heading axis x " << fHeadingRotationAxis.x() << " y " << fHeadingRotationAxis.y() << " z " << fHeadingRotationAxis.z() << std::endl;
//    std::cout << " roll axis x " << fRollRotationAxis.x() << " y " << fRollRotationAxis.y() << " z " << fRollRotationAxis.z() << std::endl;
//    std::cout << " pitch axis x " << fPitchRotationAxis.x() << " y " << fPitchRotationAxis.y() << " z " << fPitchRotationAxis.z() << std::endl;


}


//Non static thingies
void AnitaGeomTool::getAntXYZ(Int_t ant, Double_t &x, Double_t &y, Double_t &z,AnitaPol::AnitaPol_t pol)
{
  if(ant>=0 && ant<NUM_SEAVEYS) {
    x=xPhaseCentreFromVerticalHorn[ant][pol];
    y=yPhaseCentreFromVerticalHorn[ant][pol];
    z=zPhaseCentreFromVerticalHorn[ant][pol];
    //std::cout << "ant " << x << " y " << y << " z " << std::endl;
  }
}

Double_t AnitaGeomTool::getAntZ(Int_t ant, AnitaPol::AnitaPol_t pol) {
  if(fUsePhotogrammetryNumbers==0) {
    if(ant>=0 && ant<NUM_SEAVEYS) {
      //      std::cout << "Using simon z\n";
      return zPhaseCentreFromVerticalHorn[ant][pol];
    }
  }
  else {
    if(ant>=0 && ant<NUM_SEAVEYS) {
      //      std::cout << "Using kurt z\n";
      return zPhaseCentreFromVerticalHornPhotogrammetry[ant][pol];
    }      
  }
  return 0;
}

Double_t AnitaGeomTool::getAntR(Int_t ant, AnitaPol::AnitaPol_t pol) {
  if(fUsePhotogrammetryNumbers==0) {
    //    std::cout << "Using simon R\n";
    if(ant>=0 && ant<NUM_SEAVEYS) {
      return rPhaseCentreFromVerticalHorn[ant][pol];
    }
  }
  else {
    //    std::cout << "Using kurt R\n";
    if(ant>=0 && ant<NUM_SEAVEYS) {
      return rPhaseCentreFromVerticalHornPhotogrammetry[ant][pol];
    }
  }
  return 0;
}

Double_t AnitaGeomTool::getAntPhiPosition(Int_t ant, AnitaPol::AnitaPol_t pol) {

  if(fUsePhotogrammetryNumbers==0) {
    //    std::cout << "Using simon phi\n";
    if(ant>=0 && ant<NUM_SEAVEYS) {
      return azPhaseCentreFromVerticalHorn[ant][pol];
    }
  }
  else {
    if(ant>=0 && ant<NUM_SEAVEYS) {
      //      std::cout << "Using kurt phi\n";
      return azPhaseCentreFromVerticalHornPhotogrammetry[ant][pol];
    }
  }
  return 0;
}

Double_t AnitaGeomTool::getAntPhiPositionRelToAftFore(Int_t ant, AnitaPol::AnitaPol_t pol) {

  if(fUsePhotogrammetryNumbers==0) {
    //    std::cout << "Using simon phi-rel\n";
    if(ant>=0 && ant<NUM_SEAVEYS) {
      Double_t phi=azPhaseCentreFromVerticalHorn[ant][pol]-aftForeOffsetAngleVertical;
      if(phi<0)
	phi+=TMath::TwoPi();
      if(phi>TMath::TwoPi())
	phi-=TMath::TwoPi();
      return phi;
    }
  }
  else {
    if(ant>=0 && ant<NUM_SEAVEYS) {
      //      std::cout << "Using kurt phi-rel\n";
      Double_t phi=azPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]-aftForeOffsetAngleVerticalPhotogrammetry;
      //      std::cout << ant << " " << phi*TMath::RadToDeg() << std::endl;

      if(phi<0)
	phi+=TMath::TwoPi();
      if(phi>TMath::TwoPi())
	phi-=TMath::TwoPi();
      return phi;
    }
  }      
  return 0;
}

Double_t AnitaGeomTool::getMeanAntPairPhiRelToAftFore(Int_t firstAnt, Int_t secondAnt, AnitaPol::AnitaPol_t pol) {

   if(firstAnt>=0 && firstAnt<NUM_SEAVEYS && secondAnt>=0 && secondAnt<NUM_SEAVEYS) {
      //Calculating the phi of each antenna pair
      Double_t meanPhi=this->getAntPhiPositionRelToAftFore(firstAnt,pol); 
      if(TMath::Abs(meanPhi-this->getAntPhiPositionRelToAftFore(secondAnt,pol))<TMath::PiOver2()) {	 
	 meanPhi+=this->getAntPhiPositionRelToAftFore(secondAnt,pol);	 
	 meanPhi*=0.5;       
      }       
      else if(TMath::Abs(meanPhi-(this->getAntPhiPositionRelToAftFore(secondAnt,pol)+TMath::TwoPi()))<TMath::PiOver2()) {	 
	 meanPhi+=(this->getAntPhiPositionRelToAftFore(secondAnt,pol)+TMath::TwoPi());	 
	 meanPhi*=0.5;       
      }       
      else if(TMath::Abs(meanPhi-(this->getAntPhiPositionRelToAftFore(secondAnt,pol)-TMath::TwoPi()))<TMath::PiOver2()) {	 
	 meanPhi+=(this->getAntPhiPositionRelToAftFore(secondAnt,pol)-TMath::TwoPi());	 
	 meanPhi*=0.5;       
      }      
      return meanPhi;
   }
   return -999;
}



Int_t AnitaGeomTool::getTopAntNearestPhiWave(Double_t phiWave, AnitaPol::AnitaPol_t pol) {
  if(phiWave<0) phiWave+=TMath::TwoPi();
  if(phiWave>TMath::TwoPi()) phiWave-=TMath::TwoPi();
  Double_t minDiff=TMath::TwoPi();
  Int_t minAnt=0;;
  for(Int_t ant=0;ant<16;ant++) {
    //      std::cout << ant << "\t" << azPhaseCentreFromVerticalHorn[ant][pol] << "\t" << phiPrime << "\t" << TMath::Abs(azPhaseCentreFromVerticalHorn[ant][pol]-phiPrime) << "\n";
    if(TMath::Abs(getAntPhiPositionRelToAftFore(ant,pol)-phiWave)<minDiff) {
      minDiff=TMath::Abs(getAntPhiPositionRelToAftFore(ant,pol)-phiWave);
      minAnt=ant;
    }
    
    if(TMath::Abs((getAntPhiPositionRelToAftFore(ant,pol)-TMath::TwoPi())-phiWave)<minDiff) {
      minDiff=TMath::Abs((getAntPhiPositionRelToAftFore(ant,pol)-TMath::TwoPi())-phiWave);
      minAnt=ant;      
    }

    if(TMath::Abs((getAntPhiPositionRelToAftFore(ant,pol)+TMath::TwoPi())-phiWave)<minDiff) {
      minDiff=TMath::Abs((getAntPhiPositionRelToAftFore(ant,pol)+TMath::TwoPi())-phiWave);
      minAnt=ant;      
    }

  }
  return minAnt;
}

Int_t AnitaGeomTool::getUpperAntNearestPhiWave(Double_t phiWave, AnitaPol::AnitaPol_t pol) {
  return getTopAntNearestPhiWave(phiWave,pol);
}


//Non static thingies
void AnitaGeomTool::getAntFaceXYZ(Int_t ant, Double_t &x, Double_t &y, Double_t &z)
{
  if(ant>=0 && ant<NUM_SEAVEYS) {
    x=xAntFromVerticalHorn[ant];
    y=yAntFromVerticalHorn[ant];
    z=zAntFromVerticalHorn[ant];
  }
}

Double_t AnitaGeomTool::getAntFaceZ(Int_t ant) {
  if(ant>=0 && ant<NUM_SEAVEYS) {
    return zAntFromVerticalHorn[ant];
    //      return zAntFaceFromDeckHorn[ant];
  }
  return 0;
}

Double_t AnitaGeomTool::getAntFaceR(Int_t ant) {
  if(ant>=0 && ant<NUM_SEAVEYS) {
    return rAntFromVerticalHorn[ant];
    //return rAntFromDeckHorn[ant];
  }
  return 0;
}

Double_t AnitaGeomTool::getAntFacePhiPosition(Int_t ant) {
  if(ant>=0 && ant<NUM_SEAVEYS) {
    return azCentreFromVerticalHorn[ant];
  }
  return 0;
}

Double_t AnitaGeomTool::getAntFacePhiPositionRelToAftFore(Int_t ant) {
  if(ant>=0 && ant<NUM_SEAVEYS) {
    Double_t phi=azCentreFromVerticalHorn[ant]-aftForeOffsetAngleVertical;
    if(phi<0)
      phi+=TMath::TwoPi();
    return phi;
  }
  return 0;
}

Int_t AnitaGeomTool::getTopAntFaceNearestPhiWave(Double_t phiWave) {
  Double_t phiPrime=phiWave+aftForeOffsetAngleVertical;
  if(phiPrime>TMath::TwoPi()) 
    phiPrime-=TMath::TwoPi();
  Double_t minDiff=TMath::TwoPi();
  Int_t minAnt=0;;
  for(Int_t ant=0;ant<16;ant++) {
    //      std::cout << ant << "\t" << azPhaseCentreFromVerticalHorn[ant][pol] << "\t" << phiPrime << "\t" << TMath::Abs(azPhaseCentreFromVerticalHorn[ant][pol]-phiPrime) << "\n";
    if(TMath::Abs(azCentreFromVerticalHorn[ant]-phiPrime)<minDiff) {
      minDiff=TMath::Abs(azCentreFromVerticalHorn[ant]-phiPrime);
      minAnt=ant;
    }
  }
  return minAnt;
}

void AnitaGeomTool::updateAnt(Double_t deltaR,Double_t deltaRL,Double_t deltaUD){
  
   Double_t phaseCentreToAntFront=0.2+deltaR;
  
   Double_t   deltaXRL = 0;
   Double_t   deltaYRL = 0;
   
   Double_t  deltaZUD = 0;
   Double_t  deltaXUD = 0;
   Double_t  deltaYUD = 0;
   
   std::cout << deltaXRL << "  " << deltaXUD << "  "<< deltaYUD << "  "<< deltaYRL  <<  "  " <<  deltaZUD << " " << deltaRL << std::endl;

  for(Int_t pol=AnitaPol::kHorizontal;pol<=AnitaPol::kVertical;pol++) {
  for(Int_t ant=0;ant<32;ant++) {
    
    //deltaXRL = -deltaRL*TMath::Sin(apertureAzFromVerticalHorn[ant]);
	    //       deltaYRL = deltaRL*TMath::Cos(apertureAzFromVerticalHorn[ant]);
    
    deltaZUD = deltaUD*TMath::Cos(apertureElFromVerticalHorn[ant]);
    deltaXUD = -deltaUD*TMath::Sin(apertureElFromVerticalHorn[ant])*TMath::Cos(apertureAzFromVerticalHorn[ant]);
    deltaYUD = -deltaUD*TMath::Sin(apertureElFromVerticalHorn[ant])*TMath::Sin(apertureAzFromVerticalHorn[ant]);
    
    //std::cout << deltaXRL << "  " << deltaXUD << "  "<< deltaYUD << "  "<< deltaYRL  <<  "  " <<  deltaZUD << std::endl;
    
    //Now try to find the location of the antenna phaseCentre poInt_t     
    xPhaseCentreFromVerticalHorn[ant][pol]=(xAntFromVerticalHorn[ant] + deltaXRL + deltaXUD) -
      phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    yPhaseCentreFromVerticalHorn[ant][pol]=(yAntFromVerticalHorn[ant] + deltaYRL + deltaYUD) -
      phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    zPhaseCentreFromVerticalHorn[ant][pol]=(zAntFromVerticalHorn[ant] + deltaZUD) -phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m

    
    rPhaseCentreFromVerticalHorn[ant][pol]=TMath::Sqrt(xPhaseCentreFromVerticalHorn[ant][pol]*xPhaseCentreFromVerticalHorn[ant][pol]+
						  yPhaseCentreFromVerticalHorn[ant][pol]*yPhaseCentreFromVerticalHorn[ant][pol]);//m
    azPhaseCentreFromVerticalHorn[ant][pol]=TMath::ATan(yPhaseCentreFromVerticalHorn[ant][pol]/xPhaseCentreFromVerticalHorn[ant][pol]); //radians
    if(xPhaseCentreFromVerticalHorn[ant][pol]<0)
      azPhaseCentreFromVerticalHorn[ant][pol]+=TMath::Pi();
    if(azPhaseCentreFromVerticalHorn[ant][pol]<0)
      azPhaseCentreFromVerticalHorn[ant][pol]+=TMath::TwoPi();

  }
}


  for(Int_t pol=AnitaPol::kHorizontal;pol<=AnitaPol::kVertical;pol++) {
     for(Int_t ant=32;ant<48;ant++) {
    
    Double_t   deltaXRL = 0;
    Double_t   deltaYRL = 0;
    
    Double_t  deltaZUD = 0;
    Double_t  deltaXUD = 0;
    Double_t  deltaYUD = 0;
    
    deltaZUD = deltaUD*TMath::Cos(apertureElFromVerticalHorn[ant]);
    deltaXUD = -deltaUD*TMath::Sin(apertureElFromVerticalHorn[ant])*TMath::Cos(apertureAzFromVerticalHorn[ant]);
    deltaYUD = -deltaUD*TMath::Sin(apertureElFromVerticalHorn[ant])*TMath::Sin(apertureAzFromVerticalHorn[ant]);
    
    //    rAntFromVerticalHorn[ant]=(101*INCHTOMETER)-0.38;
    rAntFromVerticalHorn[ant]=(101*INCHTOMETER)-0.3;
    zAntFromVerticalHorn[ant]=(-21*INCHTOMETER)-1.8;
    azCentreFromVerticalHorn[ant]=(-67.5 + 45*(ant-32))*TMath::DegToRad();
    apertureAzFromVerticalHorn[ant]=azCentreFromVerticalHorn[ant];
    apertureElFromVerticalHorn[ant]=-10*TMath::DegToRad();
    xAntFromVerticalHorn[ant]=rAntFromVerticalHorn[ant]*TMath::Cos(azCentreFromVerticalHorn[ant]);
    yAntFromVerticalHorn[ant]=rAntFromVerticalHorn[ant]*TMath::Sin(azCentreFromVerticalHorn[ant]);
    
      
    xPhaseCentreFromVerticalHorn[ant][pol]=(xAntFromVerticalHorn[ant] + deltaXRL + deltaXUD) -
      phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    yPhaseCentreFromVerticalHorn[ant][pol]=(yAntFromVerticalHorn[ant] + deltaYRL + deltaYUD) -
      phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    zPhaseCentreFromVerticalHorn[ant][pol]=(zAntFromVerticalHorn[ant] + deltaZUD) -phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m

    //      //Now try to find the location of the antenna phaseCentre poInt_t     
    //      xPhaseCentreFromVerticalHorn[ant]=xAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      yPhaseCentreFromVerticalHorn[ant]=yAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      zPhaseCentreFromVerticalHorn[ant]=zAntFromVerticalHorn[ant]-phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m
    rPhaseCentreFromVerticalHorn[ant][pol]=TMath::Sqrt(xPhaseCentreFromVerticalHorn[ant][pol]*xPhaseCentreFromVerticalHorn[ant][pol]+
						  yPhaseCentreFromVerticalHorn[ant][pol]*yPhaseCentreFromVerticalHorn[ant][pol]);//m
    azPhaseCentreFromVerticalHorn[ant][pol]=TMath::ATan(yPhaseCentreFromVerticalHorn[ant][pol]/xPhaseCentreFromVerticalHorn[ant][pol]); //radians
    if(xPhaseCentreFromVerticalHorn[ant][pol]<0)
      azPhaseCentreFromVerticalHorn[ant][pol]+=TMath::Pi();
    if(azPhaseCentreFromVerticalHorn[ant][pol]<0)
      azPhaseCentreFromVerticalHorn[ant][pol]+=TMath::TwoPi();

 //    std::cout << "Simon's:\t" << ant << " "  
// 	      << rPhaseCentreFromVerticalHorn[ant][pol] << " " 
// 	      << azPhaseCentreFromVerticalHorn[ant][pol] << " "
// 	      << zPhaseCentreFromVerticalHorn[ant][pol] << "\n";
  }
}



};



void AnitaGeomTool::printAntPos(){

   for(Int_t pol=AnitaPol::kHorizontal;pol<=AnitaPol::kVertical;pol++) {
      for(Int_t ant = 0; ant < 32; ant++){
	 std::cout << rPhaseCentreFromVerticalHorn[ant][pol] << std::endl;
      }     
   }
}



Int_t AnitaGeomTool::getPhiFromAnt(Int_t ant)
{
  if(ant<16)
    return AnitaGeom::topPhiNums[ant];
  else if(ant<32)
    return AnitaGeom::middlePhiNums[ant-16];
  else if(ant<48)
    return AnitaGeom::bottomPhiNums[ant-32];
  std::cerr << "There isn't an antenna " << ant << " (0-47 only)\n";
  return -1;

}


Int_t AnitaGeomTool::getAntFromPhiRing(Int_t phi, AnitaRing::AnitaRing_t ring)
{
  switch(ring) {
  case AnitaRing::kTopRing:
    return AnitaGeom::topAntNums[phi];
  case AnitaRing::kMiddleRing:
    return AnitaGeom::middleAntNums[phi];
  case AnitaRing::kBottomRing:
    return AnitaGeom::bottomAntNums[phi];
  default:
    return -1;
  }

}

void AnitaGeomTool::readSimonsNumbers() {

  char calibDir[FILENAME_MAX];
  char fileName[FILENAME_MAX];
  char *calibEnv=getenv("ANITA_CALIB_DIR");
  if(!calibEnv) {
    char *utilEnv=getenv("ANITA_UTIL_INSTALL_DIR");
    if(!utilEnv)
      sprintf(calibDir,"calib");
    else
      sprintf(calibDir,"%s/share/anitaCalib",utilEnv);    
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }

  sprintf(fileName,"%s/simonsPositionAndTimingOffsets.dat",calibDir);
  std::ifstream SimonFile(fileName);
  if(!SimonFile) {
    std::cerr << "Couldn't open:\t" << fileName << "\n";
    return;
  }
  
  Int_t antNum;
  Double_t deltaT,deltaR,deltaPhi,deltaZ;
  char firstLine[180];
  
  SimonFile.getline(firstLine,179);
  while(SimonFile >> antNum >> deltaT >> deltaR >> deltaPhi >> deltaZ) {
    deltaRPhaseCentre[antNum][AnitaPol::kVertical]=deltaR;
    deltaPhiPhaseCentre[antNum][AnitaPol::kVertical]=deltaPhi;
    deltaZPhaseCentre[antNum][AnitaPol::kVertical]=deltaZ;
  }


  //The H-pol file is (for now at least) just a delta file relative to the 
  // V-pol
  sprintf(fileName,"%s/simonsHPolPositionAndTimingOffsets.dat",calibDir);
  std::ifstream SimonHPolFile(fileName);
  if(!SimonHPolFile) {
    std::cerr << "Couldn't open:\t" << fileName << "\n";
    return;
  }
  
  SimonHPolFile.getline(firstLine,179);
  while(SimonHPolFile >> antNum >> deltaT >> deltaR >> deltaPhi >> deltaZ) {
    deltaRPhaseCentre[antNum][AnitaPol::kHorizontal]=
       deltaRPhaseCentre[antNum][AnitaPol::kVertical]+deltaR;
    deltaPhiPhaseCentre[antNum][AnitaPol::kHorizontal]=
       deltaPhiPhaseCentre[antNum][AnitaPol::kVertical]+deltaPhi;
    deltaZPhaseCentre[antNum][AnitaPol::kHorizontal]=
       deltaZPhaseCentre[antNum][AnitaPol::kVertical]+deltaZ;
  }
 

}



void AnitaGeomTool::readPhaseCenterNumbers(int version) {

  char calibDir[FILENAME_MAX];
  char fileName[FILENAME_MAX];
  char *calibEnv=getenv("ANITA_CALIB_DIR");
  if(!calibEnv) {
    char *utilEnv=getenv("ANITA_UTIL_INSTALL_DIR");
    if(!utilEnv)
      sprintf(calibDir,"calib");
    else
      sprintf(calibDir,"%s/share/anitaCalib",utilEnv);    
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }

  switch(version) {
  case 3 : sprintf(fileName,"%s/phaseCenterPositionsRelativeToPhotogrammetryAnita3.dat",calibDir);
    break;       // and exits the switch
  case 4 : sprintf(fileName,"%s/phaseCenterPositionsRelativeToPhotogrammetryAnita4.dat",calibDir);
    break;
  default :
    std::cerr << "Unknown AnitaVersion " << version << "\n";
    return;
  }
  
  
  std::ifstream PhaseCenterFile(fileName);
  if(!PhaseCenterFile) {
    std::cerr << "Couldn't open:\t" << fileName << "\n";
    return;
  }
  
  Int_t antNum, pol;
  Double_t deltaR,deltaPhi,deltaZ;
  char firstLine[180];
  
  PhaseCenterFile.getline(firstLine,179);
  while(PhaseCenterFile >> antNum >> pol >> deltaR >> deltaPhi >> deltaZ) {
    deltaRPhaseCentre[antNum][pol]=deltaR;
    deltaPhiPhaseCentre[antNum][pol]=deltaPhi*TMath::DegToRad();
    deltaZPhaseCentre[antNum][pol]=deltaZ;
  } 

}








void AnitaGeomTool::readPhotogrammetry(int version)
{
  //  return; //Hack for now
  char calibDir[FILENAME_MAX];
  char fileName[FILENAME_MAX];
  char *calibEnv=getenv("ANITA_CALIB_DIR");
  if(!calibEnv) {
    char *utilEnv=getenv("ANITA_UTIL_INSTALL_DIR");
    if(!utilEnv)
      sprintf(calibDir,"calib");
    else
      sprintf(calibDir,"%s/share/anitaCalib",utilEnv);    
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }


  switch(version) {
  case 3 : sprintf(fileName,"%s/anitaIIIPhotogrammetry.csv",calibDir);
    break;       // and exits the switch
  case 4 : sprintf(fileName,"%s/anitaIVPhotogrammetry.csv",calibDir);
    break;
  default :
    std::cerr << "Unknown AnitaVersion " << version << "\n";
    return;
  }

  std::ifstream AnitaPhotoFile(fileName);
  if(!AnitaPhotoFile) {
    std::cerr << "Couldn't open:\t" << fileName << "\n";
    return;
  }
  
  
  //First up are the antenna positions
  TString line;
  for(int i=0;i<2;i++) {
    line.ReadLine(AnitaPhotoFile);
    //std::cout << line.Data() << "\n";
  }
  
  for(int ant=0;ant<48;ant++) {
    line.ReadLine(AnitaPhotoFile);
    //std::cout << "Seavey:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<8;j++) {
      const TString subString = ((TObjString*)tokens->At(j))->GetString();
      //	TString *subString = (TString*) tokens->At(j);
      //	std::cout << j << "\t" << subString.Data() << "\n";
      switch(j) {
      case 0:
	if (ant+1 != subString.Atoi()) {
	  std::cerr << "Antenna number mismatch\n";
	}
	break;
      case 1:	   
	xAntFromVerticalHornPhotogrammetry[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yAntFromVerticalHornPhotogrammetry[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zAntFromVerticalHornPhotogrammetry[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 4:	   
	rAntFromVerticalHornPhotogrammetry[ant]=subString.Atof()*INCHTOMETER; //m
	//	   std::cout << ant << "\t" << rAntFromVerticalHornKurtAnitaII[ant] << "\n";
	break;
      case 5:	   
	azCentreFromVerticalHornPhotogrammetry[ant]=subString.Atof()*TMath::DegToRad(); //radians
	break;
      case 6:	   
	apertureAzFromVerticalHornPhotogrammetry[ant]=subString.Atof()*TMath::DegToRad(); //radians
	break;
      case 7:	   
	apertureElFromVerticalHornPhotogrammetry[ant]=subString.Atof()*TMath::DegToRad(); //radians
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();

  }

  for(int pol=AnitaPol::kHorizontal;pol<=AnitaPol::kVertical;pol++) {
     for(int ant=0;ant<NUM_SEAVEYS;ant++) {
    Double_t phaseCentreToAntFront=ringPhaseCentreOffset[Int_t(this->getRingFromAnt(ant))];
    //Need to think about this at some point, but for now will just hard code in 20cm
    phaseCentreToAntFront=0.2;
    
    
    //     Now try to find the location of the antenna phaseCentre point  
    xPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]=xAntFromVerticalHornPhotogrammetry[ant] -
      phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHornPhotogrammetry[ant])*TMath::Cos(apertureElFromVerticalHornPhotogrammetry[ant]); //m
    yPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]=yAntFromVerticalHornPhotogrammetry[ant] -
      phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHornPhotogrammetry[ant])*TMath::Cos(apertureElFromVerticalHornPhotogrammetry[ant]); //m
    zPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]=zAntFromVerticalHornPhotogrammetry[ant] - phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHornPhotogrammetry[ant]); //m
    
    rPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]=TMath::Sqrt(xPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]*xPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]+yPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]*yPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]);//m
    azPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]=TMath::ATan(yPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]/xPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]); //radians
    if(xPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]<0)
      azPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]+=TMath::Pi();
    if(azPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]<0)
      azPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]+=TMath::TwoPi();
    
    
     }
  }
  
  aftForeOffsetAngleVerticalPhotogrammetry= +45*TMath::DegToRad();
  aftForeOffsetAngleVertical=aftForeOffsetAngleVerticalPhotogrammetry;
  //Will now make the default numbers the ones that start with Simon's and modify the positions

  //Removed corrections for the moment
  for(int pol=AnitaPol::kHorizontal;pol<=AnitaPol::kVertical;pol++) {
     for(int ant=0;ant<NUM_SEAVEYS;ant++) {
	rPhaseCentreFromVerticalHorn[ant][pol]=rPhaseCentreFromVerticalHornPhotogrammetry[ant][pol];
	azPhaseCentreFromVerticalHorn[ant][pol]=azPhaseCentreFromVerticalHornPhotogrammetry[ant][pol];    
	if(azPhaseCentreFromVerticalHorn[ant][pol]<0)
	   azPhaseCentreFromVerticalHorn[ant][pol]+=TMath::TwoPi();
	if(azPhaseCentreFromVerticalHorn[ant][pol]>TMath::TwoPi())
	   azPhaseCentreFromVerticalHorn[ant][pol]-=TMath::TwoPi();    
	zPhaseCentreFromVerticalHorn[ant][pol]=zPhaseCentreFromVerticalHornPhotogrammetry[ant][pol];
	xPhaseCentreFromVerticalHorn[ant][pol]=rPhaseCentreFromVerticalHorn[ant][pol]*TMath::Cos(azPhaseCentreFromVerticalHorn[ant][pol]);
	yPhaseCentreFromVerticalHorn[ant][pol]=rPhaseCentreFromVerticalHorn[ant][pol]*TMath::Sin(azPhaseCentreFromVerticalHorn[ant][pol]);
	//	std::cout << xPhaseCentreFromVerticalHorn[ant][pol] << std::endl;

     }
  }

  if (fUsePhotogrammetryNumbers==0){
    addPhaseCenters();
  }


}


void AnitaGeomTool::addPhaseCenters(){


  // Now add in ANITA-3 phase centre corrections
  // std::cerr << "Inside AnitaGeomTool we have..." << std::endl;
  // std::cerr << "pol\tant\tr\tz\tphi" << std::endl;  
  for(int pol=AnitaPol::kHorizontal;pol<=AnitaPol::kVertical;pol++) {
    for(int ant=0;ant<NUM_SEAVEYS;ant++) {
      rPhaseCentreFromVerticalHorn[ant][pol]=rPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]+deltaRPhaseCentre[ant][pol];
      azPhaseCentreFromVerticalHorn[ant][pol]=azPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]+deltaPhiPhaseCentre[ant][pol];    
      if(azPhaseCentreFromVerticalHorn[ant][pol]<0)
	azPhaseCentreFromVerticalHorn[ant][pol]+=TMath::TwoPi();
      if(azPhaseCentreFromVerticalHorn[ant][pol]>TMath::TwoPi())
	azPhaseCentreFromVerticalHorn[ant][pol]-=TMath::TwoPi();    
      zPhaseCentreFromVerticalHorn[ant][pol]=zPhaseCentreFromVerticalHornPhotogrammetry[ant][pol]+deltaZPhaseCentre[ant][pol];
      xPhaseCentreFromVerticalHorn[ant][pol]=rPhaseCentreFromVerticalHorn[ant][pol]*TMath::Cos(azPhaseCentreFromVerticalHorn[ant][pol]);
      yPhaseCentreFromVerticalHorn[ant][pol]=rPhaseCentreFromVerticalHorn[ant][pol]*TMath::Sin(azPhaseCentreFromVerticalHorn[ant][pol]);
      //	std::cout << xPhaseCentreFromVerticalHorn[ant][pol] << std::endl;


      // std::cerr << pol << "\t" << ant << "\t" << rPhaseCentreFromVerticalHorn[ant][pol] << "\t" << zPhaseCentreFromVerticalHorn[ant][pol] << "\t" << getAntPhiPositionRelToAftFore(ant, (AnitaPol::AnitaPol_t)pol)*TMath::RadToDeg() << std::endl;
      //      azPhaseCentreFromVerticalHorn[ant][pol]*TMath::RadToDeg() << std::endl;
      
    }
  }
  
}
