//////////////////////////////////////////////////////////////////////////////
/////  AnitaGeomTool.cxx        ANITA Geometry Tool                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     Class for accessing ANITA II Geometry Data                     /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <fstream>
#include <iostream>
#include <cstring>
#include "AnitaGeomTool.h"
#include "AnitaConventions.h"

#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TVector3.h"

#define INCHTOMETER 0.0254

double deltaRL = 0.0;
double deltaUD = 0.0;

namespace AnitaGeom {
   
  //Positive is horizontal
  //Negative is vetical
  int antToSurfMap[NUM_SEAVEYS]={5,7,6,1,5,7,6,1,0,2,3,4,0,2,3,4,
				 0,5,2,7,3,6,4,1,0,5,2,7,3,6,4,1,
				 8,9,8,9,8,9,8,9};
				  
  int hAntToChan[NUM_SEAVEYS]={4,4,4,4,5,5,5,5,4,4,4,4,5,5,5,1,
			       6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,
			       4,4,5,5,6,6,7,7};
  
  int vAntToChan[NUM_SEAVEYS]={0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,5,
			       2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,
			       0,0,1,1,2,2,3,3};
   
  ///< 1 is normal orientation, -1 is 180 degree flip, -2 is 90 degree flip (so only H-channels need flipping).
  int antOrientationMap[NUM_SEAVEYS]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
				      1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,1,-1,
				      -2,-2,-2,-2,-2,-2,-2,-2}; 


  // Note that this array uses antenna number 1-42 as it needs
  // the negative sign to indicate polarization
   // Note that this array uses antenna number 1-42 as it needs
   // the negative sign to indicate polarization (-ve is vertical)

  int surfToAntMap[ACTIVE_SURFS][RFCHAN_PER_SURF]=
    {{-9,-13,-17,-25,9,13,17,25},
     {-4,-8,-24,-32,4,8,24,32},
     {-10,-14,-19,-27,10,14,19,27},
     {-11,-15,-21,-29,11,15,21,29},
     {-12,+16,-23,-31,12,-16,23,31},
     {-1,-5,-18,-26,1,5,18,26},
     {-3,-7,-22,-30,3,7,22,30},
     {-2,-6,-20,-28,2,6,20,28},
     {-33,-35,-37,-39,33,35,37,39},
     {-34,-36,-38,-40,34,36,38,40}};
      
  //Map from phi to antenna both start counting at zero
  int upperAntNums[NUM_PHI]={8,0,9,1,10,2,11,3,12,4,13,5,14,6,15,7};
  int lowerAntNums[NUM_PHI]={16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
  int nadirAntNums[NUM_PHI]={32,-1,33,-1,34,-1,35,-1,36,-1,37,-1,38,-1,39,-1};
   
  //and the inverse (using ANT-1 and ANT-17 and ANT-32 with the arrays)
  int upperPhiNums[NUM_PHI]={1,3,5,7,9,11,13,15,0,2,4,6,8,10,12,14};
  int lowerPhiNums[NUM_PHI]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  int nadirPhiNums[NUM_NADIRS]={0,2,4,6,8,10,12,14};
   
}

AnitaGeomTool*  AnitaGeomTool::fgInstance = 0;


