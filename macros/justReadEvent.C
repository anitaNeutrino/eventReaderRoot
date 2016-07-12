//#include "AnitaConventions.h"

void justReadEvent() {
  gSystem->Load("libAnitaEvent.so");
  RawAnitaEvent *event = 0;
  TFile *fp = new TFile("eventFile1023.root");
  TTree *eventTree = (TTree*) gDirectory->Get("eventTree");

  eventTree->SetBranchAddress("event",&event);
  eventTree->GetEntry(11);
  UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullRG);
  cout << realEvent.eventNumber << endl;


  TGraph *gr = realEvent.getGraph(14,AnitaPol::kHorizontal);
  gr->Draw("al");

}
