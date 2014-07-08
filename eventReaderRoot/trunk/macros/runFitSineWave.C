

void runFitSineWave() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  //  gSystem->AddIncludePath("-I${EVENT_READER_DIR}");
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  //  cout << gSystem->GetIncludePath() <<endl;

  gSystem->Load("libMathMore.so");
  gSystem->Load("libPhysics.so");
  gSystem->Load("libfftw3.so");
  gSystem->Load("libRootFftwWrapper.so");		  
  gSystem->Load("libAnitaEvent.so");

  gSystem->CompileMacro("fitSineWaves.C","k");
  fitSineWaves("/home/rjn/anita/data/palestine/root/",3871,0,1000);
}
