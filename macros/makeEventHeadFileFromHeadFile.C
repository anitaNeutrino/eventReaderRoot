void makeEventHeadFileFromHeadFile(int run, const char * dir = 0) 
{

  if (!dir) 
    dir = getenv("ANITA_ROOT_DATA"); 

  RawAnitaEvent * ev = 0; 
  RawAnitaHeader * hd = 0; 

  TFile headFile(TString::Format("%s/run%d/headFile%d.root",dir,run,run)); 
  TTree * headTree = (TTree*) headFile.Get("headTree"); 
  headTree->BuildIndex("eventNumber"); 
  headTree->SetBranchAddress("header",&hd); 

  TFile eventFile(TString::Format("%s/run%d/eventFile%d.root",dir,run,run)); 
  TTree * eventTree = (TTree*) eventFile.Get("eventTree"); 
  eventTree->SetBranchAddress("event",&ev); 

  TFile eventHeadFile(TString::Format("%s/run%d/eventHeadFile%d.root",dir,run,run),"RECREATE"); 
  TTree * eventHeadTree = new TTree("headTree","event Head tree"); 
  eventHeadTree->Branch("header",&hd); 


  for (int i = 0; i < eventTree->GetEntries(); i++) 
  {
    eventTree->GetEntry(i); 
    headTree->GetEntryWithIndex(ev->eventNumber); 
    eventHeadFile.cd(); 
    eventHeadTree->Fill(); 
  }

  eventHeadFile.Write(); 




}
