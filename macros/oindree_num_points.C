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

void oindree_num_points(int start_run, int end_run);


void oindree_num_points()
{
   cout << "Usage: For drawing a distribution of minimum (over chans) number of points in the waveform for ANITA-IV, oindree_num_points(42,367)\n";
   //  oindree_num_points(42,367);
}
  
void oindree_num_points(int start_run, int end_run) {
   
  const int num_ant = 48; 
  const int num_phi = 16; 
  double numPoints[num_ant];

  AnitaVersion::set(4); 
  AnitaPol::AnitaPol_t pol = AnitaPol::kHorizontal; 

  for (int i = 0; i < num_ant; i++)
    {
      numPoints[i] = 0.0;
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

  TH1D *hnum_points = new TH1D("hnum_points",";MinOverChans(Number of points in waveform);Number of Events",100,0,300);   

  for(int ientry=0; ientry < header_num_entries; ientry=ientry+1000) 
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
     double min_numPoints = 0.0;
   
     for (int j = 0; j < num_ant; j++)
       {
	 numPoints[j] = 0.0; 
       } 

     for (int iant = 0; iant < num_ant; iant++)
      {
	TGraph *gr = new TGraph(0);
	gr = realEvent.getGraph(iant,pol); 

	numPoints[iant] = gr->GetN(); // number of points in the waveform
	//cout << numPoints[iant] << endl; 

	delete gr; 
      } // loop over phi sectors 
  
    min_numPoints = numPoints[TMath::LocMin(num_ant,numPoints)]; 
    //cout << "min numPoints is " << min_numPoints << endl; 

    hnum_points->Fill(min_numPoints);

  } //loop over events ends

  cerr << endl;
  cout << "Processed " << count << " events.\n";

  //just to check how a graph looks
  //TCanvas *c = new TCanvas("c","c",1000,800); 
  //gr->Draw("alp"); 
  //gr->GetXaxis()->SetTitle("Time (ns)");
  //gr->GetYaxis()->SetTitle("Voltage (mV)"); 
  //gr->SetTitle("Voltage-time waveform using UsefulAnitaEvent"); 
  //c->SaveAs("gr.png"); 
  //delete c; 

  TCanvas *h = new TCanvas("h","h",1000,800); 
  h->SetLogy(); 
  hnum_points->SetStats(0); 
  hnum_points->Draw("");
  hnum_points->SaveAs(Form("hnum_points_pol%i.root",pol)); 
  h->SaveAs(Form("num_points_pol%i.png",pol));
  h->SaveAs(Form("num_points_pol%i.root",pol)); 

}//end of macro

