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

void oindree_pulse_asymm(int start_run, int end_run);


void oindree_pulse_asymm()
{
   cout << "Usage: For drawing a distribution of max over phi sectors pulse asymm for ANITA-IV, oindree_pulse_asymm(42,367)\n";
   //  oindree_pulse_asymm(42,367);
}
  

void oindree_pulse_asymm(int start_run, int end_run) {
   
  const int num_ant = 48; 
  const int num_phi = 16; 
  //double asymm[num_ant];
  double asymm[num_phi]; 
  
  int max_index;
  int min_index;
  double max;
  double min; 

  AnitaVersion::set(4); 
  AnitaPol::AnitaPol_t pol = AnitaPol::kHorizontal;
  AnitaRing::AnitaRing_t ring = AnitaRing::kBottomRing; 

  //for (int i = 0; i < num_ant; i++)
  //{
  //asymm[i] = 0.0;
  //}

  for (int i = 0; i < num_phi; i++)
    {
      asymm[i] = 0.0; 
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

  TH1D *hpulse_asymm = new TH1D("hpulse_asymm",";MaxOverPhiSectors(|Vmax - |Vmin|| in mV);Number of Events",100,0,100);   

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
     double max_asymm = 0.0;
    
     //for (int j = 0; j < num_ant; j++)
     //{
     //asymm[j] = 0.0; 
     //} 

     for (int j = 0; j < num_phi; j++)
       {
	 asymm[j] = 0.0; 
       } 

    //for (int iant = 0; iant < num_ant; iant++)
    //{ 
    
    //TGraph *gr = new TGraph(0); 
    // gr = realEvent.getGraph(iant,pol);
    //asymm[iant] = (gr->GetY()[TMath::LocMax(gr->GetN(),gr->GetY())]) - (gr->GetY()[TMath::LocMin(gr->GetN(),gr->GetY())]);

    //cout << asymm[iant] << endl; 

    //delete gr;

      //} //loop over antennas
     
     max_index = 0;
     min_index = 0;         
     max = 0.0;
     min = 0.0;

     for (int iphi = 0; iphi < num_phi; iphi++)
      {
	max_index = 0;
	min_index = 0;         
	max = 0.0;
	min = 0.0;

	TGraph *gr = new TGraph(0);
	gr = realEvent.getGraph(ring,iphi,pol); 

	max_index = TMath::LocMax(gr->GetN(),gr->GetY());
	min_index = TMath::LocMin(gr->GetN(),gr->GetY());         
	max = gr->GetY()[max_index];
	min = gr->GetY()[min_index];  
	
	asymm[iphi] = fabs(max - fabs(min));  

	//cout << asymm[iphi] << endl; 

	//cout << max << "   " << min << endl; 
	//cout << max_index << "   " << min_index << endl; 

	delete gr; 
      } // loop over phi sectors 

    //min_asymm = asymm[TMath::LocMin(num_ant,asymm)];  
    max_asymm = asymm[TMath::LocMax(num_phi,asymm)]; 
    //cout << "min asymm is " << min_asymm << endl; 

    hpulse_asymm->Fill(max_asymm);

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
  hpulse_asymm->SetStats(0); 
  hpulse_asymm->Draw("");
  hpulse_asymm->SaveAs(Form("hpulse_asymm_pol%iring%i.root",pol,ring)); 
  h->SaveAs(Form("pulse_asymm_pol%iring%i.png",pol,ring));
  h->SaveAs(Form("pulse_asymm_pol%iring%i.root",pol,ring)); 

}//end of macro

