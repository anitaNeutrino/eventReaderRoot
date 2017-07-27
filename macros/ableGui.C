// The Able GUI for ANITA

#include "TGNumberEntry.h"
#include "TLegendEntry.h"
#include "TGraphAntarctica.h"
#include "AntarcticaMapPlotter.h"
#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "AnitaGeomTool.h"
#include "RawAnitaHeader.h"
#include "TimedAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "FFTtools.h"
#include "TurfRate.h"
#include "Adu5Pat.h"
#include "G12Pos.h"
#include "TFile.h"
#include "TTree.h"
#include "TF2.h"
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
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGProgressBar.h>
#include <TApplication.h>
#include <TGLayout.h>
#include <TGWindow.h>
#include <TGStatusBar.h>
#include <TGLabel.h>
#include <RQ_OBJECT.h>

const char* mainDir = getenv("BUILD_TOOL"); // base list data
const char* chosenDir = getenv("ANITA_ROOT_DATA"); // gps data
// Other functions

void padRefresh(TPad *pad,int flag=0)
{
   if (!pad) return;
   pad->Modified();
   pad->Update();
   TList *tl = pad->GetListOfPrimitives();
   if (!tl) return;
   TListIter next(tl);
   TObject *to;
   while ((to=next())) {
     if (to->InheritsFrom(TPad::Class())) padRefresh((TPad*)to,1);}
   if (flag) return;
   gSystem->ProcessEvents();
}

class Able
{
  RQ_OBJECT("Able")
  private:
  TGMainFrame         *fMain;
  TRootEmbeddedCanvas *AbleCanvas;
  TGGroupFrame        *fGframe;
  TGNumberEntry *fNumber;
  TGLabel             *fLabel;
  
public:
  Able(const TGWindow *p,UInt_t w,UInt_t h);
  virtual ~Able();
  // Able functions here
  void Bases();
  void Bases2(const char *, Int_t);
  void BasesTreaty(const char *, Int_t);
  void Station();
  void Camp();
  void AirfieldCamp();
  void Refuge();
  void Depot();
  void FixedWing();
  void AWS();
  void BAS();
  void BaseOverview(Double_t, Int_t, const char*, const char*);
  void BaseOverviewA2(Double_t, Int_t, const char*, const char*);
  void FullBaseOverview();
  void PlotRunRange(Int_t, Int_t);
  void PlotRunRangeA(Int_t, Int_t);
  void PlotRunRangeB(Int_t, Int_t);
  void PlotRunRangeG(Int_t, Int_t);
  void FlightPath();
  void FlightPathA();
  void FlightPathB();
  void FlightPathG();
  void ANITA1();
  void ANITA2();
  void ANITA3();
  void ANITA4();
  void A2(Int_t, Int_t);
  void ClearCanvas();
  void ResetAble();
  void SaveImage();
};

// Some global vars
TLegend *leg = new TLegend(0.65,0.7,0.9,0.9);
Int_t run1 = 0;
Int_t run2 = 0;
Int_t adu5Plotted = 66;
const char * anitaNumber = "baseListA4"; //A4 default
//

