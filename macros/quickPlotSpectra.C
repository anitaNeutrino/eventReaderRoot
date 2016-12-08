#include "GpuPowerSpectra.h" 
void quickPlotSpectra(const char * gpuFile, bool backwards = false, int nplot = 20)
{ 

  TFile f(gpuFile); 

  TTree * gpuTree = (TTree*) f.Get("gpuTree"); 

  GpuPowerSpectra * gpu = 0; 
  gpuTree->SetBranchAddress("gpu",&gpu); 


  TCanvas * c = new TCanvas; 

  c->Divide(4,4); 


  for (int i = 0; i < nplot; i++) 
  {

    gpuTree->GetEntry(backwards ? gpuTree->GetEntries() - i - 1 : i); 

    for (int j = 0; j < 16; j++) 
    {

      c->cd(j+1); 
      TGraph * g = gpu->getGraph(j); 
      g->SetLineColor(i+1); 
      g->SetMarkerColor(i+1); 

      g->SetTitle(TString::Format("Phi Sector %d",j+1)); 
      g->Draw(i == 0 ? "alp" : "lpsame"); 
      g->GetYaxis()->SetRangeUser(5,35); 
    }

  }

}
