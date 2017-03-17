#include "AnalysisBlinding.h"
#include "AnitaEventCalibrator.h"

#include <fstream>
#include "TFile.h"
#include "TTree.h"




AnalysisBlinding::strategy defaultStrat = AnalysisBlinding::kInsertedEvents;
AnalysisBlinding::strategy theStrat = defaultStrat;

bool firstEvent = true;
bool calledSetBlinding = false;
bool calledLoadBlindTrees = false;
TFile* fFakeEventFile = NULL;
TTree* fFakeEventTree = NULL;
UsefulAnitaEvent* fFakeEvent = NULL;
std::vector<std::pair<UInt_t, Int_t> > overwrittenEventInfo;



TString AnalysisBlinding::getDescription(AnalysisBlinding::strategy strat){

  TString description = "Current strategy: ";

  if(strat == kNoBlinding){
    description = "No blinding. ";
  }

  if(strat & kInsertedEvents){
    description += "Small number of minimum bias events overwritten by ANITA-3 WAIS pulser events with swapped polarisations. ";
  }
  return description;
}



AnalysisBlinding::strategy AnalysisBlinding::setStrategy(AnalysisBlinding::strategy newStrat){
  theStrat = newStrat;
  calledSetBlinding = true;
  firstEvent = true; // allows the no blinding warning to be printed again
  return theStrat;
}








AnalysisBlinding::strategy AnalysisBlinding::getStrategy(){
  return theStrat;
}










void loadBlindTrees() {

  if(!calledLoadBlindTrees){
    // zero internal pointers so can check we find everything.
    fFakeEventFile = NULL;
    fFakeEventTree = NULL;
    fFakeEvent = NULL;


    char calibDir[FILENAME_MAX];
    char fileName[FILENAME_MAX];
    char *calibEnv=getenv("ANITA_CALIB_DIR");
    if(!calibEnv) {
      char *utilEnv=getenv("ANITA_UTIL_INSTALL_DIR");
      if(!utilEnv){
	sprintf(calibDir,"calib");
      }
      else{
	sprintf(calibDir,"%s/share/anitaCalib",utilEnv);
      }
    }
    else {
      strncpy(calibDir,calibEnv,FILENAME_MAX);
    }

    // these are the fake events, that will be inserted in place of some min bias events
    sprintf(fileName, "%s/fakeEventFile.root", calibDir);
    fFakeEventFile = TFile::Open(fileName);
    if(fFakeEventFile){
      fFakeEventTree = (TTree*) fFakeEventFile->Get("eventTree");
    }


    // these are the min bias event numbers to be overwritten, with the entry in the fakeEventTree
    // that is used to overwrite the event
    sprintf(fileName,"%s/anita3OverwrittenEventInfo.txt",calibDir);
    std::ifstream overwrittenEventInfoFile(fileName);
    char firstLine[180];
    overwrittenEventInfoFile.getline(firstLine,179);
    UInt_t overwrittenEventNumber;
    Int_t fakeTreeEntry;
    while(overwrittenEventInfoFile >> overwrittenEventNumber >> fakeTreeEntry){
      overwrittenEventInfo.push_back(std::pair<UInt_t, Int_t>(overwrittenEventNumber, fakeTreeEntry));
      // std::cout << overwrittenEventInfo.at(overwrittenEventInfo.size()-1).first << "\t" << overwrittenEventInfo.at(overwrittenEventInfo.size()-1).second << std::endl;
    }
    if(overwrittenEventInfo.size()==0){
      std::cerr << "Warning in " << __FILE__ << std::endl;
      std::cerr << "Unable to find overwrittenEventInfo" << std::endl;
    }


    // whinge if you can't find the data
    if(fFakeEventFile && fFakeEventTree){
      // std::cerr << "fgInstance = " << fgInstance << ", but this = " << this << std::endl;
      fFakeEventTree->SetBranchAddress("event", &fFakeEvent);
    }
    else{
      std::cerr << "Warning in " << __FILE__ << std::endl;
      std::cerr << "Unable to find files for blinding" << std::endl;
      std::cerr << "fFakeEventFile = " << fFakeEventFile << std::endl;
      std::cerr << "fFakeEventTree = " << fFakeEventTree << std::endl;
    }
  }
  calledLoadBlindTrees = true;
}










Int_t isEventToOverwrite(UInt_t eventNumber){

  Int_t fakeTreeEntry = -1;
  for(UInt_t i=0; i <overwrittenEventInfo.size(); i++){
    if(overwrittenEventInfo.at(i).first==eventNumber){
      fakeTreeEntry = overwrittenEventInfo.at(i).second;
      break;
    }
  }
  return fakeTreeEntry;
}











Int_t overwriteEventMaybe(UsefulAnitaEvent *eventPtr){

  // need a flag to load this after constructor otherwise loadBlindTrees()
  // puts us into an infinite loop.
  if(!calledLoadBlindTrees){
    loadBlindTrees();
  }

  UInt_t eventNumber = eventPtr->eventNumber;
  Int_t fakeEntry = isEventToOverwrite(eventNumber);

  if(fakeEntry >= 0){
    std::vector<Int_t> surfEventsIds(NUM_SURF, 0);
    for(int surf=0; surf < NUM_SURF; surf++){
      surfEventsIds.at(surf) = eventPtr->surfEventId[surf];
    }
    fFakeEventTree->GetEntry(fakeEntry);
    (*eventPtr) = (*fFakeEvent); // relying on implicitly declared copy constructor... will this work?

    eventPtr->eventNumber = eventNumber;
    for(int surf=0; surf < NUM_SURF; surf++){
      eventPtr->surfEventId[surf] = surfEventsIds.at(surf);
    }
  }
  return fakeEntry;
}












void AnalysisBlinding::applyBlinding(UsefulAnitaEvent* event){

  // On the first event we will warn users if they didn't explicitly set the blinding
  // or if they're doing ANITA-3 analysis with no blinding.
  if(firstEvent){
    if(!calledSetBlinding){
      // warn that we're on the default blinding
      std::cerr << "Warning in " << __PRETTY_FUNCTION__ << std::endl;
      std::cerr << "You have not explicitly set a blinding strategy." << std::endl;
      std::cerr << "Currently set to: " << getDescription(defaultStrat) << std::endl;
      std::cerr << "To suppress this warning call AnalysisBlinding::setStrategy(" << defaultStrat << ") before reading event data." << std::endl;
      std::cerr << "See AnalysisBlinding.h for more options." << std::endl;
    }
    else if(AnitaVersion::get()==3 && theStrat==kNoBlinding){
      std::cerr << "Warning in " << __PRETTY_FUNCTION__ << std::endl;
      std::cerr << "Currently set to: " << getDescription(kNoBlinding) << std::endl;
      std::cerr << "See AnalysisBlinding.h for more options." << std::endl;
    }
    firstEvent = false;
  }


  if(theStrat & kInsertedEvents){
    overwriteEventMaybe(event);
  }


}
