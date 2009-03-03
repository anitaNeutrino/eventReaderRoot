

void runGetEvent() {
  //  gSystem->AddIncludePath(gSystem->ExpandPathName("-I${EVENT_READER_DIR}"));
  gSystem->AddIncludePath("-I${EVENT_READER_DIR}");
  //  cout << gSystem->GetIncludePath() <<endl;
			  
  gSystem->Load("libAnitaEvent.so");
  gSystem->CompileMacro("justGetThisEvent.C","k");

  //  int thisEvent=5585519;
  //  int thisRun=1055;
  int thisEvent=7528263;
  int thisRun=1164;
  //  int thisEvent=7962477;
  //  int thisEvent=7962483;
  //    int thisEvent=7962490;
  //  int thisEvent=7962522;
  //  int thisEvent=7962564;
  //  int thisEvent=7962586;
  //  int thisEvent=7962742;
  //  int thisRun=1168;
  //  int thisRun=-1;
//   char headName[180];
//   FileStat_t staty;
//   for(int run=1164;run<=1239;run++) {
//     sprintf(headName,"/unix/anita1/newRootData/run%d/timedHeadFile%d.root",run,run);
//     if(gSystem->GetPathInfo(headName,staty)) {
//       continue;
//     }
//     TFile fpHead(headName);
//     TTree *headTree = (TTree*) fpHead.Get("headTree");
//     Double_t minEvent=headTree->GetMinimum("eventNumber");
//     Double_t maxEvent=headTree->GetMaximum("eventNumber");
//     if(thisEvent<minEvent || thisEvent>maxEvent)
//       continue;
//     thisRun=run;
//     break;
//   } 

  cout << thisRun << "\t" << thisEvent << endl;
  justGetThisEvent(thisRun,thisEvent);
  //  justGetThisEvent(1239,8197109);
  //  justGetThisEvent(1164,7528263);
}
