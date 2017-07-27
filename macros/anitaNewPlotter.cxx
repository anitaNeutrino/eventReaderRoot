// Trivial program to plot lat and long onto continent either manually, from GPS data or from a base list

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

using namespace std;

Int_t resSize = 2048;
TLegend *globLeg = new TLegend(0.75,0.7,0.85,0.9);

const char* mainDir = getenv("BUILD_TOOL");

void anitaNewPlotter()
{

  std::cout << "Simple functions to map location" << std::endl;
  
}

//// Scroll down to the comment block for some simple functions to use

void BaseOverview(Double_t markerSize, Int_t COL, const char* anitaNumber, const char* anitaName)
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  TFile *baseFile = new TFile(TString::Format("%s/components/anitaEventCorrelator/data/baseList%s.root", mainDir, anitaNumber));
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
      gr->SetPoint(entry+maxEntries2, fullLong3, fullLat3);
       
    }

  //gr->Draw("p");

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
      gr->SetPoint(entry+maxEntries3, fullLong4, fullLat4);
       
    }

  gr->Draw("p");
  
  TLegendEntry *le = globLeg->AddEntry(gr,TString::Format("%s",anitaName),"p");
  globLeg->Draw();

  return;
  
}

void A2(Double_t markerSize, Int_t COL, const char* anitaNumber, const char* anitaName)
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  TFile *baseFile = new TFile( TString::Format("%s/%s.root",getenv("BUILD_TOOL"), anitaNumber));
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
   
  gr->SetMarkerStyle(3);
  gr->SetMarkerSize(markerSize);
  gr->SetMarkerColor(COL);
  
  gr->Draw("p");

  TLegendEntry *le = globLeg->AddEntry(gr,TString::Format("%s",anitaName),"p");
  globLeg->Draw();


}

///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// 
/////
/////
/////
/////
/////
/////
///// 
/////
///// 
/////
///// 
///// Use the below functions as a test
/////
/////
///// root anitaNewPlotter.cxx++
///// functionName()
/////
/////
/////
/////
/////
/////
///// 
/////
///// 
///// ///// ///// ///// ///// ///// ///// ///// ///// ///// ///// /////

// Using TGraphAntarctic: just plot a point
void plotAPoint()
{
  
  TGraphAntarctica* gr = new TGraphAntarctica();

  Double_t fullLong = 90; // longitude
  Double_t fullLat = -80; // latitude
  
  gr->SetPoint(0, fullLong, fullLat);

  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(2);
  gr->SetMarkerColor(6);
  gr->Draw("p");
  
}

// How to use a branch from the base list:
// Below the ANITA-3 camps and stations are plotted:
void plotBaseList()
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  TFile *baseFile = new TFile( TString::Format("%s/components/anitaEventCorrelator/data/baseListA3.root", mainDir) );
  TTree * baseCampTree = (TTree*)baseFile->Get("baseCampTree");

  std::vector<Double_t> lats;
  std::vector<Double_t> longs;

  Double_t fullLat;
  Double_t fullLong;
  
  baseCampTree->SetBranchAddress("fullLat",&fullLat);
  baseCampTree->SetBranchAddress("fullLong",&fullLong);
  
  UInt_t maxEntries = baseCampTree->GetEntries();
  UInt_t stationEntries = 0;
  
  for (unsigned int entry=0;entry<maxEntries;entry++)
    {
      baseCampTree->GetEntry(entry);

      gr->SetPoint(entry, fullLong, fullLat);
	
    }

  // Long, Lat

  TCanvas *cvs = new TCanvas("l","l",1600,1600);
  
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(2);
  gr->SetMarkerColor(6);
  
  gr->Draw("p");

  TLegend *leg = new TLegend(0.75,0.7,0.85,0.9);  

  TLegendEntry *le = leg->AddEntry(gr,"Bases","p");
  le->SetTextColor(kBlue);
  leg->Draw();
  
  cvs->SaveAs("blank.png");

  return;
  
}

// Plot the base overview (see above functions)
void FullBaseOverview()
{
  
  BaseOverview(3,4,"baseList","ANITA-4");
  BaseOverview(2,3,"baseListA3","ANITA-3");
  A2(1.2,2,"baseListA2","ANITA-2");
}

