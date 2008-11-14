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
   
   // Note that this array uses antenna number 1-32 as it needs
   // the negative sign to indicate polarization
  //Need to update this with the ANITA-II numbers
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
     {-34,-36,-38,40,34,36,38,40}};
      
  //Map from phi to antenna both start counting at zero
  int upperAntNums[NUM_PHI]={8,0,9,1,10,2,11,3,12,4,13,5,14,6,15,7};
  int lowerAntNums[NUM_PHI]={16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
  int nadirAntNums[NUM_PHI]={32,-1,33,-1,34,-1,35,-1,36,-1,37,-1,38,-1,39,-1};
   
  //and the inverse (using ANT-1 and ANT-17 and ANT-32 with the arrays)
  int upperPhiNums[NUM_PHI]={1,3,5,7,9,11,13,15,0,2,4,6,8,10,12,14};
  int lowerPhiNums[NUM_PHI]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  int nadirPhiNums[NUM_NADIRS]={0,1,2,3,4,5,6,7};
   
}

AnitaGeomTool*  AnitaGeomTool::fgInstance = 0;


AnitaGeomTool::AnitaGeomTool()
{
   //Default constructor
  feedToAntFront=0.2; //Arbitrary value selected to minimise the timing residuals.
  readPhotogrammetry();
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

void AnitaGeomTool::getPhiWave(Double_t balloonLon, Double_t balloonLat, Double_t balloonAlt, Double_t balloonHeading, Double_t sourceLon, Double_t sourceLat, Double_t sourceAlt, Double_t &thetaWave, Double_t &phiWave)
{
   Double_t thetaBalloon=getThetaFromLat(TMath::Abs(balloonLat));
   Double_t phiBalloon=getPhiFromLon(balloonLon);
   Double_t balloonHeight=getGeoid(thetaBalloon)+balloonAlt;
   
   Double_t thetaSource=getThetaFromLat(TMath::Abs(sourceLat));
   Double_t phiSource=getPhiFromLon(sourceLon);
   Double_t radiusSource=getGeoid(thetaSource)+sourceAlt;

   //Get vector from Earth's centre to source
   TVector3 fSourcePos;
   fSourcePos.SetX(radiusSource*TMath::Sin(thetaSource)*TMath::Cos(phiSource));
   fSourcePos.SetY(radiusSource*TMath::Sin(thetaSource)*TMath::Sin(phiSource));
   fSourcePos.SetZ(radiusSource*TMath::Cos(thetaSource));
   
   //Rotate such that balloon is at 0,0,balloonHeight
   fSourcePos.RotateZ(-1*phiBalloon);
   fSourcePos.RotateY(-1*thetaBalloon);

   //Now find thetaWave and phiWave
   thetaWave=TMath::ATan((balloonHeight-fSourcePos.Z())/TMath::Sqrt(fSourcePos.X()*fSourcePos.X() + fSourcePos.Y()*fSourcePos.Y()));
   
   //phiWave is just atan(yp/xp) only looks confusing to make sure I get the sign and 0-360 convention
   phiWave=0;
   if(fSourcePos.X()==0) {
      phiWave=TMath::PiOver2();
      if(fSourcePos.Y()<0)
	 phiWave+=TMath::Pi();
   }
   else if(fSourcePos.X()<0) {
      phiWave=TMath::Pi()+TMath::ATan(fSourcePos.Y()/fSourcePos.X());
   }
   else {
      phiWave=TMath::ATan(fSourcePos.Y()/fSourcePos.X());
      if(fSourcePos.Y()<0) {
	 phiWave+=TMath::TwoPi();
      }
   }   

   //Now need to take account of balloon heading
   //Will have to check heading at some point
   if(balloonHeading>=0 && balloonHeading<=360) {
      phiWave+=balloonHeading*TMath::DegToRad();
      if(phiWave>TMath::TwoPi())
	 phiWave-=TMath::TwoPi();
   }

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
   if(ant<0 || ant>39) return -1;
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
  //Need to update for nadir ring
  
  if (rx<16) 
     return AnitaGeom::lowerAntNums[AnitaGeom::upperPhiNums[rx]];
  else if(rx<32)
    return AnitaGeom::upperAntNums[AnitaGeom::lowerPhiNums[rx-16]]; 

  return -1;
}

void AnitaGeomTool::getNeighbors(int rx,int& rxleft,int& rxright)
{
    // input antenna number 0 to 31
  if (rx<0 || rx>31)    
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
  //  std::cout << "rx, rxleft, rxright " << rx << " " << rxleft << " " << rxright << "\n"; 

}

void AnitaGeomTool::getThetaPartners(int rx,int& rxleft,int& rxright)
{
   if (rx<0 || rx>31)    
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

     //Now try to find the location of the antenna feed point     
     xFeedFromDeckHorn[ant]=xAntFromDeckHorn[ant]-
       feedToAntFront*TMath::Cos( apertureAzFromDeckHorn[ant])*TMath::Cos(apertureElFromDeckHorn[ant]); //m
     yFeedFromDeckHorn[ant]=yAntFromDeckHorn[ant]-
       feedToAntFront*TMath::Sin( apertureAzFromDeckHorn[ant])*TMath::Cos(apertureElFromDeckHorn[ant]); //m
     zFeedFromDeckHorn[ant]=zAntFromDeckHorn[ant]-feedToAntFront*TMath::Sin(apertureElFromDeckHorn[ant]); //m
     rFeedFromDeckHorn[ant]=TMath::Sqrt(xFeedFromDeckHorn[ant]*xFeedFromDeckHorn[ant]+
					yFeedFromDeckHorn[ant]*yFeedFromDeckHorn[ant]);//m
     azFeedFromDeckHorn[ant]=TMath::ATan(yFeedFromDeckHorn[ant]/xFeedFromDeckHorn[ant]); //radians
     if(xFeedFromDeckHorn[ant]<0)
       azFeedFromDeckHorn[ant]+=TMath::Pi();
     if(azFeedFromDeckHorn[ant]<0)
       azFeedFromDeckHorn[ant]+=TMath::TwoPi();

//      std::cout << "Face v Feed -- Ant " << ant  << std::endl;
//      std::cout << apertureAzFromDeckHorn[ant] << "\t" << apertureElFromDeckHorn[ant] << "\n";
//      std::cout << "x\t" << xAntFromDeckHorn[ant] << "\t" << xFeedFromDeckHorn[ant] << std::endl;
//      std::cout << "y\t" << yAntFromDeckHorn[ant] << "\t" << yFeedFromDeckHorn[ant] << std::endl;
//      std::cout << "z\t" << zAntFromDeckHorn[ant] << "\t" << zFeedFromDeckHorn[ant] << std::endl;
//      std::cout << "r\t" << rAntFromDeckHorn[ant] << "\t" << rFeedFromDeckHorn[ant] << std::endl;
//      std::cout << "phi\t" << azCentreFromDeckHorn[ant] << "\t" << azFeedFromDeckHorn[ant] << std::endl;
     
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
	   gpsPlaneFromDeck[j-1]=subString.Atof()*INCHTOMETER; //m
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
	   gpsHeadingFromDeck[j-1]=subString.Atof()*INCHTOMETER; //m
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

     
     //Now try to find the location of the antenna feed point     
     xFeedFromVerticalHorn[ant]=xAntFromVerticalHorn[ant]-
       feedToAntFront*TMath::Cos( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
     yFeedFromVerticalHorn[ant]=yAntFromVerticalHorn[ant]-
       feedToAntFront*TMath::Sin( apertureAzFromVerticalHorn[ant])*TMath::Cos(apertureElFromVerticalHorn[ant]); //m
     zFeedFromVerticalHorn[ant]=zAntFromVerticalHorn[ant]-feedToAntFront*TMath::Sin(apertureElFromVerticalHorn[ant]); //m
     rFeedFromVerticalHorn[ant]=TMath::Sqrt(xFeedFromVerticalHorn[ant]*xFeedFromVerticalHorn[ant]+
					yFeedFromVerticalHorn[ant]*yFeedFromVerticalHorn[ant]);//m
     azFeedFromVerticalHorn[ant]=TMath::ATan(yFeedFromVerticalHorn[ant]/xFeedFromVerticalHorn[ant]); //radians
     if(xFeedFromVerticalHorn[ant]<0)
       azFeedFromVerticalHorn[ant]+=TMath::Pi();
     if(azFeedFromVerticalHorn[ant]<0)
       azFeedFromVerticalHorn[ant]+=TMath::TwoPi();

       
//      std::cout << "Face v Feed -- Ant " << ant  << std::endl;
//      std::cout << "x\t" << xAntFromVerticalHorn[ant] << "\t" << xFeedFromVerticalHorn[ant] << std::endl;
//      std::cout << "y\t" << yAntFromVerticalHorn[ant] << "\t" << yFeedFromVerticalHorn[ant] << std::endl;
//      std::cout << "z\t" << zAntFromVerticalHorn[ant] << "\t" << zFeedFromVerticalHorn[ant] << std::endl;
//      std::cout << "r\t" << rAntFromVerticalHorn[ant] << "\t" << rFeedFromVerticalHorn[ant] << std::endl;
//      std::cout << "phi\t" << azCentreFromVerticalHorn[ant] << "\t" << azFeedFromVerticalHorn[ant] << std::endl;

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
	   gpsPlaneFromVertical[j-1]=subString.Atof()*INCHTOMETER; //m
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
	   gpsHeadingFromVertical[j-1]=subString.Atof()*INCHTOMETER; //m
	   break;
	default:	   
	   break;
	}
	
     }
     tokens->Delete();
  }  
  aftForeOffsetAngleVertical=TMath::ATan(gpsHeadingFromVertical[1]/gpsHeadingFromVertical[0]);


}


