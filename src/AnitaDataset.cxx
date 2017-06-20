#include "AnitaDataset.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "Adu5Pat.h"
#include "PrettyAnitaHk.h"
#include "CalibratedAnitaEvent.h"
#include "TTreeIndex.h" 
#include "AnitaVersion.h" 
#include <math.h>
#include "TFile.h" 
#include "TTree.h" 
#include <stdlib.h>
#include <unistd.h>
#include "TurfRate.h" 
#include "TMath.h"
#include "SurfHk.h"
#include "TROOT.h"
#include "TEventList.h" 
#include "TCut.h" 
#include "TMutex.h" 
#include "TruthAnitaEvent.h" 
#include <dirent.h>
#include <algorithm>
#include "TEnv.h" 
#include <iostream>
#include <fstream>
#include "AnitaConventions.h"


static bool checkIfFileExists(const char * file)
{
  return access(file, R_OK) !=-1; 
}

static const char * checkIfFilesExist(int num, ...)
{

  va_list args; 
  va_start(args, num); 

  for (int i = 0; i < num; i++) 
  {
    const char * f = va_arg(args, const char *); 

    if (checkIfFileExists(f))
    {
      return f; 
    }

  }

  va_end(args); 

  return 0; 
}

static const char  anita_root_data_dir_env[]  = "ANITA_ROOT_DATA"; 
static const char  anita_versioned_root_data_dir_env[]  = "ANITA%d_ROOT_DATA"; 
static const char  mc_root_data_dir[] = "ANITA_MC_DATA"; 

const char * AnitaDataset::getDataDir(DataDirectory dir) 
{
  int version = (int) dir; 

  if (version > 0) 
  {

    char env_string[sizeof(anita_versioned_root_data_dir_env)+20]; 
    sprintf(env_string,anita_versioned_root_data_dir_env, version); 
    const char * tryme = getenv(env_string); 
    if (!tryme)
    {
      fprintf(stderr,"%s, not defined, will try %s\n",env_string, anita_root_data_dir_env); 
    }
    else return tryme; 
  }

  if (version == 0) 
  {
    const char * tryme = getenv(mc_root_data_dir); 
    if (!tryme)
    {
      fprintf(stderr,"%s, not defined, will try %s\n",mc_root_data_dir, anita_root_data_dir_env); 
    }
    else return tryme; 
  }

  if (const char * tryme = getenv(anita_root_data_dir_env))
  {
    return tryme; 
  }
  else
  {
      fprintf(stderr,"%s, not defined, please define it!", anita_root_data_dir_env); 
      return 0; 
  }
}


AnitaDataset::AnitaDataset(int run, bool decimated, WaveCalType::WaveCalType_t cal, DataDirectory version, BlindingStrategy strategy)
  : 
  fHeadTree(0), fHeader(0), 
  fEventTree(0), fCalEvent(0), fRawEvent(0), fUseful(0), 
  fGpsTree(0), fGps(0), 
  fHkTree(0), fHk(0),
  fTurfTree(0), fTurf(0),
  fSurfTree(0), fSurf(0),
  fTruthTree(0), fTruth(0), 
  fCutList(0), fRandy()
{
  setCalType(cal); 
  setStrategy(strategy); 
  loadedBlindTrees = false;
  zeroBlindPointers();
  loadBlindTrees();

  loadRun(run, decimated, version); 


}

void  AnitaDataset::unloadRun() 
{

  for (unsigned i = 0; i < filesToClose.size(); i++) 
  {
    filesToClose[i]->Close(); 
    delete filesToClose[i]; 
  }

  fHeadTree = 0; 
  fDecimatedHeadTree = 0; 
  fEventTree = 0; 
  fHkTree = 0; 
  fGpsTree = 0; 
  fTurfTree = 0; 
  fSurfTree = 0; 
  fRunLoaded = false;

  if (fCutList) 
  {
    delete fCutList; 
    fCutList = 0; 
  }

}


Adu5Pat * AnitaDataset::gps(bool force_load)
{

  if (fHaveGpsEvent)
  {
    if (fGpsTree->GetReadEntry() != fWantedEntry || force_load) 
    {
      fGpsTree->GetEntry(fWantedEntry); 
    }
  }
  else
  {
    if (fGpsDirty || force_load)
    {
      //try one that matches realtime
      int gpsEntry = fGpsTree->GetEntryNumberWithBestIndex(round(header()->triggerTime + header()->triggerTimeNs  / 1e9)); 
      int offset = 0; 
      fGpsTree->GetEntry(gpsEntry); 
      while (fGps->attFlag == 1 && abs(offset) < 30)
      {
        offset = offset >= 0 ? -(offset+1) : -offset; 
        if (gpsEntry + offset < 0) continue; 
        if (gpsEntry + offset >= fGpsTree->GetEntries()) continue; 
        fGpsTree->GetEntry(gpsEntry+offset); 
      }
      if (fGps->attFlag==1)
      {
        fprintf(stderr,"WARNING: Could not find good GPS within 30 entries... reverting to closest bad one\n"); 
        fGpsTree->GetEntry(gpsEntry); 
      }

      fGpsDirty = false; 
    }

  }

  return fGps; 

}

