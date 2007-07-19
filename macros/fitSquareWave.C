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
#include <iostream>
#include <fstream>

const float clockPeriod=29.970;


Double_t myFuncSquareWave(Double_t *x, Double_t *par);
void fitSquareWave(int run, int startEntry, int numEntries);

void fitSquareWave()
{
   gStyle->SetStatH(0.25);
   gStyle->SetStatW(0.25);
   fitSquareWave(1028,0,100);
}
  

void fitSquareWave(int run, int startEntry, int numEntries) {


  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  char hkName[FILENAME_MAX];
  sprintf(eventName,"/unix/anita1/webData/firstDay/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"/unix/anita1/webData/firstDay/run%d/headFile%d.root",run,run);
  sprintf(hkName,"/unix/anita1/webData/firstDay/run%d/prettyHkFile%d.root",run,run);

  RawAnitaEvent *event = 0;
  RawAnitaHeader *header =0;
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
  TGraph *gr[9]={0};


  TF1 *sqaurey = new TF1("sqaurey",myFuncSquareWave,5,90,5);
  //  sqaurey->SetNpx(1000);
  sqaurey->SetParameters(25,200,-200,0.44,0.5);
  sqaurey->SetParLimits(0,0,35);
  sqaurey->SetParLimits(1,50,350);
  sqaurey->SetParLimits(2,-400,-50);
  sqaurey->SetParLimits(3,0.44,0.44);
  sqaurey->SetParLimits(4,0.5,0.5);


  TFile *fp = new TFile("clockPhaseCalib.root","RECREATE");
  TH1F *histLowFrac[9];
  TH1F *histSlopeTime[9];

  
  TH1F *histPhiDiff[9];
  TH1F *histPhiDiffChip[9][4];

  char histName[180];
  for(int surf=0;surf<9;surf++) {
     sprintf(histName,"histLowFrac%d",surf);
     histLowFrac[surf] = new TH1F(histName,histName,1000,0,1);
     sprintf(histName,"histSlopeTime%d",surf);
     histSlopeTime[surf] = new TH1F(histName,histName,1000,0,1);


     sprintf(histName,"histPhiDiff%d",surf);
     histPhiDiff[surf] = new TH1F(histName,histName,200,-10,10);
     for(int chip=0;chip<4;chip++) {
	sprintf(histName,"histPhiDiffChip%d_%d",surf,chip);
	histPhiDiffChip[surf][chip] = new TH1F(histName,histName,200,-10,10);
     }

  }
  
  TH1F *histLowFracAll = new TH1F("histLowFracAll","histLowFracAll",1000,0,1);
  TH1F *histSlopeTimeAll = new TH1F("histSlopeTimeAll","histSlopeTimeAll",1000,0,1);


  Long64_t nentries=eventChain->GetEntries();
  
  Long64_t maxEntry=startEntry+numEntries;
  if(nentries<maxEntry)
     maxEntry=nentries;

  for(Long64_t entry=startEntry;entry<maxEntry;entry++) {
    
    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    prettyHkTree->GetEntry(entry);
       
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlus,hk);
    if((entry-startEntry)%100==0)
       cout << realEvent.eventNumber << endl;
    
    Float_t phi0=0;
    for(int surf=0;surf<9;surf++) {
       //       for(int chan=0;chan<9;chan++) {
       //	  cout << realEvent.fNumPoints[realEvent.getChanIndex(surf,chan)] << endl;
       //       }
       //       graphPad->cd(surf+1);
       Int_t ci=UsefulAnitaEvent::getChanIndex(surf,8);
       gr[surf] = realEvent.getGraph(ci);

       Int_t numPoints= gr[surf]->GetN();
       //       Double_t mean = gr[surf]->GetMean(2);
       //       cout << mean << endl;
       Double_t *times = gr[surf]->GetX();
       Double_t *volts = gr[surf]->GetY();

       //       for(int i=0;i<numPoints;i++) {
	  //	  cout << times[i] << "\t" << volts[i] << endl;
       // volts[i]-=mean;
       //       }
     
       
//        //       gr[surf]->Draw("al");
//        //Take a guess at phi
       Double_t vMax=TMath::MaxElement(numPoints,volts);
       Double_t vMin=TMath::MinElement(numPoints,volts);


       Float_t phiGuess=0;
       for(int i=0;i<realEvent.fNumPoints[ci]-1;i++) {
	  if(volts[i]>=0 &&
	     volts[i+1]<0) {
	     phiGuess=times[i];
	     if(i>3)
		break;
	  }
       }
       //       cout << phiGuess << endl; 
       {
	  TGraph grTemp(numPoints,times,volts);
	  //	  sqaurey->SetParameters(phiGuess,200,-200,0.44,0.55);
	  sqaurey->SetParameters(phiGuess,vMax,vMin,0.44,0.5);

  
	  grTemp.Fit(sqaurey,"QR","goff");
	  //gr[surf]->Fit(sqaurey,"QR","goff");

	  if(surf==0) 
	     phi0=sqaurey->GetParameter(0);


	  
	  
	  //	  float period=sqaurey->GetParameter(3)+sqaurey->GetParameter(4)+
	  //	     sqaurey->GetParameter(5)+sqaurey->GetParameter(6);
	  
	  float phi=sqaurey->GetParameter(0);
	  if((phi-phi0)>15)
	     phi-=clockPeriod;
	  if((phi-phi0)<-15)
	     phi+=clockPeriod;
	  
	  
	  //       if(surf==4)
	  //	  cout << phi-phi0 << "\t" << phi << "\t" << phi0 << endl;
	  histPhiDiff[surf]->Fill(phi-phi0);

	  Int_t labChip=realEvent.getLabChip(ci);
	  histPhiDiffChip[surf][labChip]->Fill(phi-phi0);


	  histSlopeTimeAll->Fill(sqaurey->GetParameter(4));
	  histSlopeTime[surf]->Fill(sqaurey->GetParameter(4));
	  
	  histLowFracAll->Fill(sqaurey->GetParameter(3));
	  histLowFrac[surf]->Fill(sqaurey->GetParameter(3));

       }
       delete gr[surf];
    }
    
  }
//   TCanvas * canLowFrac = new TCanvas("canLowFrac","canLowFrac");
//   canLowFrac->Divide(2,5);
//   for(int surf=0;surf<9;surf++) {
//      canLowFrac->cd(surf+1);
//      histLowFrac[surf]->Draw();
//   }


//   TCanvas * canSlopeTime = new TCanvas("canSlopeTime","canSlopeTime");
//   canSlopeTime->Divide(2,5);
//   for(int surf=0;surf<9;surf++) {
//      canSlopeTime->cd(surf+1);
//      histSlopeTime[surf]->Draw();
//   }


  ofstream CalibNums("clockCalibNumsFitted.dat");
  CalibNums << "#SURF" << "\t" << "CHIP" << "\t" << "Calib" << endl;
  TCanvas * canPhiChip = new TCanvas("canPhiChip","canPhiChip");
  canPhiChip->Divide(4,9);
  for(int surf=0;surf<9;surf++) {
     for(int chip=0;chip<4;chip++) {
	int index=(4*surf)+chip+1;
	canPhiChip->cd(index);
	histPhiDiffChip[surf][chip]->Draw();
	float calib=0;
	if(surf>0) {
	   histPhiDiffChip[surf][chip]->Fit("gaus");
	   calib=histPhiDiffChip[surf][chip]->GetFunction("gaus")->GetParameter(1);
	}
	histPhiDiffChip[surf][chip]->Write();
	CalibNums << surf << "\t" << chip << "\t" << calib << endl;
     }
  }


}


Double_t myFuncSquareWave(Double_t *x, Double_t *par)
{
   Double_t phi=par[0];
   Double_t a=par[1];
   Double_t b=par[2];

   Double_t sllh=par[4];
   Double_t slhl=par[4];
   
   Double_t periodLeft=clockPeriod-2*par[4];   
   Double_t dtLow=par[3]*periodLeft;
   Double_t dtHigh=(1-par[3])*periodLeft;


   Double_t t=x[0]-phi;
   
   Double_t mlh=(a-b)/sllh;
   Double_t mhl=(b-a)/slhl;

   //   cout << t << "\t" << clockPeriod << endl;
   while(t<0) {
      t+=clockPeriod;
   }
   while(t>clockPeriod) {
      t-=clockPeriod;
   }
   if(t<dtLow)
      return b;
   if(t<dtLow+sllh) {
      Double_t t1=t-dtLow;
      return (t1*mlh)+b;
   }
   if(t<dtLow+sllh+dtHigh)
      return a;
   if(t<dtLow+sllh+dtHigh+slhl) {
      Double_t t2=t-(dtLow+sllh+dtHigh);
      return (mhl*t2)+a;
   }
   
      
   return a;



}
