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

#include <fstream>
#include <cstring>
#include "AnitaConventions.h"

#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TVector3.h"



class UsefulAnitaEvent;

//Are actually geometry things
#define R_EARTH 6.378137E6
#define  GEOID_MAX 6.378137E6 // parameters of geoid model
#define  GEOID_MIN 6.356752E6
#define C_LIGHT 299792458 //meters
#define FLATTENING_FACTOR (1./298.257223563)


//!  AnitaGeomTool -- The ANITA Geometry Tool
/*!
  This is a utility class of useful thingies that can be used for some of the standard conversions that are needed in ANITA analysis. Some of the functions are static and can be called via AnitaGeomTool::functionName(), others require the user to get an instance of the AnitaGeomTool first using AnitaGeomTool *myGeomTool = AnitaGeomTool::Instance()
  \ingroup rootclasses
*/

class AnitaGeomTool 
{
 public:

  /********************************************************************************************************
  Constructor and destructor functions
  ********************************************************************************************************/
  AnitaGeomTool();
  ~AnitaGeomTool();


  /********************************************************************************************************
  Static member functions
  ********************************************************************************************************/
  static AnitaGeomTool*  Instance(); ///<Instance generator
  static AnitaRing::AnitaRing_t getRingFromAnt(Int_t ant); ///< Get ring form antenna number (0-39)
  static void getSurfChanAntFromRingPhiPol(AnitaRing::AnitaRing_t ring,
					   Int_t phi,
					   AnitaPol::AnitaPol_t pol,
					   Int_t &surf, Int_t &chan, Int_t &ant); ///< Convert ring-phi-pol to surf-chan-ant
  static void getRingAntPolPhiFromSurfChan(Int_t surf, Int_t chan,
					  AnitaRing::AnitaRing_t &ring,
					  Int_t &ant,
					  AnitaPol::AnitaPol_t &pol,
					   Int_t &phi); ///< Convert surf-chan to ring-ant-pol-phi
  static Int_t getChanIndex(Int_t surf, Int_t chan);///< Surf + channel to channel index
  static Int_t getChanIndexFromRingPhiPol(AnitaRing::AnitaRing_t ring,
					Int_t phi,
					AnitaPol::AnitaPol_t pol); ///< Convert ring-phi-pol to logical index
  static Int_t getChanIndexFromAntPol(Int_t ant,
				    AnitaPol::AnitaPol_t pol); ///< Convert ant-pol to logical index
  static Int_t getSurfFromAnt(Int_t ant);
  static Int_t getChanFromAntPol(Int_t ant,AnitaPol::AnitaPol_t pol);
  static Int_t getSurfChanFromChanIndex(Int_t chanIndex, // input channel index
				      Int_t &surf,Int_t &chan); ///< Convert logical index to  surf and channel
  static Int_t getPhiRingPolFromSurfChanTrigger(Int_t surf, Int_t chan, Int_t &phi,
						AnitaRing::AnitaRing_t &ring,AnitaPol::AnitaPol_t &pol);
  static Int_t getSurfChanTriggerFromPhiRingPol(Int_t phi, AnitaRing::AnitaRing_t ring,
						AnitaPol::AnitaPol_t pol,Int_t &surf, Int_t &chan);
  static Int_t getPhiPolFromSurfL1Chan(Int_t surf, Int_t l1Chan, Int_t &phi,
				       AnitaPol::AnitaPol_t &pol);
  static Int_t getSurfL1TriggerChanFromPhiPol(Int_t phi, AnitaPol::AnitaPol_t pol,
					      Int_t &surf, Int_t &l1Chan);
  static Int_t getAntPolFromSurfChan(Int_t surf, Int_t chan, Int_t &ant,
				     AnitaPol::AnitaPol_t &pol); ///< Convert surf-chan to ant-pol
  static Int_t getAzimuthPartner(Int_t rx); ///< output the antenna that points to the same place in phi as the input antenna
  static void getThetaPartners(Int_t rx, int& rxleft, int& rxright); ///< output the antennas that are in neighbouring phi sectors (not the neighbouring antennas)
  static void getNeighbors(Int_t rx, int& rxleft, int& rxright); ///< output the neighboring antennas 
  static Int_t getPhiSector(Int_t rx); ///< phi sector of this antenna.  rx runs from 0 to 31.
  static Int_t getLayer(Int_t irx); ///< get layer given antenna number
  static Int_t getPhiFromAnt(Int_t ant); ///< get phi from ant
  static Int_t getAntFromPhiRing(Int_t phi, AnitaRing::AnitaRing_t ring); ///< get antenna number from phi and ring
  static Int_t getAntOrientation(Int_t ant); ///< Some of the antennas have their orientation reversed relative to nominal. The effect of this is to switch the sign the of the signal (as up is down and left is right). Returns 1 for nominal orientation and -1 for flipped.
  static Double_t getPhiDiff(Double_t firstPhi, Double_t secondPhi); ///< Returns the angular difference between two phi values (in radians)




