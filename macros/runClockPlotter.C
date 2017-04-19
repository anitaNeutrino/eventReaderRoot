

void runClockPlotter() {
  gSystem->AddIncludePath("-I${ANITA_UTIL_INSTALL_DIR}/include");
  gSystem->Load("libMathMore.dylib");
  gSystem->Load("libPhysics.dylib");
  gSystem->Load("libfftw3.dylib");
  gSystem->Load("libRootFftwWrapper.dylib");		  
  gSystem->Load("libAnitaEvent.dylib"); 
  gSystem->CompileMacro("plotAllClocks.C","k");
  // gSystem->CompileMacro("getClockPeriod.C","k");

  // plotAllClocks("/Users/sylarcp/UCL/ANITA/flight1617/root",292,0,10);
}
