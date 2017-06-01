//#include "UsefulAnitaEvent.h"
//#include "AnitaConventions.h"
//#include "RawAnitaEvent.h"
//#include "TimedAnitaHeader.h"
//#include "PrettyAnitaHk.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>
#include "TMath.h" 

void oindree_min_signal(int start_run, int end_run);


void oindree_min_signal()
{
   cout << "Usage: For drawing a distribution of minimum signal over antennas for ANITA-IV, oindree_min_signal(42,367)\n";
   //  oindree_min_signal(42,367);
}
  

void oindree_min_signal(int start_run, int end_run) {
   
  const int num_ant = 48; 
  double pkpkV[num_ant] = {};
  double pkpkH[num_ant] = {}; 

  for (int i = 0; i < num_ant; i++)
  {
    pkpkV[i] = 0.0;
    pkpkH[i] = 0.0; 
  }

  TChain headChain("headTree");
  for (int run_number = start_run; run_number <= end_run; run_number++) { headChain.Add(TString::Format("$ANITA_ROOT_DATA/run%d/timedHeadFile%d.root",run_number,run_number)); }
  TimedAnitaHeader *header = NULL;
  headChain.SetBranchAddress("header",&header);
  int header_num_entries = headChain.GetEntries();
  cout << "number of entries in header anita-4 is " << header_num_entries << endl;

  TChain eventChain("eventTree");
  for (int run_number = start_run; run_number <= end_run; run_number++) { eventChain.Add(TString::Format("$ANITA_ROOT_DATA/run%d/eventFile%d.root",run_number,run_number)); }
  RawAnitaEvent *event = NULL;
  eventChain.SetBranchAddress("event",&event);
  int event_num_entries = eventChain.GetEntries();
  cout << "number of entries in event anita-4 is " << event_num_entries << endl;

  TChain prettyChain("prettyHkTree");
  for (int run_number = start_run; run_number <= end_run; run_number++) { prettyChain.Add(TString::Format("$ANITA_ROOT_DATA/run%d/prettyHkFile%d.root",run_number,run_number)); }
  PrettyAnitaHk *hk = NULL;
  prettyChain.SetBranchAddress("hk",&hk);
  int hk_num_entries = prettyChain.GetEntries();
  cout << "number of entries in prettyHk anita-4 is " << hk_num_entries << endl;

  UInt_t count=0;

  TH1D *hmin_signalV = new TH1D("hmin_signalV",";MinOverVChans(pk-pk voltage in mV);Number of Events",100,-2,100);   
  TH1D *hmin_signalH = new TH1D("hmin_signalH",";MinOverHChans(pk-pk voltage in mV);Number of Events",100,-2,100);   

  for(int ientry=0; ientry < header_num_entries; ientry=ientry+1000000) 
  {
     eventChain.GetEntry(ientry);
     headChain.GetEntry(ientry);
     prettyChain.GetEntry(ientry);

     //cout << "realTime is " << header->realTime << endl; 
     //cout << " here " << endl; 
   
    //This step produces a calibrated UsefulAnitaEvent
    UsefulAnitaEvent realEvent(event,WaveCalType::kDefault,hk);
    //cout << " after useful " << endl; 

    //cout << realEvent.eventNumber << " " << header->eventNumber << endl;
    //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
    count++;

    //initialize 
    double min_pkpkV = 0.0;
    double min_pkpkH = 0.0;  
    
    for (int j = 0; j < num_ant; j++)
    {
      pkpkH[j] = 0.0; 
      pkpkV[j] = 0.0; 
    }  

    for (int iant = 0; iant < num_ant; iant++)
    { 
    
      TGraph *gr_hpol = new TGraph(0); 
      gr_hpol = realEvent.getGraph(iant,AnitaPol::kHorizontal);
      pkpkH[iant] = gr_hpol->GetY()[TMath::LocMax(gr_hpol->GetN(),gr_hpol->GetY())] - gr_hpol->GetY()[TMath::LocMin(gr_hpol->GetN(),gr_hpol->GetY())];

      TGraph *gr_vpol = new TGraph(0); 
      gr_vpol = realEvent.getGraph(iant,AnitaPol::kVertical); 
      pkpkV[iant] = gr_vpol->GetY()[TMath::LocMax(gr_vpol->GetN(),gr_vpol->GetY())] - gr_vpol->GetY()[TMath::LocMin(gr_vpol->GetN(),gr_vpol->GetY())]; 

    } //loop over antennas

    min_pkpkV = TMath::LocMin(num_ant,pkpkV);  
    min_pkpkH = TMath::LocMin(num_ant,pkpkH);

    hmin_signalV->Fill(min_pkpkV);
    hmin_signalH->Fill(min_pkpkH); 

  } //loop over events ends
  cerr << endl;
  cout << "Processed " << count << " events.\n";

  TCanvas *h = new TCanvas("h","h",1000,800); 
  hmin_signalV->Draw("");
  h->SaveAs("min_signalV.png");
  h->SaveAs("min_signalV.root"); 

  TCanvas *hh = new TCanvas("hh","hh",1000,800); 
  hmin_signalH->Draw("");
  hh->SaveAs("min_signalH.png");
  hh->SaveAs("min_signalH.root"); 

}//end of macro

