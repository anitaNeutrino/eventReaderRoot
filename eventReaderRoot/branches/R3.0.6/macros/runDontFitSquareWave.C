

void runDontFitSquareWave() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${EVENT_READER_DIR}");
  //  cout << gSystem->GetIncludePath() <<endl;
			  
  gSystem->Load("libAnitaEvent.so");
  gSystem->CompileMacro("dontFitSquareWave.C","k");
  dontFitSquareWave(1028,0,100000);
}