//Non static thingies
void AnitaGeomTool::getAntXYZ(int ant, Double_t &x, Double_t &y, Double_t &z)
{
   if(ant>=0 && ant<NUM_SEAVEYS) {
      x=xFeedFromVerticalHorn[ant];
      y=yFeedFromVerticalHorn[ant];
      z=zFeedFromVerticalHorn[ant];
   }
}

Double_t AnitaGeomTool::getAntZ(int ant) {
   if(ant>=0 && ant<NUM_SEAVEYS) {
      return zFeedFromVerticalHorn[ant];
      //      return zAntFromDeckHorn[ant];
   }
   return 0;
}

Double_t AnitaGeomTool::getAntR(int ant) {
   if(ant>=0 && ant<NUM_SEAVEYS) {
      return rFeedFromVerticalHorn[ant];
	    //return rAntFromDeckHorn[ant];
   }
   return 0;
}

Double_t AnitaGeomTool::getAntPhiPosition(int ant) {
   if(ant>=0 && ant<NUM_SEAVEYS) {
      return azFeedFromVerticalHorn[ant];
   }
   return 0;
}

Double_t AnitaGeomTool::getAntPhiPositionRelToAftFore(int ant) {
   if(ant>=0 && ant<NUM_SEAVEYS) {
      Double_t phi=azFeedFromVerticalHorn[ant]-aftForeOffsetAngleVertical;
      if(phi<0)
	 phi+=TMath::TwoPi();
      return phi;
   }
   return 0;
}

