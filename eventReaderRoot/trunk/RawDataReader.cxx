//////////////////////////////////////////////////////////////////////////////
/////  RawDataReader.cxx        ANITA event reading class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in raw ANITA events and produces     ///// 
/////   calibrated time and voltage stuff                                /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "RawDataReader.h"
#include "RawAnitaEvent.h"
#include "RawAnitaHeader.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "TTreeIndex.h"


RawDataReader::RawDataReader() 
  :eventFile(0)
{
   //Default Constructor

   
}

RawDataReader::~RawDataReader() {
   //Default Destructor
}

int RawDataReader::openFile(char *psevName, char *headName)
{
   eventFile=gzopen(psevName,"rb");
   if(!eventFile) {
      std::cerr << "Error opening: " << psevName << std::endl;
      return -1;
   }
   headFile=gzopen(headName,"rb");
   if(!headFile) {
      std::cerr << "Error opening: " << headName << std::endl;
      return -1;
   }
   return 0;
}

UsefulAnitaEvent *RawDataReader::getNextEvent(WaveCalType::WaveCalType_t calType)
{
   //First check if file is open
   if(!eventFile || ! headFile) {
      std::cerr << "File not open, must call RawDataReader::openFile first" << std::endl;
      return NULL;
   }

   //Now try to read in raw data
   int numBytes=gzread(eventFile,&psBody,sizeof(PedSubbedEventBody_t));
   if(numBytes!=sizeof(PedSubbedEventBody_t)) {
     //Will change this later to work
      std::cerr << "Only read " << numBytes << " bytes from file" << std::endl;
      gzclose(eventFile);
      return NULL;
   }

   //Now try to read in raw data
   numBytes=gzread(headFile,&theHeader,sizeof(AnitaEventHeader_t));
   if(numBytes!=sizeof(AnitaEventHeader_t)) {
     //Will change this later to work
      std::cerr << "Only read " << numBytes << " bytes from file" << std::endl;
      gzclose(headFile);
      return NULL;
   }
   

   RawAnitaEvent rawEv(&psBody);
   RawAnitaHeader rawHead(&theHeader,0,0,0,0,0);
   UsefulAnitaEvent *evPtr = new UsefulAnitaEvent(&rawEv,calType,&rawHead);      
   return evPtr;
}


