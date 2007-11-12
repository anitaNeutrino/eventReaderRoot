//////////////////////////////////////////////////////////////////////////////
/////  AnitaGeomTool.cxx        ANITA Geometry Tool                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for accessing ANITA Geometry Data                        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include "AnitaGeomTool.h"

namespace AnitaGeom {
   
   //Positive is horizontal
   //Negative is vetical
   int antToSurfMap[32]={1,3,5,7,1,3,5,7,0,2,4,6,0,2,4,6,
			 0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7};
   int hAntToChan[32]={4,4,4,4,5,5,5,5,4,4,4,4,5,5,5,5,
		       6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7};
   int vAntToChan[32]={0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,
		       2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3};
   
   // Note that this array uses antenna number 1-32 as it needs
   // the negative sign to indicate polarization
   int surfToAntMap[9][8]={{-9,-13,-17,-25,9,13,17,25},
			   {-1,-5,-18,-26,1,5,18,26},
			   {-10,-14,-19,-27,10,14,19,27},
			   {-2,-6,-20,-28,2,6,20,28},
			   {-11,-15,-21,-29,11,15,21,29},
			   {-3,-7,-22,-30,3,7,22,30},
			   {-12,-16,-23,-31,12,16,23,31},
			   {-4,-8,-24,-32,4,8,24,32},
			   {37,38,39,40,33,34,35,36}};
   
   int biconeIndexArray[4]={72,73,74,75}; //phi=2,6,10,14
   int disconeIndexArray[4]={77,78,79,76}; //phi=4,8,12,16
   
