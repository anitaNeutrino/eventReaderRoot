void getL2OverUnder(int start_run, int end_run, double over_sigma=2.5, double under_sigma=2.3)
{

  gStyle->SetOptFit(1); 

  TChain c("surfHkTree"); 


  for (int i = start_run; i <= end_run; i++)
  {
    c.Add(TString::Format("%s/run%d/surfHkFile%d.root", getenv("ANITA_ROOT_DATA"), i,i)); 
  }


  TCanvas * c1 = new TCanvas("surf_dists","Surf Distributions", 1800,900); 
  c1->Divide(4,4); 

  for (int i = 0; i < 16; i++) 
  {
     c1->cd(1+i); 
     c.Draw(TString::Format("surf->getL2Scaler(%d)",i)); 
     TH1 * h = (TH1*) gPad->FindObject("htemp"); 
     double surf_mean = h->GetMean(); 
     double surf_rms = h->GetRMS(); 
     h->SetName(TString::Format("h%d", i)); 
     h->SetTitle(TString::Format("Phi %d", i)); 

     TF1 * gamma = new TF1(TString::Format("gamma%d",i), "[0] * TMath::GammaDist(x,[1],0,[2])", h->GetXaxis()->GetXmin(), h->GetXaxis()->GetXmax()); 
     gamma->SetParameter(0,h->Integral()); 
     gamma->SetParName(0,"Constant"); 
     gamma->SetParameter(1,h->GetMean()); 
     gamma->SetParName(1,"k"); 
     gamma->SetParameter(2,1.); 
     gamma->SetParName(2,"theta"); 
     h->Fit(gamma,"Q"); 

     double turf_mean = 1000 * gamma->GetParameter(1) * gamma->GetParameter(2); 
     double turf_sigma = 2*sqrt(1000) * gamma->GetParameter(1) * gamma->GetParameter(2) * gamma->GetParameter(2); 

     printf("Predicted TURF Phi Sector %d L2 distribution using Gamma Fit: Gaus(%0.1f,%0.1f).\n\tL2 OVER: %0.1f\n\tL2 UNDER %0.1f\n\n", i, turf_mean, turf_sigma, turf_mean + over_sigma * turf_sigma, turf_mean + under_sigma *turf_sigma); 


     turf_mean = surf_mean * 1000; 
     turf_sigma = surf_rms * sqrt(1000)*2; 
     printf("Predicted TURF Phi Sector %d L2 distribution using moment scaling: Gaus(%0.1f,%0.1f).\n\tL2 OVER: %0.1f\n\tL2 UNDER %0.1f\n\n", i, turf_mean, turf_sigma, turf_mean + over_sigma * turf_sigma, turf_mean + under_sigma *turf_sigma); 
  }
}
