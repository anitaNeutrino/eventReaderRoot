

void runQuickZcPlot() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  //  gSystem->AddIncludePath("-I${EVENT_READER_DIR}");
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  //  cout << gSystem->GetIncludePath() <<endl;

  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");
  gSystem->Load("libfftw3.so");
  gSystem->Load("libRootFftwWrapper.so");		  
  gSystem->Load("libAnitaEvent.so");

  gSystem->CompileMacro("quickZCPlot.C","k");
  //  quickPulserSearch();
  quickZCPlot("/unix/anita2/palestine08/root/",3451,0,10000000);
}