CalibratedAnitaEvent * AnitaDataset::calibrated(bool force_load) 
{
  if (fEventTree->GetReadEntry() != fWantedEntry || force_load) 
  {
    fEventTree->GetEntry(fWantedEntry); 
    fCalDirty = true; 
  }

  if (!fHaveCalibFile && fCalDirty)
  {
     if (!fCalEvent) 
     {
       fCalEvent = new CalibratedAnitaEvent; 
     }
     fCalEvent->~CalibratedAnitaEvent(); 
     fCalEvent = new (fCalEvent) CalibratedAnitaEvent(useful()); 
     fCalDirty = false; 
  }

  return fCalEvent; 
}


PrettyAnitaHk * AnitaDataset::hk(bool force_load) 
{
  if (fHkTree->GetReadEntry() != fWantedEntry || force_load) 
  {
    fHkTree->GetEntry(fWantedEntry); 
  }
  return fHk; 
}


RawAnitaHeader * AnitaDataset::header(bool force_load) 
{
  if (fDecimated)
  {
    if (force_load) 
    {
      fDecimatedHeadTree->GetEntry(fDecimatedEntry); 
    }
  }
  else if ((fHeadTree->GetReadEntry() != fWantedEntry || force_load)) 
  {
    fHeadTree->GetEntry(fWantedEntry); 
  }



  if(theStrat & kInsertedVPolEvents){
    Int_t fakeTreeEntry = needToOverwriteEvent(AnitaPol::kVertical, fHeader->eventNumber);
    if(fakeTreeEntry > -1){
      overwriteHeader(fHeader, AnitaPol::kVertical, fakeTreeEntry);
    }
  }


  if(theStrat & kInsertedHPolEvents){
    Int_t fakeTreeEntry = needToOverwriteEvent(AnitaPol::kHorizontal, fHeader->eventNumber);
    if(fakeTreeEntry > -1){
      overwriteHeader(fHeader, AnitaPol::kHorizontal, fakeTreeEntry);
    }
  }

  return fHeader; 
}


RawAnitaEvent * AnitaDataset::raw(bool force_load) 
{
  if (fEventTree->GetReadEntry() != fWantedEntry || force_load) 
  {
    fEventTree->GetEntry(fWantedEntry); 
  }
  return fHaveCalibFile ? fCalEvent : fRawEvent; 
}


UsefulAnitaEvent * AnitaDataset::useful(bool force_load) 
{

  if (fEventTree->GetReadEntry() != fWantedEntry || force_load) 
  {

    fEventTree->GetEntry(fWantedEntry); 
    fUsefulDirty = true; 
  }
  
  if (fUsefulDirty)
  {
    if (!fUseful) 
    {
      fUseful = new UsefulAnitaEvent; 
    }

    fUseful->~UsefulAnitaEvent();

    if (fHaveCalibFile)
    {
      new (fUseful) UsefulAnitaEvent(fCalEvent, fCalType); 
    }
    else if (fRawEvent)
    {
      new (fUseful) UsefulAnitaEvent(fRawEvent, fCalType, header()); 
    }
    fUsefulDirty = false; 
  }

  // This is the blinding implementation for the header

  if(theStrat & kInsertedVPolEvents){
    Int_t fakeTreeEntry = needToOverwriteEvent(AnitaPol::kVertical, fUseful->eventNumber);
    if(fakeTreeEntry > -1){
      overwriteEvent(fUseful, AnitaPol::kVertical, fakeTreeEntry);
    }
  }


  if(theStrat & kInsertedHPolEvents){
    Int_t fakeTreeEntry = needToOverwriteEvent(AnitaPol::kHorizontal, fUseful->eventNumber);
    if(fakeTreeEntry > -1){
      overwriteEvent(fUseful, AnitaPol::kHorizontal, fakeTreeEntry);
    }
  }


  if ((theStrat & kRandomizePolarity) && maybeInvertPolarity(fUseful->eventNumber))
  {
    // std::cerr << "Inverting event " << fUseful->eventNumber << std::endl;
    for(int surf=0; surf < NUM_SURF; surf++){
      for(int chan=0; chan < RFCHAN_PER_SURF; chan++){
        int chanIndex = surf*NUM_CHAN + chan;
        for(int samp=0; samp < NUM_SAMP; samp++){
          fUseful->fVolts[chanIndex][samp] *= -1;
          fUseful->data[chanIndex][samp] *= -1; // do the pedestal subtracted data too
        }
      }
    }
  }

  return fUseful; 
}

// Calling this function on it's own is just for unblinding, please use honestly
Bool_t AnitaDataset::maybeInvertPolarity(UInt_t eventNumber){
  // add additional check here for clarity, in case people call this function on it's own?
  if((theStrat & kRandomizePolarity) > 0){
    fRandy.SetSeed(eventNumber); // set seed from event number, makes this deterministic regardless of order events are processed
    Double_t aboveZeroFiftyPercentOfTheTime = fRandy.Uniform(-1, 1); // uniformly distributed random number between -1 and 1  
    return (aboveZeroFiftyPercentOfTheTime < 0);
  }
  return false;
}

