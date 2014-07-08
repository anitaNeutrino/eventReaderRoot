//#include "AnitaConventions.h"
#include <iostream>
#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "AnitaEventCalibrator.h"
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
#include "TSystem.h"
#include "TROOT.h"
#include "TMath.h"
#include "TPaveText.h"
#include "FFTtools.h"
#include <iostream>
#include <fstream>

void takeDerivative(Int_t numPoints, Double_t *inputX, Double_t *inputY, Double_t *outputX, Double_t *outputY);
float Get_Interpolation_X(float x1, float y1, float x2, float y2, float y);

void correlateClocks()
{
   gStyle->SetPadTopMargin(0.05);
   gStyle->SetPadBottomMargin(0.2);
   gStyle->SetPadLeftMargin(0.12);
   gStyle->SetPadRightMargin(0.05);
   gStyle->SetTitleH(0.1);
   gStyle->SetLabelSize(0.1,"xy");
   gStyle->SetTitleSize(0.1,"xy");
   gStyle->SetTitleOffset(0.5,"y");
   gStyle->SetOptTitle(0);
   //   plotAllClocks(1028,1612,1);
}
  

void correlateClocks(char *baseName, int run, int startEntry, int numEntries) {

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


  //  char textLabel[180];
  //  char pngName[180];
  TGraph *gr[NUM_SURF]={0};
  TGraph *grNorm[NUM_SURF]={0};
  TGraph *grDeriv[NUM_SURF]={0};
  TGraph *grCorr[NUM_SURF]={0};
  TGraph *grDerivCorr[NUM_SURF]={0};




  TFile *fp = new TFile("clockFile.root","RECREATE");
  Double_t peakCorr[NUM_SURF];
  TTree *clockTree = new TTree("clockTree","Tree of Clock Correlations");
  clockTree->Branch("peakCorr",peakCorr,"peakCorr[10]/D");
  

  for(int entry=startEntry;entry<startEntry+numEntries;entry++) {

    
    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    //    prettyHkTree->GetEntry(entry);	
    //    Double_t clockPeriod=1./header->c3poNum;

           
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTLabAGCrossCorClock,header);
    //    cout << entry << "\t" <<  realEvent.eventNumber << " " << header->eventNumber << endl;
    //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;

 
    for(int surf=0;surf<NUM_SURF;surf++) {
      if(gr[surf]) delete gr[surf];
      if(grNorm[surf]) delete grNorm[surf];
      if(grDeriv[surf]) delete grDeriv[surf];
      if(grCorr[surf]) delete grCorr[surf];
      if(grDerivCorr[surf]) delete grDerivCorr[surf];
    }


  
    AnitaEventCalibrator *myCally = AnitaEventCalibrator::Instance();

    for(int surf=0;surf<NUM_SURF;surf++) {
      peakCorr[surf]=myCally->clockPhiArray[surf];    
    }
    clockTree->Fill();
  }
  clockTree->AutoSave();
}


void takeDerivative(Int_t numPoints, Double_t *inputX, Double_t *inputY, Double_t *outputX, Double_t *outputY) {
  int count=0;
  for(int samp=1;samp<numPoints;samp++) {
    Double_t deltaT=inputX[samp]-inputX[samp-1];
    Double_t deltaV=inputY[samp]-inputY[samp-1];
    outputX[count]=inputX[samp];
    outputY[count]=deltaV/deltaT;
    count++;
  }
}


float Get_Interpolation_X(float x1, float y1, float x2, float y2, float y){
  float x=(x2-x1)/(y2-y1)*(y-y1)+x1;
  return x;
}
