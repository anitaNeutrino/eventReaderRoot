

void quickFillEpsilonCalibFile() 
{

  //  TFile *fp = new TFile("epsilonTree.root");
  TFile *fp = new TFile("epsilonTree3871.root");
  TTree *epsilonTree = (TTree*) fp->Get("epsilonTree");
  Double_t meanBin[10][4][2];


  char plotArg[180];
  char plotCond[180];
  char histName[180];

  TF1 *fitty = new TF1("fitty","gaus",0,3);
  
  for(int surf=0;surf<10;surf++) {
    TCanvas *can = new TCanvas();
    can->Divide(2,4);
    for(int chip=0;chip<4;chip++) {
      for(int rco=0;rco<2;rco++) {
	can->cd(1 +rco +2*chip);
	sprintf(plotArg,"epsilon");
	sprintf(plotCond,"surf==%d && chip==%d && rco==%d",surf,chip,rco);
	epsilonTree->Draw(plotArg,plotCond);
	TH1F *htemp = gROOT->FindObject("htemp");
	if(htemp) {
	  sprintf(histName,"htemp_%d_%d_%d",surf,chip,rco);
	  htemp->SetName(histName);
	  Double_t peak=htemp->GetBinCenter(htemp->GetMaximumBin());
	  fitty->SetRange(peak-0.1,peak+0.1);
	  htemp->Fit("fitty","QR");
	  meanBin[surf][chip][rco]=fitty->GetParameter(1);	  
	}

	cout << surf << "\t" << chip << "\t" << rco << "\t" << meanBin[surf][chip][rco] << "\n";
      }
    }
  }



 ofstream Calib("epsilonFromAbby3871.dat");
  Calib << "SURF\tChip\tEnd RCO\tEpsilon\n";
  for(int surf=0;surf<10;surf++) {
    for(int chip=0;chip<4;chip++) {
      for(int rco=0;rco<2;rco++) {
	Calib << surf << "\t" << chip << "\t" << rco << "\t" << meanBin[surf][chip][rco] << endl;
      }
    }
  }  
  Calib.close();
}
