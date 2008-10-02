#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TF1.h"
#include "TStyle.h"
#include "FFTtools.h"
#include <iostream>
#include <fstream>

const float clockPeriod=29.970;


void quickZCPlot(char *baseName,int run,  int startEntry, int numEntries);

void quickZCPlot()
{
   gStyle->SetStatH(0.25);
   gStyle->SetStatW(0.25);
}
  

void quickZCPlot(char *baseName, int run, int startEntry, int numEntries) {
  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  //  char hkName[FILENAME_MAX];
  sprintf(eventName,"%s/run%d/eventFile%d.root",baseName,run,run);
  sprintf(headerName,"%s/run%d/headFile%d.root",baseName,run,run);
  //  sprintf(hkName,"/unix/anita1/webData/firstDay/run%d/prettyHkFile%d.root",run,run);

  RawAnitaEvent *event = 0;
  RawAnitaHeader *header =0;
  //  PrettyAnitaHk *hk = 0;
  
  TChain *eventChain = new TChain("eventTree");
  eventChain->Add(eventName);
  eventChain->SetBranchAddress("event",&event);

   for(int extra=1;extra<100;extra++) {
     sprintf(eventName,"%s/run%d/eventFile%d_%d.root",baseName,run,run,extra);
    TFile *fpTest = TFile::Open(eventName);
    if(!fpTest) 
      break;
    else {
      delete fpTest;
      eventChain->Add(eventName);
    }
  }

  TFile *fpHead = TFile::Open(headerName);
  TTree *headTree = (TTree*) fpHead->Get("headTree");
  headTree->SetBranchAddress("header",&header);

  //  TFile *fpHk = new TFile(hkName);
  //  TTree *prettyHkTree = (TTree*) fpHk->Get("prettyHkTree");
  //  prettyHkTree->SetBranchAddress("hk",&hk);


  //Friends only seem to work with TTree::Draw and similar commands
  //if you are manually calling GetEntry (i.e in a loop) you must call
  //the GetEntry for each tree separately.
  //  eventChain->AddFriend(headTree);
  //  eventChain->AddFriend(prettyHkTree);


  //  char textLabel[180];
  //  char pngName[180];
  //  TGraph *gr[9]={0};

  
  Long64_t nentries=eventChain->GetEntries();
  
  Long64_t maxEntry=startEntry+numEntries;
  if(nentries<maxEntry)
     maxEntry=nentries;

  char outName[180];
  sprintf(outName,"zcFile%d.root",run);

  TFile *fpOut = new TFile(outName,"RECREATE");
  TH1F *histFirstRco[4];
  TH1F *histSecondRco[4];
  TH1F *histAll[4]; 
  TTree *zcTree = new TTree("zcTree","Tree of ZC stuff");
  Int_t zcPreHitbus,zcPostHitbus;
  Int_t sampPreHitbus,sampPostHitbus;
  Int_t earliestSample,latestSample;
  zcTree->Branch("sampPreHitbus",&sampPreHitbus,"sampPreHitbus/I");
  zcTree->Branch("sampPostHitbus",&sampPostHitbus,"sampPostHitbus/I");
  zcTree->Branch("zcPreHitbus",&zcPreHitbus,"zcPreHitbus/I");
  zcTree->Branch("zcPostHitbus",&zcPostHitbus,"zcPostHitbus/I");
  zcTree->Branch("earliestSample",&earliestSample,"earliestSample/I");
  zcTree->Branch("latestSample",&latestSample,"latestSample/I");

  char histName[180];
  char histTitle[180];
  for(int lab=0;lab<4;lab++) {
    sprintf(histName,"histFirstRco_%d",lab);
    sprintf(histTitle,"Zero Crossings First RCO Lab %d",lab);
    histFirstRco[lab] = new TH1F(histName,histTitle,260,-0.5,259.5);
    sprintf(histName,"histSecondRco_%d",lab);
    sprintf(histTitle,"Zero Crossings Second RCO Lab %d",lab);
    histSecondRco[lab] = new TH1F(histName,histTitle,260,-0.5,259.5);
    sprintf(histName,"histAll_%d",lab);
    sprintf(histTitle,"Zero Crossings Both RCO Lab %d",lab);
    histAll[lab] = new TH1F(histName,histTitle,260,-0.5,259.5);
  }

  Long64_t starEvery=maxEntry/40;
  if(starEvery==0) starEvery++;

  for(Long64_t entry=startEntry;entry<maxEntry;entry++) {
    
    if(entry%starEvery==0) 
      cerr << "*";

    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    //    prettyHkTree->GetEntry(entry);
   
    
    
    UsefulAnitaEvent realEvent(event,WaveCalType::kNoCalib);
    TGraph *gr = realEvent.getGraphFromSurfAndChan(0,0);
    Int_t labChip=realEvent.getLabChip(0);
    earliestSample=realEvent.getEarliestSample(0);
    latestSample=realEvent.getLatestSample(0);

    Double_t offset=gr->GetMean(2);
    Double_t *adcs=gr->GetY();

    //    cout << earliestSample << "\t" << latestSample << endl;
    if(latestSample<earliestSample) {
      //We have two RCO's
      
      Int_t countZC=0;
      for(int samp=earliestSample;samp<259;samp++) {
	Double_t firstVal=adcs[samp]-offset;
	Double_t secondVal=adcs[samp+1]-offset;
	//	if(firstVal>=0 && secondVal<0) {
	if((firstVal>=0 && secondVal<0) || (firstVal<0 && secondVal>=0)) {  
	  //We have a ZC
	  countZC++;
	  if(TMath::Abs(firstVal)<TMath::Abs(secondVal)) {
	    histFirstRco[labChip]->Fill(samp);
	    histAll[labChip]->Fill(samp);
	  }
	  else if(TMath::Abs(firstVal)>TMath::Abs(secondVal)) {
	    histFirstRco[labChip]->Fill(samp+1);
	    histAll[labChip]->Fill(samp+1);
	  }
	  else {
	    histFirstRco[labChip]->Fill(samp,0.5);
	    histAll[labChip]->Fill(samp,0.5);
	    histFirstRco[labChip]->Fill(samp+1,0.5);
	    histAll[labChip]->Fill(samp+1,0.5);
	  }
	}
      }
      zcPostHitbus=countZC;
      sampPostHitbus=261-earliestSample;
      countZC=0;
      if(latestSample>0) {
	for(int samp=0;samp<latestSample-1;samp++) {
	  Double_t firstVal=adcs[samp]-offset;
	  Double_t secondVal=adcs[samp+1]-offset;
	  //	  if(firstVal>=0 && secondVal<0) {
	  if((firstVal>=0 && secondVal<0) || (firstVal<0 && secondVal>=0)) {   
	    //We have a ZC
	    countZC++;
	    if(TMath::Abs(firstVal)<TMath::Abs(secondVal)) {
	      histSecondRco[labChip]->Fill(samp);
	      histAll[labChip]->Fill(samp);
	    }
	    else if(TMath::Abs(firstVal)>TMath::Abs(secondVal)) {
	      histSecondRco[labChip]->Fill(samp+1);
	      histAll[labChip]->Fill(samp+1);
	    }
	    else {
	      histSecondRco[labChip]->Fill(samp,0.5);
	      histAll[labChip]->Fill(samp,0.5);
	      histSecondRco[labChip]->Fill(samp+1,0.5);
	      histAll[labChip]->Fill(samp+1,0.5);
	    }
	  }
	}
	zcPreHitbus=countZC;
	sampPreHitbus=latestSample+1;
      }
    

    }
    else {
      //Only one RCO
      Int_t countZC=0;
      for(int samp=earliestSample;samp<latestSample-1;samp++) {
	Double_t firstVal=adcs[samp]-offset;
	Double_t secondVal=adcs[samp+1]-offset;
	if((firstVal>=0 && secondVal<0) || (firstVal<0 && secondVal>=0)) {
	  //We have a ZC
	  countZC++;
	  if(TMath::Abs(firstVal)<TMath::Abs(secondVal)) {
	    histFirstRco[labChip]->Fill(samp);
	    histAll[labChip]->Fill(samp);
	  }
	  else if(TMath::Abs(firstVal)>TMath::Abs(secondVal)) {
	    histFirstRco[labChip]->Fill(samp+1);
	    histAll[labChip]->Fill(samp+1);
	  }
	  else {
	    histFirstRco[labChip]->Fill(samp,0.5);
	    histAll[labChip]->Fill(samp,0.5);
	    histFirstRco[labChip]->Fill(samp+1,0.5);
	    histAll[labChip]->Fill(samp+1,0.5);
	  }
	}
      }
      zcPostHitbus=countZC;
      sampPostHitbus=(latestSample+1)-earliestSample;
      zcPreHitbus=0;
      sampPreHitbus=0;
    }
    zcTree->Fill();


    //    if((entry-startEntry)%100==0)
    //      cout << realEvent.eventNumber << endl;
    

  }
  zcTree->AutoSave();
  fpOut->Write();

}

