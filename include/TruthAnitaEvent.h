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
#include "TVector3.h"
#include "TString.h"
#include <ctime>
#include <cmath> 

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
  Double_t        showerE;                ///< Shower energy  
  Double_t        rfExitNor[5][3];        ///< Normal vector in direction of exit point to balloon - 5 iterations
  Double_t        rfExitPos[5][3];        ///< Position where the RF exits the ice- 5 iterations, 3 dimensions each
  Int_t           nu_pdg;                 ///< Neutrino PDG code
  Int_t           canary_c0fefe;                ///< this shoudl always equal 0xc0fefe 
  Double_t        e_component[48];            ///< E comp along polarization
  Double_t        h_component[48];            ///< H comp along polarization
  Double_t        n_component[48];            ///< Normal comp along polarization
  Double_t        e_component_k[48];          ///< Component of e-field along the rx e-plane
  Double_t        h_component_k[48];          ///< Component of the e-field along the rx h-plane
 Double_t        n_component_k[48];          ///< Component of the e-field along the normal 
  Int_t           canary_deadbeef;           ///< this shoudl always equal 0xdeadbeef 
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
  Double_t        weight_prob;                    ///< weight including probability of interacting! 
  Double_t        weight1;                ///< Absorption weight assigned by icemc
  Double_t        phaseWeight;                 ///< Phase weight assigned by icemc
  Double_t        timeWeight;                 ///< Relative Time weight assigned by icemc
  Double_t        sourceTimeWeight;           ///< Relative Time weight for the given source assigned by icemc
  Double_t        projectedArea;              ///< If in unbiased mode, the appropriate sampling area for this interaction
  Double_t        l_int;                      ///< Interaction length
  // Source origin info
  Double_t        RA;                 ///  Right ascension of source
  Double_t        dec;                 ///  Declination of source
  TString        objName;                 ///  Name of the source
  Int_t source_index; 
  Int_t          canary_12345;            //should always equal 0x12345 


  Double_t fTimes[NUM_DIGITZED_CHANNELS][NUM_SAMP];           ///< Array of unwrapped (unless kNoCalib) times for each channel
  Double_t fSignalAtTrigger[NUM_DIGITZED_CHANNELS][NUM_SAMP]; ///< Array of signal at trigger
  Double_t fNoiseAtTrigger[NUM_DIGITZED_CHANNELS][NUM_SAMP];  ///< Array of noise at trigger
  Double_t fSignalAtDigitizer[NUM_DIGITZED_CHANNELS][NUM_SAMP]; ///< Array of signal at digitizer
  Double_t fNoiseAtDigitizer[NUM_DIGITZED_CHANNELS][NUM_SAMP];  ///< Array of noise at digitizer
  Double_t fDiodeOutput[NUM_DIGITZED_CHANNELS][NUM_SAMP];     ///< Array of tunnel diode output
  Double_t SNRAtTrigger[NUM_DIGITZED_CHANNELS];               ///< Array of SNR at trigger
  Double_t maxSNRAtTriggerV;                                  ///< Max SNR at trigger V-POL
  Double_t maxSNRAtTriggerH;                                  ///< Max SNR at trigger H-POL
  Double_t SNRAtDigitizer[NUM_DIGITZED_CHANNELS];             ///< Array of SNR at digitizer
  Double_t maxSNRAtDigitizerV;                                ///< Max SNR at digitizer V-POL
  Double_t maxSNRAtDigitizerH;                                ///< Max SNR at digitizer H-POL
  Double_t thresholds[NUM_DIGITZED_CHANNELS];                 ///< Channel thresholds used in icemc
  Short_t tuffIndex;                                           ///< TUFF configuration index
  Int_t          canary_09f91102;            //should always equal 09f91102 

  
  ClassDef(TruthAnitaEvent,21);
};


/** Analog of TruthAnitaEvent for all events, including events that don't pass the trigger.
 *  special care is made for space efficiency here since we save for every event! 
 *  
 * */
class TruthAnitaNeutrino
{
  public: 

    double nuE() const {return pow(10,log10_nuMom); }
    double showerE() const { return nuE() * (hadFrac() + emFrac()); }
    TVector3 nuDir() const { return TVector3(_nuDir); } 
    TVector3 nuPos() const { return haveNu() ? TVector3(nupos_R_km*1e3 * cos(nupos_phi) * cos(nupos_theta), 
                                       nupos_R_km*1e3 * sin(nupos_phi) * cos(nupos_theta), 
                                       nupos_R_km*1e3*sin(nupos_theta)) : TVector3(0,0,0); }
        
    TVector3 anitaPos() const { return TVector3(anita_R_km*1e3 * cos(anita_phi) * cos(anita_theta), 
                                       anita_R_km*1e3 * sin(anita_phi) * cos(anita_theta), 
                                       anita_R_km*1e3*sin(anita_theta)); }
 
    bool weightWasSet() const { return have_weight; } 
    double weight() const { return !have_weight ? 0 : pow(10,log10_weight); }
    double phaseSpaceWeight() const { return !have_weight ? 0 : pow(10,log10_phase_weight); }
    double timeWeight() const { return !have_weight ? 0 : pow(10,log10_time_weight); }


