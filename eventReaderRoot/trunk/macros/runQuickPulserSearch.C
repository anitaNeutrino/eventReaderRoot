

void runQuickPulserSearch() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  //  gSystem->AddIncludePath("-I${EVENT_READER_DIR}");
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  //  cout << gSystem->GetIncludePath() <<endl;

  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");
  gSystem->Load("/sw/lib/libfftw3.so");
  gSystem->Load("libRootFftwWrapper.so");		  
  gSystem->Load("libAnitaEvent.so");

  gSystem->CompileMacro("quickPulserSearch.C","k");
  //  quickPulserSearch();
  quickPulserSearch("root://anita@192.168.2.103//data/anita/palestine08/root",3799,19,0,1000000);
}
