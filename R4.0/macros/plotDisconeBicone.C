//#include "AnitaConventions.h"

void plotDisconeBicone()
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
   plotDisconeBicone(1030,10000,1);
}
  

void plotDisconeBicone(int run, int startEntry, int numEntries) {
  gSystem->Load("libAnitaEvent.so");

  char eventName[FILENAME_MAX];
  char headerName[FILENAME_MAX];
  char hkName[FILENAME_MAX];
  sprintf(eventName,"/unix/anita1/webData/initial/run%d/eventFile%d*.root",run,run);
  sprintf(headerName,"/unix/anita1/webData/initial/run%d/timedHeadFile%d.root",run,run);
  sprintf(hkName,"/unix/anita1/webData/initial/run%d/prettyHkFile%d.root",run,run);

  RawAnitaEvent *event = 0;
  TimedAnitaHeader *header =0;
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


  TCanvas *canClock = (TCanvas*) gROOT->FindObject("canClock");
  if(!canClock) {
    canClock = new TCanvas("canClock","canClock",800,800);
  }
  char textLabel[180];
  char pngName[180];
  TGraph *gr[9]={0};
  for(int entry=startEntry;entry<startEntry+numEntries;entry++) {
    
    //Stupidly must do this to be perfectly safe  
    eventChain->GetEntry(entry);
    headTree->GetEntry(entry);
    prettyHkTree->GetEntry(entry);	
       
    
       
    UsefulAnitaEvent realEvent(event,WaveCalType::kVTFullJWPlusFastClockZero,hk);
    cout << entry << "\t" <<  realEvent.eventNumber << " " << header->eventNumber << endl;
    //  cout << realEvent.gotCalibTemp << " " << realEvent.calibTemp << endl;
    canClock->Clear();
    canClock->SetTopMargin(0);
    TPaveText *topPave = new TPaveText(0.05,0.92,0.95,0.98);
    topPave->SetBorderSize(0);
    
    sprintf(textLabel,"Event %lu",realEvent.eventNumber);
    TText *eventText = topPave->AddText(textLabel);
    eventText->SetTextColor(50);
    topPave->Draw();
    
    TPad *graphPad = new TPad("graphPad","This is pad1",0,0,1,0.9);
    graphPad->SetTopMargin(0);
    graphPad->Draw();
    graphPad->Clear();
    //    graphPad->Clear();
    for(int surf=0;surf<9;surf++) {
      if(gr[surf]) delete gr[surf];
    }
    graphPad->Divide(2,4);

    cout << "Float_t discVals[8][260]={\n";
    for(int chan=0;chan<8;chan++) {
      graphPad->cd(chan+1);
      gr[chan] = realEvent.getGraph(UsefulAnitaEvent::getChanIndex(8,chan));
      gr[chan]->Draw("al");
      //      if(chan==0) {
      cout << "{";
	Double_t *x=gr[chan]->GetY();
	Double_t N=gr[chan]->GetN();
	for(int i=0;i<260;i++) {
	  if(i<N) {
	    cout << x[i] << ",";
	  }
	  else cout << 0 << ",";
	}
	cout << "},\n";
	//      }


    }
    canClock->Update();
    sprintf(pngName,"clocks%lu.png",realEvent.eventNumber);
    //    canClock->Print(pngName);
    //       break;
    gSystem->Sleep(1000);
  
  }
}
