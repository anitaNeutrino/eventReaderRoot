

void runFitSquareWave() {
  gSystem->AddIncludePath("/tmp/eventReaderRoot/trunk/");
  gSystem->Load("libAnitaEvent.so");
  gSystem->CompileMacro("fitSquareWave.C","k");
  fitSquareWave();
}
