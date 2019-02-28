

int rtlAll(int startRun, int endRun) 
{

  RTLSpectrum * spec =0; 

  TChain c("rtlTree"); 

  TString str; 
  for (int i = startRun; i<=endRun; i++) 
  {
    str.Form("%s/run%d/rtlSpectrumFile%d.root",getenv("ANITA4_ROOT_DATA"), i,i); 
    c.Add(str.Data()); 
  }


  TFile out("rtlAll.root","RECREATE"); 
  TH2F * h[6]; 

  //now find out how many for each spectrum 

  std::vector<double> the_y_bins; 
  the_y_bins.reserve(4097); 
  for (int i = 0; i <=4096; i++) { the_y_bins.push_back(100-0.08 + 0.16*i) ; } 
  for (int i = 0; i < 6; i++) 
  {
    str.Form("rtlNum==%d",i+1); 
    int nbins = c.Draw("unixTimeStart",str.Data(),"goff"); 
    std::vector<double> the_bins(c.GetV1(), c.GetV1()+nbins); 
    the_bins.push_back(the_bins[nbins-1]+(the_bins[nbins-1]-the_bins[nbins-2])); 

    str.Form("RTL%d", i+1); 

    h[i] = new TH2F(str.Data(),str.Data(), nbins,&the_bins[0], 4096, &the_y_bins[0]); 
    h[i]->GetXaxis()->SetTitle("Scan Time"); 
    h[i]->GetYaxis()->SetTitle("Frequency"); 
    h[i]->GetZaxis()->SetTitle("Power"); 
    h[i]->GetXaxis()->SetTimeDisplay(1); 
    h[i]->SetEntries(nbins); 
  }

  //now let's fill 
  
  int index[6] = {0}; 
  
  c.SetBranchAddress("rtl",&spec); 
  for (int i = 0; i < c.GetEntries(); i++) 
  {
    c.GetEntry(i); 
    int which = spec->getRTLNum()-1; 
  //  printf("%d %d\n",i,which); 
    index[which]++; 

    if(!spec->getSpectrum()) continue; 
    for (int j = 1; j<=4096; j++) 
    {
      h[which]->SetBinContent(index[which], j, spec->getSpectrum()->GetY()[j-1]-spec->getGain());
    }
  }

  for (int i = 0; i < 6; i++) h[i]->Write(); 

  return 0; 

}
