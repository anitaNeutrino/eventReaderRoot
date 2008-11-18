//#include "AnitaConventions.h"
#include <iostream>
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
#include "TSystem.h"
#include "TROOT.h"
#include "TMath.h"
#include "TPaveText.h"
#include "FFTtools.h"
#include <iostream>
#include <fstream>

void takeDerivative(Int_t numPoints, Double_t *inputX, Double_t *inputY, Double_t *outputX, Double_t *outputY);
float Get_Interpolation_X(float x1, float y1, float x2, float y2, float y);

void plotAllClocks()
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
  

void plotAllClocks(int run, int startEntry, int numEntries) {

  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  //  char hkName[FILENAME_MAX];
  sprintf(eventName,"/TBdata/anita/antarctica08/root/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"/TBdata/anita/antarctica08/root/run%d/headFile%d.root",run,run);
  //  sprintf(hkName,"/TBdata/anita/antarctica08/root/run%d/prettyHkFile%d.root",run,run);

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


  TCanvas *canClock = (TCanvas*) gROOT->FindObject("canClock");
  if(!canClock) {
    canClock = new TCanvas("canClock","canClock",800,800);
  }

  TCanvas *canClockNorm = (TCanvas*) gROOT->FindObject("canClockNorm");
  if(!canClockNorm) {
    canClockNorm = new TCanvas("canClockNorm","canClockNorm",800,800);
  }


  TCanvas *canClockDeriv = (TCanvas*) gROOT->FindObject("canClockDeriv");
  if(!canClockDeriv) {
    canClockDeriv = new TCanvas("canClockDeriv","canClockDeriv",800,800);
  }


  TCanvas *canClockCorr = (TCanvas*) gROOT->FindObject("canClockCorr");
  if(!canClockCorr) {
    canClockCorr = new TCanvas("canClockCorr","canClockCorr",800,800);
  }


  TCanvas *canClockDerivCorr = (TCanvas*) gROOT->FindObject("canClockDerivCorr");
  if(!canClockDerivCorr) {
    canClockDerivCorr = new TCanvas("canClockDerivCorr","canClockDerivCorr",800,800);
  }

  char textLabel[180];
  //  char pngName[180];
  TGraph *gr[NUM_SURF]={0};
  TGraph *grNorm[NUM_SURF]={0};
  TGraph *grDeriv[NUM_SURF]={0};
  TGraph *grCorr[NUM_SURF]={0};
  TGraph *grDerivCorr[NUM_SURF]={0};

  for(int entry=startEntry;entry<startEntry+numEntries;entry++) {

    
    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    //    prettyHkTree->GetEntry(entry);	
    Double_t clockPeriod=1./header->c3poNum;

    
       
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTLabAG,header);
    cout << entry << "\t" <<  realEvent.eventNumber << " " << header->eventNumber << endl;
    //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;

    {
      canClock->Clear();
      canClock->SetTopMargin(0);
      TPaveText *topPave = new TPaveText(0.05,0.92,0.95,0.98);
      topPave->SetBorderSize(0);
      sprintf(textLabel,"Raw Clock -- Event %u",realEvent.eventNumber);
      TText *eventText = topPave->AddText(textLabel);
      eventText->SetTextColor(50);
      topPave->Draw();
    }  
    TPad *graphPadClock = new TPad("graphPadClock","This is pad1",0,0,1,0.9);
    graphPadClock->SetTopMargin(0);
    graphPadClock->Draw();
    graphPadClock->Clear();
    graphPadClock->Divide(2,5);
    
    {
      canClockNorm->Clear();
      canClockNorm->SetTopMargin(0);
      TPaveText *topPave = new TPaveText(0.05,0.92,0.95,0.98);
      topPave->SetBorderSize(0);
      sprintf(textLabel,"Normalised Clock -- Event %u",realEvent.eventNumber);
      TText *eventText = topPave->AddText(textLabel);
      eventText->SetTextColor(50);
      topPave->Draw();
    }
    TPad *graphPadClockNorm = new TPad("graphPadClockNorm","This is pad1",0,0,1,0.9);
    graphPadClockNorm->SetTopMargin(0);
    graphPadClockNorm->Draw();
    graphPadClockNorm->Clear();
    graphPadClockNorm->Divide(2,5);
    
    {
      canClockDeriv->Clear();
      canClockDeriv->SetTopMargin(0);
      TPaveText *topPave = new TPaveText(0.05,0.92,0.95,0.98);
      topPave->SetBorderSize(0);
      sprintf(textLabel,"Differentiated Clock -- Event %u",realEvent.eventNumber);
      TText *eventText = topPave->AddText(textLabel);
      eventText->SetTextColor(50);
      topPave->Draw();
    }
    TPad *graphPadClockDeriv = new TPad("graphPadClockDeriv","This is pad1",0,0,1,0.9);
    graphPadClockDeriv->SetTopMargin(0);
    graphPadClockDeriv->Draw();
    graphPadClockDeriv->Clear();
    graphPadClockDeriv->Divide(2,5);
    
    {
      canClockCorr->Clear();
      canClockCorr->SetTopMargin(0);
      TPaveText *topPave = new TPaveText(0.05,0.92,0.95,0.98);
      topPave->SetBorderSize(0);
      sprintf(textLabel,"Correlated Clock -- Event %u",realEvent.eventNumber);
      TText *eventText = topPave->AddText(textLabel);
      eventText->SetTextColor(50);
      topPave->Draw();
    }
    TPad *graphPadClockCorr = new TPad("graphPadClockCorr","This is pad1",0,0,1,0.9);
    graphPadClockCorr->SetTopMargin(0);
    graphPadClockCorr->Draw();
    graphPadClockCorr->Clear();
    graphPadClockCorr->Divide(2,5);
      
    {
      canClockDerivCorr->Clear();
      canClockDerivCorr->SetTopMargin(0);
      TPaveText *topPave = new TPaveText(0.05,0.92,0.95,0.98);
      topPave->SetBorderSize(0);
      sprintf(textLabel,"Differentiated Correlated Clock -- Event %u",realEvent.eventNumber);
      TText *eventText = topPave->AddText(textLabel);
      eventText->SetTextColor(50);
      topPave->Draw();
    }  
    TPad *graphPadClockDerivCorr = new TPad("graphPadClockDerivCorr","This is pad1",0,0,1,0.9);
    graphPadClockDerivCorr->SetTopMargin(0);
    graphPadClockDerivCorr->Draw();
    graphPadClockDerivCorr->Clear();
    graphPadClockDerivCorr->Divide(2,5);
      

    //    graphPad->Clear();
    for(int surf=0;surf<NUM_SURF;surf++) {
      if(gr[surf]) delete gr[surf];
      if(grNorm[surf]) delete grNorm[surf];
      if(grDeriv[surf]) delete grDeriv[surf];
      if(grCorr[surf]) delete grCorr[surf];
      if(grDerivCorr[surf]) delete grDerivCorr[surf];
    }


    Double_t fLowArray[NUM_SAMP]={0};
    Double_t fHighArray[NUM_SAMP]={0};
    
    

    Double_t normClock[NUM_SAMP]={0};
    Double_t clockTimes[NUM_SAMP]={0};
    Double_t clockDeriv[NUM_SAMP]={0};

    Double_t phiRising[NUM_SURF];
    Double_t phiFalling[NUM_SURF];
    Double_t peakCorr[NUM_SURF];
    Double_t peakDeriv[NUM_SURF];
    Double_t peakDerivCorr[NUM_SURF];


    for(int surf=0;surf<NUM_SURF;surf++) {
      gr[surf] = realEvent.getGraph(UsefulAnitaEvent::getChanIndex(surf,8));
      

      Double_t *mvArray = gr[surf]->GetY();
      Double_t *times =gr[surf]->GetX();
      
      //First fill temp arrays
      Int_t numPoints=gr[surf]->GetN();
      Int_t numHigh=0;
      Int_t numLow=0;
      for(int samp=0;samp<numPoints;samp++) {
	if(mvArray[samp]>0) {
	  fHighArray[numHigh]=mvArray[samp];
	  numHigh++;
	}
	else {
	  fLowArray[numLow]=mvArray[samp];
	  numLow++;
	}
      }
      Double_t meanHigh=TMath::Mean(numHigh,fHighArray);
      Double_t meanLow=TMath::Mean(numLow,fLowArray);
      Double_t offset=(meanHigh+meanLow)/2;
      Double_t maxVal=meanHigh-offset;
      //       Double_t minVal=meanLow-offset;
      //       cout << maxVal << "\t" << minVal << endl;
      //       std::cout << offset << "\t" << maxVal << "\t" << minVal << std::endl;
      
      for(int i=0;i<numPoints;i++) {
	Double_t tempV=mvArray[i]-offset;	
	//	 if(tempV>maxVal*0.9)
	//	   volts[i]=1;
	//	 else if(tempV<minVal*0.9)
	//	   volts[i]=-1;
	//	 else {
	normClock[i]=tempV/maxVal;
	//	 }	
      }
      takeDerivative(numPoints,times,normClock,clockTimes,clockDeriv);
      grDeriv[surf] = new TGraph(numPoints-1,clockTimes,clockDeriv);
      //      grDeriv[surf]->Draw("al");

      grNorm[surf] = new TGraph(numPoints,times,normClock);
      //      grNorm[surf]->Draw("al");
      
      graphPadClock->cd(surf+1);
      gr[surf]->Draw("al");
      graphPadClockNorm->cd(surf+1);
      grNorm[surf]->Draw("al");
      graphPadClockDeriv->cd(surf+1);
      grDeriv[surf]->Draw("al");


      graphPadClockCorr->cd(surf+1);
      grCorr[surf]=FFTtools::getInterpolatedCorrelationGraph(grNorm[0],grNorm[surf],1./(2.6*64));
      grCorr[surf]->Draw("al");

      graphPadClockDerivCorr->cd(surf+1);
      grDerivCorr[surf]=FFTtools::getInterpolatedCorrelationGraph(grDeriv[0],grDeriv[surf],1./(2.6*64));
      grDerivCorr[surf]->Draw("al");


      //Now try and fins some phis
      {
	//Falling edge
	Double_t phi0=0;
	Double_t phiGuess=0;
	for(int i=0;i<numPoints-1;i++) {
	  if(normClock[i]>=0 &&
	     normClock[i+1]<0) {
	    phiGuess=Get_Interpolation_X(times[i],normClock[i],times[i+1],normClock[i+1],0);
	    //	     	     std::cout << surf << "\t" << 8 << "\t" << times[i] << "\t" << times[i+1] 
	    //	     		       << "\t" << volts[i] << "\t" << volts[i+1] << "\t" << phiGuess << std::endl;
	    if(i>3)
	      break;
	  }
	}
	
	if(surf==0) 
	  phi0=phiGuess;
	
	double phi=phiGuess;
	if((phi-phi0)>(clockPeriod/2))
	  phi-=clockPeriod;
	if((phi-phi0)<-1*(clockPeriod/2))
	  phi+=clockPeriod;
	
	//      cout << surf << "\t" << phi << "\n";
	phiFalling[surf]=phi-phi0;
      }
      {
	///Rising edge
	Double_t phi0=0;
	Double_t phiGuess=0;
	for(int i=0;i<numPoints-1;i++) {
	  if(normClock[i]<0 &&
	     normClock[i+1]>=0) {
	    phiGuess=Get_Interpolation_X(times[i],normClock[i],times[i+1],normClock[i+1],0);
	    //	     	     std::cout << surf << "\t" << 8 << "\t" << times[i] << "\t" << times[i+1] 
	    //	     		       << "\t" << volts[i] << "\t" << volts[i+1] << "\t" << phiGuess << std::endl;
	    if(i>3)
	      break;
	  }
	}
	
	if(surf==0) 
	  phi0=phiGuess;
	
	double phi=phiGuess;
	if((phi-phi0)>(clockPeriod/2))
	  phi-=clockPeriod;
	if((phi-phi0)<-1*(clockPeriod/2))
	  phi+=clockPeriod;
	
	//      cout << surf << "\t" << phi << "\n";
	phiRising[surf]=phi-phi0;
      }
      {
	//Peak of correlation
	Double_t peakTime,peakVal;
	Double_t phi0=0;
	Double_t phiGuess=0;
	Int_t peakBin=FFTtools::getPeakBin(grCorr[surf]);
	grCorr[surf]->GetPoint(peakBin,peakTime,peakVal);
	phiGuess=peakTime;      	
	peakCorr[surf]=phiGuess;
      }
      {
	//Peak of derivative correlation
	Double_t peakTime,peakVal;
	Double_t phi0=0;
	Double_t phiGuess=0;
	Int_t peakBin=FFTtools::getPeakBin(grDerivCorr[surf]);
	grDerivCorr[surf]->GetPoint(peakBin,peakTime,peakVal);
	phiGuess=peakTime;      
	peakDerivCorr[surf]=phiGuess;
      }

//       gr[surf]->Draw("al");
//       if(surf==0) {
// 	cout << "Float_t clockVals[260]={";
// 	Double_t *x=gr[surf]->GetY();
// 	Double_t N=gr[surf]->GetN();
// 	for(int i=0;i<260;i++) {
// 	  if(i<N) {
// 	    cout << x[i] << ",";
// 	  }
// 	  else cout << 0 << ",";
// 	}
// 	cout << "};\n";
//       }


    }
    for (int surf=0;surf<NUM_SURF;surf++) {
      std::cout << surf << "\t" << phiFalling[surf]
		<< "\t" << phiRising[surf] 
		<< "\t" << peakCorr[surf] 
		<< "\t" << peakDerivCorr[surf] << "\n";
    }

    canClock->Update();
    //    sprintf(pngName,"clocks%u.png",realEvent.eventNumber);
    //    canClock->Print(pngName);
    //       break;
    gSystem->Sleep(1000);
  
  }
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
