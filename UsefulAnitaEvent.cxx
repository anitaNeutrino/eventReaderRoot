//////////////////////////////////////////////////////////////////////////////
/////  UsefulAnitaEvent.cxx        ANITA event reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in useful ANITA events and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "UsefulAnitaEvent.h"
#include "CalibratedAnitaEvent.h"
#include "AnitaGeomTool.h"
#include "PrettyAnitaHk.h"
#include "RawAnitaHeader.h"
#include "AnitaEventCalibrator.h"
#include <TGraph.h>
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(UsefulAnitaEvent);


UsefulAnitaEvent::UsefulAnitaEvent() 
  : RawAnitaEvent()
{
  gotCalibTemp=0;
  fLastEventGuessed=0;
  fC3poNum=0;
  fFromCalibratedAnitaEvent=0;
  fCalibrator=0;

  //Default Constructor
}


UsefulAnitaEvent::UsefulAnitaEvent(CalibratedAnitaEvent *calibratedPtr, WaveCalType::WaveCalType_t calType) 
  : RawAnitaEvent(*calibratedPtr)
{
  fCalibrator=0;
  fFromCalibratedAnitaEvent=1;
  // fC3poNum=calibratedPtr->fC3poNum;
  fLastEventGuessed=calibratedPtr->eventNumber;
  gotCalibTemp=0;
  fTempFactorGuess=calibratedPtr->fTempFactorGuess;
  for(int surf=0;surf<NUM_SURF;surf++) {
    fRcoArray[surf]=calibratedPtr->fRcoArray[surf];
    fClockPhiArray[surf]=calibratedPtr->fClockPhiArray[surf];    
  }
  fCalType=calType;
  calibrateEvent(fCalType);
}

UsefulAnitaEvent::UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, PrettyAnitaHk *theHk) 
  : RawAnitaEvent(*eventPtr)
{
  fCalibrator=0;
  fLastEventGuessed=0;
  if(theHk) {
    gotCalibTemp=1;
    calibTemp=theHk->intTemps[2];
  }
  else 
    gotCalibTemp=0;
   
  calibrateEvent(calType);
}

UsefulAnitaEvent::UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, RawAnitaHeader *theHd) 
  : RawAnitaEvent(*eventPtr)
{
  fCalibrator=0;
  fFromCalibratedAnitaEvent=0;
  fC3poNum=theHd->c3poNum;
  fLastEventGuessed=0;
  gotCalibTemp=0;

  if(theHd->eventNumber != eventPtr->eventNumber){
    std::cerr << "Warning! eventNumber mismatch in " << __FILE__ << " line " << __LINE__ << "." << std::endl;
    std::cerr << "RawAnitaHeader->eventNumber = " << theHd->eventNumber << " but "
	      << "UsefulAnitaEvent->eventNumber = " << eventPtr->eventNumber
	      << std::endl;
  }

  calibrateEvent(calType);
}

UsefulAnitaEvent::UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, Double_t surfTemp) 
  : RawAnitaEvent(*eventPtr)
{
  fCalibrator=0;
  fFromCalibratedAnitaEvent=0;
  gotCalibTemp=1;
  calibTemp=surfTemp;
     
  calibrateEvent(calType);
}

UsefulAnitaEvent::~UsefulAnitaEvent() {
  //Default Destructor
}

int UsefulAnitaEvent::calibrateEvent(WaveCalType::WaveCalType_t calType)
{
  fCalibrator=AnitaEventCalibrator::Instance();
  fCalibrator->calibrateUsefulEvent(this,calType);
  return 0;
}


TGraph *UsefulAnitaEvent::getGraphFromSurfAndChan(int surf, int chan) 
{
  return getGraph(getChanIndex(surf,chan));
}

TGraph *UsefulAnitaEvent::getGraph(int chanIndex)
{
  if(chanIndex<0 || chanIndex>=NUM_DIGITZED_CHANNELS)
    return NULL;
  //   std::cout << chanIndex << std::endl;
   
  TGraph *grPtr=new TGraph(fNumPoints[chanIndex],
			   fTimes[chanIndex],
			   fVolts[chanIndex]);
  return grPtr;
}

TGraph *UsefulAnitaEvent::getGraph(int ant, AnitaPol::AnitaPol_t pol){
  //Antenna numbers count from 0
  return getGraph(AnitaGeomTool::getChanIndexFromAntPol(ant,pol));

}

TGraph *UsefulAnitaEvent::getGraph(AnitaRing::AnitaRing_t ring,
				   int phi,
				   AnitaPol::AnitaPol_t pol){
  //Phi numbers count from 0
  return getGraph(AnitaGeomTool::getChanIndexFromRingPhiPol(ring,phi,pol));

}


Int_t UsefulAnitaEvent::guessRco(int chanIndex){
  // deprecated...

  Int_t surf = chanIndex / NUM_CHAN;
  return fRcoArray[surf];

  //RJN removed clock guessing for now.
  // if(fLastEventGuessed!=this->eventNumber) 
  //   analyseClocksForGuesses();
  // //  if(
  // return fRcoArray[chanIndex/9];
  //return getRCO(chanIndex);
  return -1;
}

Int_t UsefulAnitaEvent::getRcoCorrected(int chanIndex)

{
  int rcoFirm = this->getRCO(chanIndex);
  int rcoSoft = 1-rcoFirm; ///< Factor of -1 in definition...
  return rcoSoft;
}


// std::vector<std::vector<Double_t> > UsefulAnitaEvent::getNeighbouringClockCorrelations(Double_t lowPassClockFilterFreq, Int_t giveGraphsToInputArrays, TGraph* grInterpClocksArray[NUM_SURF], TGraph* grCorClocksArray[NUM_SURF][2]){
//   return fCalibrator->getNeighbouringClockCorrelations(this, lowPassClockFilterFreq, 
// 						       giveGraphsToInputArrays,
// 						       grInterpClocksArray,
// 						       grCorClocksArray);
// }


// std::vector<std::vector<Double_t> > UsefulAnitaEvent::getNeighbouringClockCorrelations2(bool writeGraphs){
//   return fCalibrator->getNeighbouringClockCorrelations2(this, writeGraphs);
// }


std::vector<Double_t> UsefulAnitaEvent::getClockAlignment(std::vector<Int_t> listOfClockNums){
  // std::cout << __PRETTY_FUNCTION__ << "\t" << fCalibrator->numPointsArray << std::endl;
  // for(Int_t surf=0; surf<NUM_SURF; surf++){
  //   std::cout << fCalibrator->numPointsArray[surf] << std::endl;
  // }
  return fCalibrator->getClockAlignment(this,
					fCalibrator->numPointsArray,
					fCalibrator->voltsArray,
					fCalibrator->timeArray,
					listOfClockNums);
}
