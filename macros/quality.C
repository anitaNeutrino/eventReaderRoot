////////////// Oindree Banerjee ////////////////////
///////////// July 4-5 2017 ////////////////////////
//////////// for characterizing various quality cuts /////////////////////
//////////// to run do root -l quality.C"(131,132)" for example ////////////////
///////////// editing on Dec 12 2017 /////////////////////////////

void quality(int whichrun, int whichanita);
void quality(int whichrun, int whichanita)

{
  gStyle->SetOptStat(1111111111); 
  AnitaVersion::set(whichanita);
  cout << "ANITA version is " << whichanita << endl; 

  double count=0.;

  AnitaDataset d(whichrun);
  cout << "On run " << whichrun << endl; 
  cout << "number of entries is " << d.N() << endl;
    
  const int num_phi = 16;
  AnitaPol::AnitaPol_t pol = AnitaPol::kVertical;
  
  cout << "after pol set" << endl; 
  
  TGraph *gr_top = new TGraph(0);
  TGraph *gr_middle = new TGraph(0); 
  TGraph *gr_bottom = new TGraph(0);

  ofstream myfile_blast_variables;
  myfile_blast_variables.open(Form("Anita%iRun%iBlastVariables.txt",whichanita,whichrun), std::ofstream::out | std::ofstream::app); 
  myfile_blast_variables << "runNumber,eventNumber,maxRatio,maxReverseRatio,maxRms,currents0,currents1,currents2,currents3,currents4,currents5,currents6 \n"; 
   
  cout << "after new file set up " << endl; 
 
  ofstream myfile_passed_blast_cut;
  myfile_passed_blast_cut.open("passedBlastCut.txt", std::ofstream::out | std::ofstream::app);
  myfile_passed_blast_cut << "max ratio,event number,run number \n";
    
  ofstream myfile_less_than_1;
  myfile_less_than_1.open("bottomOverTopLessThan1.txt",std::ofstream::out | std::ofstream::app);
  myfile_less_than_1 << "max_ratio,event number,run number \n";

  ofstream myfile_passed_reverse_blast_cut;
  myfile_passed_reverse_blast_cut.open("passedReverseBlastCut.txt", std::ofstream::out | std::ofstream::app);
  myfile_passed_reverse_blast_cut << "max ratio,event number,run number \n";
    
  ofstream myfile_reverse_less_than_1;
  myfile_reverse_less_than_1.open("topOverBottomLessThan1.txt",std::ofstream::out | std::ofstream::app);
  myfile_reverse_less_than_1 << "max_ratio,event number,run number \n";
    
  int top_max_index = 0;
  int top_min_index = 0;
  double top_max = 0.0;
  double top_min = 0.0;

  int bottom_max_index = 0;
  int bottom_min_index = 0;
  double bottom_max = 0.0;
  double bottom_min = 0.0;

  int middle_max_index = 0;
  int middle_min_index = 0;
  double middle_max = 0.0;
  double middle_min = 0.0;

  double average_pkpk = 0.0; 
 
  double max_ratio = 0.0;
  double max_ratio_reverse = 0.0;
  double max_abs_volt = 0.0; 
  double min_pkpk = 0.0;
  double max_asymm = 0.0;
  double max_rms = 0.0;   
  
  double ratio[num_phi];
  double ratio_reverse[num_phi]; 
  double abs_volt[num_phi];
  double pkpk[num_phi]; 
  double asymm[num_phi];
  double rms[num_phi];  

  for (int iphi = 0; iphi < num_phi; iphi++)
  {
    ratio[iphi] = 0.0;
    ratio_reverse[iphi] = 0.0; 
    abs_volt[iphi] = 0.0; 
    pkpk[iphi] = 0.0; 
    asymm[iphi] = 0.0;
    rms[iphi] = 0.0;  
  }
  
  TH1D *hmax_rms = new TH1D("hmax_rms",";MaxOverPhiSectors(RMS of pk-pk voltage across all rings);Number of Events",100,0,3500);   
  TH1D *hmax_ratio = new TH1D("hmax_ratio",";MaxOverPhiSectors((Bottom ring pk-pk voltage)/(Top ring pk-pk voltage));Number of Events",100,0,30);
  TH1D *hmax_ratio_reverse = new TH1D("hmax_ratio_reverse",";MaxOverPhiSectors((Top ring pk-pk voltage)/(Bottom ring pk-pk voltage));Number of Events",100,0,30);
  TH2D *hmin_signal = new TH2D("hmin_signal",";MaxOverPhiSectors((Bottom ring pk-pk voltage)/(Top ring pk-pk voltage));MinOverPhiSectors(Bottom ring pk-pk voltage in mV)",100,0,30,100,0,1000);
  TH2D *h2 = new TH2D("h2",";MaxOverPhiSectors((Bottom ring pk-pk voltage)/(Top ring pk-pk voltage));MaxOverPhiSectors(Bottom Ring |V| in mV)",100,0,30,100,0,2000); 
  TH2D *hblastandreverse = new TH2D("hblastandreverse",";Max((Bottom ring pk-pk voltage)/(Top ring pk-pk voltage));Max((Top ring pk-pk voltage)/(Bottom ring pk-pk voltage))",100,0,30,100,0,30); 
  TH2D *hblastandasymm = new TH2D("hblastandasymm",";MaxOverPhiSectors((Bottom ring pk-pk voltage)/(Top ring pk-pk voltage));MaxOverPhiSectors(|Vmax - |Vmin|| in mV)",100,0,30,100,0,100); 
  
  for (int ientry = 0; ientry < d.N(); ientry = ientry+1)

  {
    count = ientry*1.;
    d.getEntry(ientry);
      
    max_ratio = 0.0;
    max_ratio_reverse = 0.0;
    max_abs_volt = 0.0;
    min_pkpk = 0.0;
    max_asymm = 0.0;
    max_rms = 0.0;    
      
    for (int iphi = 0; iphi < num_phi; iphi++) //loop over phi sectors
    {
      top_max_index = 0;
      top_min_index = 0;
      top_max = 0.0;
      top_min = 0.0;
      bottom_max_index = 0;
      bottom_min_index = 0;
      bottom_max = 0.0;
      bottom_min = 0.0;
      middle_max_index = 0;
      middle_min_index = 0;
      middle_max = 0.0;
      middle_min = 0.0; 
      average_pkpk = 0.0; 

      ratio[iphi] = 0.0;
      ratio_reverse[iphi] = 0.0;
      abs_volt[iphi] = 0.0;
      pkpk[iphi] = 0.0;
      asymm[iphi] = 0.0;
      rms[iphi] = 0.0;    

      gr_top = d.useful() -> getGraph(AnitaRing::kTopRing,iphi,pol);
      gr_bottom = d.useful() -> getGraph(AnitaRing::kBottomRing,iphi,pol);
      gr_middle = d.useful() -> getGraph(AnitaRing::kMiddleRing,iphi,pol);         

      top_max_index = TMath::LocMax(gr_top->GetN(),gr_top->GetY());
      top_min_index = TMath::LocMin(gr_top->GetN(),gr_top->GetY());
      top_max = gr_top->GetY()[top_max_index];
      top_min = gr_top->GetY()[top_min_index];
        
      //cout << "top max, min in mV are                 " << top_max << ",   " << top_min << endl;
        
      bottom_max_index = TMath::LocMax(gr_bottom->GetN(),gr_bottom->GetY());
      bottom_min_index = TMath::LocMin(gr_bottom->GetN(),gr_bottom->GetY());
      bottom_max = gr_bottom->GetY()[bottom_max_index];
      bottom_min = gr_bottom->GetY()[bottom_min_index];
        
      //cout << "bottom max, min in mV are              " << bottom_max << ",   " << bottom_min << endl;
      
      middle_max_index = TMath::LocMax(gr_middle->GetN(),gr_middle->GetY());
      middle_min_index = TMath::LocMin(gr_middle->GetN(),gr_middle->GetY()); 
      middle_max = gr_middle->GetY()[middle_max_index];
      middle_min = gr_middle->GetY()[middle_min_index]; 
     
      average_pkpk = ((bottom_max - bottom_min) + (middle_max - middle_min) + (top_max - top_min))/3;  
      rms[iphi] = sqrt(pow(((bottom_max - bottom_min) - average_pkpk),2) + pow(((middle_max - middle_min) - average_pkpk),2) + pow(((top_max - top_min) - average_pkpk),2)); 
      //cout << "average pkpk " << average_pkpk << endl; 
      //cout << bottom_max - bottom_min << " " << middle_max - middle_min << " " << top_max - top_min << endl; 
      //cout << "rms " << rms[iphi] << endl; 

      ratio[iphi] = (bottom_max - bottom_min) / (top_max - top_min);
      ratio_reverse[iphi] = (top_max - top_min) / (bottom_max - bottom_min);
      pkpk[iphi] = (bottom_max - bottom_min);
      asymm[iphi] = fabs(bottom_max - fabs(bottom_min)); 
      //cout << "ratio for phi number            " << iphi << " is                    " << ratio[iphi] << endl;
      if (fabs(bottom_max) > fabs(bottom_min)) { abs_volt[iphi] = fabs(bottom_max); } 
      if (fabs(bottom_min) > fabs(bottom_max)) { abs_volt[iphi] = fabs(bottom_min); }          

      delete gr_top;
      delete gr_middle; 
      delete gr_bottom;
    }
    
    max_rms = rms[TMath::LocMax(num_phi,rms)];  
    //cout << "max rms " << max_rms << endl;  
    max_ratio = ratio[TMath::LocMax(num_phi,ratio)]; //LocMax gives the index
    max_ratio_reverse = ratio_reverse[TMath::LocMax(num_phi,ratio)]; //LocMax gives the index
    min_pkpk = pkpk[TMath::LocMin(num_phi,pkpk)]; 
    //cout << "max_ratio for ientry =  " << ientry << " is " << max_ratio << endl;
    max_abs_volt = abs_volt[TMath::LocMax(num_phi,abs_volt)];    
    max_asymm = asymm[TMath::LocMax(num_phi,asymm)];  
 
    hmax_rms->Fill(max_rms); 
    hmax_ratio->Fill(max_ratio);
    hmax_ratio_reverse->Fill(max_ratio_reverse);
    h2->Fill(max_ratio,max_abs_volt); 
    hmin_signal->Fill(max_ratio,min_pkpk);
    hblastandreverse->Fill(max_ratio,max_ratio_reverse); 
    hblastandasymm->Fill(max_ratio,max_asymm);  

    //if (min_pkpk < 50) {cout << "min pk pk is " << min_pkpk << " run is " << d.header()->run << " event number is " << d.header()->eventNumber << endl; }   
      
    if (max_ratio > 2.5)
    {
      myfile_passed_blast_cut << setprecision(3) << max_ratio << "," << setprecision(11) << d.header()->eventNumber << "," << d.header()->run << "    " <<"\n"; //write to file
    }
      
    if (max_ratio < 1.0)
    {
      myfile_less_than_1 << setprecision(3) << max_ratio << "," << setprecision(11) << d.header()->eventNumber << "," << d.header()->run << "    " <<"\n"; //write to file
    }
      
    if (max_ratio_reverse > 2.0)
    {
      myfile_passed_reverse_blast_cut << setprecision(3) << max_ratio << "," << setprecision(11) << d.header()->eventNumber << "," << d.header()->run << "    " <<"\n"; //write to file
    }
      
    if (max_ratio_reverse < 1.0)
    {
      myfile_reverse_less_than_1 << setprecision(3) << max_ratio << "," << setprecision(11) << d.header()->eventNumber << "," << d.header()->run << "    " <<"\n"; //write to file
    }

    myfile_blast_variables << d.header()->run << "," << d.header()->eventNumber << "," << max_ratio << "," << max_ratio_reverse << "," << max_rms << ","; 
    myfile_blast_variables << d.hk()->currents[0] << "," << d.hk()->currents[1] << "," << d.hk()->currents[2] << "," << d.hk()->currents[3] << ","; 
    myfile_blast_variables << d.hk()->currents[4] << "," << d.hk()->currents[5] << "," << d.hk()->currents[6] << "\n"; 

    //cout << d.header()->run << "  " << d.header()->eventNumber << "  " << max_ratio << "  " << max_ratio_reverse << "  " << max_rms << "\n"; //write to file
    
    cout << setprecision(5) << (count / d.N())*100 << "% done" << endl;   
    
  } //loop over events
    
  TCanvas *crms = new TCanvas("crms","crms",1000,800);
  hmax_rms->Draw();
  crms->SetLogy();
  crms->SaveAs(Form("max_rms_pol%i.png",pol));
  hmax_rms->SaveAs(Form("hmax_rms_pol%i.root",pol));
  crms->SaveAs(Form("max_rms_pol%i.root",pol));
  delete crms; 

  /*
 
  TCanvas *h = new TCanvas("h","h",1000,800);
  hmax_ratio->SetStats(0);
  hmax_ratio->SetTitle(Form("Run %i",whichrun));
  hmax_ratio->Draw();
  h->SetLogy();
  h->SaveAs(Form("blast_max_ratio%i.png",pol));
  hmax_ratio->SaveAs(Form("hblast_max_ratio%i.root",pol));
  h->SaveAs(Form("blast_max_ratio%i.root",pol));
  delete h;

  TCanvas *hr = new TCanvas("hr","hr",1000,800);
  hmax_ratio_reverse->SetStats(0);
  hmax_ratio_reverse->SetTitle(Form("Run %i",whichrun));
  hmax_ratio_reverse->Draw();
  hr->SetLogy();
  hr->SaveAs(Form("reverse_blast_max_ratio%i.png",pol));
  hmax_ratio_reverse->SaveAs(Form("hreverse_blast_max_ratio%i.root",pol));
  hr->SaveAs(Form("reverse_blast_max_ratio%i.root",pol));
  delete hr;

  TCanvas *hh = new TCanvas("hh","hh",1000,800);
  hh->SetRightMargin(0.14); 
  h2->SetStats(0);
  h2->SetTitle(Form("Run %i",whichrun));
  h2->GetYaxis()->SetTitleOffset(1.4); 
  h2->GetZaxis()->SetTitle("Number of Events");
  h2->Draw("colz");
  hh->SetLogz();
  hh->SaveAs(Form("blast_max_ratio_dig_sat%i.png",pol));
  h2->SaveAs(Form("hblast_max_ratio_dig_sat%i.root",pol));
  hh->SaveAs(Form("blast_max_ratio_dig_sat%i.root",pol));
  delete hh;

  TCanvas *hm = new TCanvas("hm","hm",1000,800);
  hm->SetRightMargin(0.14); 
  hmin_signal->SetStats(0);
  hmin_signal->SetTitle(Form("Run %i",whichrun));
  hmin_signal->GetYaxis()->SetTitleOffset(1.4); 
  hmin_signal->GetZaxis()->SetTitle("Number of Events");
  hmin_signal->Draw("colz");
  hm->SetLogz();
  hm->SaveAs(Form("blast_max_ratio_min_signal%i.png",pol));
  hmin_signal->SaveAs(Form("hblast_max_ratio_min_signal%i.root",pol));
  hm->SaveAs(Form("blast_max_ratio_min_signal%i.root",pol));
  delete hm;

  TCanvas *hbr = new TCanvas("hbr","hbr",1000,800);
  hbr->SetRightMargin(0.14); 
  hblastandreverse->SetStats(0);
  hblastandreverse->SetTitle(Form("Run %i",whichrun));
  hblastandreverse->GetYaxis()->SetTitleOffset(1.4); 
  hblastandreverse->GetZaxis()->SetTitle("Number of Events");
  hblastandreverse->Draw("colz");
  hbr->SetLogz();
  hbr->SaveAs(Form("blast_max_ratio_vs_reverse%i.png",pol));
  hblastandreverse->SaveAs(Form("hblast_max_ratio_vs_reverse%i.root",pol));
  hbr->SaveAs(Form("blast_max_ratio_vs_reverse%i.root",pol));
  delete hbr;

  TCanvas *hba = new TCanvas("hba","hba",1000,800);
  hba->SetRightMargin(0.14); 
  hblastandasymm->SetStats(0);
  hblastandasymm->SetTitle(Form("Run %i",whichrun));
  hblastandasymm->GetYaxis()->SetTitleOffset(1.4); 
  hblastandasymm->GetZaxis()->SetTitle("Number of Events");
  hblastandasymm->Draw("colz");
  hba->SetLogz();
  hba->SaveAs(Form("blast_max_ratio_vs_asymm%i.png",pol));
  hblastandasymm->SaveAs(Form("hblast_max_ratio_vs_asymm%i.root",pol));
  hba->SaveAs(Form("blast_max_ratio_vs_asymm%i.root",pol));
  delete hba;

  */ 

  delete hmax_rms; 
  delete hmax_ratio;
  delete hmax_ratio_reverse;
  delete hmin_signal; 
  delete h2;
  delete hblastandreverse;
  delete hblastandasymm; 
  
} //macro ends