// An example of plotting multiple points and saving Antarctica canvas
void quickPlot()
{

  Double_t fullLatTT = 0;
  Double_t fullLongTT = 0;
  
  TGraphAntarctica* gr = new TGraphAntarctica();
  
  for (int entry=0;entry<5;entry++)
    {
      fullLongTT = -150 + entry*10;
      fullLatTT = -  77 -  entry;

      cout << "entry = " << entry << endl;
      cout << "long = " << fullLongTT << endl;
      cout << "lat = " << fullLatTT << endl;

      gr->SetPoint(entry, fullLongTT, fullLatTT);
	
    }

  // Long, Lat

  TCanvas *cvs = new TCanvas("l","l",1600,1600);
  
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(2);
  gr->SetMarkerColor(6);
  
  gr->Draw("p");
  
  cvs->SaveAs("blank.png");

  return;
  
}

// Konstantin's original Russian bases
void russian()
{

  TGraphAntarctica* gr = new TGraphAntarctica();
  
  Double_t fullLat[100]={};
  Double_t fullLong[100]={};

  Int_t latDeg[100] = {66,78,70,62,69,67,69,74,69};
  Int_t latMin[100] = {33,28,46,12,24,40,44,43,30};

  Int_t longDeg[100] = {93,106,11,58,76,45,70,137,159};
  Int_t longMin[100] = {01,48,50,58,24,51,43,07,23};
  std::string longCar[100] = {"E", "E", "E", "W", "E", "E", "E", "W", "W"};

    for (int entry=0;entry<10;entry++)
      {
    
	fullLat[entry] = -latDeg[entry] - latMin[entry]/60;
      
	if(longCar[entry] == "E")
	  {
	    fullLong[entry] = longDeg[entry] + (longMin[entry])/60;
	  }

	else if(longCar[entry] == "W")
	  {
	    fullLong[entry] = -(longDeg[entry] + (longMin[entry])/60);
	  }

	gr->SetPoint(entry, fullLong[entry], fullLat[entry]);

	
      }
  

  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(2);
  gr->SetMarkerColor(6);	
  gr->Draw();

  
}

//Konstantin's original Russian bases after modification
void russian2()
{

  TLegend *LegLeg = new TLegend(0.75,0.7,0.85,0.9);
  
  TGraphAntarctica* gr = new TGraphAntarctica();
  
  Double_t fullLat[100]={};
  Double_t fullLong[100]={};

  Int_t latDeg[100] = {66,78,70,62,69};
  Int_t latMin[100] = {33,28,46,12,24};

  Int_t longDeg[100] = {93,106,11,58,76};
  Int_t longMin[100] = {01,48,50,58,24};
  
  std::string longCar[100] = {"E", "E", "E", "W", "E"};

    for (int entry=0;entry<10;entry++)
      {
    
	fullLat[entry] = -latDeg[entry] - latMin[entry]/60;
      
	if(longCar[entry] == "E")
	  {
	    fullLong[entry] = longDeg[entry] + (longMin[entry])/60;
	  }

	else if(longCar[entry] == "W")
	  {
	    fullLong[entry] = -(longDeg[entry] + (longMin[entry])/60);
	  }

	gr->SetPoint(entry, fullLong[entry], fullLat[entry]);

	
      }

  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(2);
  gr->SetMarkerColor(6);	
  gr->Draw();

  TLegendEntry *lelele = LegLeg->AddEntry(gr,"Stations","p");
  
  
  Double_t fullLatAWS[100]={};
  Double_t fullLongAWS[100]={};

  Int_t latDegAWS[100] = {67,69,74,69};
  Int_t latMinAWS[100] = {40,44,43,30};

   Int_t longDegAWS[100] = {45,70,137,159};
  Int_t longMinAWS[100] = {51,43,07,23};
  
  std::string longCarAWS[100] = {"E", "E", "W", "E"};

  TGraphAntarctica* gr2 = new TGraphAntarctica();

      for (int entry=0;entry<10;entry++)
      {
    
	fullLatAWS[entry] = -latDegAWS[entry] - latMinAWS[entry]/60;
      
	if(longCarAWS[entry] == "E")
	  {
	    fullLongAWS[entry] = longDegAWS[entry] + (longMinAWS[entry])/60;
	  }

	else if(longCarAWS[entry] == "W")
	  {
	    fullLongAWS[entry] = -(longDegAWS[entry] + (longMinAWS[entry])/60);
	  }

	gr2->SetPoint(entry, fullLongAWS[entry], fullLatAWS[entry]);

	
      }

      gr2->SetMarkerStyle(20);
      gr2->SetMarkerSize(2);
      gr2->SetMarkerColor(3);	
      gr2->Draw();

      TLegendEntry *le = LegLeg->AddEntry(gr2,"AWS","p");
      LegLeg->Draw();

  
}