int AnitaDataset::getEntry(int entryNumber)
{

  //invalidate the indices 
  fIndex = -1; 
  fCutIndex=-1; 
  
 
  if (entryNumber < 0 || entryNumber >= (fDecimated ? fDecimatedHeadTree : fHeadTree)->GetEntries())
  {
    fprintf(stderr,"Requested entry %d too big or small!\n", entryNumber); 
  }
  else
  {
    (fDecimated ? fDecimatedEntry : fWantedEntry) = entryNumber; 
    if (fDecimated)
    {
      fDecimatedHeadTree->GetEntry(fDecimatedEntry); 
      fWantedEntry = fHeadTree->GetEntryNumberWithIndex(fHeader->eventNumber); 

    }
    fUsefulDirty = true; 
    fCalDirty = true; 
    fGpsDirty = true; 
    fTurfDirty = true; 
    fSurfDirty = true; 
  }


  // use the header to set the ANITA version 
  AnitaVersion::setVersionFromUnixTime(header()->realTime); 

  return fDecimated ? fDecimatedEntry : fWantedEntry; 
}


int AnitaDataset::getEvent(int eventNumber)
{

  int entry  =  (fDecimated ? fDecimatedHeadTree : fHeadTree)->GetEntryNumberWithIndex(eventNumber); 
  if (entry < 0) 
  {
    fprintf(stderr,"WARNING: event %lld not found in header tree\n", fWantedEntry); 
    if (fDecimated) 
    {
      fprintf(stderr,"\tWe are using decimated tree, so maybe that's why?\n"); 

    }
  }
  else
  {
    getEntry(entry);
  }

  return fDecimated ? fDecimatedEntry : fWantedEntry; 
}
  
AnitaDataset::~AnitaDataset() 
{

  unloadRun(); 



  if (fHeader) 
    delete fHeader; 

  if (fCalEvent) 
    delete fCalEvent; 

  if (fUseful) 
  {
    delete fUseful; 
  }

  if (fRawEvent) 
    delete fRawEvent; 

  if (fGps) 
    delete fGps; 

  if (fHk) 
    delete fHk; 

  if (fTurf) 
    delete fTurf; 

  if (fSurf) 
    delete fSurf; 

  if (fTruth) 
    delete fTruth; 

  if (fCutList) 
    delete fCutList; 

}

