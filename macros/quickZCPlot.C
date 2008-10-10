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
  TH1F *histZcFirstRco[10][4];
  TH1F *histZcSecondRco[10][4];
  TH1F *histZcChip[10][4];
  TH1F *histZcAll = new TH1F("histZcAll","histZcAll",260,-0.5,259.5); 
  TH1F *histSampFirstRco[10][4];
  TH1F *histSampSecondRco[10][4];
  TH1F *histSampChip[10][4];
  TH1F *histSampAll = new TH1F("histSampAll","histSampAll",260,-0.5,259.5); 
  TTree *zcTree = new TTree("zcTree","Tree of ZC stuff");
  Int_t zcPreHitbus,zcPostHitbus;
  Int_t sampPreHitbus,sampPostHitbus;
  Int_t earliestSample,latestSample;
  Int_t surfNum,chipNum;
  Int_t got255;
  zcTree->Branch("sampPreHitbus",&sampPreHitbus,"sampPreHitbus/I");
  zcTree->Branch("sampPostHitbus",&sampPostHitbus,"sampPostHitbus/I");
  zcTree->Branch("zcPreHitbus",&zcPreHitbus,"zcPreHitbus/I");
  zcTree->Branch("zcPostHitbus",&zcPostHitbus,"zcPostHitbus/I");
  zcTree->Branch("earliestSample",&earliestSample,"earliestSample/I");
  zcTree->Branch("latestSample",&latestSample,"latestSample/I");
  zcTree->Branch("surf",&surfNum,"surf/I");
  zcTree->Branch("chip",&chipNum,"chip/I");
  zcTree->Branch("got255",&got255,"got255/I");

  char histName[180];
  char histTitle[180];
  for(int surf=0;surf<10;surf++) {
     for(int lab=0;lab<4;lab++) {
	sprintf(histName,"histZcFirstRco_%d_%d",surf,lab);
	sprintf(histTitle,"Zero Crossings First RCO (SURF %d, Chip %d)",surf,lab);
	histZcFirstRco[surf][lab] = new TH1F(histName,histTitle,260,-0.5,259.5);
	sprintf(histName,"histZcSecondRco_%d_%d",surf,lab);
	sprintf(histTitle,"Zero Crossings Second RCO (SURF %d , Chip %d)",surf,lab);
	histZcSecondRco[surf][lab] = new TH1F(histName,histTitle,260,-0.5,259.5);
	sprintf(histName,"histZcChip_%d_%d",surf,lab);
	sprintf(histTitle,"Zero Crossings Both RCO (SURF %d, Chip %d)",surf,lab);
	histZcChip[surf][lab] = new TH1F(histName,histTitle,260,-0.5,259.5);

	sprintf(histName,"histSampFirstRco_%d_%d",surf,lab);
	sprintf(histTitle,"Zero Crossings First RCO (SURF %d, Chip %d)",surf,lab);
	histSampFirstRco[surf][lab] = new TH1F(histName,histTitle,260,-0.5,259.5);
	sprintf(histName,"histSampSecondRco_%d_%d",surf,lab);
	sprintf(histTitle,"Zero Crossings Second RCO (SURF %d , Chip %d)",surf,lab);
	histSampSecondRco[surf][lab] = new TH1F(histName,histTitle,260,-0.5,259.5);
	sprintf(histName,"histSampChip_%d_%d",surf,lab);
	sprintf(histTitle,"Zero Crossings Both RCO (SURF %d, Chip %d)",surf,lab);
	histSampChip[surf][lab] = new TH1F(histName,histTitle,260,-0.5,259.5);
     }
  }

  Long64_t starEvery=maxEntry/40;
  if(starEvery==0) starEvery++;

  Int_t chan=0;

  for(Long64_t entry=startEntry;entry<maxEntry;entry++) {
    
    if(entry%starEvery==0) 
      cerr << "*";

    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    //    prettyHkTree->GetEntry(entry);
   
    UsefulAnitaEvent realEvent(event,WaveCalType::kNoCalib);
    
    for(int surf=0;surf<10;surf++) {
       TGraph *gr = realEvent.getGraphFromSurfAndChan(surf,chan);
       Int_t labChip=realEvent.getLabChip(surf*9 + chan);
       earliestSample=realEvent.getEarliestSample(surf*9 + chan);
       //       earliestSample+=1; //To avoid HITBUS crosstalk
       latestSample=realEvent.getLatestSample(surf*9 + chan);
       //       latestSample-=1; // To avoid HITBUS crosstalk
       Double_t offset=gr->GetMean(2);
       Double_t *adcs=gr->GetY();
       
       surfNum=surf;
       chipNum=labChip;

       if(earliestSample>259) 
	  earliestSample=0;
       if(latestSample<0) 
	  latestSample=259;

       //    cout << earliestSample << "\t" << latestSample << endl;
       got255=0;
       if(latestSample<earliestSample) {
	  //We have two RCO's	  
	  Int_t countZC=0;
	  for(int samp=earliestSample;samp<259;samp++) {
	     histSampAll->Fill(samp,0.5);
	     histSampAll->Fill(samp+1,0.5);
	     histSampChip[surf][labChip]->Fill(samp,0.5);
	     histSampChip[surf][labChip]->Fill(samp+1,0.5);
	     histSampFirstRco[surf][labChip]->Fill(samp,0.5);
	     histSampFirstRco[surf][labChip]->Fill(samp+1,0.5);
	     Double_t firstVal=adcs[samp]-offset;
	     Double_t secondVal=adcs[samp+1]-offset;
	     //	if(firstVal>=0 && secondVal<0) {
	     if((firstVal>=0 && secondVal<0) || (firstVal<0 && secondVal>=0)) {  

	       //	       if(surf==0 && samp==254) 
	       //		 cout << realEvent.eventNumber << "\t" << samp
	       //		      << "\t" << firstVal << "\t" << secondVal << "\tA\n";
		//We have a ZC
		countZC++;
		Double_t sum=TMath::Abs(firstVal)+TMath::Abs(secondVal);
		Double_t right=TMath::Abs(firstVal)/sum;
		Double_t left=TMath::Abs(firstVal)/sum;
		histZcFirstRco[surf][labChip]->Fill(samp,left);
		histZcFirstRco[surf][labChip]->Fill(samp+1,right);
		histZcChip[surf][labChip]->Fill(samp,left);
		histZcChip[surf][labChip]->Fill(samp+1,right);
		histZcAll->Fill(samp,left);
		histZcAll->Fill(samp+1,right);
		if(samp==254 || samp==255)
		  got255++;
	// 	if(TMath::Abs(firstVal)<TMath::Abs(secondVal)) {
// 		   histZcFirstRco[surf][labChip]->Fill(samp);
// 		   histZcChip[surf][labChip]->Fill(samp);
// 		   histZcAll->Fill(samp);
// 		}
// 		else if(TMath::Abs(firstVal)>TMath::Abs(secondVal)) {
// 		   histZcFirstRco[surf][labChip]->Fill(samp+1);
// 		   histZcChip[surf][labChip]->Fill(samp+1);
// 		   histZcAll->Fill(samp+1);
// 		}
// 		else {
// 		   histZcFirstRco[surf][labChip]->Fill(samp,0.5);
// 		   histZcFirstRco[surf][labChip]->Fill(samp+1,0.5);
// 		   histZcChip[surf][labChip]->Fill(samp,0.5);
// 		   histZcChip[surf][labChip]->Fill(samp+1,0.5);
// 		   histZcAll->Fill(samp,0.5);
// 		   histZcAll->Fill(samp+1,0.5);
// 		}
	     }
	  }
	  zcPostHitbus=countZC;
	  sampPostHitbus=260-earliestSample;
	  countZC=0;
	  if(latestSample>0) {
	     for(int samp=0;samp<latestSample;samp++) {
		histSampAll->Fill(samp,0.5);
		histSampAll->Fill(samp+1,0.5);
		histSampChip[surf][labChip]->Fill(samp,0.5);
		histSampChip[surf][labChip]->Fill(samp+1,0.5);
		histSampSecondRco[surf][labChip]->Fill(samp,0.5);
		histSampSecondRco[surf][labChip]->Fill(samp+1,0.5);
		Double_t firstVal=adcs[samp]-offset;
		Double_t secondVal=adcs[samp+1]-offset;
		//	  if(firstVal>=0 && secondVal<0) {
		if((firstVal>=0 && secondVal<0) || (firstVal<0 && secondVal>=0)) {   
		   //We have a ZC
		  //		  if(surf==0 && samp==254) 
		  //		    cout << realEvent.eventNumber << "\t" << samp
		  // << "\t" << firstVal << "\t" << secondVal << "\tB\n";

		   countZC++;
		   Double_t sum=TMath::Abs(firstVal)+TMath::Abs(secondVal);
		   Double_t right=TMath::Abs(firstVal)/sum;
		   Double_t left=TMath::Abs(firstVal)/sum;
		   histZcSecondRco[surf][labChip]->Fill(samp,left);
		   histZcSecondRco[surf][labChip]->Fill(samp+1,right);
		   histZcChip[surf][labChip]->Fill(samp,left);
		   histZcChip[surf][labChip]->Fill(samp+1,right);
		   histZcAll->Fill(samp,left);
		   histZcAll->Fill(samp+1,right);
		   if(samp==254 || samp==255)
		     got255++;
	// 	   if(TMath::Abs(firstVal)<TMath::Abs(secondVal)) {
// 		      histZcSecondRco[surf][labChip]->Fill(samp);
// 		      histZcChip[surf][labChip]->Fill(samp);
// 		      histZcAll->Fill(samp);
// 		   }
// 		   else if(TMath::Abs(firstVal)>TMath::Abs(secondVal)) {
// 		      histZcSecondRco[surf][labChip]->Fill(samp+1);
// 		      histZcChip[surf][labChip]->Fill(samp+1);
// 		      histZcAll->Fill(samp+1);
// 		   }
// 		   else {
// 		      histZcSecondRco[surf][labChip]->Fill(samp,0.5);
// 		      histZcChip[surf][labChip]->Fill(samp,0.5);
// 		      histZcSecondRco[surf][labChip]->Fill(samp+1,0.5);
// 		      histZcChip[surf][labChip]->Fill(samp+1,0.5);
// 		      histZcAll->Fill(samp,0.5);
// 		      histZcAll->Fill(samp+1,0.5);
// 		   }
		}
	     }
	     zcPreHitbus=countZC;
	     sampPreHitbus=latestSample+1;
	  }
	  
	  
       }
       else {
	  //Only one RCO
	  Int_t countZC=0;
	  for(int samp=earliestSample;samp<latestSample;samp++) {
	     histSampAll->Fill(samp,0.5);
	     histSampAll->Fill(samp+1,0.5);
	     histSampChip[surf][labChip]->Fill(samp,0.5);
	     histSampChip[surf][labChip]->Fill(samp+1,0.5);
	     histSampFirstRco[surf][labChip]->Fill(samp,0.5);
	     histSampFirstRco[surf][labChip]->Fill(samp+1,0.5);
	     Double_t firstVal=adcs[samp]-offset;
	     Double_t secondVal=adcs[samp+1]-offset;
	     if((firstVal>=0 && secondVal<0) || (firstVal<0 && secondVal>=0)) {
		//We have a ZC
	       //	       if(surf==0 && samp==254) 
	       //		 cout << realEvent.eventNumber << "\t" << samp
	       //		      << "\t" << firstVal << "\t" << secondVal << "\tC\n";

		countZC++;
		Double_t sum=TMath::Abs(firstVal)+TMath::Abs(secondVal);
		Double_t right=TMath::Abs(firstVal)/sum;
		Double_t left=TMath::Abs(firstVal)/sum;
		histZcFirstRco[surf][labChip]->Fill(samp,left);
		histZcFirstRco[surf][labChip]->Fill(samp+1,right);
		histZcChip[surf][labChip]->Fill(samp,left);
		histZcChip[surf][labChip]->Fill(samp+1,right);
		histZcAll->Fill(samp,left);
		histZcAll->Fill(samp+1,right);
		if(samp==254 || samp==255)
		  got255++;
// 		if(TMath::Abs(firstVal)<TMath::Abs(secondVal)) {
// 		   histZcFirstRco[surf][labChip]->Fill(samp);
// 		   histZcChip[surf][labChip]->Fill(samp);
// 		   histZcAll->Fill(samp);
// 		}
// 		else if(TMath::Abs(firstVal)>TMath::Abs(secondVal)) {
// 		   histZcFirstRco[surf][labChip]->Fill(samp+1);
// 		   histZcChip[surf][labChip]->Fill(samp+1);
// 		   histZcAll->Fill(samp+1);
// 		}
// 		else {
// 		   histZcFirstRco[surf][labChip]->Fill(samp,0.5);
// 		   histZcFirstRco[surf][labChip]->Fill(samp+1,0.5);
// 		   histZcChip[surf][labChip]->Fill(samp,0.5);
// 		   histZcChip[surf][labChip]->Fill(samp+1,0.5);
// 		   histZcAll->Fill(samp,0.5);
// 		   histZcAll->Fill(samp+1,0.5);
// 		}
	     }
	  }
	  zcPostHitbus=countZC;
	  sampPostHitbus=(latestSample+1)-earliestSample;
	  zcPreHitbus=0;
	  sampPreHitbus=0;
       }
       zcTree->Fill();
       delete gr;
    }

    //    if((entry-startEntry)%100==0)
    //      cout << realEvent.eventNumber << endl;
    

  }
  zcTree->AutoSave();
  fpOut->Write();

}

