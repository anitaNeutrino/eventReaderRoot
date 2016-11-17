#ifndef ANITA_RTL_SPECTRUM_H
#define ANITA_RTL_SPECTRUM_H
#include "TObject.h" 
#include "simpleStructs.h" 


class TGraph; 

class RTLSpectrum : public TObject 
{

  public: 
    double SPECTRUM_OFFSET() const { return -40.; }
    double SPECTRUM_SCALE() const { return 4.; }
    RTLSpectrum(); 
    RTLSpectrum(RtlSdrPowerSpectraStruct_t * raw, TGraph * calibration = 0); 
    ~RTLSpectrum(); 

    TGraph * getSpectrum() { return spectrum; } 
    Double_t getGain() { return gain; } 
    Int_t getRTLNum() { return rtlNum; } 
    Int_t getUnixTime() { return unixTimeStart; } 
    Float_t getScanTime() { return scanTime; } 

  private: 
    TGraph * spectrum; 
    Double_t gain; 
    Int_t rtlNum; 
    Int_t unixTimeStart; 
    Float_t scanTime; 


    ClassDef(RTLSpectrum, 1); 
}; 



#endif
