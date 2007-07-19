//#include "AnitaConventions.h"

void plotClockChannels()
{
  plotClockChannels(1028,1000,1);
}
  

void plotClockChannels(int run, int startEntry, int numEntries) {
  gSystem->Load("libAnitaEvent.so");

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
  eventChain->AddFriend(headTree);
  eventChain->AddFriend(prettyHkTree);


  TCanvas *canClocks = (TCanvas*) gROOT->FindObject("canClocks");
  if(!canClocks) {
    canClocks = new TCanvas("canClocks","canClocks",600,900);
  }
  TGraph *gr[9]={0};
  TF1 *sqaurey = new TF1("sqaurey",funcSquareWave,5,90,7);
  sqaurey->SetNpx(1000);
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

  for(int entry=startEntry;entry<startEntry+numEntries;entry++) {
    
    //Stupidly most do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    prettyHkTree->GetEntry(entry);
    
    
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlusClockZero,hk);
    cout << realEvent.eventNumber << " " << header->eventNumber << endl;
    //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
    
    canClocks->Clear();
    //    canClocks->Clear();
    for(int chan=0;chan<9;chan++) {
      if(gr[chan]) delete gr[chan];
    }
    canClocks->Divide(1,9,0);

    gStyle->SetStatX(1);
    gStyle->SetStatY(1);
    gStyle->SetStatH(0.3);
    gStyle->SetLabelSize(0.1,"xy");

    for(int surf=0;surf<9;surf++) {
       canClocks->cd(surf+1);
       gPad->SetRightMargin(0.005);
       gPad->SetGridx();
       TH1F *framey = gPad->DrawFrame(-10,-300,110,+300);
       framey->GetXaxis()->SetNdivisions();
  //       sqaurey->SetParameters(25,200,-200,15,15,0.33,0.33);
       Int_t ci=UsefulAnitaEvent::getChanIndex(surf,8);
       gr[surf] = realEvent.getGraph(ci);
       gr[surf]->Draw("l");
       //       if(surf==5)
       
       
       //Take a guess at phi
       Float_t phiGuess=0;
       for(int i=0;i<realEvent.fNumPoints[ci]-1;i++) {
	  if(realEvent.fVolts[ci][i]>=0 &&
	     realEvent.fVolts[ci][i+1]<0) {
	     phiGuess=realEvent.fTimes[ci][i];
	     if(i>3)
		break;
	  }
       }



       //       cout << phiGuess << endl;
       sqaurey->SetParameters(phiGuess,200,-200,12.881,16.428,0.33,0.33);
       //       sqaurey->SetParameters(phiGuess,200,-200,12.8,16.4,0.33,0.33);
       gr[surf]->Fit(sqaurey,"QR");
    }
    canClocks->Update();
    gSystem->Sleep(1000);
  }
  //  canClocks->cd(10);
  //  sqaurey->Draw();
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


   //   cout << "Test: " << t << "\t" << period << " " << dtLow << " " << dtHigh << " " << sllh << " " << slhl << endl;
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
