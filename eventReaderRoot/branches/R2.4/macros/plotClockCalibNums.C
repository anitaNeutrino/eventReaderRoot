
void plotClockCalibNums()
{
  TFile *fp = new TFile("rootFiles/clockPhaseCalib100000.root");
  char histName[180];

  gStyle->SetOptTitle(0);
  gStyle->SetOptFit(0);
  gStyle->SetStatX(1);
  gStyle->SetStatY(1);
  gStyle->SetStatW(0.4);
  gStyle->SetStatH(0.5);
  gStyle->SetTitleH(0.2);
  gStyle->SetTitleW(0.4);
  gStyle->SetTitleX(0.2);
  gStyle->SetTitleY(0.95);
  gStyle->SetLabelSize(0.1,"xy");

  TCanvas *can = new TCanvas("can","can",800,1000);
  can->Divide(4,9,0.00,0.001);

  TText *texty = new TText();
  texty->SetTextSize(0.1);

  ofstream CalibNums("clockCalibNums.dat");
  CalibNums << "#SURF\tChip\tClock Offset" << endl;

  for(int surf=0;surf<9;surf++) {
    for(int chip=0;chip<4;chip++) {
	
      TPad *paddy=can->cd((4*surf)+chip+1);
      if(chip==3)
	paddy->SetRightMargin(0.005);
      //      texty->SetTextColor(1);
      //      texty->SetTextSize(0.05);
      sprintf(histName,"histPhiDiffChip%d_%d",surf,chip);
      TH1F *histTemp = (TH1F*) fp->Get(histName);

      CalibNums << surf << "\t" << chip << "\t" << histTemp->GetMean() << endl;


      //      if(surf>0)
      //	histTemp->GetFunction("gaus")->Delete();
      sprintf(histName,"SURF %d - Chip %d",surf+1,chip+1);
      //      histTemp->SetTitle(histName);
	

      

      histTemp->Draw("hist");      
      histTemp->SetLineColor(getNiceColour(chip));
      histTemp->SetFillColor(getNiceColour(chip));
      histTemp->SetFillStyle(3017);
      can->Update();
      if(chip==0) {
	texty->DrawTextNDC(0.17,0.9,histName);
	TPaveStats *staty= (TPaveStats*) histTemp->FindObject("stats");
	staty->SetX1NDC(0.69);
      }
      else {
	texty->DrawTextNDC(0.02,0.9,histName);
      
      }
    }
  }

  



}
