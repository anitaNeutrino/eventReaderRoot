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
  setAlfaFilterFlag(true);

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
  setAlfaFilterFlag(true);
}


UsefulAnitaEvent::UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, PrettyAnitaHk *theHk)
  : RawAnitaEvent(*eventPtr)
{


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
  setAlfaFilterFlag(true);

  calibrateEvent(calType);
}

UsefulAnitaEvent::UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, RawAnitaHeader* theHd)
  : RawAnitaEvent(*eventPtr)
{

  checkIfTreatingCalibratedEventAsRawEvent(eventPtr, __PRETTY_FUNCTION__);

  fCalibrator=0;
  fFromCalibratedAnitaEvent=0;
  fC3poNum=theHd->c3poNum;
  fLastEventGuessed=0;
  gotCalibTemp=0;
  fClockProblem = 0;
  setAlfaFilterFlag(true);
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
  setAlfaFilterFlag(true);

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

    const double nominalDeltaT = 1./2.6;
    TGraph* grInterpolated = FFTtools::getInterpolatedGraph(grPtr, nominalDeltaT);
    delete grPtr;

    TGraph* grFiltered = FFTtools::simplePassBandFilter(grInterpolated, 0, lowPassFreq);
    delete grInterpolated;

    grPtr = grFiltered;
  }
#endif
  return grPtr;
}


TGraph *UsefulAnitaEvent::getDeconvolvedALFA(bool filterLo, int downConvertMode)
{
  // todo...
  // For ANITA-3, ALFA was upconverted to 710-870 MHz (mixed with 790 MHz LO) and coupled with 05TH
  const int alfaChannelIndex = 11*NUM_CHAN + 5;
    
  const double freqLo = 790.0;
  const double alfaFreqMin = 25.0;
  const double alfaFreqMax = 80.0;
  double freqMin = 0.;
  double freqMax = 0.;
    
  //only works if the 700 MHz low pass filter is turned off
  setAlfaFilterFlag(false);
  TGraph *grPtr = getGraph(alfaChannelIndex);
    
  // You should put the code that uses FFTtools inside the include guard.
#ifdef USE_FFT_TOOLS
  
   //choose the sideband to downConvert
  switch( downConvertMode ){
      case 1: // lower sideband
          freqMin = freqLo - alfaFreqMax;
          freqMax = freqLo - alfaFreqMin;
          break;
      case 2: // upper sideband
          freqMin = freqLo + alfaFreqMin;
          freqMax = freqLo + alfaFreqMax;
          break;
          
      case 3: // both sidebandss
          freqMin = freqLo - alfaFreqMax;
          freqMax = freqLo + alfaFreqMax;
          break;

      case 4: // I-Q downconversion
          // not implemented yet
          return grPtr;
          break;
      default:
          //lower sideband
          freqMin = freqLo - alfaFreqMax;
          freqMax = freqLo - alfaFreqMin;
          break;
  }
  
  // need to notch filter the LO
  grPtr = FFTtools::simpleNotchFilter(grPtr, freqLo - freqMin, freqLo + freqMin);
  // now select the sideband to downConvert
  grPtr = FFTtools::simplePassBandFilter(grPtr, freqMin, freqMax );

  int length = grPtr->GetN();
  FFTWComplex *fft = FFTtools::doFFT(length, grPtr->GetY());
   
  // initialize variables for the downconversion
  double *timeArray = grPtr->GetY();
  double deltaT = timeArray[1]-timeArray[0];
  double deltaF = 1./(deltaT*length);
  int loIndex = floor(freqLo/deltaF);
  FFTWComplex* downcon = new FFTWComplex[length];
  for(int iFreqIndex=0; iFreqIndex < length; iFreqIndex++){
      downcon[iFreqIndex].setMagPhase(0., 0.);
  }

  //zero frequency
  downcon[loIndex].setMagPhase(0., 0.);

    int minIndex =0;
    int maxIndex =0;
  switch( downConvertMode ){
    case 1:              // lower sideband
          minIndex = 0;
          maxIndex = loIndex;
              for(int iFreqIndex=minIndex; iFreqIndex < maxIndex; iFreqIndex++)
                {
                    double newMag = fft[loIndex - iFreqIndex].getAbs();
                    double newPhase = fft[loIndex - iFreqIndex].getPhase();
                    
                    // this part assumes that you know the phase of the LO, which we don't
                    // used in testing isoAlfa.C macro
                    // might be useful later on for sine-subtraction
                    /*if(Lower == true)
                        newPhase = loPhase - phase;
                    }
                    else
                    {
                        newPhase = phase - loPhase;
                    }
                                            
                    */
                    
                    fft[iFreqIndex].setMagPhase(newMag,newPhase);
                }
          break;
      case 2:          // upper sideband
          minIndex = 0;
          maxIndex = length - loIndex;

          for(int iFreqIndex=minIndex; iFreqIndex < maxIndex; iFreqIndex++)
          {
              double newMag = fft[loIndex + iFreqIndex].getAbs();
              double newPhase = fft[loIndex + iFreqIndex].getPhase();
              fft[iFreqIndex].setMagPhase(newMag,newPhase);
          }
          break;
      case 3: // lower and upper sidebands
          minIndex = 0;
          maxIndex = loIndex;
          
          for(int iFreqIndex=minIndex; iFreqIndex < maxIndex; iFreqIndex++)
          {
              // right now just averaging the upper and lower sidebands
              // might be able to recover the LO phase
              double newMag = 0.5*(fft[loIndex - iFreqIndex].getAbs() + fft[loIndex + iFreqIndex].getAbs());
              double newPhase = 0.5*(fft[loIndex - iFreqIndex].getPhase() + fft[loIndex + iFreqIndex].getPhase());
              fft[iFreqIndex].setMagPhase(newMag,newPhase);
          }
          break;
      case 4:
          //not yet implemented
          break;
      default:
          minIndex = 0;
          maxIndex = loIndex;
          for(int iFreqIndex=minIndex; iFreqIndex < maxIndex; iFreqIndex++)
          {
              double newMag = fft[loIndex - iFreqIndex].getAbs();
              double newPhase = fft[loIndex - iFreqIndex].getPhase();
              fft[iFreqIndex].setMagPhase(newMag,newPhase);
          }
          break;
          
  }
  double *downWave = FFTtools::doInvFFT(length, fft);
    for(int iPoint=0; iPoint<grPtr->GetN(); iPoint++){
        grPtr->SetPoint(iPoint, timeArray[iPoint], downWave[iPoint]);

    }  //
#endif
  
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