Able::Able(const TGWindow *p,UInt_t w,UInt_t h)
{
  // Create a main frame
  fMain = new TGMainFrame(p,w,h);
  // Create canvas widget
  AbleCanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,1400,1400);
  TGraphAntarctica* gr = new TGraphAntarctica();
  gr->SetPoint(0,0,0);
  gr->Draw();
  fMain->AddFrame(AbleCanvas, new TGLayoutHints(kLHintsExpandX |
						kLHintsExpandY, 10,10,10,1));
  // Create a horizontal frame widget with buttons
  /*
  fNumber = new TGNumberEntry(p, 0, 9,999, TGNumberFormat::kNESInteger,
			      TGNumberFormat::kNEANonNegative,
			      TGNumberFormat::kNELLimitMinMax,
			      0, 99999);

  fNumber->Connect("ValueSet(Long_t)", "Able", this, "FlightPath()");
  (fNumber->GetNumberEntry())->Connect("ReturnPressed()", "Able", this,
				       "FlightPath()");
  hframe->AddFrame(fNumber, new TGLayoutHints(kLHintsCenterX,
                                            4,5,3,4));

  */

    // Frame 1
  TGHorizontalFrame *topframe = new TGHorizontalFrame(fMain,200,40);

  TGTextButton *drawa1 = new TGTextButton(topframe,"ANITA-1");
  drawa1->Connect("Clicked()","Able",this,"ANITA1()");
  topframe->AddFrame(drawa1, new TGLayoutHints(kLHintsCenterX,
                                            4,5,3,4));
  
  TGTextButton *drawa2 = new TGTextButton(topframe,"ANITA-2");
  drawa2->Connect("Clicked()","Able",this,"ANITA2()");
  topframe->AddFrame(drawa2, new TGLayoutHints(kLHintsCenterX,
                                            4,5,3,4));
  
    TGTextButton *drawa3 = new TGTextButton(topframe,"ANITA-3");
  drawa3->Connect("Clicked()","Able",this,"ANITA3()");
  topframe->AddFrame(drawa3, new TGLayoutHints(kLHintsCenterX,
                                            4,5,3,4));

  TGTextButton *drawa4 = new TGTextButton(topframe,"ANITA-4");
  drawa4->Connect("Clicked()","Able",this,"ANITA4()");
  topframe->AddFrame(drawa4, new TGLayoutHints(kLHintsCenterX,
					   4,5,3,4));

  
  
  // Frame 1
  TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,200,40);      
  // Define buttons
  TGTextButton *draw = new TGTextButton(hframe,"&Draw ALL ->");
  draw->Connect("Clicked()","Able",this,"Bases()");
  hframe->AddFrame(draw, new TGLayoutHints(kLHintsCenterX,
					   4,5,3,4));

  TGTextButton *draw1 = new TGTextButton(hframe,"&Stations");
  draw1->Connect("Clicked()","Able",this,"Station()");
  hframe->AddFrame(draw1, new TGLayoutHints(kLHintsCenterX,
                                            4,5,3,4));
   
  TGTextButton *draw2 = new TGTextButton(hframe,"&Camps");
  draw2->Connect("Clicked()","Able",this,"Camp()");
  hframe->AddFrame(draw2, new TGLayoutHints(kLHintsCenterX,
                                            6,5,3,4));
   
  TGTextButton *draw3 = new TGTextButton(hframe,"&Air Camps");
  draw3->Connect("Clicked()","Able",this,"AirfieldCamp()");
  hframe->AddFrame(draw3, new TGLayoutHints(kLHintsCenterX,
                                            5,5,3,4));

  TGTextButton *draw4 = new TGTextButton(hframe,"&Refuges");
  draw4->Connect("Clicked()","Able",this,"Refuge()");
  hframe->AddFrame(draw4, new TGLayoutHints(kLHintsCenterX,
                                            5,5,3,4));

  TGTextButton *draw5 = new TGTextButton(hframe,"&Depots");
  draw5->Connect("Clicked()","Able",this,"Depot()");
  hframe->AddFrame(draw5, new TGLayoutHints(kLHintsCenterX,
                                            5,5,3,4));

  TGTextButton *draw6 = new TGTextButton(hframe,"&Fixed Wing");
  draw6->Connect("Clicked()","Able",this,"FixedWing()");
  hframe->AddFrame(draw6, new TGLayoutHints(kLHintsCenterX,
                                            5,5,3,4));

  TGTextButton *draw7 = new TGTextButton(hframe,"&AWS");
  draw7->Connect("Clicked()","Able",this,"AWS()");
  hframe->AddFrame(draw7, new TGLayoutHints(kLHintsCenterX,
                                            5,5,3,4));

  TGTextButton *draw8 = new TGTextButton(hframe,"&BAS");
  draw8->Connect("Clicked()","Able",this,"BAS()");
  hframe->AddFrame(draw8, new TGLayoutHints(kLHintsCenterX,
                                            5,5,3,4));

  //if(strcmp(anitaNumber, "baseList") == 0)
    //{
    //draw7->SetState(kButtonDisabled);
    //draw7->SetState(kButtonEngaged);
    //draw7->SetState(kButtonDisabled);
    //draw7->SetState(kButtonEngaged);
    // }
  
  TGTextButton *draw9 = new TGTextButton(hframe,"&Base Overview");
  draw9->Connect("Clicked()","Able",this,"FullBaseOverview()");
  hframe->AddFrame(draw9, new TGLayoutHints(kLHintsCenterX,
                                            5,5,3,4));
  draw8->SetToolTipText("Plot all ANITA bases on the map from different flights");

  // Start frame 2
  // GPS stuff
  TGHorizontalFrame *hframe2 = new TGHorizontalFrame(fMain,200,40);
  
  TGTextButton *drawFlight = new TGTextButton(hframe2,"&ADU5A+B Path");
  drawFlight->Connect("Clicked()","Able",this,"FlightPath()");
  hframe2->AddFrame(drawFlight, new TGLayoutHints(kLHintsCenterX,
						 5,5,3,4));
  
  TGTextButton *drawFlightA = new TGTextButton(hframe2,"&ADU5A Path");
  drawFlightA->Connect("Clicked()","Able",this,"FlightPathA()");
  hframe2->AddFrame(drawFlightA, new TGLayoutHints(kLHintsCenterX,
						  5,5,3,4));

  
  TGTextButton *drawFlightB = new TGTextButton(hframe2,"&ADU5B Path");
  drawFlightB->Connect("Clicked()","Able",this,"FlightPathB()");
  hframe2->AddFrame(drawFlightB, new TGLayoutHints(kLHintsCenterX,
						 5,5,3,4));

  TGTextButton *drawFlightG = new TGTextButton(hframe2,"&G12 Path");
  drawFlightG->Connect("Clicked()","Able",this,"FlightPathG()");
  hframe2->AddFrame(drawFlightG, new TGLayoutHints(kLHintsCenterX,
						 5,5,3,4));
  
  // End frame 2

  // Start frame 3
  //QoL buttons below
  TGHorizontalFrame *hframe3 = new TGHorizontalFrame(fMain,200,40);

  // Exit button
  TGTextButton *exit = new TGTextButton(hframe3,"&Exit",
					"gApplication->Terminate(0)");
  hframe3->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,
					   5,5,3,4));

  TGTextButton *drawSave = new TGTextButton(hframe3,"&Save Image");
  drawSave->Connect("Clicked()","Able",this,"SaveImage()");
  hframe3->AddFrame(drawSave, new TGLayoutHints(kLHintsCenterX,
						5,5,3,4));
  drawSave->SetToolTipText("Save current canvas in png and pdf formats");

  TGTextButton *drawReset = new TGTextButton(hframe3,"&New Session");
  drawReset->Connect("Clicked()","Able",this,"ResetAble()");
  hframe3->AddFrame(drawReset, new TGLayoutHints(kLHintsCenterX,
						5,5,3,4));

  TGTextButton *drawClear = new TGTextButton(hframe3,"&Clear");
  drawClear->Connect("Clicked()","Able",this,"ClearCanvas()");
  hframe3->AddFrame(drawClear, new TGLayoutHints(kLHintsCenterX,
						5,5,3,4));
  drawClear->SetToolTipText("Clear everything off of the canvas");

  // End frame 3
  
  // Add frames to main frame
  fMain->AddFrame(topframe, new TGLayoutHints(kLHintsCenterX,
					    2,2,2,2));

  fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,
					    2,2,2,2));

  fMain->AddFrame(hframe2, new TGLayoutHints(kLHintsCenterX,
					    2,2,2,2));

  fMain->AddFrame(hframe3, new TGLayoutHints(kLHintsCenterX,
					    2,2,2,2));

  // Status bar should go here



  // Set a name to the main frame
  fMain->SetWindowName("Anita Base List Extension");

  // Map all subwindows of main frame
  fMain->MapSubwindows();

  // Initialize the layout algorithm
  fMain->Resize(fMain->GetDefaultSize());

  // Map main frame
  fMain->MapWindow();
}

