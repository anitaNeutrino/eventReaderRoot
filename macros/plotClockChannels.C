//#include "AnitaConventions.h"
const float clockPeriod=29.970;

void plotClockChannels()
{
  plotClockChannels(1028,0,1);
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



  TF1 *sqaurey = new TF1("sqaurey",funcSquareWave,5,90,5);
  //  sqaurey->SetNpx(1000);
  sqaurey->SetParameters(25,1,-1,0.45,0.33);
  sqaurey->SetParLimits(0,0,35);
  sqaurey->SetParLimits(1,1,1);
  sqaurey->SetParLimits(2,-1,-1);
  sqaurey->SetParLimits(3,0.4,0.5);
  sqaurey->SetParLimits(4,0.33,0.33);
  
  TH1F histHigh("histHigh","histHigh",1000,0,1000);
  TH1F histLow("histLow","histLow",1000,-1000,0);
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
    canClocks->Divide(1,9);

    //    gStyle->SetStatX(1);
    //    gStyle->SetStatY(1);
    gStyle->SetStatH(0.3);
    gStyle->SetLabelSize(0.1,"xy");

    for(int surf=0;surf<9;surf++) {
       canClocks->cd(surf+1);
       //       gPad->SetRightMargin(0.005);
       gPad->SetGridx();
       TH1F *framey = gPad->DrawFrame(-10,-2,110,+2);
       framey->GetXaxis()->SetNdivisions();
  //       sqaurey->SetParameters(25,200,-200,15,15,0.33,0.33);
       Int_t ci=UsefulAnitaEvent::getChanIndex(surf,8);
       //       gr[surf] = realEvent.getGraph(ci);
       //       gr[surf]->Draw("l");
       //       if(surf==5)

       
       //       Double_t maxVal=TMath::MaxElement(realEvent.fNumPoints[ci],realEvent.fVolts[ci]);
       //       Double_t minVal=TMath::MinElement(realEvent.fNumPoints[ci],realEvent.fVolts[ci]);
       histLow.Reset();
       histHigh.Reset();
       for(int i=0;i<realEvent.fNumPoints[ci];i++) {
	 if(realEvent.fVolts[ci][i]>0) {
	   histHigh.Fill(realEvent.fVolts[ci][i]);
	   }
	 else {
	   histLow.Fill(realEvent.fVolts[ci][i]);
	 }
       }
       //cout << maxVal << "\t" << minVal << endl;
       //       cout << histHigh.GetMean() << "\t" << histLow.GetMean() << endl;
       Double_t offset=(histHigh.GetMean()+histLow.GetMean())/2;
       Double_t maxVal=histHigh.GetMean()-offset;
       Double_t minVal=histLow.GetMean()-offset;
       //       cout << maxVal << "\t" << minVal << endl;
       
       Double_t times[260];
       Double_t volts[260];
       for(int i=0;i<realEvent.fNumPoints[ci];i++) {
	 times[i]=realEvent.fTimes[ci][i];
	 Double_t tempV=realEvent.fVolts[ci][i]-offset;
	 if(tempV>maxVal*0.5)
	   volts[i]=1;
	 else if(tempV<minVal*0.5)
	   volts[i]=-1;
	 else {
	   volts[i]=tempV/maxVal;
	 }
       }
       gr[surf]= new TGraph(realEvent.fNumPoints[ci],times,volts);
       gr[surf]->Draw("lp");

       
       
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
       sqaurey->SetParameters(phiGuess,1,-1,0.5,0.33);
       //       sqaurey->SetParameters(phiGuess,200,-200,12.8,16.4,0.33,0.33);
       gr[surf]->Fit(sqaurey,"QR");
    }
    canClocks->Update();
    //    gSystem->Sleep(1000);
  }
  //  canClocks->cd(10);
  //  sqaurey->Draw();
}


Double_t funcSquareWave(Double_t *x, Double_t *par)
{
   Double_t phi=par[0];
   Double_t a=par[1];
   Double_t b=par[2];

   Double_t sllh=par[4];
   Double_t slhl=par[4];
   
   Double_t periodLeft=clockPeriod-2*par[4];   
   Double_t dtLow=par[3]*periodLeft;
   Double_t dtHigh=(1-par[3])*periodLeft;


   Double_t t=x[0]-phi;
   
   Double_t mlh=(a-b)/sllh;
   Double_t mhl=(b-a)/slhl;

   while(t<0) {
      t+=clockPeriod;
   }
   while(t>clockPeriod) {
      t-=clockPeriod;
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