AnitaGeomTool::AnitaGeomTool()
{
  //Default constructor
  ringPhaseCentreOffset[0]=0.2-0.042685;
  ringPhaseCentreOffset[1]=0.2+0.00653;
  ringPhaseCentreOffset[2]=0.2+0.1927;

  readSimonsNumbers();
  readPhotogrammetry();
  readAnitaIIPhotogrammetry();
  fUseKurtAnitaIINumbers=0;

  std::cout << "AnitaGeomTool::AnitaGeomTool()" << std::endl;
  //   std::cout << "AnitaGeomTool::AnitaGeomTool() end" << std::endl;

  //std::cout << "AnitaGeomTool::AnitaGeomTool()" << std::endl;
  //   std::cout << "AnitaGeomTool::AnitaGeomTool() end" << std::endl;
  fgInstance=this;
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

void AnitaGeomTool::getCartesianCoords(Double_t lat, Double_t lon, Double_t alt, Double_t p[3])
{
  if(lat<0) lat*=-1;
   //Note that x and y are switched to conform with previous standards
   lat*=TMath::DegToRad();
   lon*=TMath::DegToRad();
   //calculate x,y,z coordinates
   double C2=pow(cos(lat)*cos(lat)+(1-FLATTENING_FACTOR)*(1-FLATTENING_FACTOR)*sin(lat)*sin(lat),-0.5);
   double Q2=(1-FLATTENING_FACTOR)*(1-FLATTENING_FACTOR)*C2;
   p[1]=(R_EARTH*C2+alt)*TMath::Cos(lat)*TMath::Cos(lon);
   p[0]=(R_EARTH*C2+alt)*TMath::Cos(lat)*TMath::Sin(lon);
   p[2]=(R_EARTH*Q2+alt)*TMath::Sin(lat);
}

void AnitaGeomTool::getLatLonAltFromCartesian(Double_t p[3], Double_t &lat, Double_t &lon, Double_t &alt)
{
  //Here again x and y are flipped for confusions sake
  Double_t xt=p[1];
  Double_t yt=p[0];
  Double_t zt=p[2]; //And flipped z for a test

  static Double_t cosaeSq=(1-FLATTENING_FACTOR)*(1-FLATTENING_FACTOR);
  Double_t lonVal=TMath::ATan2(yt,xt);
  Double_t xySq=TMath::Sqrt(xt*xt+yt*yt);
  Double_t tanPsit=zt/xySq;
  Double_t latGuess=TMath::ATan(tanPsit/cosaeSq);
  Double_t nextLat=latGuess;
  Double_t geomBot=R_EARTH*R_EARTH*xySq;
  do {
    latGuess=nextLat;
    Double_t N=R_EARTH/TMath::Sqrt(cos(latGuess)*cos(latGuess)+(1-FLATTENING_FACTOR)*(1-FLATTENING_FACTOR)*sin(latGuess)*sin(latGuess));
    Double_t top=(R_EARTH*R_EARTH*zt + (1-cosaeSq)*cosaeSq*TMath::Power(N*TMath::Sin(latGuess),3));
    Double_t bottom=geomBot-(1-cosaeSq)*TMath::Power(N*TMath::Cos(latGuess),3);        
    nextLat=TMath::ATan(top/bottom);
    //    std::cout << latGuess << "\t" << nextLat << "\n";
    
  } while(TMath::Abs(nextLat-latGuess)>0.0001);
  latGuess=nextLat;
  Double_t N=R_EARTH/TMath::Sqrt(cos(latGuess)*cos(latGuess)+(1-FLATTENING_FACTOR)*(1-FLATTENING_FACTOR)*sin(latGuess)*sin(latGuess));
  Double_t height=(xySq/TMath::Cos(nextLat))-N;
  
  lat=latGuess*TMath::RadToDeg();
  lon=lonVal*TMath::RadToDeg();
  alt=height;
  if(lat>0) lat*=-1;
 
}

Double_t AnitaGeomTool::getDistanceToCentreOfEarth(Double_t lat)
{
  Double_t pVec[3];
  this->getCartesianCoords(lat,0,0,pVec);
//   Double_t cosLat=TMath::Cos(lat);
//   Double_t sinLat=TMath::Sin(lat);
//   Double_t a=R_EARTH;
//   Double_t b=a-FLATTENING_FACTOR*a;
//   Double_t radSq=(a*a*cosLat)*(a*a*cosLat)+(b*b*sinLat)*(b*b*sinLat);
//   radSq/=(a*cosLat)*(a*cosLat)+(b*sinLat)*(b*sinLat);
 //  Double_t cosSqAe=(1-FLATTENING_FACTOR)*(1-FLATTENING_FACTOR);
//   Double_t N=R_EARTH/TMath::Sqrt(cosLat*cosLat+cosSqAe*sinLat*sinLat);
//   Double_t radSq=N*N*(cosLat*cosLat+cosSqAe*cosSqAe*sinLat*sinLat);
  return TMath::Sqrt(pVec[0]*pVec[0]+pVec[1]*pVec[1]+pVec[2]*pVec[2]);
}


// void AnitaGeomTool::getPhiWave(Double_t balloonLon, Double_t balloonLat, Double_t balloonAlt, Double_t balloonHeading, Double_t sourceLon, Double_t sourceLat, Double_t sourceAlt, Double_t &thetaWave, Double_t &phiWave)
// {
//   Double_t thetaBalloon=getThetaFromLat(TMath::Abs(balloonLat));
//   Double_t phiBalloon=getPhiFromLon(balloonLon);
//   Double_t balloonHeight=getGeoid(thetaBalloon)+balloonAlt;
   
//   Double_t thetaSource=getThetaFromLat(TMath::Abs(sourceLat));
//   Double_t phiSource=getPhiFromLon(sourceLon);
//   Double_t radiusSource=getGeoid(thetaSource)+sourceAlt;

//   //Get vector from Earth's centre to source
//   TVector3 fSourcePos;
//   fSourcePos.SetX(radiusSource*TMath::Sin(thetaSource)*TMath::Cos(phiSource));
//   fSourcePos.SetY(radiusSource*TMath::Sin(thetaSource)*TMath::Sin(phiSource));
//   fSourcePos.SetZ(radiusSource*TMath::Cos(thetaSource));
   
//   //Rotate such that balloon is at 0,0,balloonHeight
//   fSourcePos.RotateZ(-1*phiBalloon);
//   fSourcePos.RotateY(-1*thetaBalloon);

//   //Now find thetaWave and phiWave
//   thetaWave=TMath::ATan((balloonHeight-fSourcePos.Z())/TMath::Sqrt(fSourcePos.X()*fSourcePos.X() + fSourcePos.Y()*fSourcePos.Y()));
   
//   //phiWave is just atan(yp/xp) only looks confusing to make sure I get the sign and 0-360 convention
//   phiWave=0;
//   if(fSourcePos.X()==0) {
//     phiWave=TMath::PiOver2();
//     if(fSourcePos.Y()<0)
//       phiWave+=TMath::Pi();
//   }
//   else if(fSourcePos.X()<0) {
//     phiWave=TMath::Pi()+TMath::ATan(fSourcePos.Y()/fSourcePos.X());
//   }
//   else {
//     phiWave=TMath::ATan(fSourcePos.Y()/fSourcePos.X());
//     if(fSourcePos.Y()<0) {
//       phiWave+=TMath::TwoPi();
//     }
//   }   

//   //Now need to take account of balloon heading
//   //Will have to check heading at some point
//   if(balloonHeading>=0 && balloonHeading<=360) {
//     phiWave+=balloonHeading*TMath::DegToRad();
//     if(phiWave>TMath::TwoPi())
//       phiWave-=TMath::TwoPi();
//   }

// }

Double_t AnitaGeomTool::getPhiDiff(int firstPhi, int secondPhi)
{
   Double_t phiDiff=firstPhi-secondPhi;
   if(TMath::Abs(phiDiff)>TMath::Abs(firstPhi-(secondPhi+TMath::TwoPi())))
      phiDiff=firstPhi-(secondPhi+TMath::TwoPi());
   if(TMath::Abs(phiDiff)>TMath::Abs(firstPhi-(secondPhi-TMath::TwoPi())))
      phiDiff=firstPhi-(secondPhi-TMath::TwoPi());
   return phiDiff;
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
  case AnitaRing::kNadirRing:
    ant=AnitaGeom::nadirAntNums[phi];
    break;
  default:
    return -1;
  }
  return getChanIndexFromAntPol(ant,pol);
}


