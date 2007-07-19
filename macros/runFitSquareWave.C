

void runFitSquareWave() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${EVENT_READER_DIR}");
  cout << gSystem->GetIncludePath() <<endl;
			  
  gSystem->Load("libAnitaEvent.so");
  gSystem->CompileMacro("fitSquareWave.C","k");
  fitSquareWave();
}
