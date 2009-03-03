

void plotClockPeriods() {
   TFile *fp = new TFile("clockPeriodFactor64.root");
   TTree *periodTree = (TTree*) fp->Get("periodTree");

   TCanvas *can = new TCanvas("can","can");
   periodTree->Draw("period1","period1>29.7 && period1<30.23");
   Double_t fullMean=0;
   {
      TH1F *htemp = (TH1F*) gROOT->FindObject("htemp");
      if(htemp) {
	 fullMean=htemp->GetMean();
      }
   }

   Double_t subMeans[9][4][2];
   Double_t calibConstants[9][4][2];

   for(int surf=0;surf<9;surf++) {
      for(int chip=0;chip<4;chip++) {
	 for(int rco=0;rco<2;rco++) {
	    char cond[180];
	    sprintf(cond,"period1>29.7 && period1<30.23 && surf==%d && chip==%d && rco==%d",surf,chip,rco);
	    periodTree->Draw("period1",cond); 
	    {
	       TH1F *htemp = (TH1F*) gROOT->FindObject("htemp");
	       if(htemp) {
		  subMeans[surf][chip][rco]=htemp->GetMean();
		  calibConstants[surf][chip][rco]=fullMean/subMeans[surf][chip][rco];
	       }
	    }
	 }
      }     
   }

   for(int surf=0;surf<9;surf++) {
      for(int chip=0;chip<4;chip++) {
	 for(int rco=0;rco<2;rco++) {
	    cout << surf << "\t" << chip << "\t" << rco << "\t" << calibConstants[surf][chip][rco] << "\n";
	 }
      }
   }


}
