#include "AnitaConventions.h"
#include "UsefulAnitaEvent.h"
#include "AnitaGeomTool.h"
#include "RawAnitaHeader.h"
#include "TimedAnitaHeader.h"
#include "PrettyAnitaHk.h"
#include "FFTtools.h"
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

void GPSresolution(int firstRun, int lastRun);
void color_palette();

void GPSresolution(int firstRun, int lastRun) {

  FileStat_t staty;
  char adu5AName[FILENAME_MAX];
  char adu5BName[FILENAME_MAX];

  Adu5Pat *patA = 0;
  TChain *adu5AChain = new TChain("adu5PatTree");

  Adu5Pat *patB = 0;
  TChain *adu5BChain = new TChain("adu5bPatTree");

  adu5AChain->SetBranchAddress("pat",&patA); 
  adu5BChain->SetBranchAddress("pat",&patB);
  adu5BChain->BuildIndex("realTime"); 
  
  for (int run=firstRun;run<lastRun+1; run++)
    { 
      sprintf(adu5AName,"/unix/anita4/flight2016/root/run%d/gpsFile%d.root",run,run);
      if(gSystem->GetPathInfo(adu5AName,staty))
	{
	  continue;
	}
      adu5AChain->Add(adu5AName);

      //

      sprintf(adu5BName,"/unix/anita4/flight2016/root/run%d/gpsFile%d.root",run,run);
      if(gSystem->GetPathInfo(adu5BName,staty))
	{
	  continue;
	}
      adu5BChain->Add(adu5BName);
      
    }

  // Latitude
  TH1D *latHistoDiff = new TH1D(Form("latHistoDiff"), Form("latHisto, Runs %d - %d;adu5A lat -adu5B lat ; Entries", firstRun, lastRun), 10000, -1, 1);
  TH2D *latHisto = new TH2D(Form("latHisto"), Form("latHisto, Runs %d - %d;adu5A lat ;adu5B lat ", firstRun, lastRun), 10000, -90, -75, 10000, -90, -75);

  // Longitude
  TH1D *longHistoDiff = new TH1D(Form("longHistoDiff"), Form("longHisto, Runs %d - %d;adu5A long -adu5B long ; Entries", firstRun, lastRun), 10000, -30, 30);
  TH2D *longHisto = new TH2D(Form("longHisto"), Form("longHisto, Runs %d - %d;adu5A long ;adu5B long ", firstRun, lastRun), 10000, -360, 360, 10000, -360, 360);

  // Altitude
  TH1D *altHistoDiff = new TH1D(Form("altHistoDiff"), Form("altHisto, Runs %d - %d;adu5A alt (m)-adu5B alt (m); Entries", firstRun, lastRun), 10000, -1300, 1300);
  TH2D *altHisto = new TH2D(Form("altHisto"), Form("altHisto, Runs %d - %d;adu5A alt (m);adu5B alt (m)", firstRun, lastRun), 10000, 37000, 41000, 10000, 37000, 41000);

  // Heading
  TH1D *headHistoDiff = new TH1D(Form("headHistoDiff"), Form("headHisto, Runs %d - %d;adu5A head -adu5B head ; Entries", firstRun, lastRun), 10000, -200, 200);
  TH2D *headHisto = new TH2D(Form("headHisto"), Form("headHisto, Runs %d - %d;adu5A head ;adu5B head ", firstRun, lastRun), 10000, 0, 360, 10000, 0, 360);

  // Pitch
  TH1D *pitchHistoDiff = new TH1D(Form("pitchHistoDiff"), Form("pitchHisto, Runs %d - %d;adu5A pitch -adu5B pitch ; Entries", firstRun, lastRun), 10000, -20, 20);
  TH2D *pitchHisto = new TH2D(Form("pitchHisto"), Form("pitchHisto, Runs %d - %d;adu5A pitch ;adu5B pitch ", firstRun, lastRun), 10000, -20, 20, 10000, -20, 20);

  // Roll
  TH1D *rollHistoDiff = new TH1D(Form("rollHistoDiff"), Form("rollHisto, Runs %d - %d;adu5A roll -adu5B roll ; Entries", firstRun, lastRun), 10000, -20, 20);
  TH2D *rollHisto = new TH2D(Form("rollHisto"), Form("rollHisto, Runs %d - %d;adu5A roll ;adu5B roll ", firstRun, lastRun), 10000, -20, 20, 10000, -20, 20);
  
  //Test
  TH2D *longHistoTest = new TH2D(Form("longHistoTest"), Form("longHistoTest, Runs %d - %d;realTime ;adu5A long ", firstRun, lastRun), 10000, 1480000000, 1484000000, 10000, -200, 200);

  TH1D *mrmsDistro = new TH1D(Form("mrms Dist"), Form("mrms Dist, Runs %d - %d;adu5A mrms ; Entries", firstRun, lastRun), 10000, 0, 2000);
  TH2D *mrmsHistoTest = new TH2D(Form("mrms"), Form("mrms, Runs %d - %d;realTime ;adu5A mrms ", firstRun, lastRun), 10000, 1480000000, 1484000000, 10000, 0, 2000);

  TH1D *brmsDistro = new TH1D(Form("brms Dist"), Form("brms Dist, Runs %d - %d;adu5A brms ; Entries", firstRun, lastRun), 10000, 0, 2000);
  TH2D *brmsHistoTest = new TH2D(Form("brms"), Form("brms, Runs %d - %d;realTime ;adu5A brms", firstRun, lastRun), 10000, 1480000000, 1484000000, 10000, 0, 2000);

  int maxEntriesA = adu5AChain->GetEntries();
  int maxEntriesB = adu5BChain->GetEntries();

  cout << "There are " << maxEntriesA << " entries in adu5A Chain " << endl;
  cout << "There are " << maxEntriesB << " entries in adu5B Chain " << endl;

  for (int entry = 0; entry < maxEntriesA; entry++) 
   {
     
    adu5AChain->GetEntry(entry);
    adu5BChain->GetEntry(entry);

    // Things to skip (CUT ON)
    if( patA->attFlag != 0 || patB->attFlag != 0 || patA->altitude == 0 || patB->altitude == 0 /*|| cpatA->mrms !=0 || patB->mrms != 0 || patA->brms != 0 || patB->brms != 0 */) // if flag isn't zero (normal), skip it
      // also skip if altitude is 0
       {
	 continue;
       }

    // Need to remember to wrap some specific variables for differences between them:
    // longitude (trans from -180 to 180)
    // heading (trans from 360 to 0)

    // With CUT
    latHisto->Fill(patA->latitude, patB->latitude);
    latHistoDiff->Fill(patA->latitude - patB->latitude);
    
    longHisto->Fill(patA->longitude, patB->longitude);
    longHistoDiff->Fill( FFTtools::wrap( patA->longitude - patB->longitude , 180, 0) );
    
    altHisto->Fill(patA->altitude, patB->altitude);
    altHistoDiff->Fill(patA->altitude - patB->altitude);

    headHisto->Fill(patA->heading, patB->heading);
    headHistoDiff->Fill( FFTtools::wrap( patA->heading - patB->heading , 360 , 0 ));

    pitchHisto->Fill(patA->pitch, patB->pitch);
    pitchHistoDiff->Fill(patA->pitch - patB->pitch);

    rollHisto->Fill(patA->roll, patB->roll);
    rollHistoDiff->Fill(patA->roll - patB->roll);

    longHistoTest->Fill(patA->realTime, patA->longitude);

    // NO CUT
    mrmsHistoTest->Fill(patA->realTime, patA->mrms);
    brmsHistoTest->Fill(patA->realTime, patA->brms);

    mrmsDistro->Fill(patA->mrms);
    brmsDistro->Fill(patA->brms);
    // end of NO CUT

    // end of WITH CUTS
    
    //if(entry % 100 == 0)
    //{
     
     //cout << "longitudeA = " << patA->longitude << ", longitudeB = " << patB->longitude << ", diff = " << FFTtools::wrap( patA->longitude - patB->longitude , 180, 0) <<  endl;
     //cout << "headingA = " << patA->heading << ", headingB = " << patB->heading << ", diff = " << FFTtools::wrap( patA->heading - patB->heading , 360, 0) <<  endl;

     if(patA->brms > 1)
       {
	 cout << "brms =" << patA->brms << endl;
       }
     
       
     //}
   }
  
  // Lat
  TCanvas *cLat = new TCanvas("cLat","cLat",2550,1440);
  cLat->Divide(1,2);
  cLat->cd(1);
  gPad->SetLogy();
  latHistoDiff->Draw();

  cLat->cd(2);
  latHisto->Draw();
  cLat->SaveAs("lat_FurtherCut.png");
  cLat->SaveAs("lat_FurtherCut.pdf");

  // Long
  TCanvas *cLong = new TCanvas();
  cLong->Divide(1,2);
  cLong->cd(1);
  gPad->SetLogy();
  longHistoDiff->Draw();

  cLong->cd(2);
  longHisto->Draw();
  cLong->SaveAs("long_FurtherCut.png");
  cLong->SaveAs("long_FurtherCut.pdf");

  
  // Alt
  TCanvas *cAlt = new TCanvas();
  cAlt->Divide(1,2);
  cAlt->cd(1);
  gPad->SetLogy();
  altHistoDiff->Draw();

  cAlt->cd(2);
  altHisto->Draw();
  cAlt->SaveAs("alt_FurtherCut.png");
  cAlt->SaveAs("alt_FurtherCut.pdf");

  // Head
  TCanvas *cHead = new TCanvas();
  cHead->Divide(1,2);
  cHead->cd(1);
  gPad->SetLogy();
  headHistoDiff->Draw();

  cHead->cd(2);
  headHisto->Draw();
  cHead->SaveAs("head_FurtherCut.png");
  cHead->SaveAs("head_FurtherCut.pdf");

  TCanvas *cPitch = new TCanvas();
  cPitch->Divide(1,2);
  cPitch->cd(1);
  gPad->SetLogy();
  pitchHistoDiff->Draw();

  cPitch->cd(2);
  pitchHisto->Draw();
  cPitch->SaveAs("pitch_FurtherCut.png");
  cPitch->SaveAs("pitch_FurtherCut.pdf");

  TCanvas *cRoll = new TCanvas();
  cRoll->Divide(1,2);
  cRoll->cd(1);
  gPad->SetLogy();
  rollHistoDiff->Draw();

  cRoll->cd(2);
  rollHisto->Draw();
  cRoll->SaveAs("roll_FurtherCut.png");
  cRoll->SaveAs("roll_FurtherCut.pdf");
*/  
  // Just some testing
  /*
  TCanvas *testCanv = new TCanvas();
  testCanv->cd(1);
  longHistoTest->Draw();
  */

  TCanvas *testCanv2 = new TCanvas();
  testCanv2->Divide(2,2);
  testCanv2->cd(1);
  gPad->SetLogy();
  mrmsHistoTest->Draw();

  testCanv2->cd(2);
  gPad->SetLogy();
  brmsHistoTest->Draw();
  
  testCanv2->cd(3);
  gPad->SetLogy();
  mrmsDistro->Draw();

  testCanv2->cd(4);
  gPad->SetLogy();
  brmsDistro->Draw();
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
