//#include "AnitaConventions.h"

void sillyFriendChainExample()
{
  sillyFriendChainExample(10125);
}
  

void sillyFriendChainExample(int run) {
  gSystem->Load("libAnitaEvent.so");

  //Filenames for the various trees
  char eventName[100];//FILENAME_MAX];
  char headerName[100];//FILENAME_MAX];
  char hkName[100];//FILENAME_MAX];
  char gpsName[100];//FILENAME_MAX];


  //Note the * in the event file name, it's there to handle those runs which
  //have more than one event root file in them (2GB file limit)
  sprintf(eventName,"/anitaStorage/antarctica14/root/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"/anitaStorage/antarctica14/root/run%d/headFile%d.root",run,run);
  sprintf(hkName,"/anitaStorage/antarctica14/root/run%d/prettyHkFile%d.root",run,run);
  sprintf(gpsName,"/anitaStorage/antarctica14/root/run%d/gpsFile%d.root",run,run);
  
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
