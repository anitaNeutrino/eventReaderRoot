/////////////////// for plotting distributions of reduced quantities (for quality cuts) for WAIS pulses  //////////////////
////////////////// to help determine efficiency for keeping WAIS pulses after quality cuts are applied /////////////////////

//#include "AnitaConventions.h"
//#include "RawAnitaEvent.h"
//#include "UsefulAnitaEvent.h"
//#include "FilteredAnitaEvent.h"
//#include "FilterStrategy.h"
//#include "TimedAnitaHeader.h"
//#include "PrettyAnitaHk.h"
//#include "UsefulAnitaEvent.h"
//#include "PrettyAnitaEvent.h"
//#include "AnitaGeomTool.h"
//#include "Adu5Pat.h"
//#include "UsefulAdu5Pat.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TH1.h"
#include "TF1.h"
//#include "AnitaDataset.h"
#include "TStyle.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>
#include "TTreeIndex.h"
//#include "Util.h"
//#include "AnitaEventSummary.h"
//#include "Analyzer.h"
//#include "AnalysisConfig.h"
//#include "BH13Filter.h"


void qualityWAIS(int run)
{
	//AnalysisWaveform::enableDebug(1);
	AnitaGeomTool * agt = AnitaGeomTool::Instance();
	agt->usePhotogrammetryNumbers(1);
	AnitaPol::AnitaPol_t pol;

	Double_t sourceLat, sourceLon, sourceAlt, timeOffset;
	Int_t cutTimeNs = 1.2e3;

	Int_t ttnsDelta;

        int top_max_index = 0;
        int top_min_index = 0;  
        int bottom_max_index = 0;
        int bottom_min_index = 0; 
        double top_max = 0.0;
        double top_min = 0.0; 
        double bottom_max = 0.0;
        double bottom_min = 0.0;  
        double ratio[16]; 
        double max_ratio = 0.0;

        for (int i = 0; i < 16; i++)
        {
         ratio[i] = 0.0; 
        } 

        cout << "Hey I am here " << endl; 

	RawAnitaEvent * event = 0;
	RawAnitaHeader * header = 0;
	Adu5Pat * pat = 0;

	TChain *evChain = new TChain("eventTree");
	TChain *adu5Chain = new TChain("adu5PatTree");
	TChain *headChain = new TChain("headTree");

	TString datadir = getenv("ANITA_ROOT_DATA");
	TString runNum = TString::Itoa(run,10);
	TString evName = datadir + "/run" + runNum + "/eventFile" + runNum + ".root";
	TString adu5Name = datadir + "/run" + runNum + "/gpsEvent" + runNum + ".root";
	TString headName = datadir + "/run" + runNum + "/headFile" + runNum + ".root";

	evChain->Add(evName.Data());
	adu5Chain->Add(adu5Name.Data());
	headChain->Add(headName.Data());

	evChain->SetBranchAddress("event", &event);
	adu5Chain->SetBranchAddress("pat", &pat);
	headChain->SetBranchAddress("header", &header);
	
	UsefulAdu5Pat * usefulPat = 0;

	Long64_t maxEntry = evChain->GetEntries();

	Double_t thetaWave, phiWave;

	Double_t ttnsExpected;
	Double_t ttns;
	int ant;
	Int_t adjust = -1000;

	//Double_t deltaT = 1./(2.6*40.);
	Int_t npadfreq = 15;

	headChain->BuildIndex("eventNumber");
	TTreeIndex *ind = (TTreeIndex*) headChain->GetTreeIndex();

	FilterStrategy strategy;
	//strategy.addOperation(new UCorrelator::BH13Filter());
	int isVert = 0;

        TGraph *gtop = new TGraph(0); 
        TGraph *gbot = new TGraph(0); 
        TH1D *hmax_ratio = new TH1D("hmax_ratio",";MaxOverPhiSectors(Bottom ring pk-pk voltage/Top ring pk-pk voltage);Number of WAIS events",100,0,30);  

        cout << "before loop starts" << endl; 

	for(Long64_t entry=0; entry < maxEntry; entry++)
	{
                //cout << "inside entry loop " << endl; 
		evChain->GetEntry(entry);
		
		int entryIndex = ind->GetEntryNumberWithIndex(event->eventNumber, 0);
		if (entryIndex == -1) continue;
		headChain->GetEntry(entryIndex);
		adu5Chain->GetEntry(entryIndex);

		if((header->trigType&1) == 0) continue;
		usefulPat = new UsefulAdu5Pat(pat);
		ttns = header->triggerTimeNs;
		
		ttnsExpected = usefulPat->getWaisDivideTriggerTimeNs();
		ttnsDelta = Int_t(ttns) - Int_t(ttnsExpected) - adjust;
		double ttnsDeltaH = ttnsDelta + 10e3;

		if((TMath::Abs(ttnsDelta) > cutTimeNs) && (TMath::Abs(ttnsDeltaH) > cutTimeNs)) continue;
		
		if((TMath::Abs(ttnsDelta) < cutTimeNs)){
			isVert = 1; //should be V pulse
			pol = AnitaPol::kVertical;
		}
		if((TMath::Abs(ttnsDeltaH) < cutTimeNs)){
			isVert = 0; //should be H pulse
			pol = AnitaPol::kHorizontal;
		}
		
		usefulPat->getThetaAndPhiWave(AnitaLocations::LONGITUDE_WAIS_A4, AnitaLocations::LATITUDE_WAIS_A4, AnitaLocations::ALTITUDE_WAIS_A4, thetaWave, phiWave);
		
                //cout << "right before uae is defined" << endl; 
                UsefulAnitaEvent * uae = new UsefulAnitaEvent(event, WaveCalType::kDefault, header);

//pointing cut
/*			
		AnitaEventSummary * aes = new AnitaEventSummary(header, usefulPat);
		int goodflag = 0;
		FilteredAnitaEvent * fae = new FilteredAnitaEvent(uae, strategy, pat, header);
		analyzer.analyze(fae, aes);	
		AnitaEventSummary::SourceHypothesis w = aes->wais;
		for (int peakNum = 0; peakNum<2; peakNum++)
		{
			AnitaEventSummary::PointingHypothesis ph = aes->peak[isVert][peakNum];
			double phiDiff = w.phi - ph.phi;
			double thetaDiff = w.theta - ph.theta;
			if(fabs(phiDiff) < 1 && fabs(thetaDiff) < 1) goodflag = 1;
			printf("phi diff = %g,theta diff = %g\n", fabs(phiDiff), fabs(thetaDiff));
		}
		if (goodflag == 0) continue;
*/
		ant = agt->getTopAntNearestPhiWave(phiWave, pol);

                max_ratio = 0.0;
                //cout << "pol is " << pol << endl;  

                if (pol == 0) 
                {
                for (int iphi = 0; iphi < 16; iphi++)
                {
                 gtop = uae->getGraph(AnitaRing::kTopRing,iphi,pol);
                 gbot = uae->getGraph(AnitaRing::kBottomRing,iphi,pol); 

                 top_max_index = 0;
                 top_min_index = 0; 
                 bottom_max_index = 0;
                 bottom_min_index = 0; 
                 top_max = 0.0; 
                 top_min = 0.0; 
                 bottom_max = 0.0;
                 bottom_min = 0.0;
                 ratio[iphi] = 0.0; 
                 
                 top_max_index = TMath::LocMax(gtop->GetN(),gtop->GetY());
                 top_min_index = TMath::LocMin(gtop->GetN(),gtop->GetY()); 

                 bottom_max_index = TMath::LocMax(gbot->GetN(),gbot->GetY()); 
                 bottom_min_index = TMath::LocMin(gbot->GetN(),gbot->GetY()); 

                 top_max = gtop->GetY()[top_max_index];
                 top_min = gtop->GetY()[top_min_index]; 

                 bottom_max = gbot->GetY()[bottom_max_index]; 
                 bottom_min = gbot->GetY()[bottom_min_index]; 

                 //cout << "top max is " << top_max << endl; 
                 //cout << "bottom max is " << bottom_max << endl; 
                 //cout << "top min is " << top_min << endl; 
                 //cout << "bottom min is " << bottom_min << endl; 

                 ratio[iphi] = (bottom_max - bottom_min) / (top_max - top_min); 

                 delete gtop; 
                 delete gbot; 

                 } //loop over phi sectors end here 

                 max_ratio = ratio[TMath::LocMax(16,ratio)]; 
                 //cout << "max ratio is " << max_ratio << endl; 
                 hmax_ratio->Fill(max_ratio); 
                 } //if pol ends here 

		delete usefulPat;
		delete uae;
		//delete aes;
		//delete gtop;
                //delete gbot; 
	} //loop over events end here 

        //g->Draw(); 
	delete evChain;
	delete adu5Chain;
	delete headChain;


        TCanvas *c = new TCanvas("c","c",1000,800);
        hmax_ratio->Draw();
        c->SetLogy(); 
        hmax_ratio->SaveAs("wais_hmax_ratio.root");
        c->SaveAs("wais_max_ratio.root");
        c->SaveAs("wais_max_ratio.png");

}//macro ends here