// Able functions with no button mapping

void Able::Bases2(const char * facility, Int_t COL)
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  TFile *baseFile = new TFile( TString::Format("%s/components/anitaEventCorrelator/data/%s.root", mainDir,anitaNumber) );
  TTree * baseCampTree = (TTree*)baseFile->Get("baseCampTree");

  std::vector<Double_t> lats;
  std::vector<Double_t> longs;

  Double_t fullLat;
  Double_t fullLong;

  std::string *m_facType = new std::string;
  
  baseCampTree->SetBranchAddress("fullLat",&fullLat);
  baseCampTree->SetBranchAddress("fullLong",&fullLong);
  baseCampTree->SetBranchAddress("facType",&m_facType);
  
  UInt_t maxEntries = baseCampTree->GetEntries();
    
  for (unsigned int entry=0;entry<maxEntries;entry++)
    {
      baseCampTree->GetEntry(entry);
      
      if(*m_facType == facility)
	{
	  gr->SetPoint(entry, fullLong, fullLat);
	}
    }

  // Long, Lat


  TCanvas *fCanvas = AbleCanvas->GetCanvas();
   
  gr->SetMarkerStyle(3);
  gr->SetMarkerSize(2);
  gr->SetMarkerColor(COL);
  
  gr->Draw("p");

  
  TLegendEntry *le = leg->AddEntry(gr,facility,"p");
  le->SetTextColor(1);
  leg->Draw();
  
  fCanvas->cd();
  fCanvas->Update();
   
}

