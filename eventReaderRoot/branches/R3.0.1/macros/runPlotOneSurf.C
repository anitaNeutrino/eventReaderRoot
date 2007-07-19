

void runPlotOneSurf() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${EVENT_READER_DIR}");
  //  cout << gSystem->GetIncludePath() <<endl;
			  
  gSystem->Load("libAnitaEvent.so");
  gSystem->CompileMacro("plotOneSurfCompile.C","k");
  plotOneSurf(1028,1000,1,1);
}