bool  AnitaDataset::loadRun(int run, bool dec,  DataDirectory dir) 
{

  fDecimated = dec; 
  fIndices = 0; 

  unloadRun(); 
  fWantedEntry = 0; 

  const char * data_dir = getDataDir(dir); 

  //seems like a good idea 
  
  int version = (int) dir; 
  if (version>0) AnitaVersion::set(version); 

  //if decimated, try to load decimated tree

  if (fDecimated) 
  {

    fDecimatedEntry = 0; 
    TString fname = TString::Format("%s/run%d/decimatedHeadFile%d.root", data_dir, run, run); 
    if (checkIfFileExists(fname.Data()))
    {
      TFile * f = new TFile(fname.Data()); 
      filesToClose.push_back(f); 
      fDecimatedHeadTree = (TTree*) f->Get("headTree"); 
      fDecimatedHeadTree->BuildIndex("eventNumber"); 
      fDecimatedHeadTree->SetBranchAddress("header",&fHeader); 
      fIndices = ((TTreeIndex*) fDecimatedHeadTree->GetTreeIndex())->GetIndex(); 
    }
    else
    {
      fprintf(stderr," Could not find decimated head file for run %d, giving up!\n", run); 
      fRunLoaded = false;
      return false; 
    }
  }
  else
  {
    fDecimatedHeadTree = 0; 
  }
  // try to load timed header file 
  
  // For telemetered crap 
  TString fname0 = TString::Format("%s/run%d/eventHeadFile%d.root", data_dir, run, run); 
  TString fname1 = TString::Format("%s/run%d/timedHeadFile%d.root", data_dir, run, run); 
  TString fname2 = TString::Format("%s/run%d/headFile%d.root", data_dir, run, run); 
  TString fname3 = TString::Format("%s/run%d/SimulatedAnitaHeadFile%d.root", data_dir, run, run);

  bool simulated = false; 

  if (const char * the_right_file = checkIfFilesExist(4, fname0.Data(), fname1.Data(), fname2.Data(), fname3.Data()))
  {

    if (strcasestr(the_right_file,"SimulatedAnitaHeadFile")) simulated = true; 

    printf("Using head file: %s\n",the_right_file); 
    TFile * f = new TFile(the_right_file); 
    filesToClose.push_back(f); 
    fHeadTree = (TTree*) f->Get("headTree"); 
  }
  else 
  {
    fprintf(stderr,"Could not find head file for run %d, giving up!\n", run); 
    fRunLoaded = false;
    return false; 
  }

  if (!fDecimated) fHeadTree->SetBranchAddress("header",&fHeader); 

  fHeadTree->BuildIndex("eventNumber"); 

  if (!fDecimated) fIndices = ((TTreeIndex*) fHeadTree->GetTreeIndex())->GetIndex(); 

  //try to load gps event file  
  TString fname = TString::Format("%s/run%d/gpsEvent%d.root", data_dir, run, run);
  fname2 = TString::Format("%s/run%d/SimulatedAnitaGpsFile%d.root", data_dir, run, run); 
  if (const char * the_right_file = checkIfFilesExist(2,fname.Data(),fname2.Data()))
  {
     TFile * f = new TFile(the_right_file); 
     filesToClose.push_back(f); 
     fGpsTree = (TTree*) f->Get("adu5PatTree"); 
     fHaveGpsEvent = true; 

  }
  // load gps file instead
  else 
  {
    fname = TString::Format("%s/run%d/gpsFile%d.root", data_dir, run, run);
    if (const char * the_right_file = checkIfFilesExist(1, fname.Data()))
    {
       TFile * f = new TFile(the_right_file); 
       filesToClose.push_back(f); 
       fGpsTree = (TTree*) f->Get("adu5PatTree"); 
       fGpsTree->BuildIndex("realTime"); 
       fHaveGpsEvent = false; 
    }
    else 
    {
      fprintf(stderr,"Could not find gps file for run %d, giving up!\n",run); 
      fRunLoaded = false;
      return false; 
    }
  }

  fGpsTree->SetBranchAddress("pat",&fGps); 


  //try to load calibrated event file 

  fname = TString::Format("%s/run%d/calibratedEventFile%d.root", data_dir, run, run);
  fname2 = TString::Format("%s/run%d/calEventFile%d.root", data_dir, run, run); 
  fname3 = TString::Format("%s/run%d/SimulatedAnitaEventFile%d.root", data_dir, run, run); 
  if (const char * the_right_file = checkIfFilesExist(2, fname.Data(), fname2.Data()))
  {
     TFile * f = new TFile(the_right_file); 
     filesToClose.push_back(f); 
     fEventTree = (TTree*) f->Get("eventTree"); 
     fHaveCalibFile = true; 
     fEventTree->SetBranchAddress("event",&fCalEvent); 
  }
  else 
  {
    fname = TString::Format("%s/run%d/eventFile%d.root", data_dir, run, run); 
    if (checkIfFileExists(fname.Data()))
    {
       TFile * f = new TFile(fname.Data()); 
       filesToClose.push_back(f); 
       fEventTree = (TTree*) f->Get("eventTree"); 
       fHaveCalibFile = false; 
       fEventTree->SetBranchAddress("event",&fRawEvent); 
    }
    else 
    {
      if (checkIfFileExists(fname3.Data()))
      {
        TFile * f = new TFile(fname3.Data()); 
        filesToClose.push_back(f); 
        fEventTree = (TTree*) f->Get("eventTree"); 
        fHaveCalibFile = false;
        fEventTree->SetBranchAddress("event",&fUseful);
      } else {
        fprintf(stderr,"Could not find event file for run %d, giving up!\n",run); 
        fRunLoaded = false;
        return false;
      }
    } 
  }
  
  //try to load hk file 
  fname = TString::Format("%s/run%d/prettyHkFile%d.root", data_dir, run, run); 
  if (checkIfFileExists(fname.Data()))
  {
     TFile * f = new TFile(fname.Data()); 
     filesToClose.push_back(f); 
     fHkTree = (TTree*) f->Get("prettyHkTree"); 
     fHkTree->SetBranchAddress("hk",&fHk); 
  }
  else 
  {
    fHkTree = 0; 
    fprintf(stderr,"Could not find hk file for run %d, no HK will be available!\n",run); 
  }



  // try to load turf file 
  fname = TString::TString::Format("%s/run%d/turfRateFile%d.root",data_dir,run,run); 
  if (checkIfFileExists(fname.Data()))
  {

     TFile * f = new TFile(fname.Data()); 
     filesToClose.push_back(f); 
     fTurfTree = (TTree*) f->Get("turfRateTree"); 
     fTurfTree->BuildIndex("realTime"); 
     fTurfTree->SetBranchAddress("turf",&fTurf); 
  }
  else 
  {
    fTurfTree = 0; 
    fprintf(stderr,"Could not find TurfRate file for run %d, no Turf will be available!\n",run); 
  }

  // try to load surf file 
  fname = TString::TString::Format("%s/run%d/surfHkFile%d.root",data_dir,run,run);
  if (checkIfFileExists(fname.Data()))
  {

     TFile * f = new TFile(fname.Data()); 
     filesToClose.push_back(f); 
     fSurfTree = (TTree*) f->Get("surfHkTree"); 
     fSurfTree->BuildIndex("payloadTime","payloadTimeUs"); 
     fSurfTree->SetBranchAddress("surf",&fSurf); 
  }
  else 
  {
    fSurfTree = 0; 
    fprintf(stderr,"Could not find SurfHK file for run %d, no SURF will be available!\n",run); 
  }

  //try to load truth 
  if (simulated)
  {
    fname = TString::TString::Format("%s/run%d/SimulatedAnitaTruthFile%d.root",data_dir,run,run);
    if (checkIfFileExists(fname.Data()))
    {
     TFile * f = new TFile(fname.Data()); 
     filesToClose.push_back(f); 
     fTruthTree = (TTree*) f->Get("truthAnitaTree"); 
     fTruthTree->SetBranchAddress("truth",&fTruth); 
    }
  }

  //load the first entry 
  getEntry(0); 
  

  fRunLoaded = true;
  return true; 
}


