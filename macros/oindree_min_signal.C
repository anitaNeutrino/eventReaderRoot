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
   cout << "Usage: For drawing a distribution of minimum signal over phi sectors for ANITA-IV, oindree_min_signal(42,367)\n";
   //  oindree_min_signal(42,367);
}
  

void oindree_min_signal(int start_run, int end_run) {
   
  const int num_ant = 48; 
  const int num_phi = 16; 
  //double pkpk[num_ant];
  double pkpk[num_phi]; 
  
  int max_index;
  int min_index;
  double max;
  double min; 

  AnitaVersion::set(4); 
  AnitaPol::AnitaPol_t pol = AnitaPol::kHorizontal;
  AnitaRing::AnitaRing_t ring = AnitaRing::kBottomRing; 

  //for (int i = 0; i < num_ant; i++)
  //{
  //pkpk[i] = 0.0;
  //}

  for (int i = 0; i < num_phi; i++)
    {
      pkpk[i] = 0.0; 
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

  TH1D *hmin_signal = new TH1D("hmin_signal",";MinOverPhiSectors(pk-pk voltage in mV);Number of Events",100,0,1000);   

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
     double min_pkpk = 0.0;
    
     //for (int j = 0; j < num_ant; j++)
     //{
     //pkpk[j] = 0.0; 
     //} 

     for (int j = 0; j < num_phi; j++)
       {
	 pkpk[j] = 0.0; 
       } 

    //for (int iant = 0; iant < num_ant; iant++)
    //{ 
    
    //TGraph *gr = new TGraph(0); 
    // gr = realEvent.getGraph(iant,pol);
    //pkpk[iant] = (gr->GetY()[TMath::LocMax(gr->GetN(),gr->GetY())]) - (gr->GetY()[TMath::LocMin(gr->GetN(),gr->GetY())]);

    //cout << pkpk[iant] << endl; 

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
	
	pkpk[iphi] = max - min;  

	//cout << pkpk[iphi] << endl; 

	//cout << max << "   " << min << endl; 
	//cout << max_index << "   " << min_index << endl; 

	delete gr; 
      } // loop over phi sectors 

    //min_pkpk = pkpk[TMath::LocMin(num_ant,pkpk)];  
    min_pkpk = pkpk[TMath::LocMin(num_phi,pkpk)]; 
    //cout << "min pkpk is " << min_pkpk << endl; 

    hmin_signal->Fill(min_pkpk);

    if (min_pkpk < 50) { cout << "min pkpk is " << min_pkpk << "run is " << header->run << "event number is " << header->eventNumber << endl; } 

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
  hmin_signal->SetStats(0); 
  hmin_signal->Draw("");
  hmin_signal->SaveAs(Form("hmin_signal_pol%iring%i.root",pol,ring)); 
  h->SaveAs(Form("min_signal_pol%iring%i.png",pol,ring));
  h->SaveAs(Form("min_signal_pol%iring%i.root",pol,ring)); 

}//end of macro

