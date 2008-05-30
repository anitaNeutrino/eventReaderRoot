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
#include "TH2.h"
#include "TF1.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TPaveText.h"
#include "TPaveLabel.h"
#include "TROOT.h"
#include "TMath.h"
#include <iostream>
#include <fstream>

void plotOneSurf(char *baseDir,int run, int startEntry, int numEntries,int surf);
void plotRMSCanvas(char *baseDir, int run,int startEntry,int numEntries=1);


void plotOneSurf()
{ 
   plotOneSurf("/unix/anita2/testing/rootFiles",1028,1000,1,1);
}
  

int getNiceColour(int index)
{
    if(index>10) return index;
    Int_t niceColours[11]={kAzure+2,kRed+1,kGreen+1,kMagenta+1,kCyan+1,kOrange+2,kGreen-2,12,40,20,41};
    return niceColours[index];
}


void plotOneSurf(char *baseDir,int run, int startEntry, int numEntries,int surf) {
  char textLabel[180];
  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  //  char hkName[FILENAME_MAX];
  sprintf(eventName,"%s/run%d/eventFile%d*.root",baseDir,run,run);
  sprintf(headerName,"%s/run%d/headFile%d.root",baseDir,run,run);
  //  sprintf(hkName,"/unix/anita2/testing/rootFiles/run%d/prettyHkFile%d.root",run,run);

  RawAnitaEvent *event = 0;
  RawAnitaHeader *header =0;
  //  PrettyAnitaHk *hk = 0;
  
  TChain *eventChain = new TChain("eventTree");
  eventChain->Add(eventName);
  eventChain->SetBranchAddress("event",&event);

  TFile *fpHead = new TFile(headerName);
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

  gStyle->SetPadTopMargin(0.00);
  gStyle->SetPadBottomMargin(0.00);
  gStyle->SetPadLeftMargin(0.05);
  gStyle->SetPadRightMargin(0.01);
  gStyle->SetTitleH(0.1);
  gStyle->SetLabelSize(0.1,"xy");
  gStyle->SetTitleSize(0.1,"xy");
  gStyle->SetTitleOffset(0.5,"y");
  gStyle->SetOptTitle(0);

  TCanvas *canSurf = (TCanvas*) gROOT->FindObject("canSurf");
  if(!canSurf) {
    canSurf = new TCanvas("canSurf","canSurf",800,600);
  }
  TGraph *gr[9]={0};

  for(int entry=startEntry;entry<startEntry+numEntries;entry++) {
    
    //Stupidly most do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    //    prettyHkTree->GetEntry(entry);
    
    
    UsefulAnitaEvent realEvent(event,WaveCalType::kJustUnwrap);
    cout << realEvent.eventNumber << " " << header->eventNumber << endl;
    //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
    
    canSurf->Clear();
    canSurf->SetTopMargin(0);
    TPaveText *topPave = new TPaveText(0.05,0.92,0.95,0.98);
    topPave->SetBorderSize(0);
    
    sprintf(textLabel,"Run %d -- Event %u -- SURF %d",run,realEvent.eventNumber,surf+1);
    TText *eventText = topPave->AddText(textLabel);
    eventText->SetTextColor(50);
    topPave->Draw();
    
//     TPad *graphPad = new TPad("graphPad","This is pad1",0,0.02,0.99,0.9);
//     graphPad->SetTopMargin(0);
//     graphPad->Draw();
//     graphPad->Clear();
//     graphPad->Divide(2,5,0.00,0.00);
    //    graphPad->Clear();
    for(int chan=0;chan<9;chan++) {
      if(gr[chan]) delete gr[chan];
    }
    Double_t left[2]={0,0.5}; //0 is even, 1 is odd
    Double_t right[2]={0.5,1};
    Double_t top[5]={0.9,0.73,0.56,0.39,0.22};
    Double_t bottom[5]={0.73,0.56,0.39,0.22,0.05};
    char padName[180];
    for(int chan=0;chan<9;chan++) {
      canSurf->cd();
      sprintf(padName,"chanPad%d",chan);
      //      cout << left[chan%2] << "\t" << bottom[chan/2] << "\t" << right[chan%2]
      //      	   << "\t" << top[chan/2] << endl;
      TPad *paddy = new TPad(padName,padName,left[chan%2],bottom[chan/2],right[chan%2],top[chan/2]);
      paddy->Draw();
      paddy->cd();
      if(chan>=7)
	paddy->SetBottomMargin(0.10);      
      gr[chan]=realEvent.getGraph(realEvent.getChanIndex(surf,chan));
      gr[chan]->SetLineColor(getNiceColour(chan));
      gr[chan]->Draw("al");
      if(chan<8) gr[chan]->GetYaxis()->SetRangeUser(-50,50);
      cout << gr[chan]->GetN() << "\t" << gr[chan]->GetMean(2) << "\t" << gr[chan]->GetRMS(2) <<  endl;
    }
    canSurf->Update();
    gSystem->Sleep(1000);
  }
  //  canSurf->cd(10);
  //  sqaurey->Draw();
}

