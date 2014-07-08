

void plotDiffTimeConstant()
{

  ifstream Input("justBinByBin.dat");
  char buffer[180];
  Input.getline(buffer,179);
  Int_t surf,chan,rco,sample;
  Double_t diff;
  TH1F *histTimeDiff = new TH1F("histTimeDiff","ANITA-I Time Bin Calibration",1000,0,1);
  while(Input >> surf >> chan >> rco >> sample) {
    if(sample==0)
      std::cout << surf << " " << chan << " " << rco << "\n";
    Input >> diff;
    //    if(surf==0 && chan==0 && rco==0) 

    if(rco==0)
      histTimeDiff->Fill(diff);

  }
  histTimeDiff->Draw();

}