   //Map from phi to antenna both start counting at zero
   //int upperAntNums[NUM_PHI]={9,1,10,2,11,3,12,4,13,5,14,6,15,7,16,8};
   //int lowerAntNums[NUM_PHI]={17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};
   int upperAntNums[NUM_PHI]={8,0,9,1,10,2,11,3,12,4,13,5,14,6,15,7};
   int lowerAntNums[NUM_PHI]={16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
   
   //and the inverse (using ANT-1 and ANT-17 with the arrays)
   int upperPhiNums[NUM_PHI]={1,3,5,7,9,11,13,15,0,2,4,6,8,10,12,14};
   int lowerPhiNums[NUM_PHI]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
   
   //Discone and bicones
   int coneAntNums[NUM_PHI]={-1,4,-1,1,-1,5,-1,2,-1,6,-1,3,-1,7,-1,0};
   int conePhiNums[8]={15,3,7,11,1,5,9,13};


}

AnitaGeomTool*  AnitaGeomTool::fgInstance = 0;


AnitaGeomTool::AnitaGeomTool()
{
   //Default constructor
   //   std::cout << "AnitaGeomTool::AnitaGeomTool()" << std::endl;
   //   std::cout << "AnitaGeomTool::AnitaGeomTool() end" << std::endl;

}

AnitaGeomTool::~AnitaGeomTool()
{
   //Default destructor
}



//______________________________________________________________________________
AnitaGeomTool*  AnitaGeomTool::Instance()
{
   //static function
   return (fgInstance) ? (AnitaGeomTool*) fgInstance : new AnitaGeomTool();
}


int AnitaGeomTool::getChanIndexFromRingPhiPol(AnitaRing::AnitaRing_t ring,
					      int phi,
					      AnitaPol::AnitaPol_t pol)
{
   int ant;
   if(phi<0 || phi>15) return -1;
   switch(ring) {
   case AnitaRing::kUpperRing:
      ant=AnitaGeom::upperAntNums[phi];
      break;
   case AnitaRing::kLowerRing:
      ant=AnitaGeom::lowerAntNums[phi];
      break;
   default:
      return -1;
   }
   return getChanIndexFromAntPol(ant,pol);
}


int AnitaGeomTool::getChanIndexFromAntPol(int ant,
					  AnitaPol::AnitaPol_t pol)
{
   if(ant<0 || ant>31) return -1;
   int surf,chan;
   surf=AnitaGeom::antToSurfMap[ant];
   if(pol==AnitaPol::kHorizontal) 
      chan=AnitaGeom::hAntToChan[ant];
   else if(pol==AnitaPol::kVertical)
      chan=AnitaGeom::vAntToChan[ant];
   else
      return -1;

   return getChanIndex(surf,chan);

}


int AnitaGeomTool::getAzimuthPartner(int rx)
{

  if (rx<16) 
     return AnitaGeom::lowerAntNums[AnitaGeom::upperPhiNums[rx]];
  else
     return AnitaGeom::upperAntNums[AnitaGeom::lowerPhiNums[rx-16]]; 

  return -1;
}

void AnitaGeomTool::getThetaPartners(int rx,int& rxleft,int& rxright)
{
  if (rx<16) {
     int phi=AnitaGeom::upperPhiNums[rx];
     int phiLeft=phi-1;
     if(phiLeft<0) phiLeft=15;
     int phiRight=phi+1;
     if(phiRight>15) phiRight=0;
     rxleft=AnitaGeom::upperAntNums[phiLeft];
     rxright=AnitaGeom::upperAntNums[phiRight];
  }
  else {
     int phi=AnitaGeom::lowerPhiNums[rx-16];
     int phiLeft=phi-1;
     if(phiLeft<0) phiLeft=15;
     int phiRight=phi+1;
     if(phiRight>15) phiRight=0;
     rxleft=AnitaGeom::lowerAntNums[phiLeft];
     rxright=AnitaGeom::lowerAntNums[phiRight];
  }
}

int AnitaGeomTool::getPhiSector(int rx)
{
  if (rx<16)
    return AnitaGeom::upperPhiNums[rx];
  else
    return AnitaGeom::lowerPhiNums[rx-16];

  return 0;
}
float AnitaGeomTool::getDirectionwrtNorth(int phimax,float heading) {

    if(phimax<0 || phimax>=16) return -1.;

    //Copied this change from Amy don't (yet) know if it is correct
    float direction=(1.*360./16)+heading-(phimax*360./16);
    //    float direction=(2*360./16)+heading-(phimax*360./16);
//    float direction=heading+(phimax*360./16);
    if(direction>360) direction-=360;
    //std::cout << direction << endl;
    return direction;

}



int AnitaGeomTool::getSurfChanFromChanIndex(int chanIndex, // input channel index
				int &surf,int &chan) // output surf and channel
{
  if(chanIndex<0 || chanIndex>=NUM_DIGITZED_CHANNELS)
    return 0;

  //  std::cout << "chanIndex is " << chanIndex << "\n";
  chan=chanIndex%9; // 0 to 8
  surf=(chanIndex-chan)/9; // 0 to 8

  return 1;

}
int AnitaGeomTool::getAntPolFromSurfChan(int surf,int chan,int &ant, AnitaPol::AnitaPol_t &pol) 
{

  ant=AnitaGeom::surfToAntMap[surf][chan];

  //  std::cout << "surf, chan, ant are " << surf << " " << chan << "\n";

  if (ant>0)
    pol=AnitaPol::kHorizontal;
  else
    pol=AnitaPol::kVertical;

  ant=abs(ant)-1; // so that it's from 0 to 31

  return 1;
}
int AnitaGeomTool::getLayer(int irx) 
{
  if (irx<8)
    return 0;
  else if (irx>=8 && irx<16)
    return 1;
  else if (irx>=16)
    return 2;

  return 0;

}

int AnitaGeomTool::getPhiFromAnt(int ant)
{
  if(ant<16)
    return AnitaGeom::upperPhiNums[ant];
  return AnitaGeom::lowerPhiNums[ant];

}


int AnitaGeomTool::getAntFromPhiRing(int phi, AnitaRing::AnitaRing_t ring)
{
  switch(ring) {
  case AnitaRing::kUpperRing:
    return AnitaGeom::upperAntNums[phi];
   case AnitaRing::kLowerRing:
      return AnitaGeom::lowerAntNums[phi];
  default:
    return -1;
  }

}
