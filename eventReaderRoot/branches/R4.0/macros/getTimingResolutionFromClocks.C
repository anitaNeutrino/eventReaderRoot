#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaEvent.h"
#include "TimedAnitaHeader.h"
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


void getTimingResolutionFromClocks(int run, int startEntry, int numEntries);

void getTimingResolutionFromClocks()
{
   gStyle->SetStatH(0.25);
   gStyle->SetStatW(0.25);
   getTimingResolutionFromClocks(1028,0,100);
}
  

void getTimingResolutionFromClocks(int run, int startEntry, int numEntries) {
   Double_t deltaT=1./(2.6*16);

  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  char hkName[FILENAME_MAX];
  sprintf(eventName,"/unix/anita1/webData/firstDay/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"/unix/anita1/webData/firstDay/run%d/timedHeadFile%d.root",run,run);
  sprintf(hkName,"/unix/anita1/webData/firstDay/run%d/prettyHkFile%d.root",run,run);

  RawAnitaEvent *event = 0;
  TimedAnitaHeader *header =0;
  PrettyAnitaHk *hk = 0;
  
  TChain *eventChain = new TChain("eventTree");
  eventChain->Add(eventName);
  eventChain->SetBranchAddress("event",&event);

  TFile *fpHead = new TFile(headerName);
  TTree *headTree = (TTree*) fpHead->Get("headTree");
  headTree->SetBranchAddress("header",&header);

  TFile *fpHk = new TFile(hkName);
  TTree *prettyHkTree = (TTree*) fpHk->Get("prettyHkTree");
  prettyHkTree->SetBranchAddress("hk",&hk);


  //Friends only seem to work with TTree::Draw and similar commands
  //if you are manually calling GetEntry (i.e in a loop) you must call
  //the GetEntry for each tree separately.
  //  eventChain->AddFriend(headTree);
  //  eventChain->AddFriend(prettyHkTree);


  //  char textLabel[180];
  //  char pngName[180];
  char histName[180];
  char histTitle[180];
  TGraph *gr[9]={0};


  TFile *fp = new TFile("clockPeriod.root","RECREATE");
  TH1F *histPeriod[9][4][2];
  for(int surf=0;surf<9;surf++) {
    for(int chip=0;chip<4;chip++) {
      for(int i=0;i<2;i++) {
	sprintf(histName,"histPeriod%d_%d_%d",surf,chip,i);
	sprintf(histTitle,"Clock Period (SURF %d, Chip %d, Peak %d)",surf+1,chip+1,i+1);
	histPeriod[surf][chip][i]=new TH1F(histName,histTitle,1000,28,32);
      }
    }
  }
  Int_t thisSurf,thisChip;
  Double_t period1,period2;
  UInt_t eventNumber;
  Int_t rco,firstHitbus,lastHitbus,wrappedHitbus;
  TTree *periodTree = new TTree("periodTree","Tree of Clock Periods");
  periodTree->Branch("eventNumber",&eventNumber,"eventNumber/i");
  periodTree->Branch("surf",&thisSurf,"surf/I");
  periodTree->Branch("chip",&thisChip,"chip/I");
  periodTree->Branch("rco",&rco,"rco/I");
  periodTree->Branch("firstHitbus",&firstHitbus,"firstHitbus/I");
  periodTree->Branch("lastHitbus",&lastHitbus,"lastHitbus/I");
  periodTree->Branch("wrappedHitbus",&wrappedHitbus,"wrappedHitbus/I");
  periodTree->Branch("period1",&period1,"period1/D");
  periodTree->Branch("period2",&period2,"period2/D");


  
  Long64_t nentries=eventChain->GetEntries();
  
  Long64_t maxEntry=startEntry+numEntries;
  if(nentries<maxEntry)
     maxEntry=nentries;
  
  TH1F histHigh("histHigh","histHigh",1000,0,1000);
  TH1F histLow("histLow","histLow",1000,-1000,0);
  for(Long64_t entry=startEntry;entry<maxEntry;entry++) {
    
    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    prettyHkTree->GetEntry(entry);
       
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlusFancyClockZero,hk);
    if((entry-startEntry)%100==0)
       cout << realEvent.eventNumber << endl;
    
    Int_t labChip;
    Double_t peakPos[9][2];
    for(int surf=0;surf<9;surf++) {
       //       for(int chan=0;chan<9;chan++) {
       //	  cout << realEvent.fNumPoints[realEvent.getChanIndex(surf,chan)] << endl;
       //       }
       //       graphPad->cd(surf+1);
       Int_t ci=UsefulAnitaEvent::getChanIndex(surf,8);

       labChip=realEvent.getLabChip(ci);
       rco=realEvent.getRCO(ci);
       firstHitbus=realEvent.getFirstHitBus(ci);
       lastHitbus=realEvent.getLastHitBus(ci);
       wrappedHitbus=realEvent.getWrappedHitBus(ci);
       //First up we make clocks that are bounded by -1 to 1.
       histLow.Reset();
       histHigh.Reset();
       for(int i=0;i<realEvent.fNumPoints[ci];i++) {
	  if(realEvent.fVolts[ci][i]>0) {
	   histHigh.Fill(realEvent.fVolts[ci][i]);
	  }
	  else {
	     histLow.Fill(realEvent.fVolts[ci][i]);
	  }
       }
       //cout << maxVal << "\t" << minVal << endl;
       //       cout << histHigh.GetMean() << "\t" << histLow.GetMean() << endl;
       Double_t offset=(histHigh.GetMean()+histLow.GetMean())/2;
       Double_t maxVal=histHigh.GetMean()-offset;
       Double_t minVal=histLow.GetMean()-offset;
       //       cout << maxVal << "\t" << minVal << endl;
       
       Int_t numPoints=realEvent.fNumPoints[ci];
       Double_t times[260];
       Double_t volts[260];
       Int_t gotPhiGuess=0;
       Float_t phiGuess=0;
       for(int i=0;i<realEvent.fNumPoints[ci];i++) {
	  times[i]=realEvent.fTimes[ci][i];
	 Double_t tempV=realEvent.fVolts[ci][i]-offset;	
// 	 if(tempV>maxVal*0.9)
// 	   volts[i]=1;
// 	 else if(tempV<minVal*0.9)
// 	   volts[i]=-1;
// 	 else {
	   volts[i]=tempV/maxVal;
	   //	 }
	 
	 if(!gotPhiGuess) {
	    if(tempV>=0 && (realEvent.fVolts[ci][i+1]-offset)<0) {
	       if(i>3) {
		  phiGuess=times[i];
		  gotPhiGuess=1;
	       }
	    }
	 }
	 
       }
       
       //Now we can make the graphs
       gr[surf] = new TGraph(numPoints,times,volts);
       //       std::cout << times[1]-times[0] <<"\t" << deltaT << "\n";
      //      canClock->cd(surf+1);
      TGraph *grCor = FFTtools::getInterpolatedCorrelationGraph(gr[surf],gr[surf],deltaT);
      //      grCor->Draw("al");
      Int_t dtZero=FFTtools::getPeakBin(grCor);
      Double_t x,y,xl,yl,xr,yr;
      Int_t corPoints=grCor->GetN();
      //Take positive peaks first
      int peakCount=0;
      for(int i=dtZero+2;i<corPoints-1;i++) {
	grCor->GetPoint(i-1,xl,yl);
	grCor->GetPoint(i,x,y);
	grCor->GetPoint(i+1,xr,yr);
	if(y>yl && y>yr && y>0) {
	  //Local maximum
	  peakPos[surf][peakCount]=x;
	  peakCount++;
	}
	if(peakCount>1) break;
      }
      histPeriod[surf][labChip][0]->Fill(peakPos[surf][0]);
      histPeriod[surf][labChip][1]->Fill(peakPos[surf][1]-peakPos[surf][0]);
      thisSurf=surf;
      thisChip=labChip;
      eventNumber=realEvent.eventNumber;
      period1=peakPos[surf][0];
      period2=peakPos[surf][1]-peakPos[surf][0];
      periodTree->Fill();
	


      //      cout << surf <<  "\t" 
      //	   << peakPos[surf][0] << "\t" 
      //	   << peakPos[surf][1] << "\n" ;
// 	   << peakPos[surf][2] << "\t" 
// 	   << peakPos[surf][3] << "\n";
      delete grCor;
    }
    for(int surf=0;surf<9;surf++) {
      if(gr[surf]) delete gr[surf];     
    }
    
    
  }
  periodTree->AutoSave();
  TCanvas *can = new TCanvas("can","can");
  can->Divide(4,9);
  for(int surf=0;surf<9;surf++) {
    for(int chip=0;chip<4;chip++) {
      for(int i=0;i<2;i++) {
	can->cd(4*surf+chip+1);
	histPeriod[surf][chip][i]->Write();
	histPeriod[surf][chip][i]->Draw();
      }
    }
  }

}

