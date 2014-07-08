


void quickPlotCalib() 
{
  char tempString[180];
  ifstream Calib("justBinByBin.dat");
  ifstream CalibNew("justBinByBinNew.dat");  
  ifstream CalibRyan("justBinByBinRyan.dat");
  ofstream CalibNewest("justBinByBinFlipped.dat");
  //  ofstream CalibStrange("justBinByBinStrange.dat");

  Double_t calib[10][4][2][260]={0};
  Double_t calibNew[10][4][2][260]={0};
  Double_t calibRyan[10][4][2][260]={0};
  

  Calib.getline(tempString,179);
  CalibNew.getline(tempString,179);
  CalibRyan.getline(tempString,179);

  
  Int_t surf,chip,rco,samp;
  Double_t calOld,calNew,calFair,calOther,calOldOther,calNewOther,calRyan,calRyanOther;
  TTree *calTree = new TTree("calTree","Tree of Cals.");
  calTree->Branch("surf",&surf,"surf/I");
  calTree->Branch("chip",&chip,"chip/I");
  calTree->Branch("rco",&rco,"rco/I");
  calTree->Branch("samp",&samp,"samp/I");
  calTree->Branch("calOld",&calOld,"calOld/D");
  calTree->Branch("calOldOther",&calOldOther,"calOldOther/D");
  calTree->Branch("calNew",&calNew,"calNew/D");
  calTree->Branch("calNewOther",&calNewOther,"calNewOther/D");
  calTree->Branch("calRyan",&calRyan,"calRyan/D");
  calTree->Branch("calRyanOther",&calRyanOther,"calRyanOther/D");
  calTree->Branch("calFair",&calFair,"calFair/D");
  calTree->Branch("calOther",&calOther,"calOther/D");

  for(surf=0;surf<10;surf++) {
    for(chip=0;chip<4;chip++) {
      for(rco=0;rco<2;rco++) {
	 for(samp=0;samp<260;samp++) {
	    Calib >> surf  >> chip  >> rco  >> samp  >> calib[surf][chip][rco][samp];
	    CalibNew >> surf  >> chip  >> rco  >> samp  >> calibNew[surf][chip][rco][samp];
	    CalibRyan >> surf  >> chip  >> rco  >> samp  >> calibRyan[surf][chip][rco][samp];
	    //	    if(surf==0 && chip==0 && rco==0) {
	    //	      cout << surf << "\t" << chip << "\t" << rco << "\t" << samp
	    //		   << "\t" << calib[surf][chip][rco][samp] << "\t" 
	    //		   << calibNew[surf][chip][rco][samp] << "\n";
	    //	    }
	    
	 }
      }
    }
  }

  CalibNewest << "SURF\tChip\tRCO\tSamp\n";
  //  CalibStrange << "SURF\tChip\tRCO\tSamp\n";
  for(surf=0;surf<10;surf++) {
    for(chip=0;chip<4;chip++) {
      for(rco=0;rco<2;rco++) { 
	for(samp=0;samp<260;samp++) {
	  calOld=calib[surf][chip][rco][samp];
	  calOldOther=calib[surf][chip][1-rco][samp];
	  calNew=calibNew[surf][chip][rco][samp];
	  calNewOther=calibNew[surf][chip][1-rco][samp];
	  calRyan=calibRyan[surf][chip][rco][samp];
	  calRyanOther=calibRyan[surf][chip][1-rco][samp];

	  CalibNewest << surf << "\t" << chip << "\t" << rco << "\t"
		      << samp << "\t" << calNewOther << "\n";

	  calFair=(calib[surf][chip][rco][samp]+calib[surf][chip][rco][samp+1])*0.5;
	  calOther=(calib[surf][chip][1-rco][samp]+calib[surf][chip][1-rco][samp+1])*0.5;
	  //	  if(samp<259)
	  //	    CalibStrange << surf << "\t" << chip << "\t" << rco << "\t"
	  //			 << samp << "\t" << calFair << "\n";
	  //	  else 
	  //	    CalibStrange << surf << "\t" << chip << "\t" << rco << "\t"
	  //			 << samp << "\t" << 0 << "\n";
	    

	  calTree->Fill();  
	}
      }
    }
 }


  Double_t epsOld;
  Double_t epsNew;
  Double_t epsFlipped;


  TTree *epsTree = new TTree("epsTree","Tree of epsilons");
  epsTree->Branch("surf",&surf,"surf/I");
  epsTree->Branch("chip",&chip,"chip/I");
  epsTree->Branch("rco",&rco,"rco/I");
  epsTree->Branch("epsOld",&epsOld,"epsOld/D");
  epsTree->Branch("epsNew",&epsNew,"epsNew/D");
  epsTree->Branch("epsFlipped",&epsFlipped,"epsFlipped/D");


  ifstream CalibEpsilonNew("epsilonFromAbby.dat");
  CalibEpsilonNew.getline(tempString,179);
  ifstream CalibOld("epsilonFromAbbySortOfWorks.dat");
  CalibOld.getline(tempString,179);
  ofstream CalibEpsilonFlipped("epsilonFromAbbyFlipped.dat");
  CalibEpsilonFlipped << "SURF\tChip\tEnd RCO\tEpsilon\n";
  Double_t calibVals[10][4][2];
  Double_t tempVal;
  for(int surf=0;surf<10;surf++) {
    for(int chip=0;chip<4;chip++) {
      for(int rco=0;rco<2;rco++) {
	CalibEpsilonNew >> surf  >> chip  >> rco  >> tempVal;
	calibVals[surf][chip][rco]=tempVal;
	//	cout << surf << "\t" << chip << "\t" << rco << "\t"
	//	     << calibVals[surf][chip][rco] << "\t" << tempVal << "\n";
      }
    }
  }  
  for(int surf=0;surf<10;surf++) {
    for(int chip=0;chip<4;chip++) {
      for(int rco=0;rco<2;rco++) {
	CalibOld >>  surf  >> chip  >> rco  >> tempVal;
	CalibEpsilonFlipped << surf  << "\t" <<  chip  << "\t" <<  rco  << "\t" << calibVals[surf][chip][1-rco] << "\n";
	epsOld=tempVal;
	epsNew=calibVals[surf][chip][rco];
	epsFlipped=calibVals[surf][chip][1-rco];
	epsTree->Fill();
      }
    }
  }  
}


void plotEpsilon() 
{
  TCanvas *canEps = new TCanvas("canEps","canEps",600,400);
  epsTree->Draw("epsNew>>heps(100,0,3)");
  heps->SetTitle("Wrap-Around Time");
  heps->GetXaxis()->SetTitle("#epsilon_{ij} (ns)");
  sortOutTitle();
  PrintEPSandPNG(canEps,"epsilon1d");
  
  TCanvas *canEps2 = new TCanvas("canEps2","canEps2",600,400); 
  epsTree->Draw("rco+2*chip:surf>>heps2(10,-0.5,9.5,8,-0.5,7.5)","epsNew","colz");
  heps2->SetTitle("Wrap-Around Time");
  heps2->GetXaxis()->SetTitle("SURF");
  heps2->GetYaxis()->SetTitle("RCO+2*LABRADOR");
  heps2->SetStats(0);
  sortOutTitle();
  PrintEPSandPNG(canEps2,"epsilon2d");
}