  /********************************************************************************************************
  Non-static member functions (requires initialization of class for accessing data)
  ********************************************************************************************************/
  Double_t getDirectionWrtNorth(Int_t phi, Double_t heading); ///< Get direction that a phi sector is pointing wrt north.  Also takes heading as a input.

  void fillAntPositionsFromPrioritizerdConfig(); ///< Used for evaluating ANITA-3 Prioritizerd performance
  void getAntXYZ(Int_t ant, Double_t &x, Double_t &y, Double_t &z,
		 AnitaPol::AnitaPol_t pol=AnitaPol::kVertical); ///< get antenna cartesian coordinates (from photogrammetry)
  Double_t getAntZ(Int_t ant, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical); ///< get antenna z position
  Double_t getAntR(Int_t ant, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical); ///< get antenna r position
  Double_t getAntPhiPosition(Int_t ant, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical); ///< get antenna phi position
  Double_t getAntPhiPositionRelToAftFore(Int_t ant, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical); ///< get antenna phi position relative to ADU5 AFT-FORE line
  Double_t getMeanAntPairPhiRelToAftFore(Int_t firstAnt, Int_t secondAnt, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical); ///< The mean of the two antenna phi positions
  Int_t getTopAntNearestPhiWave(Double_t phiWave, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical); ///< get antenna closest to given plane wave direction
  Int_t getUpperAntNearestPhiWave(Double_t phiWave, AnitaPol::AnitaPol_t pol=AnitaPol::kVertical);


  
  void getAntFaceXYZ(Int_t ant, Double_t &x, Double_t &y, Double_t &z); ///< get location fo antenna face in balloon cartesian coordinates
  Double_t getAntFaceZ(Int_t ant); ///< get z position for antenna face
  Double_t getAntFaceR(Int_t ant); ///< get r position for antenna face
  Double_t getAntFacePhiPosition(Int_t ant); ///< get phi position for antenna face
  Double_t getAntFacePhiPositionRelToAftFore(Int_t ant); ///< get phi position relative to ADU5 AFT-FORE direction
  Int_t getTopAntFaceNearestPhiWave(Double_t phiWave); ///< get upper antenna closest to given plane wave direction
  Int_t getUpperAntFaceNearestPhiWave(Double_t phiWave) { return getTopAntFaceNearestPhiWave(phiWave);}///< get upper antenna closest to given plane wave direction

