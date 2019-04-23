void testTruthTree() 
{

  TFile out("truthtest.root","RECREATE"); 

  TTree * t = new TTree("truth","truth"); 
  TruthAnitaEvent * ev  = new TruthAnitaEvent; 
  t->Branch("truth",&ev); 
  for (int i = 0; i < 1000; i++) 
  {
    TString tmp; 
    tmp.Form("s_%d",i); 
    ev->objName = tmp; 
    t->Fill(); 
  }

  t->Write();

  delete ev; 
}
