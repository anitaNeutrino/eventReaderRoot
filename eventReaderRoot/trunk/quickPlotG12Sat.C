
void quickPlotG12Sat()
{

  gSystem->Load("libAnitaEvent.so");
  TCanvas *can =new TCanvas("can","can",400,400);
  G12Sat fred;

  Int_t azs[5]={30,56,89,270,156};
  Int_t els[5]={10,15,3,30,14};
  Int_t snrs[5]={29,34,33,24,21};

  fred.numSats=5;
  for(int i=0;i<5;i++) {
    fred.snr[i]=snrs[i];
    fred.azimuth[i]=azs[i];
    fred.elevation[i]=els[i];
  }

  fred.getCirclePlot(can);
}
