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

void oindree_dig_saturation(int start_run, int end_run);


void oindree_dig_saturation()
{
   cout << "Usage: For drawing a distribution of maximum absolute voltage over antennas for ANITA-IV, oindree_dig_saturation(42,367)\n";
   //  oindree_dig_saturation(42,367);
}
  

void oindree_dig_saturation(int start_run, int end_run) {
   
  const int num_ant = 48; 
  const int num_phi = 16; 
  double absVolt[num_ant];

  int max_index = 0;
  int min_index = 0;
  double max = 0.0;
  double min = 0.0; 

  AnitaVersion::set(4); 
  AnitaPol::AnitaPol_t pol = AnitaPol::kHorizontal; 

  for (int i = 0; i < num_ant; i++)
    {
      absVolt[i] = 0.0;
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

  TH1D *hdig_saturation = new TH1D("hdig_saturation",";MaxOverChans(|V|);Number of Events",100,0,2000);   

  for(int ientry=0; ientry < header_num_entries; ientry=ientry+100000) 
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
     double min_absVolt = 0.0;

     max_index = 0;
     min_index = 0;
     max = 0.0;
     min = 0.0; 
   
     for (int j = 0; j < num_ant; j++)
       {
	 absVolt[j] = 0.0; 
       } 

     for (int iant = 0; iant < num_ant; iant++)
      {
	max_index = 0;
	min_index = 0; 
	max = 0.0;
	min = 0.0; 

	TGraph *gr = new TGraph(0);
	gr = realEvent.getGraph(iant,pol); 

	max_index = TMath::LocMax(gr->GetN(),gr->GetY()); 
        min_index = TMath::LocMin(gr->GetN(),gr->GetY());

	max = gr->GetY()[max_index];
	min = gr->GetY()[min_index]; 

	//cout << max << "   " << min << endl; 

	if (fabs(max) > fabs(min)) { absVolt[iant] = fabs(max) ;}
	if (fabs(min) > fabs(max)) { absVolt[iant] = fabs(min) ;} 

	//cout << absVolt[iant] << endl; 

	//cout << max_index << "   " << min_index << endl; 

	delete gr; 
      } // loop over phi sectors 
  
    max_absVolt = absVolt[TMath::LocMax(num_ant,absVolt)]; 
    //cout << "max absVolt is " << max_absVolt << endl; 

    hdig_saturation->Fill(max_absVolt);

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
  hdig_saturation->SetStats(0); 
  hdig_saturation->Draw("");
  hdig_saturation->SaveAs(Form("hdig_saturation_pol%i.root",pol)); 
  h->SaveAs(Form("dig_saturation_pol%i.png",pol));
  h->SaveAs(Form("dig_saturation_pol%i.root",pol)); 

}//end of macro

