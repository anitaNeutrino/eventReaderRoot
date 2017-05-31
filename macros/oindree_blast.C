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

void oindree_blast(int start_run, int end_run);


void oindree_blast()
{
   cout << "Usage: For drawing a distribution of a ratio to try and find payload blast events in ANITA-IV, oindree_blast(42,367)\n";
   //  oindree_blast(42,367);
}
  

void oindree_blast(int start_run, int end_run) {
   
  const int num_phi = 16; 

  double ratio[num_phi];
  for (int iphi = 0; iphi < num_phi; iphi++)
    {
      ratio[iphi] = 0.0; 
    }
 
  int top_max_index = 0;
  int top_min_index = 0; 
  double top_max = 0.0; 
  double top_min = 0.0;
  int bottom_max_index = 0;
  int bottom_min_index = 0; 
  double bottom_max = 0.0; 
  double bottom_min = 0.0; 

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

  TH1D *hmax_ratio = new TH1D("hmax_ratio",";MaxOverPhiSectors((Bottom ring pk-pk voltage)/(Top ring pk-pk voltage));Number of Events",100,-2,10); 
  TH1D *hmin_ratio = new TH1D("hmin_ratio",";MinOverPhiSectors((Bottom ring pk-pk voltage)/(Top ring pk-pk voltage));Number of Events",100,-2,10);   

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

    //initialize max_ratio and min_ratio
    double max_ratio = 0.0;
    double min_ratio = 0.0;  

    //make sure variables used to calculate max_ratio for each event are re-initialized to zero
    for (int i = 0; i < num_phi; i++)
    {
      ratio[i] = 0.0; 
    }
 
    top_max_index = 0;
    top_min_index = 0; 
    top_max = 0.0; 
    top_min = 0.0;
    bottom_max_index = 0;
    bottom_min_index = 0; 
    bottom_max = 0.0; 
    bottom_min = 0.0; 
    
    for (int iphi = 0; iphi < num_phi; iphi++) //loop over phi sectors
    {
      TGraph *gr_top = new TGraph(0); 
      TGraph *gr_bottom = new TGraph(0); 
      //make graphs using realEvent by calling function in UsefulAnitaEvent class
      //to obtain ratio of peak voltages from bottom ring and top ring
      gr_top = realEvent.getGraph(AnitaRing::kTopRing,iphi,AnitaPol::kVertical); 
      gr_bottom = realEvent.getGraph(AnitaRing::kBottomRing,iphi,AnitaPol::kVertical);

      top_max_index = TMath::LocMax(gr_top->GetN(),gr_top->GetY());
      top_min_index = TMath::LocMin(gr_top->GetN(),gr_top->GetY());         
      top_max = gr_top->GetY()[top_max_index];
      top_min = gr_top->GetY()[top_min_index];    

      //cout << "top max, min in mV are                 " << top_max << ",   " << top_min << endl; 
  
      bottom_max_index = TMath::LocMax(gr_bottom->GetN(),gr_bottom->GetY());
      bottom_min_index = TMath::LocMin(gr_bottom->GetN(),gr_bottom->GetY());
      bottom_max = gr_bottom->GetY()[bottom_max_index]; 
      bottom_min = gr_bottom->GetY()[bottom_min_index]; 

      //cout << "bottom max, min in mV are              " << bottom_max << ",   " << bottom_min << endl; 
 
      ratio[iphi] = (bottom_max - bottom_min) / (top_max - top_min); 
      //cout << "ratio for phi number            " << iphi << " is                    " << ratio[iphi] << endl; 

      delete gr_top;
      delete gr_bottom; 
    } //loop over phi sectors ends

    max_ratio = ratio[TMath::LocMax(num_phi,ratio)]; //LocMax gives the index 
    cout << "max_ratio for ientry =  " << ientry << " is " << max_ratio << endl; 
    min_ratio = ratio[TMath::LocMin(num_phi,ratio)];
    cout << "min_ratio for ientry =  " << ientry << " is " << min_ratio << endl;  

    hmax_ratio->Fill(max_ratio); 
    hmin_ratio->Fill(min_ratio); 
  } //loop over events ends
  cerr << endl;
  cout << "Processed " << count << " events.\n";
  
  //just to check how a graph looks
  //TCanvas *c = new TCanvas("c","c",1000,800); 
  //gr_top->Draw("alp"); 
  //gr_top->GetXaxis()->SetTitle("Time (ns)");
  //gr_top->GetYaxis()->SetTitle("Voltage (mV)"); 
  //gr_top->SetTitle("Voltage-time waveform using UsefulAnitaEvent"); 
  //c->SaveAs("gr_top.png"); 
  //delete c; 

  //Draw the histogram of max_ratio  - outliers could be due to drop down PV array
  TCanvas *h = new TCanvas("h","h",1000,800); 
  hmax_ratio->Draw(); 
  h->SaveAs("max_ratio.png"); 
  h->SaveAs("max_ratio.root"); 

  //Draw the histogram of min_ratio - outliers could be due to PV cells at the top of the payload that power the SIP 
  TCanvas *hh = new TCanvas("hh","hh",1000,800); 
  hmin_ratio->Draw(); 
  hh->SaveAs("min_ratio.png"); 
  hh->SaveAs("min_ratio.root"); 




}//end of macro

