//////////////////////////////////////////////////////////////////////////////
/////  RawDataReader.h        ANITA event reading class                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ANITA events and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef RAWDATAREADER_H
#define RAWDATAREADER_H

//Includes
#include "simpleStructs.h"
#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include <zlib.h>
#include "TFile.h"
#include "TTree.h"


class RawDataReader 
{
 public: 
  RawDataReader();
  ~RawDataReader();

  int openFile(char *psevName);
  UsefulAnitaEvent *getNextEvent(WaveCalType::WaveCalType_t calType=WaveCalType::kNoCalib);


 private: 

  //Data Reading Thingies 
  PedSubbedEventBody_t psBody; 
  gzFile eventFile;
  TFile *fpSurf;
  TTree *surfTempTree;
  Float_t surfTemp;
};


#endif //RAWDATAREADER_H