int AnitaDataset::previousEvent() 
{
  if (fIndex < 0) 
  {
    fIndex = TMath::BinarySearch(N(), fIndices, fDecimated ? fDecimatedEntry : fWantedEntry); 
  }

  if (fIndex >0) 
    fIndex--; 

  return nthEvent(fIndex); 
}


int AnitaDataset::firstEvent()
{
  return nthEvent(0); 
}

int AnitaDataset::lastEvent()
{
  return nthEvent(N()-1); 
}

int AnitaDataset::nthEvent(int n)
{
  int ret = getEntry(fIndices[n]); 
  fIndex = n; 
  return ret; 
}



int AnitaDataset::nextEvent() 
{
  if (fIndex < 0) 
  {
    fIndex = TMath::BinarySearch(N(), fIndices, fDecimated ? fDecimatedEntry : fWantedEntry); 
  }

  if (fIndex < N() -1)
    fIndex++; 

  return nthEvent(fIndex); 
}

int AnitaDataset::N() const
{
  return (fDecimated? fDecimatedHeadTree : fHeadTree)->GetEntries(); 
}



int AnitaDataset::setCut(const TCut & cut)
{
  if (fCutList) 
  {
    delete fCutList; 
  }

  int n = (fDecimated? fDecimatedHeadTree : fHeadTree)->Draw(">>evlist1",cut,"goff"); 
  fCutList = (TEventList*) gDirectory->Get("evlist1");
  return n; 
}



int AnitaDataset::NInCut() const
{

  if (fCutList)
  {
    return fCutList->GetN(); 
  }

  return -1; 
}

int AnitaDataset::firstInCut() 
{
  return nthInCut(0); 
}

int AnitaDataset::lastInCut() 
{
  return nthInCut(NInCut()-1); 
}


int AnitaDataset::nthInCut(int i)
{
  if (!fCutList) 
    return -1; 
  int ret = getEntry(fCutList->GetEntry(i)); 

  fCutIndex = i;
  return ret;
}

int AnitaDataset::nextInCut()
{
  if (!fCutList) return -1; 
  if (fCutIndex < 0) 
  {
    fCutIndex = TMath::BinarySearch(NInCut(), fCutList->GetList(), (Long64_t) (fDecimated ? fDecimatedEntry : fWantedEntry)); 
  }

  if (fCutIndex <  NInCut() - 1) 
  {
    fCutIndex++; 
  }
  return nthInCut(fCutIndex); 

}

int AnitaDataset::previousInCut()
{
  if (!fCutList) return -1; 
  if (fCutIndex < 0) 
  {
    fCutIndex = TMath::BinarySearch(NInCut(), fCutList->GetList(),(Long64_t)  (fDecimated ? fDecimatedEntry : fWantedEntry)); 
  }

  if (fCutIndex >  0) 
  {
    fCutIndex--; 
  }
  return nthInCut(fCutIndex); 

}


TurfRate* AnitaDataset::turf(bool force_reload) 
{

  if (!fTurfTree) return 0; 

  if (fTurfDirty || force_reload) 
  {
    int entry = fTurfTree->GetEntryNumberWithBestIndex(round(header()->triggerTime + header()->triggerTimeNs /1e9 ) ); 
    fTurfTree->GetEntry(entry); 
    fTurfDirty = false; 
  }

  return fTurf; 
}

SurfHk* AnitaDataset::surf(bool force_reload) 
{

  if (!fSurfTree) return 0; 

  if (fSurfDirty || force_reload) 
  {
    int entry = fSurfTree->GetEntryNumberWithBestIndex(header()->payloadTime, header()->payloadTimeUs); 
    fSurfTree->GetEntry(entry); 
    fSurfDirty = false; 
  }

  return fSurf; 
}


TruthAnitaEvent * AnitaDataset::truth(bool force_reload) 
{

  if (!fTruthTree) return 0; 
  if (fTruthTree->GetReadEntry() != fWantedEntry || force_reload) 
  {
    fTruthTree->GetEntry(fWantedEntry); 
  }

  return fTruth; 
}


struct run_info
{

  double start_time; 
  double stop_time; 
  int run;

  bool operator< (const run_info & other) const 
  {
    return stop_time < other.stop_time; 
  }


}; 

static std::vector<run_info> run_times[NUM_ANITAS+1]; 

