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

void oindree_ML(int eventNum, int run);

void oindree_ML(int eventNum, int run)
{
   cout << "Usage: For spitting out variables and features for a ML spreadsheet for given event number and run number, oindree_ML(11964005,90)\n";
   //  oindree_ML(11964005,90);
}

void oindree_ML(int eventNum, int run) {

  ofstream myfile_ML; 
  myfile_ML.open("ML_input.txt", std::ofstream::out | std::ofstream::app);
  myfile_ML << "run_number,event_number,max_topPkPk,max_bottomPkPk,min_topPkPk,min_bottomPkPk\n";
  
  AnitaVersion::set(4); 
  const int num_phi = 16; 
  AnitaPol::AnitaPol_t pol = AnitaPol::kHorizontal;

  double topPkPk[num_phi];
  double bottomPkPk[num_phi];
  for (int iphi = 0; iphi < num_phi; iphi++)
    {
      topPkPk[iphi] = 0.0; 
      bottomPkPk[iphi] = 0.0; 
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
  for (int run_number = run; run_number <= run; run_number++) { headChain.Add(TString::Format("$ANITA_ROOT_DATA/run%d/timedHeadFile%d.root",run_number,run_number)); }
  TimedAnitaHeader *header = NULL;
  headChain.SetBranchAddress("header",&header);
  int header_num_entries = headChain.GetEntries();
  cout << "number of entries in header anita-4 is " << header_num_entries << endl;
  headChain.BuildIndex("eventNumber"); 

  TChain eventChain("eventTree");
  for (int run_number = run; run_number <= run; run_number++) { eventChain.Add(TString::Format("$ANITA_ROOT_DATA/run%d/eventFile%d.root",run_number,run_number)); }
  RawAnitaEvent *event = NULL;
  eventChain.SetBranchAddress("event",&event);
  int event_num_entries = eventChain.GetEntries();
  cout << "number of entries in event anita-4 is " << event_num_entries << endl;
  eventChain.BuildIndex("eventNumber"); 

  TChain prettyChain("prettyHkTree");
  for (int run_number = run; run_number <= run; run_number++) { prettyChain.Add(TString::Format("$ANITA_ROOT_DATA/run%d/prettyHkFile%d.root",run_number,run_number)); }
  PrettyAnitaHk *hk = NULL;
  prettyChain.SetBranchAddress("hk",&hk);
  int hk_num_entries = prettyChain.GetEntries();
  cout << "number of entries in prettyHk anita-4 is " << hk_num_entries << endl;
  prettyChain.BuildIndex("realTime"); 

  UInt_t count=0;

  eventChain.GetEntryWithIndex(eventNum);
  headChain.GetEntryWithIndex(eventNum);
  prettyChain.GetEntryWithIndex(header->realTime);

  //cout << "realTime is " << header->realTime << endl; 
  //cout << " here " << endl; 
   
  //This step produces a calibrated UsefulAnitaEvent
  UsefulAnitaEvent realEvent(event,WaveCalType::kDefault,hk);
  //cout << " after useful " << endl; 

  //cout << realEvent.eventNumber << " " << header->eventNumber << endl;
  //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
  count++;

  //initialize max_ratio and min_ratio
  double max_topPkPk = 0.0;
  double max_bottomPkPk = 0.0; 
  double min_bottomPkPk = 0.0;  
  double min_topPkPk = 0.0; 

  //make sure variables used to calculate max_ratio for each event are re-initialized to zero
  for (int i = 0; i < num_phi; i++)
    {
      topPkPk[i] = 0.0; 
      bottomPkPk[i] = 0.0; 
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
      gr_top = realEvent.getGraph(AnitaRing::kTopRing,iphi,pol); 
      gr_bottom = realEvent.getGraph(AnitaRing::kBottomRing,iphi,pol);

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

      topPkPk[iphi] = top_max - top_min; 
      bottomPkPk[iphi] = bottom_max - bottom_min; 
      
      delete gr_top;
      delete gr_bottom; 
    } //loop over phi sectors ends

  max_topPkPk = topPkPk[TMath::LocMax(num_phi,topPkPk)]; //LocMax gives the index 
  max_bottomPkPk = bottomPkPk[TMath::LocMax(num_phi,bottomPkPk)]; 
  min_topPkPk = topPkPk[TMath::LocMin(num_phi,topPkPk)];
  min_bottomPkPk = bottomPkPk[TMath::LocMin(num_phi,bottomPkPk)];
  
  myfile_ML << run << "," << setprecision(11) << eventNum << "," << max_topPkPk << "," << max_bottomPkPk << "," << min_topPkPk  << "," << min_bottomPkPk << "\n"; //write to file

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

}//end of macro

