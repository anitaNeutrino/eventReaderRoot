#include "AnitaDataset.h"
#include "UsefulAnitaEvent.h"
#include "RawAnitaHeader.h"
#include "Adu5Pat.h"
#include "PrettyAnitaHk.h"
#include "CalibratedAnitaEvent.h"
#include "TFile.h" 
#include "TTree.h" 
#include <stdlib.h>

static bool checkIfFileExists(const char * file)
{
  return access(file, R_OK) !=-1; 
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
  fHkTree(0), fHk(0) 
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
      int gpsEntry = fGpsTree->GetEntryNumberWithBestIndex(header()->realTime); 
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
    else 
    {
      new (fUseful) UsefulAnitaEvent(fRawEvent, fCalType, header()); 
    }
    fUsefulDirty = false; 
  }

  return fUseful; 

}

int AnitaDataset::getEntry(int entryNumber)
{

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
  }
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
}

bool  AnitaDataset::loadRun(int run, bool dec,  int version) 
{

  fDecimated = dec; 

  unloadRun(); 
  fWantedEntry = 0; 

  const char * data_dir = getDataDir(version); 

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
  
  TString fname = TString::Format("%s/run%d/timedHeadFile%d.root", data_dir, run, run); 
  if (checkIfFileExists(fname.Data()))
  {
    TFile * f = new TFile(fname.Data()); 
    filesToClose.push_back(f); 
    fHeadTree = (TTree*) f->Get("headTree"); 
  }
  else 
  {
    fname = TString::Format("%s/run%d/headFile%d.root", data_dir, run, run); 
    if (checkIfFileExists(fname.Data()))
    {
      TFile * f = new TFile(fname.Data()); 
      filesToClose.push_back(f); 
      fHeadTree = (TTree*) f->Get("headTree"); 
    }
    else 
    {
      fprintf(stderr,"Could not find head file for run %d, giving up!\n", run); 
      return false; 
    }
  }

  if (!fDecimated) fHeadTree->SetBranchAddress("header",&fHeader); 
  fHeadTree->BuildIndex("eventNumber"); 

  //try to load gps event file  
  fname = TString::Format("%s/run%d/gpsEvent%d.root", data_dir, run, run); 
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
    if (checkIfFileExists(fname.Data()))
    {
       TFile * f = new TFile(fname.Data()); 
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
  if (checkIfFileExists(fname.Data()))
  {
     TFile * f = new TFile(fname.Data()); 
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
      fprintf(stderr,"Could not find event file for run %d, giving up!\n",run); 
      return false; 
    }
  }


  //try to load hk file 
  fname = TString::Format("%s/run%d/prettyHkFile%d.root", data_dir, run, run); 
  if (checkIfFileExists(fname.Data()))
  {
     TFile * f = new TFile(fname.Data()); 
     filesToClose.push_back(f); 
     fHkTree = (TTree*) f->Get("prettyHkTree"); 
  }
  else 
  {
    fprintf(stderr,"Could not find hk file for run %d, giving up!\n",run); 
    return false; 
  }

  fHkTree->SetBranchAddress("hk",&fHk); 
  getEntry(0); 
  return true; 
}

int AnitaDataset::N() const
{
  return (fDecimated? fDecimatedHeadTree : fHeadTree)->GetEntries(); 
}