void Able::BasesTreaty(const char * facility, Int_t COL)
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  TFile *baseFile = new TFile( TString::Format("%s/components/anitaEventCorrelator/data/%s.root", mainDir,anitaNumber) );
  TTree * baseCampTree = (TTree*)baseFile->Get("antarcticTreatyTree");

  std::vector<Double_t> lats;
  std::vector<Double_t> longs;

  Double_t fullLat;
  Double_t fullLong;

  std::string *m_facType = new std::string;
  
  baseCampTree->SetBranchAddress("fullLat",&fullLat);
  baseCampTree->SetBranchAddress("fullLong",&fullLong);
  baseCampTree->SetBranchAddress("facType",&m_facType);
  
  UInt_t maxEntries = baseCampTree->GetEntries();
    
  for (unsigned int entry=0;entry<maxEntries;entry++)
    {
      baseCampTree->GetEntry(entry);
      
      if(*m_facType == facility)
	{
	  gr->SetPoint(entry, fullLong, fullLat);
	}
    }

  // Long, Lat


  TCanvas *fCanvas = AbleCanvas->GetCanvas();
   
  gr->SetMarkerStyle(3);
  gr->SetMarkerSize(2);
  gr->SetMarkerColor(COL);
  
  gr->Draw("p");
  
  fCanvas->cd();
  fCanvas->Update();
   
}

void Able::FixedWing()
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  TFile *baseFile = new TFile( TString::Format("%s/components/anitaEventCorrelator/data/%s.root", mainDir,anitaNumber) );
  TTree * fixedWingTree = (TTree*)baseFile->Get("fixedWingTree");

  std::vector<Double_t> lats;
  std::vector<Double_t> longs;

  Double_t fullLat;
  Double_t fullLong;
  
  fixedWingTree->SetBranchAddress("fullLat",&fullLat);
  fixedWingTree->SetBranchAddress("fullLong",&fullLong);
  
  UInt_t maxEntries = fixedWingTree->GetEntries();
    
  for (unsigned int entry=0;entry<maxEntries;entry++)
    {
      fixedWingTree->GetEntry(entry);
      
      gr->SetPoint(entry, fullLong, fullLat);
    }

  // Long, Lat

  TCanvas *fCanvas = AbleCanvas->GetCanvas();
   
  gr->SetMarkerSize(2);
  gr->SetMarkerColor(41);
  gr->SetMarkerStyle(5);
  
  gr->Draw("p");
  
  TLegendEntry *le = leg->AddEntry(gr,"Fixed Wing", "p");
  le->SetTextColor(1);
  leg->Draw();
  
  fCanvas->cd();
  fCanvas->Update();   
}

void Able::AWS()
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  TFile *baseFile = new TFile( TString::Format("%s/components/anitaEventCorrelator/data/%s.root", mainDir,anitaNumber) );
  TTree * awsTree = (TTree*)baseFile->Get("awsTree");

  std::vector<Double_t> lats;
  std::vector<Double_t> longs;

  Double_t fullLat;
  Double_t fullLong;
  
  awsTree->SetBranchAddress("fullLat",&fullLat);
  awsTree->SetBranchAddress("fullLong",&fullLong);
  
  UInt_t maxEntries = awsTree->GetEntries();
    
  for (unsigned int entry=0;entry<maxEntries;entry++)
    {
      awsTree->GetEntry(entry);
      
      gr->SetPoint(entry, fullLong, fullLat);
    }

  // Long, Lat

  TCanvas *fCanvas = AbleCanvas->GetCanvas();
   
  gr->SetMarkerSize(2);
  gr->SetMarkerColor(840);
  gr->SetMarkerStyle(5);
  
  gr->Draw("p");
  
  TLegendEntry *le = leg->AddEntry(gr,"AWS", "p");
  le->SetTextColor(1);
  leg->Draw();
  
  fCanvas->cd();
  fCanvas->Update();   
}

void Able::BAS()
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  TFile *baseFile = new TFile( TString::Format("%s/components/anitaEventCorrelator/data/%s.root", mainDir,anitaNumber) );
  TTree * basInstrumentsTree = (TTree*)baseFile->Get("basInstrumentsTree");

  std::vector<Double_t> lats;
  std::vector<Double_t> longs;

  Double_t fullLat;
  Double_t fullLong;
  
  basInstrumentsTree->SetBranchAddress("fullLat",&fullLat);
  basInstrumentsTree->SetBranchAddress("fullLong",&fullLong);
  
  UInt_t maxEntries = basInstrumentsTree->GetEntries();
    
  for (unsigned int entry=0;entry<maxEntries;entry++)
    {
      basInstrumentsTree->GetEntry(entry);
      
      gr->SetPoint(entry, fullLong, fullLat);
    }

  // Long, Lat

  TCanvas *fCanvas = AbleCanvas->GetCanvas();
   
  gr->SetMarkerSize(2);
  gr->SetMarkerColor(94);
  gr->SetMarkerStyle(5);
  
  gr->Draw("p");
  
  TLegendEntry *le = leg->AddEntry(gr,"BAS", "p");
  le->SetTextColor(1);
  leg->Draw();
  
  fCanvas->cd();
  fCanvas->Update();   
}


