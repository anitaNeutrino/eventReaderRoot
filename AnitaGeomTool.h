//////////////////////////////////////////////////////////////////////////////
/////  AnitaGeomTool.h        ANITA Geom Tool                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for converting between physical channel (ring,  /////
/////     antenna, phi, etc.) and logical channel (surf, chan, etc.)     /////
/////     It should be noted that phi, antenna, etc are numbered from 0  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ANITAGEOMTOOL_H
#define ANITAGEOMTOOL_H

//Includes
#include <TObject.h>
#include <TMath.h>
#include "AnitaConventions.h"


class UsefulAnitaEvent;


//Are actually geometry things
#define R_EARTH 6.378137E6
#define  GEOID_MAX 6.378137E6 // parameters of geoid model
#define  GEOID_MIN 6.356752E6
#define C_LIGHT 299792458 //meters



class AnitaGeomTool 
{
 public:
   
  AnitaGeomTool();
  ~AnitaGeomTool();

  //Instance generator
  static AnitaGeomTool*  Instance();
  

  
  
  inline Double_t getGeoid(Double_t theta) {
     Double_t c=TMath::Cos(theta);
     return GEOID_MIN*GEOID_MAX/TMath::Sqrt(GEOID_MIN*GEOID_MIN-
					    (GEOID_MIN*GEOID_MIN-GEOID_MAX*GEOID_MAX)*c*c);    
  }
  

  inline Double_t getLat(Double_t theta) {      
     return (90.-((theta*TMath::RadToDeg()))); 
  }
  
  inline Double_t getLon(Double_t phi){ 
     //Need to fix this somehow
     double phi_deg = phi*TMath::RadToDeg();
     if (phi_deg>270)
	phi_deg-=360;	
     return (90.-phi_deg);
  }

  inline Double_t getThetaFromLat(Double_t lat) {       
     return (90.- lat)*TMath::DegToRad(); 
  }
  
  inline Double_t getPhiFromLon(Double_t lon){ 
     //Need to fix this somehow
     double phi_deg = 90. - lon;
     if(phi_deg<0) phi_deg+=360;
     return phi_deg*TMath::DegToRad();
  }
  
  
  //Generally useful function
  static int getChanIndex(int surf, int chan)
     {return chan+(9*surf);}

  static int getChanIndexFromRingPhiPol(AnitaRing::AnitaRing_t ring,
					int phi,
					AnitaPol::AnitaPol_t pol);
  static int getChanIndexFromAntPol(int ant,
				    AnitaPol::AnitaPol_t pol);


  static int getSurfChanFromChanIndex(int chanIndex, // input channel index
				      int &surf,int &chan); // output surf and channel
  
  static int getAntPolFromSurfChan(int surf,int chan,int &ant,AnitaPol::AnitaPol_t &pol);
  
  static int getAzimuthPartner(int rx); // output the antenna that points to the same place in phi as the input antenna
  
  static void getThetaPartners(int rx,int& rxleft,int& rxright); // output the antennas that are in neighbouring phi sectors (not the neighbouring antennas)

  static void getNeighbors(int rx,int& rxleft,int& rxright); // output the neighboring antennas 

  static int getPhiSector(int rx); // phi sector of this antenna.  rx runs from 0 to 31.

  static float getDirectionwrtNorth(int phi,float heading); // get direction that a phi sector is pointing wrt north.  Also takes heading as a input.

  static int getLayer(int irx); // get layer given antenna number



  //Non static thingies
  void getAntXYZ(int ant, Double_t &x, Double_t &y, Double_t &z);
  Double_t getAntZ(int ant);
  Double_t getAntR(int ant);
  Double_t getAntPhiPosition(int ant);
  Double_t getAntPhiPositionRelToAftFore(int ant);
  Int_t getUpperAntNearestPhiWave(Double_t phiWave);


   Float_t xAntFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t yAntFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t zAntFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t rAntFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t azCentreFromDeckHorn[NUM_SEAVEYS]; //radians
   Float_t apertureAzFromDeckHorn[NUM_SEAVEYS]; //radians
   Float_t apertureElFromDeckHorn[NUM_SEAVEYS]; //radians
   
   Float_t xAntFromDeckBicone[NUM_BICONES]; //m
   Float_t yAntFromDeckBicone[NUM_BICONES]; //m
   Float_t zAntFromDeckBicone[NUM_BICONES]; //m

   Float_t xAntFromDeckDiscone[NUM_DISCONES]; //m
   Float_t yAntFromDeckDiscone[NUM_DISCONES]; //m
   Float_t zAntFromDeckDiscone[NUM_DISCONES]; //m

   Float_t xAnitaBoxFromDeckCorner[4];
   Float_t yAnitaBoxFromDeckCorner[4];
   Float_t zAnitaBoxFromDeckCorner[4];
   Float_t xBatteryBoxFromDeckCorner[4];
   Float_t yBatteryBoxFromDeckCorner[4];
   Float_t zBatteryBoxFromDeckCorner[4];
   Float_t xSipBoxFromDeckCorner[4];
   Float_t ySipBoxFromDeckCorner[4];
   Float_t zSipBoxFromDeckCorner[4];
   
   Float_t gpsPlaneFromDeck[3];
   Float_t gpsHeadingFromDeck[3];
   Float_t aftForeOffsetAngleDeck;

   Float_t xAntFromVerticalHorn[NUM_SEAVEYS]; //m
   Float_t yAntFromVerticalHorn[NUM_SEAVEYS]; //m
   Float_t zAntFromVerticalHorn[NUM_SEAVEYS]; //m
   Float_t rAntFromVerticalHorn[NUM_SEAVEYS]; //m
   Float_t azCentreFromVerticalHorn[NUM_SEAVEYS]; //radians
   Float_t apertureAzFromVerticalHorn[NUM_SEAVEYS]; //radians
   Float_t apertureElFromVerticalHorn[NUM_SEAVEYS]; //radians
   
   Float_t xAntFromVerticalBicone[NUM_BICONES]; //m
   Float_t yAntFromVerticalBicone[NUM_BICONES]; //m
   Float_t zAntFromVerticalBicone[NUM_BICONES]; //m

   Float_t xAntFromVerticalDiscone[NUM_DISCONES]; //m
   Float_t yAntFromVerticalDiscone[NUM_DISCONES]; //m
   Float_t zAntFromVerticalDiscone[NUM_DISCONES]; //m

   Float_t xAnitaBoxFromVerticalCorner[4];
   Float_t yAnitaBoxFromVerticalCorner[4];
   Float_t zAnitaBoxFromVerticalCorner[4];
   Float_t xBatteryBoxFromVerticalCorner[4];
   Float_t yBatteryBoxFromVerticalCorner[4];
   Float_t zBatteryBoxFromVerticalCorner[4];
   Float_t xSipBoxFromVerticalCorner[4];
   Float_t ySipBoxFromVerticalCorner[4];
   Float_t zSipBoxFromVerticalCorner[4];
   
   Float_t gpsPlaneFromVertical[3];
   Float_t gpsHeadingFromVertical[3];
   Float_t aftForeOffsetAngleVertical;

 protected:
   static AnitaGeomTool *fgInstance;  
   // protect against multiple instances

 private:
   void readPhotogrammetry();

};


#endif //ANITAGEOMTOOL_H