int AnitaDataset::getRunAtTime(double t)
{

  int version= AnitaVersion::getVersionFromUnixTime(t); 

  if (!run_times[version].size())
  {
    static TMutex m; 
    m.Lock(); 
    if (!run_times[version].size()) 
    {

      // load from cache
      bool found_cache = false; 


      TString cache_file1= TString::Format("%s/timerunmap_%d.txt", getenv("ANITA_CALIB_DIR"),version) ; 
      TString cache_file2= TString::Format("%s/share/anitaCalib/timerunmap_%d.txt", getenv("ANITA_UTIL_INSTALL_DIR"),version) ; 
      TString cache_file3= TString::Format("./calib/timerunmap_%d.txt",version); 

      const char * cache_file_name = checkIfFilesExist(3, cache_file1.Data(), cache_file2.Data(), cache_file3.Data()); 

      if (checkIfFileExists(cache_file_name))
      {

          found_cache = true; 
          FILE * cf = fopen(cache_file_name,"r"); 
          run_info r; 
          while(!feof(cf))
          {
            fscanf(cf,"%d %lf %lf\n", &r.run, &r.start_time, &r.stop_time); 
            run_times[version].push_back(r); 
          }
          fclose(cf); 
      }


      if (!found_cache) 
      {
        //temporarily suppress errors and disable recovery
        int old_level = gErrorIgnoreLevel;
        int recover = gEnv->GetValue("TFile.Recover",1); 
        gEnv->SetValue("TFile.Recover",1); 
        gErrorIgnoreLevel = kFatal; 

        const char * data_dir = getDataDir((DataDirectory)version); 
        fprintf(stderr,"Couldn't find run file map. Regenerating %s from header files in %s\n", cache_file_name,data_dir); 
        DIR * dir = opendir(data_dir); 

        while(struct dirent * ent = readdir(dir))
        {
          int run; 
          if (sscanf(ent->d_name,"run%d",&run))
          {

            TString fname1 = TString::Format("%s/run%d/timedHeadFile%d.root", data_dir, run, run); 
            TString fname2 = TString::Format("%s/run%d/headFile%d.root", data_dir, run, run); 

            if (const char * the_right_file = checkIfFilesExist(2, fname1.Data(), fname2.Data()))
            {
              TFile f(the_right_file); 
              TTree * t = (TTree*) f.Get("headTree"); 
              if (t) 
              {
                run_info  ri; 
                ri.run = run; 
                //TODO do this to nanosecond precision 
                ri.start_time= t->GetMinimum("triggerTime"); 
                ri.stop_time = t->GetMaximum("triggerTime") + 1; 
                run_times[version].push_back(ri); 
              }
            }
          }
        }

        gErrorIgnoreLevel = old_level; 
        gEnv->SetValue("TFile.Recover",recover); 
        std::sort(run_times[version].begin(), run_times[version].end()); 

        TString try2write;  
        try2write.Form("./calib/timerunmap_%d.txt",version); 
        FILE * cf = fopen(try2write.Data(),"w"); 

        if (cf) 
        {
          const std::vector<run_info> &  v = run_times[version]; 
          for (unsigned i = 0; i < v.size(); i++)
          {
              printf("%d %0.9f %0.9f\n", v[i].run, v[i].start_time, v[i].stop_time); 
              fprintf(cf,"%d %0.9f %0.9f\n", v[i].run, v[i].start_time, v[i].stop_time); 
          }

         fclose(cf); 
        }

      }
    }
    m.UnLock(); 
  }
  
  run_info test; 
  test.start_time =t; 
  test.stop_time =t; 
  const std::vector<run_info> & v = run_times[version]; 
  std::vector<run_info>::const_iterator it = std::upper_bound(v.begin(), v.end(), test); 

  if (it == v.end()) return -1; 
  if (it == v.begin() && (*it).start_time >t) return -1; 
  return (*it).run; 
}

void AnitaDataset::zeroBlindPointers(){
  loadedBlindTrees = false;

  for(int pol=0; pol < AnitaPol::kNotAPol; pol++){
    fBlindHeadTree[pol] = NULL;
    fBlindEventTree[pol] = NULL;
    fBlindHeader[pol] = NULL;
    fBlindEvent[pol] = NULL;
  }

  fBlindFile = NULL;
}

TString AnitaDataset::getDescription(BlindingStrategy strat){

  TString description = "Current strategy: ";

  if(strat == kNoBlinding){
    description = "No blinding. ";
  }

  if(strat & kInsertedVPolEvents){
    description += "VPol events inserted. ";
  }

  if(strat & kInsertedHPolEvents){
    description += "HPol events inserted. ";
  }

  if(strat & kRandomizePolarity){
    description += "Polarity randomized. ";
  }


  return description;
}

AnitaDataset::BlindingStrategy AnitaDataset::setStrategy(BlindingStrategy newStrat){
  theStrat = newStrat;
  return theStrat;
}


AnitaDataset::BlindingStrategy AnitaDataset::getStrategy(){
  return theStrat;
}

