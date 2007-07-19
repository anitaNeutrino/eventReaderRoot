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



Double_t funcSquareWave(Double_t *x, Double_t *par);
void fitSquareWave(int run, int startEntry, int numEntries);

void fitSquareWave()
{
   gStyle->SetStatH(0.25);
   gStyle->SetStatW(0.25);
   fitSquareWave(1028,0,100000);
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

  
  TF1 *sqaurey = new TF1("sqaurey",funcSquareWave,5,90,7);
  //  sqaurey->SetNpx(1000);
  sqaurey->SetParameters(25,200,-200,12.881,16.428,0.33,0.33);
  sqaurey->SetParLimits(0,0,35);
  sqaurey->SetParLimits(1,50,350);
  sqaurey->SetParLimits(2,-400,-50);
  //  sqaurey->SetParLimits(3,8,16);
  //  sqaurey->SetParLimits(4,10,20);
  sqaurey->SetParLimits(3,12.881,12.881);
  sqaurey->SetParLimits(4,16.428,16.428);
  sqaurey->SetParLimits(5,0.33,0.33);
  sqaurey->SetParLimits(6,0.33,0.33);


  TFile *fp = new TFile("clockPhaseCalib.root","RECREATE");
  TH1F *histLowDt[9];
  TH1F *histHighDt[9];

  TH1F *histLowAdcChip[9][4];
  TH1F *histHighAdcChip[9][4];
  
  TH1F *histPhiDiff[9];
  TH1F *histPhiDiffChip[9][4];

  char histName[180];
  for(int surf=0;surf<9;surf++) {
     sprintf(histName,"histLowDt%d",surf);
     histLowDt[surf] = new TH1F(histName,histName,1000,0,30);
     sprintf(histName,"histHighDt%d",surf);
     histHighDt[surf] = new TH1F(histName,histName,1000,0,30);


     sprintf(histName,"histPhiDiff%d",surf);
     histPhiDiff[surf] = new TH1F(histName,histName,200,-10,10);
     for(int chip=0;chip<4;chip++) {
	sprintf(histName,"histPhiDiffChip%d_%d",surf,chip);
	histPhiDiffChip[surf][chip] = new TH1F(histName,histName,200,-10,10);
	sprintf(histName,"histLowAdcChip%d_%d",surf,chip);
	histLowAdcChip[surf][chip] = new TH1F(histName,histName,500,-500,0);
	sprintf(histName,"histHighAdcChip%d_%d",surf,chip);
	histHighAdcChip[surf][chip] = new TH1F(histName,histName,500,0,500);
     }

  }

  TH1F *histLowDtAll = new TH1F("histLowDtAll","histLowDtAll",3000,0,30);
  TH1F *histHighDtAll = new TH1F("histHighDtAll","histHighDtAll",3000,0,30);
  

  UInt_t eventNumber;
  Int_t surfNum,chipNum;
  Double_t phase,phase0;
  Double_t low,high;
  TTree *phaseTree = new TTree("phaseTree","Tree of Clock Phase Related Quantities");
  phaseTree->Branch("eventNumber",&eventNumber,"eventNumber/i");
  phaseTree->Branch("surf",&surfNum,"surf/I");
  phaseTree->Branch("chip",&chipNum,"chip/I");
  phaseTree->Branch("phase",&phase,"phase/D");
  phaseTree->Branch("phase0",&phase0,"phase0/D");
  phaseTree->Branch("low",&low,"low/D");
  phaseTree->Branch("high",&high,"high/D");



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
	  sqaurey->SetParameters(phiGuess,200,-200,12.881,16.428,0.33,0.33);
	  grTemp.Fit(sqaurey,"QR","goff");
	  //gr[surf]->Fit(sqaurey,"QR","goff");

	  if(surf==0) 
	     phi0=sqaurey->GetParameter(0);

	  histLowDt[surf]->Fill(sqaurey->GetParameter(3));
	  histLowDtAll->Fill(sqaurey->GetParameter(3));
	  histHighDt[surf]->Fill(sqaurey->GetParameter(4));
	  histHighDtAll->Fill(sqaurey->GetParameter(4));
	  
	  
	  
	  float period=sqaurey->GetParameter(3)+sqaurey->GetParameter(4)+
	     sqaurey->GetParameter(5)+sqaurey->GetParameter(6);
	  
	  float phi=sqaurey->GetParameter(0);
	  if((phi-phi0)>15)
	     phi-=period;
	  if((phi-phi0)<-15)
	     phi+=period;
	  
	  
	  //       if(surf==4)
	  //	  cout << phi-phi0 << "\t" << phi << "\t" << phi0 << endl;
	  histPhiDiff[surf]->Fill(phi-phi0);

	  Int_t labChip=realEvent.getLabChip(ci);
	  histPhiDiffChip[surf][labChip]->Fill(phi-phi0);
	  histLowAdcChip[surf][labChip]->Fill(sqaurey->GetParameter(2));
	  histHighAdcChip[surf][labChip]->Fill(sqaurey->GetParameter(1));


	  eventNumber=realEvent.eventNumber;
	  surfNum=surf;
	  chipNum=labChip;
	  low=sqaurey->GetParameter(2);
	  high=sqaurey->GetParameter(1);
	  phase=phi;
	  phase0=phi0;
	  phaseTree->Fill();

       }
       delete gr[surf];
    }
    
  }
//   TCanvas * canLow = new TCanvas("canLow","canLow");
//   canLow->Divide(2,5);
//   for(int surf=0;surf<9;surf++) {
//      canLow->cd(surf+1);
//      histLowAdc[surf]->Draw();
//   }


//   TCanvas * canHigh = new TCanvas("canHigh","canHigh");
//   canHigh->Divide(2,5);
//   for(int surf=0;surf<9;surf++) {
//      canHigh->cd(surf+1);
//      histHighAdc[surf]->Draw();
//   }

//   TCanvas * canPhi = new TCanvas("canPhi","canPhi");
//   canPhi->Divide(2,5);
//   for(int surf=0;surf<9;surf++) {
//      canPhi->cd(surf+1);
//      histPhiDiff[surf]->Draw();
//      histPhiDiff[surf]->Write();
//   }



  ofstream CalibNums("clockCalibNums.dat");
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
	histLowAdcChip[surf][chip]->Write();
	histHighAdcChip[surf][chip]->Write();
	CalibNums << surf << "\t" << chip << "\t" << calib << endl;
     }
  }
  phaseTree->Write();
  //  fp->Close();
//   TCanvas *canAll = new TCanvas("canAll","canAll");
//   canAll->Divide(1,2);
//   canAll->cd(1);
//   histLowDtAll->Draw();
//   canAll->cd(2);
//   histHighDtAll->Draw();


}

Double_t funcSquareWave(Double_t *x, Double_t *par)
{
   Double_t phi=par[0];
   Double_t a=par[1];
   Double_t b=par[2];
   Double_t dtLow=par[3];
   Double_t dtHigh=par[4];
   Double_t sllh=par[5];
   Double_t slhl=par[6];


   Double_t period=dtLow+dtHigh+sllh+slhl;

   Double_t t=x[0]-phi;
   
   Double_t mlh=(a-b)/sllh;
   Double_t mhl=(b-a)/slhl;

   while(t<0) {
      t+=period;
   }
   while(t>period) {
      t-=period;
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