Int_t AnitaGeomTool::getUpperAntNearestPhiWave(Double_t phiWave) {
   Double_t phiPrime=phiWave+aftForeOffsetAngleVertical;
   if(phiPrime>TMath::TwoPi()) 
      phiPrime-=TMath::TwoPi();
   Double_t minDiff=TMath::TwoPi();
   Int_t minAnt=0;;
   for(int ant=0;ant<16;ant++) {
      //      std::cout << ant << "\t" << azFeedFromVerticalHorn[ant] << "\t" << phiPrime << "\t" << TMath::Abs(azFeedFromVerticalHorn[ant]-phiPrime) << "\n";
      if(TMath::Abs(azFeedFromVerticalHorn[ant]-phiPrime)<minDiff) {
	 minDiff=TMath::Abs(azFeedFromVerticalHorn[ant]-phiPrime);
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
      //      std::cout << ant << "\t" << azFeedFromVerticalHorn[ant] << "\t" << phiPrime << "\t" << TMath::Abs(azFeedFromVerticalHorn[ant]-phiPrime) << "\n";
      if(TMath::Abs(azCentreFromVerticalHorn[ant]-phiPrime)<minDiff) {
	 minDiff=TMath::Abs(azCentreFromVerticalHorn[ant]-phiPrime);
	 minAnt=ant;
      }
   }
   return minAnt;
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
  default:
    return -1;
  }

}