void AnitaDataset::loadBlindTrees() {

  // std::cout << __PRETTY_FUNCTION__ << std::endl;

  /* for now, just A3 */ 
  if(!loadedBlindTrees && AnitaVersion::get()==3){

    // prepare to put ROOT's current directory pointer back to what it was before we opened the blinding file in read mode.
    // TDirectory* originalDir = gDirectory;


    // std::cout << __PRETTY_FUNCTION__ << ": here 1" << std::endl;
    // gDirectory->pwd();
    // gDirectory->ls();

    fBlindFile = NULL;

    char calibDir[FILENAME_MAX];
    char fileName[FILENAME_MAX];
    char *calibEnv=getenv("ANITA_CALIB_DIR");
    if(!calibEnv) {
      char *utilEnv=getenv("ANITA_UTIL_INSTALL_DIR");
      if(!utilEnv){
	sprintf(calibDir,"calib");
      }
      else{
	sprintf(calibDir,"%s/share/anitaCalib",utilEnv);
      }
    }
    else {
      strncpy(calibDir,calibEnv,FILENAME_MAX);
    }

    // std::cout << __PRETTY_FUNCTION__ << ": here 2" << std::endl;


    // these are the fake events, that will be inserted in place of some min bias events
    sprintf(fileName, "%s/insertedDataFile.root", calibDir);
    fBlindFile = TFile::Open(fileName);


    if(fBlindFile){

      TString polPrefix[AnitaPol::kNotAPol];
      polPrefix[AnitaPol::kHorizontal] = "HPol";
      polPrefix[AnitaPol::kVertical] = "VPol";

      for(int pol=0; pol < AnitaPol::kNotAPol; pol++){

	TString headTreeName = polPrefix[pol] + "HeadTree";
	fBlindHeadTree[pol] = (TTree*) fBlindFile->Get(headTreeName);

	TString eventTreeName = polPrefix[pol] + "EventTree";
	fBlindEventTree[pol] = (TTree*) fBlindFile->Get(eventTreeName);

	// If you found the data then prepare for data reading
	if(fBlindHeadTree[pol] && fBlindEventTree[pol]){

	  fBlindHeadTree[pol]->SetBranchAddress("header", &fBlindHeader[pol]);
	  fBlindEventTree[pol]->SetBranchAddress("event", &fBlindEvent[pol]);


	  // for(Long64_t entry=0; entry < fBlindHeadTree[pol]->GetEntries(); entry++){
	  //   // std::cerr << entry << "\t";
	  //   fBlindHeadTree[pol]->GetEntry(entry);
	  //   std::cerr << entry << "\t";
	  //   fBlindEventTree[pol]->GetEntry(entry);
	  //   std::cerr << entry << std::endl;
	  // }
	}
	else{
	  // complain if you can't find the data
	  std::cerr << "Error in " << __PRETTY_FUNCTION__ << ": "
		    << "fBlindHeadTree[" << pol << "] = " << fBlindHeadTree[pol] << ", "
		    << "fBlindEventTree[" << pol << "] = " << fBlindEventTree[pol] << std::endl;
	}
      }
    }
    else{
      std::cerr << "Error in " << __PRETTY_FUNCTION__ << ": "
		<< "Unable to find " << fileName << " for inserted event blinding." << std::endl;
    }

    // std::cout << __PRETTY_FUNCTION__ << ": here 3" << std::endl;

    // these are the min bias event numbers to be overwritten, with the entry in the fakeEventTree
    // that is used to overwrite the event
    sprintf(fileName,"%s/anita%dOverwrittenEventInfo.txt",calibDir, AnitaVersion::get());
    std::ifstream overwrittenEventInfoFile(fileName);
    char firstLine[180];
    overwrittenEventInfoFile.getline(firstLine,179);
    UInt_t overwrittenEventNumber;
    Int_t fakeTreeEntry, pol;
    Int_t numEvents = 0;
    while(overwrittenEventInfoFile >> overwrittenEventNumber >> fakeTreeEntry >> pol){
      eventsToOverwrite.push_back(overwrittenEventNumber);
      fakeTreeEntries.push_back(fakeTreeEntry);
      AnitaPol::AnitaPol_t thePol = AnitaPol::AnitaPol_t(pol);
      polarityOfEventToInsert.push_back(thePol);
      // std::cout << overwrittenEventNumber << "\t" << fakeTreeEntry << "\t" << thePol << std::endl;
      numEvents++;
    }
    if(numEvents==0){
      std::cerr << "Warning in " << __FILE__ << std::endl;
      std::cerr << "Unable to find overwrittenEventInfo" << std::endl;
    }

    // put ROOT's current directory pointer back to what it was before we opened the blinding file in read mode.

    fBlindFile->Close(); 
    delete fBlindFile; 
    loadedBlindTrees = true;

  }

//   gROOT->cd(0); 
  // std::cout << __PRETTY_FUNCTION__ << ": here 4" << std::endl;

}

/**
 * Loop through list of events to overwrite for a given polarisation and return the fakeTreeEntry we need to overwrite
 *
 * @param pol is the polarity to consider blinding
 * @param eventNumber is the eventNumber, obviously
 *
 * @return -1 if we don't overwrite, the entry in the fakeTree otherwise
 */
Int_t AnitaDataset::needToOverwriteEvent(AnitaPol::AnitaPol_t pol, UInt_t eventNumber){

  Int_t fakeTreeEntry = -1;
  for(UInt_t i=0; i < polarityOfEventToInsert.size(); i++){
    if(polarityOfEventToInsert.at(i)==pol && eventNumber == eventsToOverwrite.at(i)){
      fakeTreeEntry = fakeTreeEntries.at(i);
      break;
    }
  }
  return fakeTreeEntry;
}

