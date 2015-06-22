///////////////////////////////////////////////////////////////////////////////
/////  AnitaClock.h        ANITA-3 Clock numbers                          /////
/////                                                                     /////
/////  Description:                                                       /////
/////              A small handy file with a namespace containing ANITA-3 /////
/////              clock information.                                     /////
/////  Author:     b.strutt.12@ucl.ac.uk                                  /////
///////////////////////////////////////////////////////////////////////////////


#ifndef ANITA_CLOCK_H
#define ANITA_CLOCK_H

namespace AnitaClock {
  /* Clock calibration constants */

  const Double_t meanPeriodNs = 30;
  const Double_t lowPeriodNs = 29.93;
  const Double_t highPeriodNs = 30.07;

  // Variable for the maximum number of zero crossings per waveform.
  // 30 ns per period so this is more than sufficient for the 100 ns window.
  const Int_t maxNumZcs = 5;
  
}

#endif
