

void makeJustBinCalibFile() 
{
   Double_t meanBin[10][4];
   Double_t binWidth[10][4][2][260];

   TFile *fp  = new TFile("zcFile3871.root");
   TTree *zcTree = (TTree*) fp->Get("zcTree");
   
   //    TH1F *histProf = new TH1F("histProf","histProf",40,-0.5,39.5);
   zcTree->Draw("0.5*(zcPreHitbus+zcPostHitbus)/(sampPreHitbus+sampPostHitbus-2)/0.6:4*surf+chip>>histProf(40,-0.5,39.5)","","prof");
   
   {
      TH1F *histProf = (TH1F*) gROOT->FindObject("histProf");
      for(int surf=0;surf<10;surf++) {
	 for(int chip=0;chip<4;chip++) {
	    meanBin[surf][chip]=histProf->GetBinContent(1+chip +4*surf);
	    //	     cout << surf << "\t" << chip << "\t" << meanBin[surf][chip] << "\n";
	 }
      }
   }


   char histName[180];   
   for(int surf=0;surf<10;surf++) {
      for(int chip=0;chip<4;chip++) {
	 Double_t meanVal=0;
	 for(int rco=0;rco<2;rco++) {
	    sprintf(histName,"histZcChip_%d_%d_%d",surf,chip,rco);
	    TH1F *histZcChip = (TH1F*) fp->Get(histName);
	    sprintf(histName,"histSampChip_%d_%d_%d",surf,chip,rco);
	    TH1F *histSampChip = (TH1F*) fp->Get(histName);
	    TH1F *histRatioChip = new TH1F(*histZcChip);
	    histRatioChip->Divide(histSampChip);
	    
	    for(int samp=0;samp<259;samp++) {
	       meanVal+=histRatioChip->GetBinContent(samp+1);
	       binWidth[surf][chip][rco][samp]=meanBin[surf][chip]*histRatioChip->GetBinContent(samp+1);
	    }
	 }
	 meanVal/=2*259;
	 for(int rco=0;rco<2;rco++) {
	    for(int samp=0;samp<260;samp++) {
	       binWidth[surf][chip][rco][samp]/=meanVal;
	    }
	 }
      }
   }

   TH1F *histBinWidth = new TH1F("histBinWidth","histBinWidth",1000,0,1);
   TH1F *histBinWidth0 = new TH1F("histBinWidth0","histBinWidth",1000,0,1);
   TH1F *histBinWidth1 = new TH1F("histBinWidth1","histBinWidth",1000,0,1);
   ofstream Calib("justBinByBin.dat");
  Calib << "SURF\tChip\tRCO\tSample\tDeltaT\n";
  for(int surf=0;surf<10;surf++) {
    for(int chip=0;chip<4;chip++) {
      for(int rco=0;rco<2;rco++) {
	 for(int samp=0;samp<260;samp++) {
	    Calib << surf << "\t" << chip << "\t" << rco << "\t" << samp << "\t" << binWidth[surf][chip][rco][samp] << endl;
	    if(samp<259) {
	       histBinWidth->Fill(binWidth[surf][chip][rco][samp]);
	       if(rco==0) histBinWidth0->Fill(binWidth[surf][chip][rco][samp]);
	       if(rco==1) histBinWidth1->Fill(binWidth[surf][chip][rco][samp]);
	    }
	 }
    }
  }
  }
  Calib.close();
  histBinWidth->Draw();
  histBinWidth0->SetLineColor(getNiceColour(0));
  histBinWidth0->Draw("sames");
  histBinWidth1->SetLineColor(getNiceColour(1));
  histBinWidth1->Draw("sames");
  
}