void AnitaDataset::overwriteHeader(RawAnitaHeader* header, AnitaPol::AnitaPol_t pol, Int_t fakeTreeEntry){

  // std::cerr << pol << "\t" << fBlindHeadTree[pol] << "\t" << fakeTreeEntry << std::endl;
  // fBlindHeadTree[pol]->GetEntry(fakeTreeEntry);
  // std::cerr << pol << "\t" << fBlindHeadTree[pol] << "\t" << fakeTreeEntry << std::endl;

  // std::cout << __PRETTY_FUNCTION__ << std::endl;
  // std::cout << header << "\t" << fBlindHeader[pol] << "\t" << fBlindHeadTree[pol] << "\t" << fakeTreeEntry << std::endl;
  fBlindHeadTree[pol]->GetEntry(fakeTreeEntry);
  // std::cout << header << "\t" << fBlindHeader[pol] << "\t" << fBlindHeadTree[pol] << "\t" << fakeTreeEntry << std::endl;

  // Retain some of the header data for camouflage
  UInt_t realTime = header->realTime;
  UInt_t triggerTimeNs = header->triggerTimeNs;
  UInt_t eventNumber = header->eventNumber;
  Int_t run = header->run;
  Int_t trigNum = header->trigNum;
  UInt_t turfId = header->turfEventId;

  (*header) = (*fBlindHeader[pol]);

  header->realTime = realTime;
  header->triggerTimeNs = triggerTimeNs;
  header->eventNumber = eventNumber;
  header->run = run;
  header->trigNum = trigNum;
  header->turfEventId = turfId;

}

void AnitaDataset::overwriteEvent(UsefulAnitaEvent* useful, AnitaPol::AnitaPol_t pol, Int_t fakeTreeEntry){

  // std::cout << __PRETTY_FUNCTION__ << std::endl;
  // std::cout << useful << "\t" << fBlindEvent[pol] << "\t" << fBlindEventTree[pol] << "\t" << fakeTreeEntry << std::endl;
  fBlindEventTree[pol]->GetEntry(fakeTreeEntry);
  // std::cout << useful << "\t" << fBlindEvent[pol] << "\t" << fBlindEventTree[pol] << "\t" << fakeTreeEntry << std::endl;


  UInt_t eventNumber = useful->eventNumber;
  // std::cout << std::endl << std::endl;
  // std::cout << eventNumber << "\t" << fBlindEvent[pol]->eventNumber << std::endl;
  // std::cout << useful << "\t" << pol << std::endl;
  // std::cout << (useful->chipIdFlag[0] & 0x3) << "\t" << (fBlindEvent[pol]->chipIdFlag[0]&0x3) << std::endl;

  UInt_t surfEventIds[NUM_SURF] = {0};
  UChar_t wrongLabs[NUM_SURF*NUM_CHAN] = {0};
  UChar_t rightLabs[NUM_SURF*NUM_CHAN] = {0};
  for(int surf=0; surf < NUM_SURF; surf++){
    surfEventIds[surf] = useful->surfEventId[surf];

    for(int chan=0; chan < NUM_CHAN; chan++){
      const int chanIndex = surf*NUM_CHAN + chan;
      wrongLabs[chanIndex] = UChar_t(fBlindEvent[pol]->getLabChip(chanIndex));
      rightLabs[chanIndex] = UChar_t(useful->getLabChip(chanIndex));
      // std::cout << chanIndex << "\t" << chipIdFlags[chanIndex] << "\t" << (chipIdFlags[chanIndex] & 0x3) << std::endl;
    }
  }

  (*useful) = (*fBlindEvent[pol]);
  // for(int chanIndex=0; chanIndex < NUM_CHAN*NUM_SURF; chanIndex++){
  //   useful->fNumPoints[chanIndex] = fBlindEvent[pol]->fNumPoints[chanIndex];
  //   for(int samp=0; samp < NUM_SAMP; samp++){
  //     useful->fVolts[chanIndex][samp] = fBlindEvent[pol]->fVolts[chanIndex][samp];
  //     useful->fTimes[chanIndex][samp] = fBlindEvent[pol]->fTimes[chanIndex][samp];
  //   }
  // }



  useful->eventNumber = eventNumber;
  for(int surf=0; surf < NUM_SURF; surf++){
    useful->surfEventId[surf] = surfEventIds[surf];

    // here we manually set the bits in the chipId flag that correspond to the lab chip
    // this ensures that as you click through magic display the LABS will still go A->B->C->D->A...
    for(int chan=0; chan < NUM_CHAN; chan++){

      const int chanIndex = surf*NUM_CHAN + chan;
      // std::cerr << useful->chipIdFlag[chanIndex] << "\t";
      useful->chipIdFlag[chanIndex] -= wrongLabs[chanIndex];
      // std::cerr << useful->chipIdFlag[chanIndex] << "\t";
      useful->chipIdFlag[chanIndex] += rightLabs[chanIndex];
      // std::cerr << useful->chipIdFlag[chanIndex] << std::endl;


    }
  }

}