    bool fracWasSet() const { return have_frac; } 
    double hadFrac() const { return !have_frac ? 0 : _had_frac; } 
    double emFrac() const { return !have_frac ? 0 : _em_frac; } 
    int nuPdg() const  { return haveNu() ? _nu_pdg : -1; } 

    void setSkipped(bool skip, bool zero_frac = true, bool zero_weights  = true, bool zero_dir = true) {
      if (skip)
      {
        //these won't be defined yet
        if (zero_weights) 
        {
          log10_weight=0;
          log10_phase_weight =0;
          log10_time_weight =0;
          have_weight = false;
        }
        if (zero_frac) 
        {
          _had_frac=0;
          _em_frac=0;
          have_frac = false;
        }
        if (zero_dir) 
        {
          setNoDir(); 
        }
      }
      skipped = skip;
    }
    bool isSkipped() const { return skipped; } 


    void setWeights(double weight =1e-100, double phase_space_weight = 1e-4, double time_weight = 1e-10) 
    {
      log10_weight = log10(weight); 
      log10_phase_weight=log10(phase_space_weight); 
      log10_time_weight=log10(time_weight); 
      have_weight = true;
    }

    void setPos(double x, double y, double z, 
                double bx, double by, double bz,
                int t)
    {
      TVector3 v(x,y,z); 
      nupos_R_km = v.Mag()/1e3; 
      nupos_theta = v.Theta(); 
      nupos_phi = v.Phi(); 

      setBalloon(bx,by,bz,t);
    }

    void setNoDir() 
    {
      have_dir = false;
      _nuDir[0] = 0;
      _nuDir[1] = 0;
      _nuDir[2] = 0;
    }
    void setDir(double dx, double dy, double dz) 
    {
      double maginv = pow(dx*dx+dy*dy+dz*dz,-0.5); 
      _nuDir[0] = dx*maginv; 
      _nuDir[1] = dy*maginv; 
      _nuDir[2] = dz*maginv; 
      have_dir = true;
    }

    void setFrac(double had_frac ,double  em_frac)
    {

      _had_frac = had_frac;
      _em_frac = em_frac;
      have_frac = true; 
    }

    void setNu(double nuE, double nupdg) 
    {
      log10_nuMom = log10(nuE); 
      _nu_pdg = nupdg; 
      have_nu = true; 
    }


    void setNoNu(double bx, double by, double bz,  int t) 
    {
      have_nu = false; 
      setBalloon(bx,by,bz,t); 
      setSkipped(true); 
      //Set these to something for compression goodness
      log10_nuMom = 15; 
      nupos_R_km = 6350; 
      nupos_theta = 0; 
      nupos_phi = 0; 
    }

    virtual ~TruthAnitaNeutrino() {; } 

    bool haveDir() const {return have_dir; }
    bool haveFrac() const { return have_frac; } 
    bool haveWeight() const {return have_weight; }
    bool haveNu() const {return have_nu; }

    int time() const 
    {
      struct tm tinfo; 
      tinfo.tm_sec = sec; 
      tinfo.tm_min = min;
      tinfo.tm_hour = hour; 
      tinfo.tm_mday = day; 
      tinfo.tm_year = year + 100; 
      tinfo.tm_isdst = 0; 
      return mktime(&tinfo);  
    }

  private: 
    Double32_t log10_nuMom; //[15,23,13] 
    Double32_t _nuDir[3]; //[-1,1,16] 
    Double32_t nupos_R_km; //[6350, 6400, 16]; 
    Double32_t nupos_theta; // [0,pi,26]; 
    Double32_t nupos_phi; // [-pi,pi,26]; 
    Double32_t anita_R_km; //[6350, 6450, 17]; 
    Double32_t anita_theta; // [0,pi,26]; 
    Double32_t anita_phi; // [-pi,pi,26]; 
    Double32_t log10_weight; //[-100,0,16] 
    Double32_t log10_phase_weight; //[-4,0,16] 
    Double32_t _had_frac; //[0,1,10]; 
    Double32_t _em_frac; //[0,1,10]; 
    Double32_t log10_time_weight; //[-5,3,16]; 

    void setBalloon(double bx, double by, double bz, int t) 
    {
      TVector3 b(bx,by,bz); 
      anita_R_km = b.Mag()/1e3; 
      anita_theta = b.Theta(); 
      anita_phi = b.Phi(); 
 
      time_t tmp_t = t; 
      struct tm * T = gmtime(&tmp_t); 
      year = T->tm_year-100; 
      day = T->tm_mday; 
      hour = T->tm_hour; 
      min = T->tm_min;
      sec = T->tm_sec;
    }

    //store time as ymdms since I think it will compress better! 
    int year; 
    int day; 
    int hour;
    int min; 
    int sec; 
    int _nu_pdg; 
    bool skipped;
    bool have_weight;
    bool have_frac;
    bool have_dir; 
    bool have_nu; 
  ClassDef(TruthAnitaNeutrino,4); 
};


#endif //TRUTHANITAEVENT_H
