#include "AnitaDataset.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "Adu5Pat.h"
#include "PrettyAnitaHk.h"
#include "CalibratedAnitaEvent.h"
#include "AnitaVersion.h" 
#include <math.h>
#include "TFile.h" 
#include "TTree.h" 
#include <stdlib.h>
#include <unistd.h>
#include "TurfRate.h" 
#include "TMath.h"
#include "SurfHk.h"
#include "TEventList.h" 
#include "TCut.h" 

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

const char * AnitaDataset::getDataDir(int version) 
{

  if (version > 1) 
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


AnitaDataset::AnitaDataset(int run, bool decimated, WaveCalType::WaveCalType_t cal, int version)
  : 
  fHeadTree(0), fHeader(0), 
  fEventTree(0), fCalEvent(0), fRawEvent(0), fUseful(0), 
  fGpsTree(0), fGps(0), 
  fHkTree(0), fHk(0),
  fTurfTree(0), fTurf(0),
  fSurfTree(0), fSurf(0),
  fCutList(0)
{
  setCalType(cal); 
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
       fCalEvent = (CalibratedAnitaEvent*) malloc(sizeof(CalibratedAnitaEvent)); 
     }
     else
     {
       fCalEvent->~CalibratedAnitaEvent(); 
     }

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
      fUseful = (UsefulAnitaEvent *) malloc(sizeof(UsefulAnitaEvent)); 
    }
    else
    {
     fUseful->~UsefulAnitaEvent();
    }

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

  return fUseful; 

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
    fprintf(stderr,"WARNING: event %d not found in header tree\n", fWantedEntry); 
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
    fUseful->~UsefulAnitaEvent(); 
    free(fUseful); 
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


  if (fCutList) 
    delete fCutList; 

}

bool  AnitaDataset::loadRun(int run, bool dec,  int version) 
{

  fDecimated = dec; 

  unloadRun(); 
  fWantedEntry = 0; 

  const char * data_dir = getDataDir(version); 


  //seems like a good idea 
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
      fIndices = fDecimatedHeadTree->GetIndex(); 
    }
    else
    {
      fprintf(stderr," Could not find decimated head file for run %d, giving up!\n", run); 
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

  if (const char * the_right_file = checkIfFilesExist(4, fname0.Data(), fname1.Data(), fname2.Data(), fname3.Data()))
  {
    TFile * f = new TFile(the_right_file); 
    filesToClose.push_back(f); 
    fHeadTree = (TTree*) f->Get("headTree"); 
  }
  else 
  {
    fprintf(stderr,"Could not find head file for run %d, giving up!\n", run); 
    return false; 
  }

  if (!fDecimated) fHeadTree->SetBranchAddress("header",&fHeader); 

  fHeadTree->BuildIndex("eventNumber"); 

  if (!fDecimated) fIndices = fHeadTree->GetIndex(); 

  //try to load gps event file  
  TString fname = TString::Format("%s/run%d/gpsEvent%d.root", data_dir, run, run); 
  if (checkIfFileExists(fname.Data()))
  {
     TFile * f = new TFile(fname.Data()); 
     filesToClose.push_back(f); 
     fGpsTree = (TTree*) f->Get("adu5PatTree"); 
     fHaveGpsEvent = true; 

  }
  // load gps file instead
  else 
  {
    fname = TString::Format("%s/run%d/gpsFile%d.root", data_dir, run, run); 
    fname2 = TString::Format("%s/run%d/SimulatedAnitaGpsFile%d.root", data_dir, run, run); 
    if (const char * the_right_file = checkIfFilesExist(2, fname.Data(), fname2.Data()))
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

  // try to load tusf file 
  fname = TString::TString::Format("%s/run%d/SurfHkFile%d.root",data_dir,run,run); 
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


  //load the first entry 
  getEntry(0); 
  

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