void plotRMSCanvas(char *baseDir,int run,int startEntry,int numEntries)
{
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  gStyle->SetCanvasColor(0);
   gStyle->SetFrameFillColor(0);
   //   gStyle->SetFillColor(0);
   gROOT->ForceStyle();
   char eventName[FILENAME_MAX];
   char headerName[FILENAME_MAX];
   //  char hkName[FILENAME_MAX];
   sprintf(eventName,"%s/run%d/eventFile%d*.root",baseDir,run,run);
   sprintf(headerName,"%s/run%d/headFile%d.root",baseDir,run,run);
   //  sprintf(hkName,"/unix/anita2/testing/rootFiles/run%d/prettyHkFile%d.root",run,run);
   
   RawAnitaEvent *event = 0;
   RawAnitaHeader *header =0;
   //  PrettyAnitaHk *hk = 0;
   
   TChain *eventChain = new TChain("eventTree");
   eventChain->Add(eventName);
   eventChain->SetBranchAddress("event",&event);
   
   TFile *fpHead = new TFile(headerName);
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
   
   char fileName[180];
   sprintf(fileName,"meanRms%d.root",run);     
   TFile *fpOut = new TFile(fileName,"RECREATE");
   TH2F *histRMS = new TH2F("histRMS","histRMS",ACTIVE_SURFS,0.5,ACTIVE_SURFS+0.5,8,0.5,8.5);
   TH2F *histMean = new TH2F("histMean","histMean",ACTIVE_SURFS,0.5,ACTIVE_SURFS+0.5,8,0.5,8.5);
   TH2F *histMax = new TH2F("histMax","histMax",ACTIVE_SURFS,0.5,ACTIVE_SURFS+0.5,8,0.5,8.5);
   TH2F *histP2P = new TH2F("histP2P","histP2P",ACTIVE_SURFS,0.5,ACTIVE_SURFS+0.5,8,0.5,8.5);
   TTree *meanTree = new TTree("meanTree","Tree of Means");
   UInt_t treeEntry;
   UInt_t eventNumber;
   UInt_t triggerTime;
   UInt_t triggerTimeNs;
   Double_t chanMean[ACTIVE_SURFS][9];
   Double_t chanRMS[ACTIVE_SURFS][9];
   Double_t chanMax[ACTIVE_SURFS][9];
   Double_t chanMin[ACTIVE_SURFS][9];
   meanTree->Branch("entry",&treeEntry,"entry/i");
   meanTree->Branch("eventNumber",&eventNumber,"eventNumber/i");
   meanTree->Branch("triggerTime",&triggerTime,"triggerTime/i");
   meanTree->Branch("triggerTimeNs",&triggerTimeNs,"triggerTimeNs/i");
   meanTree->Branch("chanMean",&chanMean[0][0],"chanMean[10][9]/D");
   meanTree->Branch("chanRMS",&chanRMS[0][0],"chanRMS[10][9]/D");
   meanTree->Branch("chanMax",&chanMax[0][0],"chanMax[10][9]/D");
   meanTree->Branch("chanMin",&chanMin[0][0],"chanMin[10][9]/D");

  Int_t count=0;
  for(int entry=startEntry;entry<startEntry+numEntries;entry++) {
  
     //Stupidly most do this to be perfectly safe  
     eventChain->GetEntry(entry);
     headTree->GetEntry(entry);
     //    prettyHkTree->GetEntry(entry);
     
  
     UsefulAnitaEvent realEvent(event,WaveCalType::kJustUnwrap);
     //     cout << realEvent.eventNumber << " " << header->eventNumber << endl;
     //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
     treeEntry=entry;
     eventNumber=header->eventNumber;
     triggerTime=header->triggerTime;
     triggerTimeNs=header->triggerTimeNs;
     
     
     for(int surf=1;surf<ACTIVE_SURFS;surf++) {
	for(int chan=0;chan<8;chan++) {
	   TGraph *gr=realEvent.getGraph(realEvent.getChanIndex(surf,chan));
	   Double_t *volts = gr->GetY();
	   Double_t maxVolt=TMath::MaxElement(gr->GetN(),volts);
	   Double_t minVolt=TMath::MinElement(gr->GetN(),volts);
	   histRMS->Fill(surf+1,chan+1,gr->GetRMS(2));
	   histMean->Fill(surf+1,chan+1,gr->GetMean(2));
	   histMax->Fill(surf+1,chan+1,maxVolt);
	   histP2P->Fill(surf+1,chan+1,maxVolt-minVolt);
	   chanMean[surf][chan]=gr->GetMean(2);
	   chanRMS[surf][chan]=gr->GetRMS(2);
	   chanMax[surf][chan]=maxVolt;
	   chanMin[surf][chan]=minVolt;
	   delete gr;
	}
     }
     meanTree->Fill();

     count++;
  }
  meanTree->AutoSave();
  Double_t scale=1./count;
  histMean->Scale(scale);
  histRMS->Scale(scale);
  histMax->Scale(scale);
  histP2P->Scale(scale);
  TCanvas *canRMS = (TCanvas*) gROOT->FindObject("canRMS");
  if(!canRMS) {
     canRMS = new TCanvas("canRMS","canRMS",800,600);
  }
  canRMS->Clear();  
  histRMS->SetTitle("Waveform RMS (No Clock)");
  histRMS->SetXTitle("SURF");
  histRMS->SetYTitle("Channel");
  histRMS->SetStats(0);
  histRMS->Draw("colz");
  
  TCanvas *canMean = (TCanvas*) gROOT->FindObject("canMean");
  if(!canMean) {
     canMean = new TCanvas("canMean","canMean",800,600);
  }
  canMean->Clear();  
  histMean->SetTitle("Waveform Mean (No Clock)");
  histMean->SetXTitle("SURF");
  histMean->SetYTitle("Channel");
  histMean->SetStats(0);
  histMean->Draw("colz");
   
  TCanvas *canMax = (TCanvas*) gROOT->FindObject("canMax");
  if(!canMax) {
     canMax = new TCanvas("canMax","canMax",800,600);
  }
  canMax->Clear();  
  histMax->SetTitle("Waveform Max (No Clock)");
  histMax->SetXTitle("SURF");
  histMax->SetYTitle("Channel");
  histMax->SetStats(0);
  histMax->Draw("colz");
   
  TCanvas *canP2P = (TCanvas*) gROOT->FindObject("canP2P");
  if(!canP2P) {
     canP2P = new TCanvas("canP2P","canP2P",800,600);
  }
  canP2P->Clear();  
  histP2P->SetTitle("Waveform P2P (No Clock)");
  histP2P->SetXTitle("SURF");
  histP2P->SetYTitle("Channel");
  histP2P->SetStats(0);
  histP2P->Draw("colz");
  

}
