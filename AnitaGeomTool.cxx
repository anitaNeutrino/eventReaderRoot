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

#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"

#define INCHTOMETER 0.0254

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

void AnitaGeomTool::getNeighbors(int rx,int& rxleft,int& rxright)
{
    // input antenna number 0 to 31
  if (rx<0 || rx>31)    
    std::cout << "Antenna number out of range!\n";  
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


void AnitaGeomTool::readPhotogrammetry()
{

  char calibDir[FILENAME_MAX];
  char fileName[FILENAME_MAX];
  char *calibEnv=getenv("ANITA_CALIB_DIR");
  if(!calibEnv) {
    sprintf(calibDir,"calib");
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
   if(ant>=0 && ant<32) {
      x=xAntFromVerticalHorn[ant];
      y=yAntFromVerticalHorn[ant];
      z=zAntFromVerticalHorn[ant];
   }
}

Double_t AnitaGeomTool::getAntZ(int ant) {
   if(ant>=0 && ant<32) {
      return zAntFromVerticalHorn[ant];
      //      return zAntFromDeckHorn[ant];
   }
   return 0;
}

Double_t AnitaGeomTool::getAntR(int ant) {
   if(ant>=0 && ant<32) {
      return rAntFromVerticalHorn[ant]-0.3;
	    //return rAntFromDeckHorn[ant];
   }
   return 0;
}

Double_t AnitaGeomTool::getAntPhiPosition(int ant) {
   if(ant>=0 && ant<32) {
      return azCentreFromVerticalHorn[ant];
   }
   return 0;
}

Double_t AnitaGeomTool::getAntPhiPositionRelToAftFore(int ant) {
   if(ant>=0 && ant<32) {
      Double_t phi=azCentreFromVerticalHorn[ant]-aftForeOffsetAngleVertical;
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
      //      std::cout << ant << "\t" << azCentreFromVerticalHorn[ant] << "\t" << phiPrime << "\t" << TMath::Abs(azCentreFromVerticalHorn[ant]-phiPrime) << "\n";
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

