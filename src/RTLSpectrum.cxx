#include "RTLSpectrum.h" 
#include "TGraph.h" 
#include "simpleStructs.h" 


RTLSpectrum::RTLSpectrum () 
{
  spectrum = 0; 
}

RTLSpectrum::~RTLSpectrum () 
{
  delete spectrum; 
}

RTLSpectrum::RTLSpectrum(RtlSdrPowerSpectraStruct_t * raw, TGraph * calibration) 
{
  rtlNum = raw->rtlNum; 
  unixTimeStart = raw->unixTimeStart; 
  scanTime = raw->scanTime; 
  gain = raw->gain; 
  if (raw->nFreq == 0) 
  {
    spectrum = 0; 
  }
  else
  {
    spectrum = new TGraph(raw->nFreq); 

    double * x = spectrum->GetX(); 
    double * y = spectrum->GetY(); 

    for (unsigned i = 0; i < raw->nFreq; i++) 
    {
      x[i] = raw->startFreq + i * raw->freqStep; 
      y[i] = raw->spectrum[i] / SPECTRUM_SCALE() + SPECTRUM_OFFSET();   

      if (calibration) y[i]-=calibration->Eval(x[i]); 
    }
  }
}


