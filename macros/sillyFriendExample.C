//#include "AnitaConventions.h"

void katieTest()
{
  katieTest(10103);
}
  

void katieTest(int run) {
  gSystem->Load("libAnitaEvent.so");

  char eventName[100];//FILENAME_MAX];
  char headerName[100];//FILENAME_MAX];
  char hkName[100];//FILENAME_MAX];
  char gpsName[100];//FILENAME_MAX];
  sprintf(eventName,"/anitaStorage/antarctica14/root/run%d/eventFile%d.root",run,run);
  sprintf(headerName,"/anitaStorage/antarctica14/root/run%d/headFile%d.root",run,run);
  sprintf(hkName,"/anitaStorage/antarctica14/root/run%d/prettyHkFile%d.root",run,run);
  sprintf(gpsName,"/anitaStorage/antarctica14/root/run%d/gpsFile%d.root",run,run);
  
  TFile *fpEvent = new TFile(eventName);
  TTree *eventTree = (TTree*) fpEvent->Get("eventTree");
  eventTree->AddFriend("headTree",headerName);
  eventTree->AddFriend("prettyHkTree",hkName);
  eventTree->AddFriend("adu5PatTree",gpsName);

  eventTree->Draw("pressures[1]:altitude");

}