void Able::A2(Int_t index, Int_t COL)
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  TFile *baseFile = new TFile( TString::Format("%s/components/anitaEventCorrelator/data/%s.root", mainDir,anitaNumber) );
  TTree * A2Tree = (TTree*)baseFile->Get("A2Tree");

  std::vector<Double_t> lats;
  std::vector<Double_t> longs;

  Double_t fullLat;
  Double_t fullLong;
  Int_t indexFactor;

  A2Tree->SetBranchAddress("fullLat",&fullLat);
  A2Tree->SetBranchAddress("fullLong",&fullLong);
  A2Tree->SetBranchAddress("indexFactor",&indexFactor);
  
  UInt_t maxEntries = A2Tree->GetEntries();

  Int_t currentIndex = 0;
  
  for (unsigned int entry=0;entry<maxEntries;entry++)
    {
      A2Tree->GetEntry(entry);

      if(index == indexFactor)
	{
	  gr->SetPoint(entry, fullLong, fullLat);
	}
    }

  // Long, Lat

  TCanvas *fCanvas = AbleCanvas->GetCanvas();
   
  gr->SetMarkerSize(2);
  gr->SetMarkerColor(COL);
  gr->SetMarkerStyle(5);
  
  gr->Draw("p");

  //TLegend *leg = new TLegend(0.8,0.7,0.9,0.9);
  
  TLegendEntry *le = leg->AddEntry(gr,std::to_string(index).c_str(), "p");
  le->SetTextColor(1);
  leg->Draw();
  
  fCanvas->cd();
  fCanvas->Update();   
}


void Able::PlotRunRange(Int_t firstRun, Int_t lastRun)
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  gr->SetMarkerStyle(27);
  gr->SetMarkerSize(1.5);
  gr->SetMarkerColor(adu5Plotted);
  gr->SetLineColor(adu5Plotted);
  gr->SetLineWidth(5);
  FileStat_t staty;
  char adu5Name[FILENAME_MAX];
  
  TChain *adu5Chain = new TChain("adu5PatTree");

  for (int run=firstRun;run<lastRun+1; run++)
    {
      TString adu5Name = TString::Format("%s/run%d/timedGpsEvent%d.root",chosenDir,run,run);
      
      if(gSystem->GetPathInfo(adu5Name,staty))
	{
	  continue;
	}
      adu5Chain->Add(adu5Name);
    }
  
  Adu5Pat * pat = 0;
  adu5Chain->SetBranchAddress("pat",&pat);
  UInt_t maxEntries = adu5Chain->GetEntries();

  TCanvas *fCanvas = AbleCanvas->GetCanvas();

  Int_t quickness = 1000000;
  
  for (unsigned int entry=0;entry<maxEntries;entry+=quickness)
    {
      adu5Chain->GetEntry(entry);
      
      gr->SetPoint(entry/quickness, pat->longitude, pat->latitude);

      gr->Draw("L");
      padRefresh(fCanvas);
	  
    }
  
  gr->Draw("P");

  cout << "Finished plotting this course!" << endl;

  TLegendEntry *le = new TLegendEntry();
  
  le = leg->AddEntry(gr,TString::Format("ADU5I:Runs %d - %d",firstRun,lastRun),"p");	
  
  le->SetTextColor(1);
  leg->Draw();
  
  fCanvas->cd();
  fCanvas->Update();

  adu5Plotted+=10;  
}

void Able::PlotRunRangeA(Int_t firstRun, Int_t lastRun)
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  gr->SetMarkerStyle(27);
  gr->SetMarkerSize(1.5);
  gr->SetMarkerColor(adu5Plotted);
  gr->SetLineColor(adu5Plotted);
  gr->SetLineWidth(5);
  FileStat_t staty;
  char adu5Name[FILENAME_MAX];
  
  TChain *adu5Chain = new TChain("adu5PatTree");

  for (int run=firstRun;run<lastRun+1; run++)
    {
      TString adu5Name = TString::Format("%s/run%d/gpsFile%d.root",chosenDir,run,run);
      
      if(gSystem->GetPathInfo(adu5Name,staty))
	{
	  continue;
	}
      adu5Chain->Add(adu5Name);
    }
  
  Adu5Pat * pat = 0;
  adu5Chain->SetBranchAddress("pat",&pat);
  UInt_t maxEntries = adu5Chain->GetEntries();

  TCanvas *fCanvas = AbleCanvas->GetCanvas();

  Int_t quickness = 10000;
  
  for (unsigned int entry=0;entry<maxEntries;entry+=quickness)
    {
      adu5Chain->GetEntry(entry);
      
      gr->SetPoint(entry/quickness, pat->longitude, pat->latitude);

      gr->Draw("L");
      padRefresh(fCanvas);
	  
    }
  
  gr->Draw("P");

  cout << "Finished plotting this course!" << endl;

  TLegendEntry *le = new TLegendEntry();
  
  le = leg->AddEntry(gr,TString::Format("ADU5A:Runs %d - %d",firstRun,lastRun),"p");	
  
  le->SetTextColor(1);
  leg->Draw();
  
  fCanvas->cd();
  fCanvas->Update();

  adu5Plotted+=10;
   
}

