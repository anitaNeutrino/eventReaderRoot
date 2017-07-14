//////////////////////////////////////////////////////////////////////////////
/////  readRawData.cxx        ANITA Raw data reader                      /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple program that reads in raw ANITA events and produces   ///// 
/////   calibrated time and voltage stuff in text file format            /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;

#include "RawDataReader.h"
#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"



int main(int argc, char **argv)
{
   if(argc<4) {
      cout << "Usage: " << endl;
      cout << "\t" << argv[0] << " <psev filename> <header filename> <calib type>" << endl << endl;
      WaveCalType::listAllCalTypes();
      return 0;
   }

   int calibType=atoi(argv[3]);
   if(calibType<WaveCalType::kNoCalib || calibType>WaveCalType::kNotACalib) {
      cout << "Calibration Type " << calibType << " does not exist" << endl;
      return 0;
   }

   WaveCalType::WaveCalType_t calType = (WaveCalType::WaveCalType_t) calibType;


   //Now open file and start processing
   RawDataReader fred;
   int retVal=fred.openFile(argv[1],argv[2]);
   if(retVal!=0) {
     cout << "Problem opening: " << argv[1]  <<"\t" << argv[2] << endl;
      return -1;
   }


   char outputName[FILENAME_MAX];

   while(UsefulAnitaEvent *evPtr=fred.getNextEvent(calType)) {
//       //      UsefulAnitaEvent *evPtr=fred.getNextEvent(calType);
//       sprintf(outputName,"event_%u.txt",evPtr->eventNumber);
//       ofstream Output(outputName);
//       Output.precision(6);
//       for(int surf=0;surf<NUM_SURF;surf++) {
// 	 for(int chan=0;chan<NUM_CHAN;chan++) {
// 	    int chanIndex=evPtr->getChanIndex(surf,chan);
// 	    Int_t numPoints=evPtr->fNumPoints[chanIndex];
// 	    Float_t deltaT=0;
// 	    if(numPoints>1) {
// 	       Float_t firstTime=evPtr->fTimes[chanIndex][0];
// 	       Float_t lastTime=evPtr->fTimes[chanIndex][numPoints-1];
// 	       deltaT=(lastTime-firstTime)/float(numPoints-1);
// 	    }
// 	    Float_t lastTime=0;
// 	    for(int samp=0;samp<NUM_SAMP;samp++) {
// 	       if(samp<evPtr->fNumPoints[chanIndex]) {
// 		  lastTime=evPtr->fTimes[chanIndex][samp];
// 		  Output << surf << " " << chan << " " 
// 			 << evPtr->fTimes[chanIndex][samp] << " " 
// 			 << evPtr->fVolts[chanIndex][samp] << endl;
// 	       }
// 	       else {
// 		  lastTime+=deltaT;
// 		  Output << surf << " " << chan << " " << lastTime << " " << 0 << endl;
// 	       }
	       
// 	    }
// 	 }
//       }   
      delete evPtr;
   }

}