int AnitaGeomTool::getChanIndexFromAntPol(int ant,
					  AnitaPol::AnitaPol_t pol)
{
  if(ant<0 || ant>(NUM_SEAVEYS-1)) return -1;
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


int AnitaGeomTool::getSurfFromAnt(int ant)
{
  if(ant<0 || ant>(NUM_SEAVEYS-1)) return -1;
  int surf;
  surf=AnitaGeom::antToSurfMap[ant];
  return surf;
}


int AnitaGeomTool::getChanFromAntPol(int ant,AnitaPol::AnitaPol_t pol)
{
  int chan;
  if(pol==AnitaPol::kHorizontal) 
      chan=AnitaGeom::hAntToChan[ant];
   else if(pol==AnitaPol::kVertical)
      chan=AnitaGeom::vAntToChan[ant];
   else
      return -1;
  return chan;
}


int AnitaGeomTool::getAzimuthPartner(int rx)
{
  //Need to update for nadir ring
  
  if (rx<16) 
    return AnitaGeom::lowerAntNums[AnitaGeom::upperPhiNums[rx]];
  else if(rx<32)
    return AnitaGeom::upperAntNums[AnitaGeom::lowerPhiNums[rx-16]]; 
  else
    return AnitaGeom::lowerAntNums[AnitaGeom::nadirPhiNums[rx-32]]; 

  return -1;
}

void AnitaGeomTool::getNeighbors(int rx,int& rxleft,int& rxright)
{
  // input antenna number 0 to 31
  if (rx<0 || rx>(NUM_SEAVEYS-1))    
    std::cout << "Antenna number " << rx << " out of range!\n";  
  if (rx<8)    
    rxleft=rx+8;
  else if (rx==8)
    rxleft=7; 
  else if (rx<16)    
    rxleft=rx-9;  
  else if (rx==16)    
    rxleft=31;  
  else    
    rxleft=rx-1;  
 
  if (rx<7)    
    rxright=rx+9;  
  else if (rx==7)    
    rxright=8;  
  else if (rx<16)   
    rxright=rx-8;  
  else if (rx<31)   
    rxright=rx+1;  
  else if (rx==31)    
    rxright=16;  

  if (rx<39 && rx>31)    
    rxright=rx+1;  
  else if (rx==39)   
    rxright=32;

    if (rx<39 && rx>31)    
    rxleft=rx-1;  
  else if (rx==31)   
    rxleft=39;

 
  //  std::cout << "rx, rxleft, rxright " << rx << " " << rxleft << " " << rxright << "\n"; 

}

void AnitaGeomTool::getThetaPartners(int rx,int& rxleft,int& rxright)
{
  if (rx<0 || rx>(NUM_SEAVEYS-1))    
    std::cerr << "Antenna number out of range!\n";     
  if (rx<16) {
    int phi=AnitaGeom::upperPhiNums[rx];
    int phiLeft=phi-1;
    if(phiLeft<0) phiLeft=15;
    int phiRight=phi+1;
    if(phiRight>15) phiRight=0;
    rxleft=AnitaGeom::upperAntNums[phiLeft];
    rxright=AnitaGeom::upperAntNums[phiRight];
  }
  else if (rx<32){
    int phi=AnitaGeom::lowerPhiNums[rx-16];
    int phiLeft=phi-1;
    if(phiLeft<0) phiLeft=15;
    int phiRight=phi+1;
    if(phiRight>15) phiRight=0;
    rxleft=AnitaGeom::lowerAntNums[phiLeft];
    rxright=AnitaGeom::lowerAntNums[phiRight];
  }
  else{

if (rx<39 && rx>31)    
    rxright=rx+1;  
  else if (rx==39)   
    rxright=32;

    if (rx<39 && rx>31)    
    rxleft=rx-1;  
  else if (rx==31)   
    rxleft=39;

  }

}

int AnitaGeomTool::getPhiSector(int rx)
{
  if (rx<16)
    return AnitaGeom::upperPhiNums[rx];
  else if(rx<32)
    return AnitaGeom::lowerPhiNums[rx-16];
  else if(rx<40) {
    return AnitaGeom::nadirPhiNums[rx-32];
  }

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
  chan=chanIndex%CHANNELS_PER_SURF; // 0 to 8
  surf=(chanIndex-chan)/CHANNELS_PER_SURF; // 0 to 8

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

int AnitaGeomTool::getAntOrientation(int ant) {
  if(ant<0 || ant>=NUM_SEAVEYS)
    return 0;
  return AnitaGeom::antOrientationMap[ant];
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

AnitaRing::AnitaRing_t AnitaGeomTool::getRingFromAnt(int ant) {
  if (ant<8)
    return AnitaRing::kUpperRing;
  else if (ant>=8 && ant<16)
    return AnitaRing::kUpperRing;
  else if (ant<32)
    return AnitaRing::kLowerRing;
  else if (ant<40)
    return AnitaRing::kNadirRing;
  return AnitaRing::kNotARing;

}

void AnitaGeomTool::getRingAntPolPhiFromSurfChan(int surf, int chan,
						 AnitaRing::AnitaRing_t &ring,
						 int &ant,
						 AnitaPol::AnitaPol_t &pol,
						 int &phi)
{
  AnitaGeomTool::getAntPolFromSurfChan(surf,chan,ant,pol);
  ring=AnitaGeomTool::getRingFromAnt(ant);
  phi=AnitaGeomTool::getPhiFromAnt(ant);
}

void AnitaGeomTool::getSurfChanAntFromRingPhiPol(AnitaRing::AnitaRing_t ring,
						 int phi,
						 AnitaPol::AnitaPol_t pol,
						 int &surf, int &chan, int &ant)
{
  int chanIndex=AnitaGeomTool::getChanIndexFromRingPhiPol(ring,phi,pol);
  AnitaGeomTool::getSurfChanFromChanIndex(chanIndex,surf,chan);
  ant=AnitaGeomTool::getAntFromPhiRing(phi,ring);
}


void AnitaGeomTool::readPhotogrammetry()
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
  for(int i=0;i<4;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  
  for(int ant=0;ant<32;ant++) {
    line.ReadLine(AntennaFile);
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
  for(int ant=32;ant<40;ant++) {
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

  for(int ant=0;ant<NUM_SEAVEYS;ant++) {
    Double_t phaseCentreToAntFront=ringPhaseCentreOffset[Int_t(this->getRingFromAnt(ant))];


    double   deltaXRL = -deltaRL*TMath::Sin(apertureAzFromDeckHorn[ant]);
    double   deltaYRL = deltaRL*TMath::Cos(apertureAzFromDeckHorn[ant]);

    double  deltaZUD = deltaUD*TMath::Cos(apertureElFromDeckHorn[ant]);
    double  deltaXUD = deltaUD*TMath::Sin(apertureElFromDeckHorn[ant])*TMath::Cos(apertureAzFromDeckHorn[ant]);
    double  deltaYUD = deltaUD*TMath::Sin(apertureElFromDeckHorn[ant])*TMath::Sin(apertureAzFromDeckHorn[ant]);

    deltaXRL = 0;
    deltaYRL = 0;

    deltaZUD = 0;
    deltaXUD = 0;
    deltaYUD = 0;

    // std::cout << deltaXRL << "  " << deltaXUD << "  "<< deltaYUD << "  "<< deltaYRL  <<  "  " <<  deltaZUD << std::endl;

    //     Now try to find the location of the antenna phaseCentre point     
    xPhaseCentreFromDeckHorn[ant]=(xAntFromDeckHorn[ant] + deltaXRL + deltaXUD) -
      phaseCentreToAntFront*TMath::Cos( apertureAzFromDeckHorn[ant])*TMath::Cos(apertureElFromDeckHorn[ant]); //m
    yPhaseCentreFromDeckHorn[ant]=(yAntFromDeckHorn[ant] + deltaYRL + deltaYUD) -
      phaseCentreToAntFront*TMath::Sin( apertureAzFromDeckHorn[ant])*TMath::Cos(apertureElFromDeckHorn[ant]); //m
    zPhaseCentreFromDeckHorn[ant]=(zAntFromDeckHorn[ant] + deltaZUD) -phaseCentreToAntFront*TMath::Sin(apertureElFromDeckHorn[ant]); //m

    //  xPhaseCentreFromDeckHorn[ant]=xAntFromDeckHorn[ant] -
    //        phaseCentreToAntFront*TMath::Cos( apertureAzFromDeckHorn[ant])*TMath::Cos(apertureElFromDeckHorn[ant]); //m
    //      yPhaseCentreFromDeckHorn[ant]=yAntFromDeckHorn[ant] -
    //        phaseCentreToAntFront*TMath::Sin( apertureAzFromDeckHorn[ant])*TMath::Cos(apertureElFromDeckHorn[ant]); //m
    //      zPhaseCentreFromDeckHorn[ant]=zAntFromDeckHorn[ant] - phaseCentreToAntFront*TMath::Sin(apertureElFromDeckHorn[ant]); //m

    rPhaseCentreFromDeckHorn[ant]=TMath::Sqrt(xPhaseCentreFromDeckHorn[ant]*xPhaseCentreFromDeckHorn[ant]+
					      yPhaseCentreFromDeckHorn[ant]*yPhaseCentreFromDeckHorn[ant]);//m
    azPhaseCentreFromDeckHorn[ant]=TMath::ATan(yPhaseCentreFromDeckHorn[ant]/xPhaseCentreFromDeckHorn[ant]); //radians
    if(xPhaseCentreFromDeckHorn[ant]<0)
      azPhaseCentreFromDeckHorn[ant]+=TMath::Pi();
    if(azPhaseCentreFromDeckHorn[ant]<0)
      azPhaseCentreFromDeckHorn[ant]+=TMath::TwoPi();

    //      std::cout << "Face v PhaseCentre -- Ant " << ant  << std::endl;
    //      std::cout << apertureAzFromDeckHorn[ant] << "\t" << apertureElFromDeckHorn[ant] << "\n";
    //      std::cout << "x\t" << xAntFromDeckHorn[ant] << "\t" << xPhaseCentreFromDeckHorn[ant] << std::endl;
    //      std::cout << "y\t" << yAntFromDeckHorn[ant] << "\t" << yPhaseCentreFromDeckHorn[ant] << std::endl;
    //      std::cout << "z\t" << zAntFromDeckHorn[ant] << "\t" << zPhaseCentreFromDeckHorn[ant] << std::endl;
    //      std::cout << "r\t" << rAntFromDeckHorn[ant] << "\t" << rPhaseCentreFromDeckHorn[ant] << std::endl;
    //      std::cout << "phi\t" << azCentreFromDeckHorn[ant] << "\t" << azPhaseCentreFromDeckHorn[ant] << std::endl;
     
  }

  //Now bicones and discones  
  for(int i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }


  for(int ant=0;ant<4;ant++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Bicones:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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

  for(int ant=0;ant<4;ant++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Discones:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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
  for(int i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(int corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Anita Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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
  for(int i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(int corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Battery Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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
  for(int i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(int corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Sip Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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
  for(int i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  {
    line.ReadLine(AntennaFile);
    //std::cout << "GPS Plane:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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
    for(int j=0;j<4;j++) {
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
  for(int i=0;i<5;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  
  for(int ant=0;ant<32;ant++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Seavey:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<8;j++) {
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

    Double_t phaseCentreToAntFront=ringPhaseCentreOffset[Int_t(this->getRingFromAnt(ant))];

    double   deltaXRL = -deltaRL*TMath::Sin(apertureAzFromVerticalHorn[ant]);
    double   deltaYRL = deltaRL*TMath::Cos(apertureAzFromVerticalHorn[ant]);

    double  deltaZUD = deltaUD*TMath::Cos(apertureElFromVerticalHorn[ant]);
    double  deltaXUD = -deltaUD*TMath::Sin(apertureElFromVerticalHorn[ant])*TMath::Cos(apertureAzFromVerticalHorn[ant]);
    double  deltaYUD = -deltaUD*TMath::Sin(apertureElFromVerticalHorn[ant])*TMath::Sin(apertureAzFromVerticalHorn[ant]);

    //std::cout << deltaXRL << "  " << deltaXUD << "  "<< deltaYUD << "  "<< deltaYRL  <<  "  " <<  deltaZUD << std::endl;

    //Now try to find the location of the antenna phaseCentre point     
    xPhaseCentreFromVerticalHorn[ant]=(xAntFromVerticalHorn[ant] + deltaXRL + deltaXUD) -
      phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    yPhaseCentreFromVerticalHorn[ant]=(yAntFromVerticalHorn[ant] + deltaYRL + deltaYUD) -
      phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    zPhaseCentreFromVerticalHorn[ant]=(zAntFromVerticalHorn[ant] + deltaZUD) -phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m

    //      //Now try to find the location of the antenna phaseCentre point     
    //      xPhaseCentreFromVerticalHorn[ant]=xAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      yPhaseCentreFromVerticalHorn[ant]=yAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      zPhaseCentreFromVerticalHorn[ant]=zAntFromVerticalHorn[ant]-phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m
    rPhaseCentreFromVerticalHorn[ant]=TMath::Sqrt(xPhaseCentreFromVerticalHorn[ant]*xPhaseCentreFromVerticalHorn[ant]+
						  yPhaseCentreFromVerticalHorn[ant]*yPhaseCentreFromVerticalHorn[ant]);//m
    azPhaseCentreFromVerticalHorn[ant]=TMath::ATan(yPhaseCentreFromVerticalHorn[ant]/xPhaseCentreFromVerticalHorn[ant]); //radians
    if(xPhaseCentreFromVerticalHorn[ant]<0)
      azPhaseCentreFromVerticalHorn[ant]+=TMath::Pi();
    if(azPhaseCentreFromVerticalHorn[ant]<0)
      azPhaseCentreFromVerticalHorn[ant]+=TMath::TwoPi();

       
    //      std::cout << "Face v PhaseCentre -- Ant " << ant  << std::endl;
    //      std::cout << "x\t" << xAntFromVerticalHorn[ant] << "\t" << xPhaseCentreFromVerticalHorn[ant] << std::endl;
    //      std::cout << "y\t" << yAntFromVerticalHorn[ant] << "\t" << yPhaseCentreFromVerticalHorn[ant] << std::endl;
    //      std::cout << "z\t" << zAntFromVerticalHorn[ant] << "\t" << zPhaseCentreFromVerticalHorn[ant] << std::endl;
    //      std::cout << "r\t" << rAntFromVerticalHorn[ant] << "\t" << rPhaseCentreFromVerticalHorn[ant] << std::endl;
    //      std::cout << "phi\t" << azCentreFromVerticalHorn[ant] << "\t" << azPhaseCentreFromVerticalHorn[ant] << std::endl;

  }



  //Now we'll add a hack for the drop-down antennas
  for(int ant=32;ant<NUM_SEAVEYS;ant++) {
    Double_t phaseCentreToAntFront=ringPhaseCentreOffset[Int_t(this->getRingFromAnt(ant))];
    //    std::cout << ant << "\t" << phaseCentreToAntFront << "\n";
    double   deltaXRL = 0;
    double   deltaYRL = 0;

    double  deltaZUD = 0;
    double  deltaXUD = 0;
    double  deltaYUD = 0;

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


    xPhaseCentreFromVerticalHorn[ant]=(xAntFromVerticalHorn[ant] + deltaXRL + deltaXUD) -
      phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    yPhaseCentreFromVerticalHorn[ant]=(yAntFromVerticalHorn[ant] + deltaYRL + deltaYUD) -
      phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    zPhaseCentreFromVerticalHorn[ant]=(zAntFromVerticalHorn[ant] + deltaZUD) -phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m

    //      //Now try to find the location of the antenna phaseCentre point     
    //      xPhaseCentreFromVerticalHorn[ant]=xAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      yPhaseCentreFromVerticalHorn[ant]=yAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      zPhaseCentreFromVerticalHorn[ant]=zAntFromVerticalHorn[ant]-phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m
    rPhaseCentreFromVerticalHorn[ant]=TMath::Sqrt(xPhaseCentreFromVerticalHorn[ant]*xPhaseCentreFromVerticalHorn[ant]+
						  yPhaseCentreFromVerticalHorn[ant]*yPhaseCentreFromVerticalHorn[ant]);//m
    azPhaseCentreFromVerticalHorn[ant]=TMath::ATan(yPhaseCentreFromVerticalHorn[ant]/xPhaseCentreFromVerticalHorn[ant]); //radians
    if(xPhaseCentreFromVerticalHorn[ant]<0)
      azPhaseCentreFromVerticalHorn[ant]+=TMath::Pi();
    if(azPhaseCentreFromVerticalHorn[ant]<0)
      azPhaseCentreFromVerticalHorn[ant]+=TMath::TwoPi();

    
  //   std::cout << "Ryan's:\t" << ant << " "  
// 	      << rPhaseCentreFromVerticalHorn[ant] << " " 
// 	      << azPhaseCentreFromVerticalHorn[ant] << " "
// 	      << zPhaseCentreFromVerticalHorn[ant] << "\n";

  }


  //Now bicones and discones  
  for(int i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }


  for(int ant=0;ant<4;ant++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Bicones:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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

  for(int ant=0;ant<4;ant++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Discones:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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
  for(int i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(int corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Anita Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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
  for(int i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(int corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Battery Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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
  for(int i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  for(int corner=0;corner<4;corner++) {
    line.ReadLine(AntennaFile);
    //std::cout << "Sip Box:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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
  for(int i=0;i<2;i++) {
    line.ReadLine(AntennaFile);
    //std::cout << line.Data() << "\n";
  }
  {
    line.ReadLine(AntennaFile);
    //std::cout << "GPS Plane:\t" << line.Data() << "\n";
    TObjArray *tokens = line.Tokenize(",");
    for(int j=0;j<4;j++) {
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
    for(int j=0;j<4;j++) {
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
  //fPitchRotationAxis.SetXYZ(0,cos(-0.176*TMath::DegToRad()),sin(-0.176*TMath::DegToRad()));
  //fRollRotationAxis.SetXYZ(cos(0.460*TMath::DegToRad()),0,sin(0.460*TMath::DegToRad()));
  //fHeadingRotationAxis=fRollRotationAxis.Cross(fPitchRotationAxis);


  //Now add in Simon's corrections
  for(int ant=0;ant<NUM_SEAVEYS;ant++) {
    rPhaseCentreFromVerticalHorn[ant]+=deltaRPhaseCentre[ant];
    //    std::cout << rPhaseCentreFromVerticalHorn[ant] << "\t" << deltaRPhaseCentre[ant] << "\n";
    azPhaseCentreFromVerticalHorn[ant]+=deltaPhiPhaseCentre[ant];    
    if(azPhaseCentreFromVerticalHorn[ant]<0)
       azPhaseCentreFromVerticalHorn[ant]+=TMath::TwoPi();
    if(azPhaseCentreFromVerticalHorn[ant]>TMath::TwoPi())
       azPhaseCentreFromVerticalHorn[ant]-=TMath::TwoPi();    
     zPhaseCentreFromVerticalHorn[ant]+=deltaZPhaseCentre[ant];
     xPhaseCentreFromVerticalHorn[ant]=rPhaseCentreFromVerticalHorn[ant]*TMath::Cos(azPhaseCentreFromVerticalHorn[ant]);
     yPhaseCentreFromVerticalHorn[ant]=rPhaseCentreFromVerticalHorn[ant]*TMath::Sin(azPhaseCentreFromVerticalHorn[ant]);
   }

   std::cout << " heading axis x " << fHeadingRotationAxis.x() << " y " << fHeadingRotationAxis.y() << " z " << fHeadingRotationAxis.z() << std::endl;
   std::cout << " roll axis x " << fRollRotationAxis.x() << " y " << fRollRotationAxis.y() << " z " << fRollRotationAxis.z() << std::endl;
   std::cout << " pitch axis x " << fPitchRotationAxis.x() << " y " << fPitchRotationAxis.y() << " z " << fPitchRotationAxis.z() << std::endl;


}


//Non static thingies
void AnitaGeomTool::getAntXYZ(int ant, Double_t &x, Double_t &y, Double_t &z)
{
  if(ant>=0 && ant<NUM_SEAVEYS) {
    x=xPhaseCentreFromVerticalHorn[ant];
    y=yPhaseCentreFromVerticalHorn[ant];
    z=zPhaseCentreFromVerticalHorn[ant];
  }
}

Double_t AnitaGeomTool::getAntZ(int ant) {
  if(fUseKurtAnitaIINumbers==0) {
    if(ant>=0 && ant<NUM_SEAVEYS) {
      //      std::cout << "Using simon z\n";
      return zPhaseCentreFromVerticalHorn[ant];
    }
  }
  else {
    if(ant>=0 && ant<NUM_SEAVEYS) {
      //      std::cout << "Using kurt z\n";
      return zPhaseCentreFromVerticalHornKurtAnitaII[ant];
    }      
  }
  return 0;
}

Double_t AnitaGeomTool::getAntR(int ant) {
  if(fUseKurtAnitaIINumbers==0) {
    //    std::cout << "Using simon R\n";
    if(ant>=0 && ant<NUM_SEAVEYS) {
      return rPhaseCentreFromVerticalHorn[ant];
    }
  }
  else {
    //    std::cout << "Using kurt R\n";
    if(ant>=0 && ant<NUM_SEAVEYS) {
      return rPhaseCentreFromVerticalHornKurtAnitaII[ant];
    }
  }
  return 0;
}

Double_t AnitaGeomTool::getAntPhiPosition(int ant) {

  if(fUseKurtAnitaIINumbers==0) {
    //    std::cout << "Using simon phi\n";
    if(ant>=0 && ant<NUM_SEAVEYS) {
      return azPhaseCentreFromVerticalHorn[ant];
    }
  }
  else {
    if(ant>=0 && ant<NUM_SEAVEYS) {
      //      std::cout << "Using kurt phi\n";
      return azPhaseCentreFromVerticalHornKurtAnitaII[ant];
    }
  }
  return 0;
}

Double_t AnitaGeomTool::getAntPhiPositionRelToAftFore(int ant) {

  if(fUseKurtAnitaIINumbers==0) {
    //    std::cout << "Using simon phi-rel\n";
    if(ant>=0 && ant<NUM_SEAVEYS) {
      Double_t phi=azPhaseCentreFromVerticalHorn[ant]-aftForeOffsetAngleVertical;
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
      Double_t phi=azPhaseCentreFromVerticalHornKurtAnitaII[ant]-aftForeOffsetAngleVerticalKurtAnitaII;
      if(phi<0)
	phi+=TMath::TwoPi();
    if(phi>TMath::TwoPi())
      phi-=TMath::TwoPi();
      return phi;
    }
  }      
  return 0;
}

Double_t AnitaGeomTool::getMeanAntPairPhiRelToAftFore(int firstAnt, int secondAnt) {

   if(firstAnt>=0 && firstAnt<NUM_SEAVEYS && secondAnt>=0 && secondAnt<40) {
      //Calculating the phi of each antenna pair
      double meanPhi=this->getAntPhiPositionRelToAftFore(firstAnt); 
      if(TMath::Abs(meanPhi-this->getAntPhiPositionRelToAftFore(secondAnt))<TMath::PiOver2()) {	 
	 meanPhi+=this->getAntPhiPositionRelToAftFore(secondAnt);	 
	 meanPhi*=0.5;       
      }       
      else if(TMath::Abs(meanPhi-(this->getAntPhiPositionRelToAftFore(secondAnt)+TMath::TwoPi()))<TMath::PiOver2()) {	 
	 meanPhi+=(this->getAntPhiPositionRelToAftFore(secondAnt)+TMath::TwoPi());	 
	 meanPhi*=0.5;       
      }       
      else if(TMath::Abs(meanPhi-(this->getAntPhiPositionRelToAftFore(secondAnt)-TMath::TwoPi()))<TMath::PiOver2()) {	 
	 meanPhi+=(this->getAntPhiPositionRelToAftFore(secondAnt)-TMath::TwoPi());	 
	 meanPhi*=0.5;       
      }  
      meanPhi*=TMath::RadToDeg();      
      return meanPhi;
   }
   return -999;
}



Int_t AnitaGeomTool::getUpperAntNearestPhiWave(Double_t phiWave) {
  if(phiWave<0) phiWave+=TMath::TwoPi();
  if(phiWave>TMath::TwoPi()) phiWave-=TMath::TwoPi();
  Double_t minDiff=TMath::TwoPi();
  Int_t minAnt=0;;
  for(int ant=0;ant<16;ant++) {
    //      std::cout << ant << "\t" << azPhaseCentreFromVerticalHorn[ant] << "\t" << phiPrime << "\t" << TMath::Abs(azPhaseCentreFromVerticalHorn[ant]-phiPrime) << "\n";
    if(TMath::Abs(getAntPhiPositionRelToAftFore(ant)-phiWave)<minDiff) {
      minDiff=TMath::Abs(getAntPhiPositionRelToAftFore(ant)-phiWave);
      minAnt=ant;
    }
    
    if(TMath::Abs((getAntPhiPositionRelToAftFore(ant)-TMath::TwoPi())-phiWave)<minDiff) {
      minDiff=TMath::Abs((getAntPhiPositionRelToAftFore(ant)-TMath::TwoPi())-phiWave);
      minAnt=ant;      
    }

    if(TMath::Abs((getAntPhiPositionRelToAftFore(ant)+TMath::TwoPi())-phiWave)<minDiff) {
      minDiff=TMath::Abs((getAntPhiPositionRelToAftFore(ant)+TMath::TwoPi())-phiWave);
      minAnt=ant;      
    }

  }
  return minAnt;
}




//Non static thingies
void AnitaGeomTool::getAntFaceXYZ(int ant, Double_t &x, Double_t &y, Double_t &z)
{
  if(ant>=0 && ant<NUM_SEAVEYS) {
    x=xAntFromVerticalHorn[ant];
    y=yAntFromVerticalHorn[ant];
    z=zAntFromVerticalHorn[ant];
  }
}

Double_t AnitaGeomTool::getAntFaceZ(int ant) {
  if(ant>=0 && ant<NUM_SEAVEYS) {
    return zAntFromVerticalHorn[ant];
    //      return zAntFaceFromDeckHorn[ant];
  }
  return 0;
}

Double_t AnitaGeomTool::getAntFaceR(int ant) {
  if(ant>=0 && ant<NUM_SEAVEYS) {
    return rAntFromVerticalHorn[ant];
    //return rAntFromDeckHorn[ant];
  }
  return 0;
}

Double_t AnitaGeomTool::getAntFacePhiPosition(int ant) {
  if(ant>=0 && ant<NUM_SEAVEYS) {
    return azCentreFromVerticalHorn[ant];
  }
  return 0;
}

Double_t AnitaGeomTool::getAntFacePhiPositionRelToAftFore(int ant) {
  if(ant>=0 && ant<NUM_SEAVEYS) {
    Double_t phi=azCentreFromVerticalHorn[ant]-aftForeOffsetAngleVertical;
    if(phi<0)
      phi+=TMath::TwoPi();
    return phi;
  }
  return 0;
}

Int_t AnitaGeomTool::getUpperAntFaceNearestPhiWave(Double_t phiWave) {
  Double_t phiPrime=phiWave+aftForeOffsetAngleVertical;
  if(phiPrime>TMath::TwoPi()) 
    phiPrime-=TMath::TwoPi();
  Double_t minDiff=TMath::TwoPi();
  Int_t minAnt=0;;
  for(int ant=0;ant<16;ant++) {
    //      std::cout << ant << "\t" << azPhaseCentreFromVerticalHorn[ant] << "\t" << phiPrime << "\t" << TMath::Abs(azPhaseCentreFromVerticalHorn[ant]-phiPrime) << "\n";
    if(TMath::Abs(azCentreFromVerticalHorn[ant]-phiPrime)<minDiff) {
      minDiff=TMath::Abs(azCentreFromVerticalHorn[ant]-phiPrime);
      minAnt=ant;
    }
  }
  return minAnt;
}

void AnitaGeomTool::updateAnt(double deltaR,double deltaRL,double deltaUD){
  
   Double_t phaseCentreToAntFront=0.2+deltaR;
  
   double   deltaXRL = 0;
   double   deltaYRL = 0;
   
   double  deltaZUD = 0;
   double  deltaXUD = 0;
   double  deltaYUD = 0;
   
    //std::cout << deltaXRL << "  " << deltaXUD << "  "<< deltaYUD << "  "<< deltaYRL  <<  "  " <<  deltaZUD << std::endl;

  for(int ant=0;ant<32;ant++) {
    
    //deltaXRL = -deltaRL*TMath::Sin(apertureAzFromVerticalHorn[ant]);
	    //       deltaYRL = deltaRL*TMath::Cos(apertureAzFromVerticalHorn[ant]);
    
           deltaZUD = deltaUD*TMath::Cos(apertureElFromVerticalHorn[ant]);
          deltaXUD = -deltaUD*TMath::Sin(apertureElFromVerticalHorn[ant])*TMath::Cos(apertureAzFromVerticalHorn[ant]);
          deltaYUD = -deltaUD*TMath::Sin(apertureElFromVerticalHorn[ant])*TMath::Sin(apertureAzFromVerticalHorn[ant]);
    
    //std::cout << deltaXRL << "  " << deltaXUD << "  "<< deltaYUD << "  "<< deltaYRL  <<  "  " <<  deltaZUD << std::endl;
    
    //Now try to find the location of the antenna phaseCentre point     
    xPhaseCentreFromVerticalHorn[ant]=(xAntFromVerticalHorn[ant] + deltaXRL + deltaXUD) -
      phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    yPhaseCentreFromVerticalHorn[ant]=(yAntFromVerticalHorn[ant] + deltaYRL + deltaYUD) -
      phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    zPhaseCentreFromVerticalHorn[ant]=(zAntFromVerticalHorn[ant] + deltaZUD) -phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m

    
    rPhaseCentreFromVerticalHorn[ant]=TMath::Sqrt(xPhaseCentreFromVerticalHorn[ant]*xPhaseCentreFromVerticalHorn[ant]+
						  yPhaseCentreFromVerticalHorn[ant]*yPhaseCentreFromVerticalHorn[ant]);//m
    azPhaseCentreFromVerticalHorn[ant]=TMath::ATan(yPhaseCentreFromVerticalHorn[ant]/xPhaseCentreFromVerticalHorn[ant]); //radians
    if(xPhaseCentreFromVerticalHorn[ant]<0)
      azPhaseCentreFromVerticalHorn[ant]+=TMath::Pi();
    if(azPhaseCentreFromVerticalHorn[ant]<0)
      azPhaseCentreFromVerticalHorn[ant]+=TMath::TwoPi();

 }

  for(int ant=32;ant<40;ant++) {
    
    double   deltaXRL = 0;
    double   deltaYRL = 0;
    
    double  deltaZUD = 0;
    double  deltaXUD = 0;
    double  deltaYUD = 0;
    
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
    
      
    xPhaseCentreFromVerticalHorn[ant]=(xAntFromVerticalHorn[ant] + deltaXRL + deltaXUD) -
      phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    yPhaseCentreFromVerticalHorn[ant]=(yAntFromVerticalHorn[ant] + deltaYRL + deltaYUD) -
      phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    zPhaseCentreFromVerticalHorn[ant]=(zAntFromVerticalHorn[ant] + deltaZUD) -phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m

    //      //Now try to find the location of the antenna phaseCentre point     
    //      xPhaseCentreFromVerticalHorn[ant]=xAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      yPhaseCentreFromVerticalHorn[ant]=yAntFromVerticalHorn[ant]-
    //        phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
    //      zPhaseCentreFromVerticalHorn[ant]=zAntFromVerticalHorn[ant]-phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m
    rPhaseCentreFromVerticalHorn[ant]=TMath::Sqrt(xPhaseCentreFromVerticalHorn[ant]*xPhaseCentreFromVerticalHorn[ant]+
						  yPhaseCentreFromVerticalHorn[ant]*yPhaseCentreFromVerticalHorn[ant]);//m
    azPhaseCentreFromVerticalHorn[ant]=TMath::ATan(yPhaseCentreFromVerticalHorn[ant]/xPhaseCentreFromVerticalHorn[ant]); //radians
    if(xPhaseCentreFromVerticalHorn[ant]<0)
      azPhaseCentreFromVerticalHorn[ant]+=TMath::Pi();
    if(azPhaseCentreFromVerticalHorn[ant]<0)
      azPhaseCentreFromVerticalHorn[ant]+=TMath::TwoPi();

 //    std::cout << "Simon's:\t" << ant << " "  
// 	      << rPhaseCentreFromVerticalHorn[ant] << " " 
// 	      << azPhaseCentreFromVerticalHorn[ant] << " "
// 	      << zPhaseCentreFromVerticalHorn[ant] << "\n";

}



};



void AnitaGeomTool::printAntPos(){

  for(int ant = 0; ant < 32; ant++){
  std::cout << rPhaseCentreFromVerticalHorn[ant] << std::endl;
  }

}



int AnitaGeomTool::getPhiFromAnt(int ant)
{
  if(ant<16)
    return AnitaGeom::upperPhiNums[ant];
  else if(ant<32)
    return AnitaGeom::lowerPhiNums[ant-16];
  else if(ant<40)
    return AnitaGeom::nadirPhiNums[ant-32];
  std::cerr << "There isn't an antenna " << ant << " (0-39 only)\n";
  return -1;

}


int AnitaGeomTool::getAntFromPhiRing(int phi, AnitaRing::AnitaRing_t ring)
{
  switch(ring) {
  case AnitaRing::kUpperRing:
    return AnitaGeom::upperAntNums[phi];
  case AnitaRing::kLowerRing:
    return AnitaGeom::lowerAntNums[phi];
  case AnitaRing::kNadirRing:
    return AnitaGeom::nadirAntNums[phi];
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
    deltaRPhaseCentre[antNum]=deltaR;
    deltaPhiPhaseCentre[antNum]=deltaPhi;
    deltaZPhaseCentre[antNum]=deltaZ;
  }

 

}



void AnitaGeomTool::readAnitaIIPhotogrammetry()
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

  sprintf(fileName,"%s/anitaIIPhotogrammetry.csv",calibDir);
  std::ifstream AnitaIIPhotoFile(fileName);
  if(!AnitaIIPhotoFile) {
    std::cerr << "Couldn't open:\t" << fileName << "\n";
    return;
  }


  //First up are the antenna positions
  TString line;
  for(int i=0;i<2;i++) {
    line.ReadLine(AnitaIIPhotoFile);
    //std::cout << line.Data() << "\n";
  }
  
  for(int ant=0;ant<40;ant++) {
    line.ReadLine(AnitaIIPhotoFile);
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
	xAntFromVerticalHornKurtAnitaII[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 2:	   
	yAntFromVerticalHornKurtAnitaII[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 3:	   
	zAntFromVerticalHornKurtAnitaII[ant]=subString.Atof()*INCHTOMETER; //m
	break;
      case 4:	   
	rAntFromVerticalHornKurtAnitaII[ant]=subString.Atof()*INCHTOMETER; //m
	//	   std::cout << ant << "\t" << rAntFromVerticalHornKurtAnitaII[ant] << "\n";
	break;
      case 5:	   
	azCentreFromVerticalHornKurtAnitaII[ant]=subString.Atof()*TMath::DegToRad(); //radians
	break;
      case 6:	   
	apertureAzFromVerticalHornKurtAnitaII[ant]=subString.Atof()*TMath::DegToRad(); //radians
	break;
      case 7:	   
	apertureElFromVerticalHornKurtAnitaII[ant]=subString.Atof()*TMath::DegToRad(); //radians
	break;
      default:	   
	break;
      }
   
    }
    tokens->Delete();

  }

  
  for(int ant=0;ant<NUM_SEAVEYS;ant++) {
    Double_t phaseCentreToAntFront=ringPhaseCentreOffset[Int_t(this->getRingFromAnt(ant))];
    //Need to think about this at some point, but for now will just hard code in 20cm
    phaseCentreToAntFront=0.2;
 

    //     Now try to find the location of the antenna phaseCentre point  
    xPhaseCentreFromVerticalHornKurtAnitaII[ant]=xAntFromVerticalHornKurtAnitaII[ant] -
      phaseCentreToAntFront*TMath::Cos( apertureAzFromVerticalHornKurtAnitaII[ant])*TMath::Cos(apertureElFromVerticalHornKurtAnitaII[ant]); //m
    yPhaseCentreFromVerticalHornKurtAnitaII[ant]=yAntFromVerticalHornKurtAnitaII[ant] -
      phaseCentreToAntFront*TMath::Sin( apertureAzFromVerticalHornKurtAnitaII[ant])*TMath::Cos(apertureElFromVerticalHornKurtAnitaII[ant]); //m
    zPhaseCentreFromVerticalHornKurtAnitaII[ant]=zAntFromVerticalHornKurtAnitaII[ant] - phaseCentreToAntFront*TMath::Sin(apertureElFromVerticalHornKurtAnitaII[ant]); //m
    
    rPhaseCentreFromVerticalHornKurtAnitaII[ant]=TMath::Sqrt(xPhaseCentreFromVerticalHornKurtAnitaII[ant]*xPhaseCentreFromVerticalHornKurtAnitaII[ant]+yPhaseCentreFromVerticalHornKurtAnitaII[ant]*yPhaseCentreFromVerticalHornKurtAnitaII[ant]);//m
    azPhaseCentreFromVerticalHornKurtAnitaII[ant]=TMath::ATan(yPhaseCentreFromVerticalHornKurtAnitaII[ant]/xPhaseCentreFromVerticalHornKurtAnitaII[ant]); //radians
    if(xPhaseCentreFromVerticalHornKurtAnitaII[ant]<0)
      azPhaseCentreFromVerticalHornKurtAnitaII[ant]+=TMath::Pi();
    if(azPhaseCentreFromVerticalHornKurtAnitaII[ant]<0)
      azPhaseCentreFromVerticalHornKurtAnitaII[ant]+=TMath::TwoPi();

//   //Silly hack for testing only    
//     TVector3 kurtVector(xPhaseCentreFromVerticalHornKurtAnitaII[ant],    
// 			yPhaseCentreFromVerticalHornKurtAnitaII[ant],   
// 			zPhaseCentreFromVerticalHornKurtAnitaII[ant]);
//     kurtVector.RotateZ(+45.12*TMath::DegToRad());  
//     kurtVector.RotateY(0.75*TMath::DegToRad());   
//     kurtVector.RotateX(1*TMath::DegToRad());  
//     kurtVector.RotateZ(-45.12*TMath::DegToRad());   
//     xPhaseCentreFromVerticalHornKurtAnitaII[ant]=kurtVector.x();
//     yPhaseCentreFromVerticalHornKurtAnitaII[ant]=kurtVector.y(); 
//     zPhaseCentreFromVerticalHornKurtAnitaII[ant]=kurtVector.z();  
//     rPhaseCentreFromVerticalHornKurtAnitaII[ant]=TMath::Sqrt(kurtVector.x()*kurtVector.x()+kurtVector.y()*kurtVector.y());    
//     azPhaseCentreFromVerticalHornKurtAnitaII[ant]=kurtVector.Phi();


    
  }

  aftForeOffsetAngleVerticalKurtAnitaII=-45.12*TMath::DegToRad();

}
