

void quickCompPlot()
{
  char firstLine[180];
  Int_t surf,chip,rco;
  Double_t calib;
  Double_t epsilonFromAbby3871[10][4][2]={0};
  Double_t epsilonFromAbby3459[10][4][2]={0};
  
  std::ifstream EpsilonAbbyFile3871("epsilonFromAbby3871.dat");
  EpsilonAbbyFile3871.getline(firstLine,179);
  while(EpsilonAbbyFile3871 >> surf >> chip >> rco >> calib) {
    epsilonFromAbby3871[surf][chip][rco]=calib;
    //	cout << surf << " " << chan << " " << chip << " " << calib << std::endl;
  }

  std::ifstream EpsilonAbbyFile3459("epsilonFromAbby3459.dat");
  EpsilonAbbyFile3459.getline(firstLine,179);
  while(EpsilonAbbyFile3459 >> surf >> chip >> rco >> calib) {
    epsilonFromAbby3459[surf][chip][rco]=calib;
    //	cout << surf << " " << chan << " " << chip << " " << calib << std::endl;
  }


  TH1F *histDiff = new TH1F("histDiff","histDiff",1000,-1,1);
  for(surf=0;surf<10;surf++) {
    for(chip=0;chip<4;chip++) {
      for(rco=0;rco<2;rco++) {
	histDiff->Fill(epsilonFromAbby3459[surf][chip][rco]-epsilonFromAbby3871[surf][chip][rco]);
      }
    }
  }
  histDiff->Draw();

}
