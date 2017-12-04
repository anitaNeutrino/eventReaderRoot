//////////////////////////////////////////////////////////////////////////////
/////  TruthAnitaEvent.h        Truth ANITA event class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing truth ANITA info in a TTree         /////
/////  Author: Linda Cremonesi (l.cremonesi@hep.ucl.ac.uk)               /////
//////////////////////////////////////////////////////////////////////////////

#ifndef TRUTHANITAEVENT_H
#define TRUTHANITAEVENT_H

//Includes
#include <TObject.h>

#include "AnitaConventions.h"

//!  TruthAnitaEvent -- The Truth ANITA Event 
/*!
  The ROOT implementation of the true ANITA event filled in icemc
  \ingroup rootclasses
*/
class TruthAnitaEvent: public TObject
{
 public:
  TruthAnitaEvent(); ///< Default constructor
  ~TruthAnitaEvent(); ///< Destructor

  Int_t           run;                    ///< Run number
  UInt_t          realTime;               ///< unixTime of readout
  UInt_t          eventNumber;            ///< Software event number

  Double_t        balloonPos[3];          ///< Balloon position
  Double_t        balloonDir[3];          ///< Balloon direction
  Double_t        nuPos[3];               ///< Neutrino position
  Double_t        nuDir[3];               ///< Neutrino direction
  Double_t        nuMom;                  ///< Neutrino momentum
  Double_t        rfExitNor[5][3];        ///< Normal vector in direction of exit point to balloon - 5 iterations
  Double_t        rfExitPos[5][3];        ///< Position where the RF exits the ice- 5 iterations, 3 dimensions each
  Int_t           nu_pdg;                 ///< Neutrino PDG code
  Double_t        e_component;            ///< E comp along polarization
  Double_t        h_component;            ///< H comp along polarization
  Double_t        n_component;            ///< Normal comp along polarization
  Double_t        e_component_k;          ///< Component of e-field along the rx e-plane
  Double_t        h_component_k;          ///< Component of the e-field along the rx h-plane
  Double_t        n_component_k;          ///< Component of the e-field along the normal 
  Double_t        hitangle_e[48];         ///< Hit angles rel. to e plane stored for each antenna
  Double_t        hitangle_h[48];         ///< Hit angles rel. to h plane stored for each antenna
  Double_t        vmmhz[128];             ///< V/m/MHz at balloon (128 frequency bins)
  Double_t        vmmhz_max;              ///< Maximum signal at balloon (V/m/MHz)
  Double_t        payloadPhi;             ///< Phi of signal in payload coordinates (degrees)
  Double_t        payloadTheta;           ///< Theta of signal in payload coordinates (degrees)
  Double_t        sourceLon;              ///< RF position when leaving the ice: Longitude (using icemc model)
  Double_t        sourceLat;              ///< RF position when leaving the ice: Latitude  (using icemc model)
  Double_t        sourceAlt;              ///< RF position when leaving the ice: Altitude  (using icemc model)
  Double_t        weight;                 ///< Weight assigned by icemc


  Double_t fTimes[NUM_DIGITZED_CHANNELS][NUM_SAMP];           ///< Array of unwrapped (unless kNoCalib) times for each channel
  Double_t fSignalAtTrigger[NUM_DIGITZED_CHANNELS][NUM_SAMP]; ///< Array of signal at trigger
  Double_t fNoiseAtTrigger[NUM_DIGITZED_CHANNELS][NUM_SAMP];  ///< Array of noise at trigger
  Double_t fDiodeOutput[NUM_DIGITZED_CHANNELS][NUM_SAMP];     ///< Array of tunnel diode output
  Double_t SNRAtTrigger[NUM_DIGITZED_CHANNELS];               ///< Array of SNR at trigger
  Double_t maxSNRAtTriggerV;                                  ///< Max SNR at trigger V-POL
  Double_t maxSNRAtTriggerH;                                  ///< Max SNR at trigger H-POL
  Double_t thresholds[NUM_DIGITZED_CHANNELS];                 ///< Channel thresholds used in icemc
  
  ClassDef(TruthAnitaEvent,4);
};


#endif //TRUTHANITAEVENT_H
