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

void oindree_dc_offset(int start_run, int end_run);


void oindree_dc_offset()
{
   cout << "Usage: For drawing a distribution of maximum dc offset over antennas for ANITA-IV, oindree_dc_offset(42,367)\n";
   //  oindree_dc_offset(42,367);
}
  

void oindree_dc_offset(int start_run, int end_run) {
   
  const int num_ant = 48; 
  const int num_phi = 16; 
  double dcOffset[num_ant];

  int max_index = 0;
  int min_index = 0;
  double max = 0.0;
  double min = 0.0; 

  AnitaVersion::set(4); 
  AnitaPol::AnitaPol_t pol = AnitaPol::kHorizontal; 

  for (int i = 0; i < num_ant; i++)
    {
      dcOffset[i] = 0.0;
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

  TH1D *hdc_offset = new TH1D("hdc_offset",";MaxOverChans(Average voltage or DC Offset in mV);Number of Events",100,-200,200);   

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
     double max_dcOffset = 0.0;

     max_index = 0;
     min_index = 0;
     max = 0.0;
     min = 0.0; 
   
     for (int j = 0; j < num_ant; j++)
       {
	 dcOffset[j] = 0.0; 
       } 

     for (int iant = 0; iant < num_ant; iant++)
      {
	max_index = 0;
	min_index = 0; 
	max = 0.0;
	min = 0.0; 

	TGraph *gr = new TGraph(0);
	gr = realEvent.getGraph(iant,pol); 

	double gr_y = 0.0; 

	for (int igr = 0; igr < gr->GetN(); igr++)
	  {
	    double volt = gr->GetY()[igr]; 
	    //cout << " volt is " << volt << endl; 
	    
	    //cout << "gr_y is " << gr_y << endl; 

	    gr_y = gr_y + volt; 

	    //cout << "in the loop " << gr_y << endl; 
	  }

	//cout << "final " << gr_y << endl; 

	//cout << "num of points " << gr->GetN() << endl; 

	dcOffset[iant] = gr_y/(gr->GetN()); // average of all points in the waveform 
	
	//cout << "after dividing by num points " << dcOffset[iant] << endl; 

	//cout << max_index << "   " << min_index << endl; 

	delete gr; 
      } // loop over phi sectors 
  
    max_dcOffset = dcOffset[TMath::LocMax(num_ant,dcOffset)]; 
    //cout << "max dcOffset is " << max_dcOffset << endl; 

    hdc_offset->Fill(max_dcOffset);
   
    if (max_dcOffset > 10.0 || max_dcOffset < -10.0) {cout << "max_dcOffset is " << max_dcOffset << " run is " << header->run << " event number is " << header->eventNumber << endl; } 

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
  hdc_offset->SetStats(0); 
  hdc_offset->Draw("");
  hdc_offset->SaveAs(Form("hdc_offset_pol%i.root",pol)); 
  h->SaveAs(Form("dc_offset_pol%i.png",pol));
  h->SaveAs(Form("dc_offset_pol%i.root",pol)); 

}//end of macro

