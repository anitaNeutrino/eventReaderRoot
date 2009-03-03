//a macro to plot the average RF Power in dbm and K

void plotAvgPower(){
     gSystem->AddIncludePath("-I{EVENT_READER_DIR}");
     gSystem->Load("libAnitaEvent.so");

     TFile *fp=new TFile("/home/kjp/2anita/surfhkFile_2.root");
     TTree *surfHkTree=(TTree*)fp->Get("surfHkTree");
     
     SurfHk *surfEvent;

     surfHkTree->SetBranchAddress("surf", &surfEvent);
     
     TH2D *avgMeasPowVsTimeAll=new TH2D("avgMeasPowVsTimeAll", "Average RF Power vs. Time", 3000, 1229710442, 1232417715, 700, 0, 700);

     TH2D *avgPowVsTimeAll=new TH2D("avgPowVsTimeAll", "Average RF Power vs. Time", 3000, 1229710442, 1232417715, 700, 0, 700);

     TH2D *avgMeasPowVsTime=new TH2D("avgMeasPowVsTime", "Average RF Power vs. Time", 3000, 1229710442, 1232417715, 700, 0, 700);

     TH2D *avgPowVsTime=new TH2D("avgPowVsTime", "Average RF Power vs. Time", 3000, 1229710442, 1232417715, 700, 0, 700);

     TH2D *avgMeasPowVsTimeNo36=new TH2D("avgMeasPowVsTimeNo36", "Average RF Power vs. Time", 3000, 1229710442, 1232417715, 700, 0, 700);

     TH2D *avgPowVsTimeNo36=new TH2D("avgPowVsTimeNo36", "Average RF Power vs. Time", 3000, 1229710442, 1232417715, 700, 0, 700);

     TH2D *avgMeasPowVsTimeOnlyV=new TH2D("avgMeasPowVsTimeOnlyV", "Average RF Power vs. Time", 3000, 1229710442, 1232417715, 700, 0, 700);

     TH2D *avgPowVsTimeOnlyV=new TH2D("avgPowVsTimeOnlyV", "Average RF Power vs. Time", 3000, 1229710442, 1232417715, 700, 0, 700);

     Double_t avgMeasAll=0;
     Double_t avgPowAll=0;
     Double_t avgMeas=0;
     Double_t avgPow=0;
     Double_t avgMeasNo36=0;
     Double_t avgPowNo36=0;
     Double_t avgMeasOnlyV=0;
     Double_t avgPowOnlyV=0;

     

     UInt_t nentries=surfHkTree->GetEntries();

     for(UInt_t ent=0; ent<nentries; ent++){
	  surfHkTree->GetEntry(ent);
	  avgMeasAll=0;
	  avgPowAll=0;
	  avgMeas=0;
	  avgPow=0;
	  avgMeasNo36=0;
	  avgPowNo36=0;
	  avgMeasOnlyV=0;
	  avgPowOnlyV=0;
	  for(Int_t i=0; i<10; i++){
	       for(Int_t j=0; j<8; j++){
		    avgMeasAll+=surfEvent->getMeasuredRFPowerInK(i,j);
		    avgPowAll+=surfEvent->getRFPowerInK(i,j);
		    if(!(i==7 && j==0)){
			 avgMeas+=surfEvent->getMeasuredRFPowerInK(i, j);
			 avgPow+=surfEvent->getRFPowerInK(i, j);
			 //printf("%d %d \n", i, j);
			 if(!(i==9 &&j==1)){
			      avgMeasNo36+=surfEvent->getMeasuredRFPowerInK(i,j);
			      avgPowNo36+=surfEvent->getRFPowerInK(i,j);
			      if(j<4){
				   avgMeasOnlyV+=surfEvent->getMeasuredRFPowerInK(i,j);
				   avgPowOnlyV+=surfEvent->getRFPowerInK(i,j);
			      }
			 }
		    }
	       }
	  }
	  avgMeasAll/=80;
	  avgPowAll/=80;
	  avgMeas/=79;
	  avgPow/=79;
	  avgMeasNo36/=78;
	  avgPowNo36/=78;
	  avgMeasOnlyV/=38;
	  avgPowOnlyV/=38;
	  avgMeasPowVsTimeAll->Fill((Double_t)surfEvent->realTime, avgMeasAll);
	  avgPowVsTimeAll->Fill((Double_t)surfEvent->realTime, avgPowAll);
	  avgMeasPowVsTime->Fill((Double_t)surfEvent->realTime, avgMeas);
	  avgPowVsTime->Fill((Double_t)surfEvent->realTime, avgPow);
	  avgMeasPowVsTimeNo36->Fill((Double_t)surfEvent->realTime, avgMeasNo36);
	  avgPowVsTimeNo36->Fill((Double_t)surfEvent->realTime, avgPowNo36);
	  avgMeasPowVsTimeOnlyV->Fill((Double_t)surfEvent->realTime, avgMeasOnlyV);
	  avgPowVsTimeOnlyV->Fill((Double_t)surfEvent->realTime, avgPowOnlyV);
     }

     gStyle->SetOptStat(0);

     TCanvas *measCan=new TCanvas("measCan", "measCan", 1000,500);
     avgMeasPowVsTimeAll->SetMarkerStyle(6);
     avgMeasPowVsTimeAll->SetMarkerColor(kBlack);
     avgMeasPowVsTimeAll->Draw();
     avgMeasPowVsTime->SetMarkerStyle(6);
     avgMeasPowVsTime->SetMarkerColor(kBlue-2);
     avgMeasPowVsTime->Draw("same");
     avgMeasPowVsTimeNo36->SetMarkerStyle(6);
     avgMeasPowVsTimeNo36->SetMarkerColor(kCyan-2);
     avgMeasPowVsTimeNo36->Draw("same");
     avgMeasPowVsTimeOnlyV->SetMarkerStyle(6);
     avgMeasPowVsTimeOnlyV->SetMarkerColor(kGreen-2);
     avgMeasPowVsTimeOnlyV->Draw("same");

     TCanvas *powCan=new TCanvas("powCan", "powCan",1000,500);
     avgPowVsTimeAll->SetMarkerStyle(6);
     avgPowVsTimeAll->SetMarkerColor(kBlack);
     avgPowVsTimeAll->Draw();
     avgPowVsTime->SetMarkerStyle(6);
     avgPowVsTime->SetMarkerColor(kBlue-2);
     avgPowVsTime->Draw("same");
     avgPowVsTimeNo36->SetMarkerStyle(6);
     avgPowVsTimeNo36->SetMarkerColor(kCyan-2);
     avgPowVsTimeNo36->Draw("same");
     avgPowVsTimeOnlyV->SetMarkerStyle(6);
     avgPowVsTimeOnlyV->SetMarkerColor(kGreen-2);
     avgPowVsTimeOnlyV->Draw("same");
	 
	  
   


}
