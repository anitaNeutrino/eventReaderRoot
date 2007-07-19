#include "AnitaConventions.h"

void sillyFriendChainExample()
{
  sillyFriendChainExample(1033);
}
  

void sillyFriendChainExample(int run) {
  gSystem->Load("libAnitaEvent.so");

  //Filenames for the various trees
  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  char hkName[FILENAME_MAX];
  char gpsName[FILENAME_MAX];


  //Note the * in the event file name, it's there to handle those runs which
  //have more than one event root file in them (2GB file limit)
  sprintf(eventName,"/unix/anita1/webData/initial/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"/unix/anita1/webData/initial/run%d/headFile%d.root",run,run);
  sprintf(hkName,"/unix/anita1/webData/initial/run%d/prettyHkFile%d.root",run,run);
  sprintf(gpsName,"/unix/anita1/webData/initial/run%d/gpsFile%d.root",run,run);
  
  //Open the event chain
  TChain *eventChain = new TChain("eventTree");
  eventChain->Add(eventName);

  //Add friend trees
  eventChain->AddFriend("headTree",headerName);
  eventChain->AddFriend("prettyHkTree",hkName);
  eventChain->AddFriend("adu5PatTree",gpsName);

  //Draw something
  eventTree->Draw("header.eventNumber:eventNumber");

}
