

void runPlotTestSurf() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${EVENT_READER_DIR}");
  //  cout << gSystem->GetIncludePath() <<endl;
			  
  gSystem->Load("libAnitaEvent.so");
  gSystem->CompileMacro("plotOneSurfForTesting.C","k");
  //  plotOneSurf(2150,0,1,0);
//   PrintEPSandPNG(canSurf,"run2113/oldOrderSurf1");
//    plotOneSurf(2113,0,1,1);
//    PrintEPSandPNG(canSurf,"run2113/oldOrderSurf2");
//    plotOneSurf(2113,0,1,2);
//    PrintEPSandPNG(canSurf,"run2113/oldOrderSurf3");
//    plotOneSurf(2113,0,1,3);
//    PrintEPSandPNG(canSurf,"run2113/oldOrderSurf4");
//    plotOneSurf(2113,0,1,4);
//    PrintEPSandPNG(canSurf,"run2113/oldOrderSurf5");
//    plotOneSurf(2113,0,1,5);
//    PrintEPSandPNG(canSurf,"run2113/oldOrderSurf6");
//    plotOneSurf(2113,0,1,6);
//    PrintEPSandPNG(canSurf,"run2113/oldOrderSurf7");
//    plotOneSurf(2113,0,1,7);
//    PrintEPSandPNG(canSurf,"run2113/oldOrderSurf8");
//    plotOneSurf(2113,0,1,8);
//    PrintEPSandPNG(canSurf,"run2113/oldOrderSurf9");
//    plotOneSurf(2113,0,1,9);
//    PrintEPSandPNG(canSurf,"run2113/oldOrderSurf10");

}
