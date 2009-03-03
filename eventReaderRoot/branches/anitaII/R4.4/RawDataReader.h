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


//!  RawDataReader -- Raw data reader
/*!
  RawDataReader -- not sure if this is still 100% functional
  \ingroup rootclasses
*/
class RawDataReader 
{
 public: 
   RawDataReader(); ///<Default constructor
   ~RawDataReader(); ///< Destructor

   int openFile(char *psevName, char *headName); ///< Open this psev file
   UsefulAnitaEvent *getNextEvent(WaveCalType::WaveCalType_t calType=WaveCalType::kNoCalib); ///< Get the next event from the file


 private: 

  //Data Reading Thingies 
   PedSubbedEventBody_t psBody; ///< Raw data
   AnitaEventHeader_t theHeader; ///< The header
   gzFile eventFile; ///< File ptr
   gzFile headFile; ///< File ptr
   TFile *fpSurf; ///< Who knows
   TTree *surfTempTree; ///< Who knows
   Float_t surfTemp; ///< Who knows
};


#endif //RAWDATAREADER_H
