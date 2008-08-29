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
#include <iostream>
#include <TObject.h>
#include <TMath.h>
#include <TVector3.h>
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
  
  inline Double_t getPhi(Double_t p[3]){
      // returns phi between 0 and 2pi.
        double pt=0;
        double phi=0;
        pt=sqrt(p[0]*p[0]+p[1]*p[1]);
        if (pt==0)
          return 0.;
        else if (pt!=0) {
          if (p[1]/pt>1 || p[1]/pt<-1) {
                std::cerr << "Error in getPhi. \n";
                return 0;
            }
            phi=asin(p[1]/pt);
        }
        if (p[1]<0. && p[0]>0) phi += 2*TMath::Pi();
        else if (phi>0 && p[0]<0.) phi = TMath::Pi() - phi;
        else if (phi<0 && p[0]<0.) phi = -(TMath::Pi()+phi)+2*TMath::Pi();
        return phi;
    }

     inline Double_t getPhi(TVector3 &thePos) {
       double p[3]={thePos.X(),thePos.Y(),thePos.Z()};
       return getPhi(p);
       //return thePos.Theta();
    }

     inline Double_t getTheta(Double_t p[3]) {
      double pz,pt;
      double tantheta1=0;
      double theta=0;

      pz=p[2];
      pt=sqrt(p[0]*p[0]+p[1]*p[1]);
      tantheta1=pt/pz;
      theta=atan(tantheta1);

      if (pz<0)
        theta += TMath::Pi();
      return theta;
    }


     inline Double_t getTheta(TVector3 &thePos) {
	double p[3]={thePos.X(),thePos.Y(),thePos.Z()};
	thePos.GetXYZ(p);
	return getTheta(p);
     }
     
     inline void getLonLat(Double_t p[3],Double_t& lon,Double_t& lat) {
        lon=getLon(getPhi(p));
        lat=getLat(getTheta(p));
/*      if(p[2]<0) { */
/*         lat*=-1; */
/*      } */
    }

     inline void getLonLat(TVector3 &thePos,Double_t& lon,Double_t& lat) {
        lon=getLon(getPhi(thePos));
        lat=getLat(getTheta(thePos));
    }

  
  //Generally useful functions
  static int getChanIndex(int surf, int chan)
     {return chan+(9*surf);}

  static AnitaRing::AnitaRing_t getRingFromAnt(int ant);

  static void getSurfChanAntFromRingPhiPol(AnitaRing::AnitaRing_t ring,
					   int phi,
					   AnitaPol::AnitaPol_t pol,
					   int &surf, int &chan, int &ant);

  static void getRingAntPolPhiFromSurfChan(int surf, int chan,
					  AnitaRing::AnitaRing_t &ring,
					  int &ant,
					  AnitaPol::AnitaPol_t &pol,
					  int &phi);

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

  static int getPhiFromAnt(int ant);
  static int getAntFromPhiRing(int phi, AnitaRing::AnitaRing_t ring);


  //Non static thingies
  void getPhiWave(Double_t balloonLon, Double_t balloonLat, Double_t ballonAlt, Double_t balloonHeading, Double_t sourceLon, Double_t sourceLat, Double_t sourceAlt, Double_t &thetaWave, Double_t &phiWave);

  void getAntXYZ(int ant, Double_t &x, Double_t &y, Double_t &z);
  Double_t getAntZ(int ant);
  Double_t getAntR(int ant);
  Double_t getAntPhiPosition(int ant);
  Double_t getAntPhiPositionRelToAftFore(int ant);
  Int_t getUpperAntNearestPhiWave(Double_t phiWave);

  
  void getAntFaceXYZ(int ant, Double_t &x, Double_t &y, Double_t &z);
  Double_t getAntFaceZ(int ant);
  Double_t getAntFaceR(int ant);
  Double_t getAntFacePhiPosition(int ant);
  Double_t getAntFacePhiPositionRelToAftFore(int ant);
  Int_t getUpperAntFaceNearestPhiWave(Double_t phiWave);

  Float_t feedToAntFront; //m
 

   Float_t xAntFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t yAntFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t zAntFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t rAntFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t azCentreFromDeckHorn[NUM_SEAVEYS]; //radians
   Float_t apertureAzFromDeckHorn[NUM_SEAVEYS]; //radians
   Float_t apertureElFromDeckHorn[NUM_SEAVEYS]; //radians

   
   Float_t xFeedFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t yFeedFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t zFeedFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t rFeedFromDeckHorn[NUM_SEAVEYS]; //m
   Float_t azFeedFromDeckHorn[NUM_SEAVEYS]; //radians

   
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
  
   Float_t xFeedFromVerticalHorn[NUM_SEAVEYS]; //m
   Float_t yFeedFromVerticalHorn[NUM_SEAVEYS]; //m
   Float_t zFeedFromVerticalHorn[NUM_SEAVEYS]; //m
   Float_t rFeedFromVerticalHorn[NUM_SEAVEYS]; //m
   Float_t azFeedFromVerticalHorn[NUM_SEAVEYS]; //radians

   
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