  //  Double_t phaseCentreToAntFront; //m
  
  
  Double_t xAntFromDeckHorn[NUM_SEAVEYS]; //m
  Double_t yAntFromDeckHorn[NUM_SEAVEYS]; //m
  Double_t zAntFromDeckHorn[NUM_SEAVEYS]; //m
  Double_t rAntFromDeckHorn[NUM_SEAVEYS]; //m
  Double_t azCentreFromDeckHorn[NUM_SEAVEYS]; //radians
  Double_t apertureAzFromDeckHorn[NUM_SEAVEYS]; //radians
  Double_t apertureElFromDeckHorn[NUM_SEAVEYS]; //radians
  
   
   Double_t xPhaseCentreFromDeckHorn[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t yPhaseCentreFromDeckHorn[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t zPhaseCentreFromDeckHorn[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t rPhaseCentreFromDeckHorn[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t azPhaseCentreFromDeckHorn[NUM_SEAVEYS][NUM_POLS]; //radians

   
   Double_t xAntFromDeckBicone[NUM_BICONES]; //m
   Double_t yAntFromDeckBicone[NUM_BICONES]; //m
   Double_t zAntFromDeckBicone[NUM_BICONES]; //m

   Double_t xAntFromDeckDiscone[NUM_DISCONES]; //m
   Double_t yAntFromDeckDiscone[NUM_DISCONES]; //m
   Double_t zAntFromDeckDiscone[NUM_DISCONES]; //m

   Double_t xAnitaBoxFromDeckCorner[4];
   Double_t yAnitaBoxFromDeckCorner[4];
   Double_t zAnitaBoxFromDeckCorner[4];
   Double_t xBatteryBoxFromDeckCorner[4];
   Double_t yBatteryBoxFromDeckCorner[4];
   Double_t zBatteryBoxFromDeckCorner[4];
   Double_t xSipBoxFromDeckCorner[4];
   Double_t ySipBoxFromDeckCorner[4];
   Double_t zSipBoxFromDeckCorner[4];
   
   Double_t gpsPlaneFromDeck[3];
   Double_t gpsHeadingFromDeck[3];
   Double_t aftForeOffsetAngleDeck;

   Double_t xAntFromVerticalHorn[NUM_SEAVEYS]; //m
   Double_t yAntFromVerticalHorn[NUM_SEAVEYS]; //m
   Double_t zAntFromVerticalHorn[NUM_SEAVEYS]; //m
   Double_t rAntFromVerticalHorn[NUM_SEAVEYS]; //m
   Double_t azCentreFromVerticalHorn[NUM_SEAVEYS]; //radians
   Double_t apertureAzFromVerticalHorn[NUM_SEAVEYS]; //radians
   Double_t apertureElFromVerticalHorn[NUM_SEAVEYS]; //radians
  
   Double_t xPhaseCentreFromVerticalHorn[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t yPhaseCentreFromVerticalHorn[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t zPhaseCentreFromVerticalHorn[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t rPhaseCentreFromVerticalHorn[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t azPhaseCentreFromVerticalHorn[NUM_SEAVEYS][NUM_POLS]; //radians

   
   Double_t xAntFromVerticalBicone[NUM_BICONES]; //m
   Double_t yAntFromVerticalBicone[NUM_BICONES]; //m
   Double_t zAntFromVerticalBicone[NUM_BICONES]; //m

   Double_t xAntFromVerticalDiscone[NUM_DISCONES]; //m
   Double_t yAntFromVerticalDiscone[NUM_DISCONES]; //m
   Double_t zAntFromVerticalDiscone[NUM_DISCONES]; //m

   Double_t xAnitaBoxFromVerticalCorner[4];
   Double_t yAnitaBoxFromVerticalCorner[4];
   Double_t zAnitaBoxFromVerticalCorner[4];
   Double_t xBatteryBoxFromVerticalCorner[4];
   Double_t yBatteryBoxFromVerticalCorner[4];
   Double_t zBatteryBoxFromVerticalCorner[4];
   Double_t xSipBoxFromVerticalCorner[4];
   Double_t ySipBoxFromVerticalCorner[4];
   Double_t zSipBoxFromVerticalCorner[4];
   
   Double_t gpsPlaneFromVertical[3];
   Double_t gpsHeadingFromVertical[3];
   Double_t aftForeOffsetAngleVertical;

   //Simon's position calib numbers
   Double_t deltaRPhaseCentre[NUM_SEAVEYS][NUM_POLS]; //Relative to photogrammetry + ring offset
   Double_t deltaZPhaseCentre[NUM_SEAVEYS][NUM_POLS]; //Relative to photogrammetry + ring offset
   Double_t deltaPhiPhaseCentre[NUM_SEAVEYS][NUM_POLS]; //Relative to photogrammetry + ring offset
   Double_t ringPhaseCentreOffset[3]; //Offset for each ring, upper lower and nadir


   TVector3 fHeadingRotationAxis;
   TVector3 fPitchRotationAxis;
   TVector3 fRollRotationAxis;


   //Kurt's ANITA-3 numbers
   Double_t xAntFromVerticalHornKurtAnita3[NUM_SEAVEYS]; //m
   Double_t yAntFromVerticalHornKurtAnita3[NUM_SEAVEYS]; //m
   Double_t zAntFromVerticalHornKurtAnita3[NUM_SEAVEYS]; //m
   Double_t rAntFromVerticalHornKurtAnita3[NUM_SEAVEYS]; //m
   Double_t azCentreFromVerticalHornKurtAnita3[NUM_SEAVEYS]; //radians
   Double_t apertureAzFromVerticalHornKurtAnita3[NUM_SEAVEYS]; //radians
   Double_t apertureElFromVerticalHornKurtAnita3[NUM_SEAVEYS]; //radians
  
   Double_t xPhaseCentreFromVerticalHornKurtAnita3[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t yPhaseCentreFromVerticalHornKurtAnita3[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t zPhaseCentreFromVerticalHornKurtAnita3[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t rPhaseCentreFromVerticalHornKurtAnita3[NUM_SEAVEYS][NUM_POLS]; //m
   Double_t azPhaseCentreFromVerticalHornKurtAnita3[NUM_SEAVEYS][NUM_POLS]; //radians
   Double_t aftForeOffsetAngleVerticalKurtAnita3; //radians

   void updateAnt(Double_t deltaR,Double_t deltaRL,Double_t deltaUD);
   void printAntPos();

   void useKurtAnita3Numbers(Int_t flag) {
     fUseKurtAnita3Numbers=flag;
   }


  inline Double_t getGeoid(Double_t theta) {
    Double_t c=TMath::Cos(theta);
    return GEOID_MIN*GEOID_MAX/TMath::Sqrt(GEOID_MIN*GEOID_MIN-
					   (GEOID_MIN*GEOID_MIN-GEOID_MAX*GEOID_MAX)*c*c);    
  }   ///<Returns the geoid radiuus as a function of theta (the polar angle?)
  
  void getCartesianCoords(Double_t lat, Double_t lon, Double_t alt, Double_t p[3]);
  void getLatLonAltFromCartesian(Double_t p[3], Double_t &lat, Double_t &lon, Double_t &alt);
  Double_t getDistanceToCentreOfEarth(Double_t lat);

  inline Double_t getLat(Double_t theta) {      
     return (90.-((theta*TMath::RadToDeg()))); 
  } ///< Converts polar angle to latitude
  
  inline Double_t getLon(Double_t phi){ 
     //Need to fix this somehow
     Double_t phi_deg = phi*TMath::RadToDeg();
     if (phi_deg>270)
	phi_deg-=360;	
     return (90.-phi_deg);
  } ///< Converts a azimuthal angle to longitude

  inline Double_t getThetaFromLat(Double_t lat) {       
     return (90.- lat)*TMath::DegToRad(); 
  } ///< Converts latitude to polar angle
  
  inline Double_t getPhiFromLon(Double_t lon){ 
     //Need to fix this somehow
     Double_t phi_deg = 90. - lon;
     if(phi_deg<0) phi_deg+=360;
     return phi_deg*TMath::DegToRad();
  } ///<Converts longitude to azimuthal angle
  
  inline Double_t getPhi(Double_t p[3]){
      // returns phi between 0 and 2pi.
        Double_t pt=0;
        Double_t phi=0;
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
  } ///<Converts cartesian coordinates to azimuthal angle

     inline Double_t getPhi(TVector3 &thePos) {
       Double_t p[3]={thePos.X(),thePos.Y(),thePos.Z()};
       return getPhi(p);
       //return thePos.Theta();
    } ///<Converts cartesian coordinates to azimuthal angle

     inline Double_t getTheta(Double_t p[3]) {
      Double_t pz,pt;
      Double_t tantheta1=0;
      Double_t theta=0;

      pz=p[2];
      pt=sqrt(p[0]*p[0]+p[1]*p[1]);
      tantheta1=pt/pz;
      theta=atan(tantheta1);

      if (pz<0)
        theta += TMath::Pi();
      return theta;
    } ///<Converts cartesian coordinates to polar angle


     inline Double_t getTheta(TVector3 &thePos) {
	Double_t p[3]={thePos.X(),thePos.Y(),thePos.Z()};
	thePos.GetXYZ(p);
	return getTheta(p);
     } ///<Converts cartesian coordinates to polar angle
     
     inline void getLonLat(Double_t p[3],Double_t& lon,Double_t& lat) {
        lon=getLon(getPhi(p));
        lat=getLat(getTheta(p));
    } ///<Converts cartesian coordinates to latitude and longitude

     inline void getLonLat(TVector3 &thePos,Double_t& lon,Double_t& lat) {
        lon=getLon(getPhi(thePos));
        lat=getLat(getTheta(thePos));
    } ///<Converts cartesian coordinates to latitude and longitude

 protected:
   static AnitaGeomTool *fgInstance;  
   // protect against multiple instances

 private:
  void readPhotogrammetry();
  void readAnita3Photogrammetry();
  void readSimonsNumbers();
  void readAnita3PhaseCenterNumbers();
  Int_t fUseKurtAnita3Numbers;
  


};



  
#endif //ANITAGEOMTOOL_H
