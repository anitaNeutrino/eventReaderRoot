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
#include <iostream>
#include <fstream>
#include "TTreeIndex.h"


RawDataReader::RawDataReader() 
  :eventFile(0)
{
   //Default Constructor

   //Try to open surfTemp.root file
   char *surfTempDir=getenv("EVENT_READER_DIR");
   if(!surfTempDir) {
      std::cerr << "EVENT_READER_DIR not set defaulting to current dir" << std::endl;
      surfTempDir=".";
   }
   char surfTempName[FILENAME_MAX];
   sprintf(surfTempName,"%s/surfTemp.root",surfTempDir);
   fpSurf = new TFile(surfTempName);
   surfTempTree=0;
   if(fpSurf->IsOpen()) {
      surfTempTree=(TTree*)fpSurf->Get("surfTempTree");
      if(surfTempTree) {
	 surfTempTree->SetBranchAddress("surfTemp",&surfTemp);
      }
   }
   if(!surfTempTree) {
      std::cerr << "Couldn't open surfTemp.root -- no temperature correction" << std::endl;
   }
}

RawDataReader::~RawDataReader() {
   //Default Destructor
}

int RawDataReader::openFile(char *psevName)
{
   eventFile=gzopen(psevName,"rb");
   if(!eventFile) {
      std::cerr << "Error opening: " << psevName << std::endl;
      return -1;
   }
   return 0;
}

UsefulAnitaEvent *RawDataReader::getNextEvent(WaveCalType::WaveCalType_t calType)
{
   //First check if file is open
   if(!eventFile) {
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
   
   surfTemp=33.046;
   if(surfTempTree) {
      Long64_t entry=surfTempTree->GetEntryNumberWithBestIndex(psBody.eventNumber,psBody.eventNumber);
      if(entry>0) 
	 surfTempTree->GetEntry(entry);
      //      std::cout << entry << "\t" << surfTemp << std::endl;
   }

   RawAnitaEvent rawEv(&psBody);
   UsefulAnitaEvent *evPtr = new UsefulAnitaEvent(&rawEv,calType,surfTemp);      
   return evPtr;
}


