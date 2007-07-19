//////////////////////////////////////////////////////////////////////////////
/////  UsefulAnitaEvent.h        Useful ANITA event class                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing raw ANITA events in a TTree         /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef USEFULANITAEVENT_H
#define USEFULANITAEVENT_H

//Includes
#include <TObject.h>
#include <TGraph.h>
#include "RawAnitaEvent.h"
#include "PrettyAnitaHk.h"
#include "AnitaConventions.h"
#include "AnitaEventCalibrator.h"

class UsefulAnitaEvent: public RawAnitaEvent
{

 public:
  UsefulAnitaEvent();
  UsefulAnitaEvent(RawAnitaEvent *eventPtr,WaveCalType::WaveCalType_t calType, PrettyAnitaHk *theHk=0);
  ~UsefulAnitaEvent();
  int calibrateEvent(WaveCalType::WaveCalType_t calType);

  //Generally useful function
  static int getChanIndex(int surf, int chan)
     {return chan+(9*surf);}


  TGraph *getGraph(int surf, int chan);
  TGraph *getGraph(int chanIndex);
  TGraph *getGraph(int ant, AnitaPol::AnitaPol_t pol);
  TGraph *getGraph(AnitaRing::AnitaRing_t ring,
		   int phi,
		   AnitaPol::AnitaPol_t pol);


  //The calibrated numbers
  Int_t gotCalibTemp;
  Float_t calibTemp;
  WaveCalType::WaveCalType_t fCalType;
  int fNumPoints[NUM_DIGITZED_CHANNELS];
  double fVolts[NUM_DIGITZED_CHANNELS][NUM_SAMP];
  double fTimes[NUM_DIGITZED_CHANNELS][NUM_SAMP];
 

  AnitaEventCalibrator *fCalibrator;

  ClassDef(UsefulAnitaEvent,1);
};


#endif //USEFULANITAEVENT_H
