//////////////////////////////////////////////////////////////////////////////
/////  TruthAnitaEvent.cxx      Truth ANITA event class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing truth ANITA info in a TTree         /////
/////  Author: Linda Cremonesi (l.cremonesi@hep.ucl.ac.uk)               /////
//////////////////////////////////////////////////////////////////////////////

#include "TruthAnitaEvent.h"

ClassImp(TruthAnitaEvent);

TruthAnitaEvent::TruthAnitaEvent()
{
 
  run =-1;
  realTime = 0;
  eventNumber = 0;
  memset(balloonPos,0,sizeof(balloonPos)); 
  memset(balloonDir,0,sizeof(balloonDir)); 
  memset(nuPos,0,sizeof(nuPos)); 
  memset(nuDir,0,sizeof(nuDir)); 
  nuMom = 0; 
  showerE= 0; 
  memset(rfExitPos,0,sizeof(rfExitPos)); 
  memset(rfExitPos,0,sizeof(rfExitPos)); 
  nu_pdg = -999; 
  memset(e_component,0,sizeof(e_component)); 
  memset(h_component,0,sizeof(h_component)); 
  memset(e_component_k,0,sizeof(e_component_k)); 
  memset(h_component_k,0,sizeof(h_component_k)); 
  memset(n_component_k,0,sizeof(n_component_k)); 
  memset(hitangle_e,0,sizeof(hitangle_e));
  memset(hitangle_h,0,sizeof(hitangle_h));
  memset(vmmhz,0,sizeof(vmmhz));
  vmmhz_max = -999; 
  payloadPhi = -999; 
  payloadTheta = -999; 
  sourceLon = -999;
  sourceLat = -999;
  sourceAlt = -999;
  weight = -1; 
  weight1 = -1;
  phaseWeight =-1; 
  timeWeight=-1; 
  sourceTimeWeight = -1; 
  l_int = 0; 
  projectedArea=-1;
  RA=-999; 
  dec=-999;
  objName="None";
  source_index = -1;

  memset(fTimes,0,sizeof(fTimes));
  memset(fSignalAtTrigger,0,sizeof(fSignalAtTrigger));
  memset(fNoiseAtTrigger,0,sizeof(fNoiseAtTrigger));
  memset(fSignalAtDigitizer,0,sizeof(fSignalAtDigitizer));
  memset(fNoiseAtDigitizer,0,sizeof(fNoiseAtDigitizer));
  memset(fDiodeOutput,0,sizeof(fDiodeOutput));
  memset(SNRAtTrigger,0,sizeof(SNRAtTrigger));
  memset(SNRAtDigitizer,0,sizeof(SNRAtDigitizer));
  maxSNRAtTriggerV = 0;
  maxSNRAtTriggerH = 0;
  maxSNRAtDigitizerV = 0;
  maxSNRAtDigitizerH = 0;
  memset(thresholds,0,sizeof(thresholds)); 
  tuffIndex=-1; 

  canary_09f91102 = 0x09f91102; 
  canary_12345 = 0x12345; 
  canary_deadbeef = 0xdeadbeef; 
  canary_c0fefe = 0xc0fefe; 

  weight_prob = 0; 

  //Default Constructor
}

TruthAnitaEvent::~TruthAnitaEvent() {
   //Default Destructor
   
  if (canary_09f91102 != 0x09f91102) fprintf(stderr,"WARNING: Canary value changed, expected %x, got %x",0x09f91102, canary_09f91102);
  if (canary_12345 != 0x12345) fprintf(stderr,"WARNING: Canary value changed, expected %x, got %x",0x12345, canary_12345);
  if (canary_deadbeef != 0xdeadbeef) fprintf(stderr,"WARNING: Canary value changed, expected %x, got %x",0xdeadbeef, canary_deadbeef);
  if (canary_c0fefe != 0xc0fefe) fprintf(stderr,"WARNING: Canary value changed, expected %x, got %x",0xc0fefe, canary_c0fefe);

}