void Able::PlotRunRangeB(Int_t firstRun, Int_t lastRun)
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  gr->SetMarkerStyle(27);
  gr->SetMarkerSize(1.5);
  gr->SetMarkerColor(adu5Plotted);
  gr->SetLineColor(adu5Plotted);
  gr->SetLineWidth(5);
  FileStat_t staty;
  char adu5Name[FILENAME_MAX];
  
  TChain *adu5Chain = new TChain("adu5bPatTree");

  for (int run=firstRun;run<lastRun+1; run++)
    {
      TString adu5Name = TString::Format("%s/run%d/gpsFile%d.root",chosenDir,run,run);
      
      if(gSystem->GetPathInfo(adu5Name,staty))
	{
	  continue;
	}
      adu5Chain->Add(adu5Name);
    }
  
  Adu5Pat * pat = 0;
  adu5Chain->SetBranchAddress("pat",&pat);
  UInt_t maxEntries = adu5Chain->GetEntries();

  TCanvas *fCanvas = AbleCanvas->GetCanvas();

  Int_t quickness = 10000;
  
  for (unsigned int entry=0;entry<maxEntries;entry+=quickness)
    {
      adu5Chain->GetEntry(entry);
      
      gr->SetPoint(entry/quickness, pat->longitude, pat->latitude);

      gr->Draw("L");
      padRefresh(fCanvas);
	  
    }
  
  gr->Draw("P");

  cout << "Finished plotting this course!" << endl;

  TLegendEntry *le = new TLegendEntry();
  
  le = leg->AddEntry(gr,TString::Format("ADU5B:Runs %d - %d",firstRun,lastRun),"p");	
  
  le->SetTextColor(1);
  leg->Draw();
  
  fCanvas->cd();
  fCanvas->Update();

  adu5Plotted+=10;
   
}

void Able::PlotRunRangeG(Int_t firstRun, Int_t lastRun)
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  gr->SetMarkerStyle(27);
  gr->SetMarkerSize(1.5);
  gr->SetMarkerColor(adu5Plotted);
  gr->SetLineColor(adu5Plotted);
  gr->SetLineWidth(5);
  FileStat_t staty;
  char adu5Name[FILENAME_MAX];
  
  TChain *adu5Chain = new TChain("g12PosTree");

  for (int run=firstRun;run<lastRun+1; run++)
    {
      TString adu5Name = TString::Format("%s/run%d/gpsFile%d.root",chosenDir,run,run);
      
      if(gSystem->GetPathInfo(adu5Name,staty))
	{
	  continue;
	}
      adu5Chain->Add(adu5Name);
    }
  
  G12Pos * pos = 0;
  adu5Chain->SetBranchAddress("pos",&pos);
  UInt_t maxEntries = adu5Chain->GetEntries();

  TCanvas *fCanvas = AbleCanvas->GetCanvas();

  Int_t quickness = 50000;
  
  for (unsigned int entry=0;entry<maxEntries;entry+=quickness)
    {
      adu5Chain->GetEntry(entry);
      
      gr->SetPoint(entry/quickness, pos->longitude, pos->latitude);

      gr->Draw("L");
      padRefresh(fCanvas);
	  
    }
  
  gr->Draw("P");

  cout << "Finished plotting this course!" << endl;

  TLegendEntry *le = new TLegendEntry();
  
  le = leg->AddEntry(gr,TString::Format("G12:Runs %d - %d",firstRun,lastRun),"p");	
  
  le->SetTextColor(1);
  leg->Draw();
  
  fCanvas->cd();
  fCanvas->Update();

  adu5Plotted+=10;
   
}

