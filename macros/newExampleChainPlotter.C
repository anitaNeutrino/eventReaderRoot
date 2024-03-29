//#include "AnitaConventions.h"

void newExampleChainPlotter()
{
  newExampleChainPlotter(1030);
}
  

void newExampleChainPlotter(int run) {
  gSystem->Load("libAnitaEvent.so");

  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  char hkName[FILENAME_MAX];
  sprintf(eventName,"/unix/anita1/webData/initial/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"/unix/anita1/webData/initial/run%d/timedHeadFile%d.root",run,run);
  sprintf(hkName,"/unix/anita1/webData/initial/run%d/prettyHkFile%d.root",run,run);

  RawAnitaEvent *event = 0;
  TimedAnitaHeader *header =0;
  PrettyAnitaHk *hk = 0;
  
  TChain *eventChain = new TChain("eventTree");
  eventChain->Add(eventName);
  eventChain->SetBranchAddress("event",&event);

  TFile *fpHead = new TFile(headerName);
  TTree *headTree = (TTree*) fpHead->Get("headTree");
  headTree->SetBranchAddress("header",&header);

  TFile *fpHk = new TFile(hkName);
  TTree *prettyHkTree = (TTree*) fpHk->Get("prettyHkTree");
  prettyHkTree->SetBranchAddress("hk",&hk);


  //Friends only seem to work with TTree::Draw and similar commands
  //if you are manually calling GetEntry (i.e in a loop) you must call
  //the GetEntry for each tree separately.
  eventChain->AddFriend(headTree);
  eventChain->AddFriend(prettyHkTree);

  //Stupidly most do this to be perfectly safe
  eventChain->GetEntry(1000);
  headTree->GetEntry(1000);
  prettyHkTree->GetEntry(1000);


  UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlus,hk);
  cout << realEvent.eventNumber << " " << header->eventNumber << endl;
  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
  TGraph *gr = realEvent.getGraph(14,AnitaPol::kHorizontal);
  gr->Draw("al");

}
