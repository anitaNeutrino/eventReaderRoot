

void runClockPlotter() {
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  //  cout << gSystem->GetIncludePath() <<endl;

  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");
  gSystem->Load("libfftw3.so");
  gSystem->Load("libRootFftwWrapper.so");		  
  gSystem->Load("libAnitaEvent.so"); 

  gSystem->CompileMacro("plotAllClocks.C","k");
  plotAllClocks("root://anita@192.168.10.100///TBdata/anita/antarctica08/root",185,4,10);
}
