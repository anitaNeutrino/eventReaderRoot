//#include "AnitaConventions.h"

void plotZC()
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
  plotZC(1028,0,1000);
}
  

void plotZC(int run, int startEntry, int numEntries) {
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


//   TCanvas *canClock = (TCanvas*) gROOT->FindObject("canClock");
//   if(!canClock) {
//     canClock = new TCanvas("canClock","canClock",800,800);
//   }
  char textLabel[180];
  char pngName[180];
  TGraph *gr[9]={0};

  TH1F *histZc = new TH1F("histZc","histZc",1000,-100,100);
  TH1F *histCount = new TH1F("histCount","histCount",10,0,10);
  TH2F *hist2d = new TH2F("hist2d","hist2d",100,0,100,100,0,100);
  TH1F *histMvMax = new TH1F("histMvMax","histMvMax",1000,0,500);
  TH1F *histMvMin = new TH1F("histMvMin","histMvMin",1000,-500,0);
  TH2F *histMv2d = new TH2F("histMv2d","histMv2d",500,-500,0,500,0,500);


  for(int entry=startEntry;entry<startEntry+numEntries;entry++) {
    
    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    prettyHkTree->GetEntry(entry);
       
       
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlus,hk);
    
    //Just SURF 0 for now.
    Double_t mean=TMath::Mean(realEvent.fNumPoints[8],realEvent.fVolts[8]);

    Int_t countZC[9]={0};

    Double_t zcs[9][10]={0};


    for(int surf=0;surf<9;surf++) {
      int ci=realEvent.getChanIndex(surf,8);
      countZC[surf]=0;
      for(int i=0;i<realEvent.fNumPoints[ci]-1;i++) {
	Double_t mv1=realEvent.fVolts[ci][i]-mean;
	Double_t mv2=realEvent.fVolts[ci][i+1]-mean;
	if(mv1<0 && mv2>0) {
	  //ZC
	  Double_t t1=realEvent.fTimes[ci][i];
	  Double_t t2=realEvent.fTimes[ci][i+1];
	  
	  Double_t zct=t1 + (t2-t1)*(0-mv1)/(mv2-mv1);
	  //	  cerr << surf << "\t" << countZC[surf] << endl;
	  zcs[surf][countZC[surf]]=zct;	
	  countZC[surf]++;

	  histMvMin->Fill(mv1);
	  histMvMax->Fill(mv2);
	  histMv2d->Fill(mv1,mv2);

	}
      } 
    }
    histCount->Fill(countZC[1]);

    Int_t z1=1;
    Int_t z0=1;
    Double_t zcDiff=zcs[1][z1]-zcs[0][z0];
    if(zcDiff>20) {
      z0++;
      zcDiff=zcs[1][z1]-zcs[0][z0];
    }
    if(zcDiff<-20) {
      z1++;
      zcDiff=zcs[1][z1]-zcs[0][z0];
    }

    histZc->Fill(zcs[1][z1]-zcs[0][z0]);
    hist2d->Fill(zcs[1][z1],zcs[0][z0]);
    
  }
  
  TCanvas *can = new TCanvas("can","can");
  can->Divide(1,2);
  can->cd(1);
  histZc->Draw();
  can->cd(2);
  hist2d->Draw();
  
  TCanvas *can2 = new TCanvas("can2","can2");
  can2->Divide(1,3);
  can2->cd(1);
  histMvMin->Draw();
  can2->cd(2);
  histMvMax->Draw();
  can2->cd(3);
  histMv2d->Draw();


}
