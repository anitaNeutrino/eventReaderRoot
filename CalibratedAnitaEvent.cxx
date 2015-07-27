//////////////////////////////////////////////////////////////////////////////
/////  CalibratedAnitaEvent.cxx        ANITA event reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in useful ANITA events and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "CalibratedAnitaEvent.h"
#include "UsefulAnitaEvent.h"
#include "AnitaGeomTool.h"
#include "PrettyAnitaHk.h"
#include "RawAnitaHeader.h"
#include "AnitaEventCalibrator.h"
#include <TGraph.h>
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(CalibratedAnitaEvent);




CalibratedAnitaEvent::CalibratedAnitaEvent() 
  : RawAnitaEvent()
{

  fClockProblem=0;
  for(int surf=0;surf<NUM_SURF;surf++) {
    fRcoArray[surf]=0;
    fClockPhiArray[surf]=0;
    fTempFactorGuesses[surf]=1;
  }
  
  //Default Constructor
}

CalibratedAnitaEvent::CalibratedAnitaEvent(UsefulAnitaEvent *usefulPtr) 
 : RawAnitaEvent(*((RawAnitaEvent*)usefulPtr))
{
  fCalType=usefulPtr->fCalType;
  fClockProblem=usefulPtr->fClockProblem;
  for(int surf=0;surf<NUM_SURF;surf++) {
    fRcoArray[surf]=usefulPtr->fRcoArray[surf];
    fClockPhiArray[surf]=usefulPtr->fClockPhiArray[surf];
    fTempFactorGuesses[surf]=usefulPtr->fTempFactorGuesses[surf];
  }
}


CalibratedAnitaEvent::~CalibratedAnitaEvent() {
  //Default Destructor
}


