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
#include "AnitaVersion.h" 

ClassImp(UsefulAnitaEvent);


UsefulAnitaEvent::UsefulAnitaEvent() 
  : RawAnitaEvent()
{
  gotCalibTemp=0;
  fLastEventGuessed=0;
  fC3poNum=0;
  fFromCalibratedAnitaEvent=0;
  fCalibrator=0;
  setAlfaFilterFlag(false);

  //Default Constructor
}


UsefulAnitaEvent::UsefulAnitaEvent(CalibratedAnitaEvent *calibratedPtr, WaveCalType::WaveCalType_t calType) 
  : RawAnitaEvent(*calibratedPtr)
{

  // std::cout << __PRETTY_FUNCTION__ << std::endl;
  fCalibrator=0;
  fFromCalibratedAnitaEvent=1;
  // fC3poNum=calibratedPtr->fC3poNum;
  fLastEventGuessed=calibratedPtr->eventNumber;
  gotCalibTemp=0;
  fClockProblem = calibratedPtr->fClockProblem;
  for(int surf=0;surf<NUM_SURF;surf++) {
    fRcoArray[surf]=calibratedPtr->fRcoArray[surf];
    fClockPhiArray[surf]=calibratedPtr->fClockPhiArray[surf];
    fTempFactorGuesses[surf]=calibratedPtr->fTempFactorGuesses[surf];
  }
  fCalType=calType;
  calibrateEvent(fCalType);
  setAlfaFilterFlag(AnitaVersion::get() == 3);  
}


UsefulAnitaEvent::UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, PrettyAnitaHk *theHk) 
  : RawAnitaEvent(*eventPtr)
{


  //In this case, we can automatically detect the ANITA version  
  AnitaVersion::setVersionFromUnixTime(theHk->realTime); 

  checkIfTreatingCalibratedEventAsRawEvent(eventPtr, __PRETTY_FUNCTION__);

  fCalibrator=0;
  fLastEventGuessed=0;
  fFromCalibratedAnitaEvent=0;  
  if(theHk) {
    gotCalibTemp=1;
    calibTemp=theHk->intTemps[2];
  }
  else{
    gotCalibTemp=0;
  }

  setAlfaFilterFlag(AnitaVersion::get() == 3);  
  calibrateEvent(calType);
}

UsefulAnitaEvent::UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, RawAnitaHeader* theHd) 
  : RawAnitaEvent(*eventPtr)
{

  //In this case, we can automatically detect the ANITA version  
  AnitaVersion::setVersionFromUnixTime(theHd->realTime); 

  checkIfTreatingCalibratedEventAsRawEvent(eventPtr, __PRETTY_FUNCTION__);

  fCalibrator=0;
  fFromCalibratedAnitaEvent=0;
  fC3poNum=theHd->c3poNum;
  fLastEventGuessed=0;
  gotCalibTemp=0;
  fClockProblem = 0;


  setAlfaFilterFlag(AnitaVersion::get() == 3);  

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

  checkIfTreatingCalibratedEventAsRawEvent(eventPtr, __PRETTY_FUNCTION__);

  fCalibrator=0;
  fFromCalibratedAnitaEvent=0;
  gotCalibTemp=1;
  calibTemp=surfTemp;
  setAlfaFilterFlag(false);
     
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

#ifdef USE_FFT_TOOLS 
  // let's try the simplest thing first
  const int alfaChannelIndex = 11*NUM_CHAN + 5;
  if(fFilterAlfaChannel==true && chanIndex==alfaChannelIndex){    
    const double lowPassFreq = 700; // MHz
    TGraph* grFiltered = FFTtools::simplePassBandFilter(grPtr, 0, lowPassFreq);
    delete grPtr;
    grPtr = grFiltered;
  }
#endif
  return grPtr;
}


TGraph *UsefulAnitaEvent::getDeconvolvedALFA()
{
  // todo...

  // You should put the code that uses FFTtools inside the include guard.
#ifdef USE_FFT_TOOLS
  // 
#endif

  return NULL;
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


Int_t UsefulAnitaEvent::checkIfTreatingCalibratedEventAsRawEvent(RawAnitaEvent* rawEventPtr, const char* funcName){
  // This function is a check that you're not treating a CalibratedAnitaEvent as a RawAnitaEvent.
  // If you are then it will print a warning.

  // There is probably some alternative that could cause things to fail to compile... would that be better?
  // Is a it better to turn a bug into a run-time warning or a compile time error?
  // Anyway, enough philosophising...


  // This line should return NULL if you can't dynamically cast the rawEventPtr as a pointer to a CalibratedAnitaEvent.
  // i.e. if the pointer really is of type RawAnitaEvent this will return NULL.

  CalibratedAnitaEvent* calEventPtr = dynamic_cast<CalibratedAnitaEvent*>(rawEventPtr);

  // If you can cast it as a pointer to a CalibratedAnitaEvent...
  if(calEventPtr){
    // ... then this is a calibratedAnitaEvent getting treated like a RawAnitaEvent, i.e. being calibrated on the fly.
    // At best this slow. At worst this will mess up things like the rolling temperature correction.
    // Now I will fill your terminal with warnings so verbose you will hate me.

    std::cerr << "WARNING in " << funcName << ":" << std::endl;
    std::cerr << "\tCalling constructor that that takes a RawAnitaEvent* with a CalibratedAnitaEvent*. " << std::endl;
    std::cerr << "\tThe CalibratedAnitaEvent numbers are not being applied but are being recalculated on the fly." << std::endl;
    std::cerr << "\tAt best this is slow. At worst this could change rolling averages." << std::endl;
    std::cerr << "\tPlease use the constructor that takes a CalibratedAnitaEvent* instead." << std::endl;
    return 1;
  }
  else{
    // If it really is a RawAnitaEvent, then that's fine.
    return 0;
  }
}


Bool_t UsefulAnitaEvent::getAlfaFilterFlag(){
#ifdef USE_FFT_TOOLS
  return fFilterAlfaChannel;
#else
  return false;
#endif
}

Bool_t UsefulAnitaEvent::setAlfaFilterFlag(Bool_t newBoolianFlag){
#ifdef USE_FFT_TOOLS
  fFilterAlfaChannel = newBoolianFlag;
#else
  fFilterAlfaChannel = false;
#endif
  return fFilterAlfaChannel;
}
  
