#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "AnitaGeomTool.h"
#include "RawAnitaHeader.h"
#include "TimedAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "TurfRate.h"
#include "Adu5Pat.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH2.h"
#include "TH1.h"
#include "TColor.h"
#include "TImage.h"
#include "TMath.h"
#include "TStyle.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>
#include "TMarker.h"
#include "TLatex.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TPaveText.h"

void plotFindc3poNum(int firstRun, int lastRun);
void color_palette();

void plotFindc3poNum(int firstRun, int lastRun) {

  FileStat_t staty;
  char headName[FILENAME_MAX];

  RawAnitaHeader *header = 0;
  TChain *headChain = new TChain("headTree");

  for (int run=firstRun;run<lastRun+1; run++){
    sprintf(headName,"/unix/anita3/flight1415/root/run%d/headFile%d.root",run,run);

    if(gSystem->GetPathInfo(headName,staty)) {
      continue;
    }
    headChain->Add(headName);
    
  }
  
  headChain->SetBranchAddress("header", &header);

  UInt_t headEntries=headChain->GetEntries();

  cout << "There are " << headEntries << " entries in Head Chain " << endl;

  const int granularity = 60;

  cout << "The chosen granularity is : " << granularity << " seconds" << endl;

  headChain->GetEntry(0);
  double firstTS = header->realTime;
  headChain->GetEntry(headEntries-1);
  double lastTS = header->realTime;

  while(fmod(firstTS*1., granularity*1.)!=0) firstTS = firstTS-1.;
  while(fmod(lastTS*1., granularity*1.)!=0) lastTS = lastTS+1.; 

  time_t tt = firstTS;
  struct tm *tm = localtime(&tt);
  char firstDate[30];
  strftime(firstDate, sizeof(firstDate), "%Y-%b-%d, %H:%M", tm);
  cout << "Events starting from : " << firstDate << endl;
  tt = lastTS;
  tm = localtime(&tt);
  char lastDate[30];
  strftime(lastDate, sizeof(lastDate), "%Y-%b-%d, %H:%M", tm);
  cout << "And finishing on : " << lastDate << endl;

  Double_t times[1000000];
  int countTimes=0;
  times[0]=firstTS;
  while(times[countTimes]<lastTS){
    countTimes=countTimes+1;
    times[countTimes]=times[countTimes-1]+granularity*1.;
  }

  double phiSector[18];
  for (int j=0;j<17;j++) phiSector[j] = 0.5 + 1*j;

  TH1D *hc3poNum = new TH1D(Form("hc3poNum"), Form("c3poNum, Runs %d - %d;c3poNum (Hz);Entries", firstRun, lastRun), 10000, 200e6, 300e6);

  TH1D *hc3poNum_fine = new TH1D(Form("hc3poNum_fine"), Form("c3poNum, Runs %d - %d;c3poNum (Hz);Entries", firstRun, lastRun), 20000, 249.99e6, 250.01e6);

  TH2D *hc3poNum_fine_time = new TH2D(Form("hc3poNum_fine_time"), Form("c3poNum, Runs %d - %d;Time;c3poNum (Hz)", firstRun, lastRun), 10000, firstTS, lastTS, 2000, 249.99e6, 250.01e6);

  TH1D *hc3poNum2 = new TH1D(Form("hc3poNum2"), Form("c3poNum, Runs %d - %d;c3poNum (Hz);Entries", firstRun, lastRun), 10000, 200e6, 300e6);

  TH1D *hc3poNum_fine2 = new TH1D(Form("hc3poNum_fine2"), Form("c3poNum, Runs %d - %d;c3poNum (Hz);Entries", firstRun, lastRun), 20000, 249.99e6, 250.01e6);

  TH2D *hc3poNum_fine_time2 = new TH2D(Form("hc3poNum_fine_time2"), Form("c3poNum, Runs %d - %d;Time;c3poNum (Hz)", firstRun, lastRun), 10000, firstTS, lastTS, 2000, 249.99e6, 250.01e6);


  TH1D *hc3poNum_finer = new TH1D(Form("hc3poNum_finer"), Form("c3poNum, Runs %d - %d;c3poNum;Entries", firstRun, lastRun), 3000, 249.994e6 + 0.5, 249.997e6 +0.5);
  TH1D *hc3poNum_finer2 = new TH1D(Form("hc3poNum_finer2"), Form("c3poNum, Runs %d - %d;c3poNum;Entries", firstRun, lastRun), 3000, 249.994e6 + 0.5, 249.997e6 +0.5);


  // Look for starting point



  double c3poNumStart = 0;
  int count = 0;
  int corrected = 0;

  double c3poNumOld = 0;

  double max =100;
  Double_t tempc3poNum;
  for (unsigned int entry=0;entry<headEntries;entry++){
 
  if (entry%5000==0){ 
      cout << entry*100./headEntries << " %               \r" << flush;
      c3poNumOld=c3poNumStart;
      c3poNumStart = 0;
      count =0;
      for (unsigned int j=0;j<5000;j++){
	if (entry==headEntries) break;
	headChain->GetEntry(entry+j);
	if (header->c3poNum>249.994e6 && header->c3poNum<250.0e6){
	  c3poNumStart+=double(header->c3poNum);
	  count++;
	}
      }
      if (count==0) c3poNumStart=c3poNumOld;
      else c3poNumStart/=count;
      //      cout << c3poNumStart << endl;
    }


    headChain->GetEntry(entry);
    tempc3poNum = Double_t(header->c3poNum);

    hc3poNum->Fill(tempc3poNum);

    hc3poNum_fine->Fill(tempc3poNum);

    hc3poNum_fine_time->Fill(header->realTime, tempc3poNum);

    hc3poNum_finer->Fill(tempc3poNum);

    //    cout << fixed << tempc3poNum << " " << c3poNumStart << endl;
    
    if (header->run > 127) max =10;
    if (TMath::Abs(tempc3poNum-c3poNumStart)>max){
      tempc3poNum=c3poNumStart;
      corrected ++;
    }// else c3poNumStart = tempc3poNum;
    hc3poNum_fine_time2->Fill(header->realTime, tempc3poNum);
    hc3poNum2->Fill(tempc3poNum);
    hc3poNum_fine2->Fill(tempc3poNum);
    hc3poNum_finer2->Fill(tempc3poNum);


    if (tempc3poNum<200e6 ) cout << tempc3poNum << endl;
    else if (tempc3poNum>300e6 ) cout << tempc3poNum << endl;


  } 


  gStyle->SetOptStat(0);


  TCanvas *c2 = new TCanvas("c2", "", 1200, 500);

  hc3poNum_fine_time->Draw("colz");
  hc3poNum_fine_time->GetXaxis()->SetTimeDisplay(1);
  hc3poNum_fine_time->GetXaxis()->SetTimeFormat("%b %d, %H:%M");

  c2->Print(Form("c3poNum_fine_time.png", firstRun, lastRun));
  c2->Print(Form("c3poNum_fine_time.pdf", firstRun, lastRun));


  hc3poNum_fine_time2->Draw("colz");
  hc3poNum_fine_time2->GetXaxis()->SetTimeDisplay(1);
  hc3poNum_fine_time2->GetXaxis()->SetTimeFormat("%b %d, %H:%M");
  
  c2->Print(Form("c3poNum_fine_time2.png", firstRun, lastRun));
  c2->Print(Form("c3poNum_fine_time2.pdf", firstRun, lastRun));




  TCanvas *c1 = new TCanvas("c1");
  c1->SetLogy();

  hc3poNum->Draw();

  c1->Print(Form("c3poNum_%d_%d.png", firstRun, lastRun));
  c1->Print(Form("c3poNum_%d_%d.pdf", firstRun, lastRun));
  
  hc3poNum_fine->Draw();
  
  c1->Print(Form("c3poNum_fine_%d_%d.png", firstRun, lastRun));
  c1->Print(Form("c3poNum_fine_%d_%d.pdf", firstRun, lastRun));
  
  hc3poNum_finer->Draw();
  
  c1->Print(Form("c3poNum_finer_%d_%d.png", firstRun, lastRun));
  c1->Print(Form("c3poNum_finer_%d_%d.pdf", firstRun, lastRun));


  hc3poNum2->Draw();

  c1->Print(Form("c3poNum2_%d_%d.png", firstRun, lastRun));
  c1->Print(Form("c3poNum2_%d_%d.pdf", firstRun, lastRun));
  
  hc3poNum_fine2->Draw();
  
  c1->Print(Form("c3poNum2_fine_%d_%d.png", firstRun, lastRun));
  c1->Print(Form("c3poNum2_fine_%d_%d.pdf", firstRun, lastRun));
  
  hc3poNum_finer2->Draw();
  
  c1->Print(Form("c3poNum2_finer_%d_%d.png", firstRun, lastRun));
  c1->Print(Form("c3poNum2_finer_%d_%d.pdf", firstRun, lastRun));
  
  cout << fixed << "Mean : " << hc3poNum_finer->GetMean() << endl;
  cout << fixed << "Mode : " << hc3poNum_finer->GetXaxis()->GetBinCenter(hc3poNum_finer->GetMaximumBin()) << endl;

  cout << "Corrected " << corrected*100./headEntries << "% " << endl;


}

void color_palette(){
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;
  Double_t stops[NRGBs] = {0.00, 0.34, 0.61, 0.84, 1.00};
  Double_t red[NRGBs]   = {0.00, 0.00, 0.87, 1.00, 0.51};
  Double_t green[NRGBs] = {0.00, 0.81, 1.00, 0.20, 0.00};
  Double_t blue[NRGBs]  = {0.51, 1.00, 0.12, 0.00, 0.00};
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);

}