void Able::BaseOverview(Double_t markerSize, Int_t COL, const char* anitaNumber, const char* anitaName)
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  TFile *baseFile = new TFile( TString::Format("%s/components/anitaEventCorrelator/data/%s.root", mainDir,anitaNumber) );
  TTree * baseCampTree = (TTree*)baseFile->Get("baseCampTree");

  std::vector<Double_t> lats;
  std::vector<Double_t> longs;

  Double_t fullLat;
  Double_t fullLong;
  
  baseCampTree->SetBranchAddress("fullLat",&fullLat);
  baseCampTree->SetBranchAddress("fullLong",&fullLong);
  
  UInt_t maxEntries = baseCampTree->GetEntries();
    
  for (unsigned int entry=0;entry<maxEntries;entry++)
    {
      baseCampTree->GetEntry(entry);
      gr->SetPoint(entry, fullLong, fullLat);
       
    }

  // Long, Lat
   
  gr->SetMarkerStyle(3);
  gr->SetMarkerSize(markerSize);
  gr->SetMarkerColor(COL);
  
  gr->Draw("p");
  ///////
 
  TTree * antarcticTreatyTree = (TTree*)baseFile->Get("antarcticTreatyTree");

  std::vector<Double_t> lats2;
  std::vector<Double_t> longs2;

  Double_t fullLat2;
  Double_t fullLong2;
  
  antarcticTreatyTree->SetBranchAddress("fullLat",&fullLat2);
  antarcticTreatyTree->SetBranchAddress("fullLong",&fullLong2);
  
  UInt_t maxEntries2 = antarcticTreatyTree->GetEntries();

  for (unsigned int entry=0;entry<maxEntries2;entry++)
    {
      antarcticTreatyTree->GetEntry(entry);
      gr->SetPoint(entry+maxEntries, fullLong2, fullLat2);
       
    }

  //gr->SetMarkerColor(4);
  gr->Draw("p");

  TTree * fixedWingTree = (TTree*)baseFile->Get("fixedWingTree");

  std::vector<Double_t> lats3;
  std::vector<Double_t> longs3;

  Double_t fullLat3;
  Double_t fullLong3;
  
  fixedWingTree->SetBranchAddress("fullLat",&fullLat3);
  fixedWingTree->SetBranchAddress("fullLong",&fullLong3);
  
  UInt_t maxEntries3 = fixedWingTree->GetEntries();

  for (unsigned int entry=0;entry<maxEntries3;entry++)
    {
      fixedWingTree->GetEntry(entry);
      gr->SetPoint(entry+maxEntries+maxEntries2, fullLong3, fullLat3);
       
    }

  gr->Draw("p");

  TTree * awsTree = (TTree*)baseFile->Get("awsTree");

  std::vector<Double_t> lats4;
  std::vector<Double_t> longs4;

  Double_t fullLat4;
  Double_t fullLong4;
  
  awsTree->SetBranchAddress("fullLat",&fullLat4);
  awsTree->SetBranchAddress("fullLong",&fullLong4);
  
  UInt_t maxEntries4 = awsTree->GetEntries();

  for (unsigned int entry=0;entry<maxEntries4;entry++)
    {
      awsTree->GetEntry(entry);
      gr->SetPoint(entry+maxEntries+maxEntries2+maxEntries3, fullLong4, fullLat4);
       
    }

  TCanvas *fCanvas = AbleCanvas->GetCanvas();
  
  gr->Draw("p");

  TTree *basInstrumentsTree = (TTree*)baseFile->Get("basInstrumentsTree");

  std::vector<Double_t> lats5;
  std::vector<Double_t> longs5;

  Double_t fullLat5;
  Double_t fullLong5;
  
  basInstrumentsTree->SetBranchAddress("fullLat",&fullLat5);
  basInstrumentsTree->SetBranchAddress("fullLong",&fullLong5);
  
  UInt_t maxEntries5 = basInstrumentsTree->GetEntries();

  for (unsigned int entry=0;entry<maxEntries5;entry++)
    {
      basInstrumentsTree->GetEntry(entry);
      gr->SetPoint(entry+maxEntries+maxEntries2+maxEntries3+maxEntries4, fullLong5, fullLat5);
       
    }

  gr->Draw("p");

  
  TLegendEntry *le = leg->AddEntry(gr,TString::Format("%s",anitaName),"p");
  leg->Draw();

  

  fCanvas->cd();
  fCanvas->Update();


  return;
  
}

void Able::BaseOverviewA2(Double_t markerSize, Int_t COL, const char* anitaNumber, const char* anitaName)
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  TFile *baseFile = new TFile( TString::Format("%s/components/anitaEventCorrelator/data/%s.root", mainDir,anitaNumber) );
  TTree * A2Tree = (TTree*)baseFile->Get("A2Tree");

  std::vector<Double_t> lats;
  std::vector<Double_t> longs;

  Double_t fullLat;
  Double_t fullLong;
  
  A2Tree->SetBranchAddress("fullLat",&fullLat);
  A2Tree->SetBranchAddress("fullLong",&fullLong);
  
  UInt_t maxEntries = A2Tree->GetEntries();
    
  for (unsigned int entry=0;entry<maxEntries;entry++)
    {
      A2Tree->GetEntry(entry);
      gr->SetPoint(entry, fullLong, fullLat);
       
    }

  // Long, Lat

  TCanvas *fCanvas = AbleCanvas->GetCanvas();
  
  gr->SetMarkerStyle(3);
  gr->SetMarkerSize(markerSize);
  gr->SetMarkerColor(COL);
  
  gr->Draw("p");

  TLegendEntry *le = leg->AddEntry(gr,TString::Format("%s",anitaName),"p");
  leg->Draw();

  fCanvas->cd();
  fCanvas->Update();

}

