

void plotZcThingies() {
  Int_t surf=0;
  Int_t chip=0;


    TFile *fp  = new TFile("zcFile3871.root");
    //  TFile *fp  = new TFile("zcFile3451.root");
  
  
  TH1F *histZcAll = (TH1F*) fp->Get("histZcAll");
  TH1F *histSampAll = (TH1F*) fp->Get("histSampAll");
  TH1F *histRatioAll = new TH1F(*histZcAll);
  histRatioAll->Divide(histSampAll);
  TCanvas *canAll = new TCanvas("canAll","canAll");
  canAll->Divide(1,2);
  canAll->cd(1);
  histSampAll->SetLineColor(9);
  histSampAll->SetMinimum(0);
  histSampAll->SetStats(0);
  histSampAll->SetXTitle("Capacitor Number");
  histSampAll->Draw();
  histZcAll->SetLineColor(8);
  histZcAll->Draw("same");
  canAll->cd(2);
  histRatioAll->SetTitle("Fraction events with zero-crossing");
  histRatioAll->SetStats(0);
  histRatioAll->Draw();


  char histName[180];
  sprintf(histName,"histZcChip_%d_%d",surf,chip);
  TH1F *histZcChip = (TH1F*) fp->Get(histName);
  sprintf(histName,"histSampChip_%d_%d",surf,chip);
  TH1F *histSampChip = (TH1F*) fp->Get(histName);
  TH1F *histRatioChip = new TH1F(*histZcChip);
  histRatioChip->Divide(histSampChip);
  TCanvas *canChip = new TCanvas("canChip","canChip");
  canChip->Divide(1,2);
  canChip->cd(1);
  histSampChip->SetLineColor(9);
  histSampChip->SetMinimum(0);
  histSampChip->SetStats(0);
  histSampChip->SetXTitle("Capacitor Number");
  histSampChip->Draw();
  histZcChip->SetLineColor(8);
  histZcChip->Draw("same");
  canChip->cd(2);
  histRatioChip->SetTitle("Fraction events with zero-crossing");
  histRatioChip->SetStats(0);
  histRatioChip->Draw();

  TTree *zcTree = (TTree*) fp->Get("zcTree");
//   TCanvas *canSampRate = new TCanvas("canSampRate","canSampRate");
//   canSampRate->Divide(1,2);
//   canSampRate->cd(1);
//   zcTree->Draw("0.5*zcPreHitbus/sampPreHitbus/0.6","sampPreHitbus>100 && surf==0 && chip==0");
//   canSampRate->cd(2);
//   zcTree->Draw("0.5*zcPostHitbus/sampPostHitbus/0.6","sampPostHitbus>100 && surf==0 && chip==0","");
//   zcTree->SetLineColor(kRed-1);
//   zcTree->Draw("0.5*zcPostHitbus/sampPostHitbus/0.6","sampPostHitbus>100 && surf==0 && chip==0  && (sampPreHitbus>100 && (0.5*zcPreHitbus/sampPreHitbus/0.6)>0.3945)","same");
//   zcTree->SetLineColor(kBlue-1);
//   zcTree->Draw("0.5*zcPostHitbus/sampPostHitbus/0.6","sampPostHitbus>100 && surf==0 && chip==0  && (sampPreHitbus>100 && (0.5*zcPreHitbus/sampPreHitbus/0.6)<0.3945)","same");


//   TCanvas *canChip = new TCanvas("canChip","canChip");
//   canChip->cd();
//   zcTree->Draw("0.5*zcPreHitbus/sampPreHitbus/0.6:4*surf + chip","sampPreHitbus>100","box");
//   zcTree->SetLineColor(8);
//   zcTree->Draw("0.5*zcPostHitbus/sampPostHitbus/0.6:4*surf + chip","sampPostHitbus>100","same box");
  

//   TCanvas *canPrePost = new TCanvas("canPrePost","canPrePost");
//   canPrePost->cd();
//   zcTree->Draw("0.5*zcPreHitbus/sampPreHitbus/0.6:0.5*zcPostHitbus/sampPostHitbus/0.6","sampPreHitbus>100 && sampPostHitbus>100 && surf==0 && chip==0","box");


   TCanvas *canPreOnly = new TCanvas("canPreOnly","canPreOnly");
   canPreOnly->Divide(1,2);
   canPreOnly->cd(1);
   zcTree->Draw("0.5*zcPreHitbus/(sampPreHitbus-1)/0.6","surf==0 && chip==0 && sampPreHitbus>1","");
   canPreOnly->cd(2);
   zcTree->Draw("0.5*zcPreHitbus/(sampPreHitbus-1)/0.6:sampPreHitbus","surf==0 && chip==0 && sampPreHitbus>1","prof");


   TCanvas *canPostOnly = new TCanvas("canPostOnly","canPostOnly");
   canPostOnly->Divide(1,2);
   canPostOnly->cd(1);
   zcTree->Draw("0.5*zcPostHitbus/(sampPostHitbus-1)/0.6","surf==0 && chip==0 && sampPostHitbus>1","");
   canPostOnly->cd(2);
   zcTree->Draw("0.5*zcPostHitbus/(sampPostHitbus-1)/0.6:sampPostHitbus","surf==0 && chip==0 && sampPostHitbus>1","prof");


}
