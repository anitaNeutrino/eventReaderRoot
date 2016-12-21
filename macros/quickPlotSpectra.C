#include "GpuPowerSpectra.h"

void quickPlotSpectra(const char * gpuFile, bool backwards = false, int nplot = 20)
{ 
  
  TFile f(gpuFile); 

  TTree * gpuTree = (TTree*) f.Get("gpuTree"); 

  GpuPowerSpectra * gpu = 0; 
  gpuTree->SetBranchAddress("gpu",&gpu); 

  TCanvas * c = new TCanvas("c","c",1920,1080); 
  c->Divide(4,4); 

  if (nplot < 0 || nplot > gpuTree->GetEntries()) nplot = gpuTree->GetEntries();
  for (int i = 0; i < nplot; i++) 
    {

      gpuTree->GetEntry(backwards ? gpuTree->GetEntries() - i - 1 : i); 

      for (int j = 0; j < 16; j++) 
	{
	  c->cd(j+1);
      
	  TGraph * g = gpu->getGraph(j);
            
	  g->SetLineColor(3*i+3);
	  g->SetMarkerColor(3*i+3);
	  
	  g->SetTitle(TString::Format("Phi Sector %d",j+1));
	  g->Draw(i == 0 ? "alp" : "lpsame");
	  g->GetYaxis()->SetLabelSize(0.06);
	  g->GetYaxis()->SetRangeUser(5,35);
	  //g->GetXaxis()->SetTitle("Frequency (MHz)");
	  g->GetXaxis()->SetTitleSize(0.045);
	  g->GetXaxis()->SetLabelOffset(0.01);
	  g->GetXaxis()->SetTitleOffset(1.1);
	  g->GetXaxis()->SetLabelSize(0.06);
      
	}

    }

  TString pngFile = TString::Format("%s_Spec.png",gpuFile);
  c->Print(pngFile,"png");
  
}