// Some categories aren't filled because they have no category: i.e. X or -999
void Able::FullBaseOverview()
{  
  Able::BaseOverview(3,4,"baseListA4","ANITA-4");
  Able::BaseOverview(2,3,"baseListA3","ANITA-3");
  Able::BaseOverviewA2(1.2,2,"baseListA2","ANITA-2");
}


// v Able functions with button mapping

void Able::Bases()
{
  if ( strcmp(anitaNumber, "baseListA4") == 0)
    {
      Able::FixedWing();
      Able::AWS();
      Able::BAS();
      Able::AirfieldCamp();
      Able::Depot();
      Able::Refuge();
      Able::Camp();
      Able::Station();
    }
  
  if ( strcmp(anitaNumber, "baseListA3") == 0 )
    {
      Able::FixedWing();
      Able::AWS();
      Able::BAS();
      Able::Refuge();
      Able::Camp();
      Able::Station();
    }

  if ( strcmp(anitaNumber, "baseListA2") == 0 )
    {
      Able::A2(0,1);
      Able::A2(1,2);
      Able::A2(2,3);
      Able::A2(3,4);
    }
  
}


void Able::Station()
{ 
  Able::Bases2("Station",2);
  Able::BasesTreaty("Station",2);
}

void Able::Camp()
{ 
  Able::Bases2("Camp",3);
  Able::BasesTreaty("Camp",3);
}

void Able::AirfieldCamp()
{ 
  Able::Bases2("Airfield Camp",4);
}

void Able::Refuge()
{ 
  Able::Bases2("Refuge",51);
  Able::BasesTreaty("Refuge",51);
}

void Able::Depot()
{ 
  Able::Bases2("Depot",1);
}

void Able::FlightPath()
{
  run1 = 0;
  run2 = 0;

  cout << "Enter run 1: " << endl;
  cin >> run1;
  cout << "Enter run 2: " << endl;
  cin >> run2;
  
  Able::PlotRunRange(run1,run2);
}

void Able::FlightPathA()
{
  run1 = 0;
  run2 = 0;

  cout << "Enter run 1: " << endl;
  cin >> run1;
  cout << "Enter run 2: " << endl;
  cin >> run2;
  
  Able::PlotRunRangeA(run1,run2);
}

void Able::FlightPathB()
{
  run1 = 0;
  run2 = 0;

  cout << "Enter run 1: " << endl;
  cin >> run1;
  cout << "Enter run 2: " << endl;
  cin >> run2;
  
  Able::PlotRunRangeB(run1,run2);
}

void Able::FlightPathG()
{
  run1 = 0;
  run2 = 0;

  cout << "Enter run 1: " << endl;
  cin >> run1;
  cout << "Enter run 2: " << endl;
  cin >> run2;
  
  Able::PlotRunRangeG(run1,run2);
}


// ANITA-options

void Able::ANITA4()
{
  anitaNumber = "baseListA4";
}

void Able::ANITA3()
{
  anitaNumber = "baseListA3";
}

void Able::ANITA2()
{
  anitaNumber = "baseListA2"; 
}

void Able::ANITA1()
{
  //anitaNumber = "baseListA2";  
}


// Custom functions not to do with the flight

void Able::ClearCanvas()
{

  TCanvas *fCanvas = AbleCanvas->GetCanvas();
  fCanvas->Clear();
  leg->Clear();
  TGraphAntarctica* gr = new TGraphAntarctica();
  gr->SetPoint(0,0,0);
  gr->Draw();
  fCanvas->Update();
}

void Able::SaveImage()
{
  TCanvas *fCanvas = AbleCanvas->GetCanvas();

  if(run1 == 0 && run2 == 0)
    {
      fCanvas->SaveAs(TString::Format("ableCanvasNoPath_%s.png",anitaNumber));
		      fCanvas->SaveAs(TString::Format("ableCanvasNoPath_%s.pdf",anitaNumber));
    }
  
  else
    {
      fCanvas->SaveAs(TString::Format("ableCanvas_%d_%d_%s.png",run1,run2,anitaNumber));
      fCanvas->SaveAs(TString::Format("ableCanvas_%d_%d_%s.pdf",run1,run2,anitaNumber));
    }
      
}

void Able::ResetAble()
{
  new Able(gClient->GetRoot(),200,200);
}

Able::~Able() {
  // Clean up used widgets: frames, buttons, layout hints
  fMain->Cleanup();
  delete fMain;
}
void ableGui() {
  // Popup the GUI...
  new Able(gClient->GetRoot(),200,200);
}

