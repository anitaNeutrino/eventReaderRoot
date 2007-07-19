#include "AnitaConventions.h"

void sillyFriendExample()
{
  sillyFriendExample(1023);
}
  

void sillyFriendExample(int run) {
  gSystem->Load("libAnitaEvent.so");

  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  char hkName[FILENAME_MAX];
  char gpsName[FILENAME_MAX];
  sprintf(eventName,"/unix/anita1/newRootData/run%d/eventFile%d.root",run,run);
  sprintf(headerName,"/unix/anita1/newRootData/run%d/headFile%d.root",run,run);
  sprintf(hkName,"/unix/anita1/newRootData/run%d/prettyHkFile%d.root",run,run);
  sprintf(gpsName,"/unix/anita1/newRootData/run%d/gpsFile%d.root",run,run);
  
  TFile *fpEvent = new TFile(eventName);
  TTree *eventTree = (TTree*) fpEvent->Get("eventTree");
  eventTree->AddFriend("headTree",headerName);
  eventTree->AddFriend("prettyHkTree",hkName);
  eventTree->AddFriend("adu5PatTree",gpsName);

  eventTree->Draw("pressures[1]:altitude");

}
