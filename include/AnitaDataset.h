#ifndef ANITA_DATASET_H
#define ANITA_DATASET_H

/** The AnitaDataset class simplifies reading of data (it loads all the trees automagically, if 
 * it can, with some fallbacks). 
 *
 *
 * In particular: 
 *
 *  -it prefers calibrated event files , but can fall back to event files
 *  -it prefers gpsEvent files, but can fall back to reading (only ADU5A) from gpsFiles
 *  -it prefers timedHeader files, but can fall back to reading header files 
 *  -it also supports reading decimated files, but also requires the full header file (to get the correct entry in the other trees). 
 *
 * 
 *
 *
 *  Cosmin Deaconu <cozzyd@kicp.uchicago.edu> 
 *
 **/ 

#include <vector> 
#include "AnitaConventions.h" 

class RawAnitaHeader; 
class TTree; 
class Adu5Pat; 
class UsefulAnitaEvent; 
class RawAnitaEvent; 
class CalibratedAnitaEvent; 
class PrettyAnitaHk; 
class TFile;
class TurfRate; 
class TCut; 
class TEventList; 
class SurfHk; 


class AnitaDataset
{
  public: 
    /** Get the data directory for the anita version based on environmental variables.  Negative is default and corresponds to defining ANITA_ROOT_DIR **/ 
    static const char * getDataDir(int anita_version = -1); 

    /** Constructor loading a run with calibration type and anita version.  If decimated is true, the decimated header file is read and you only have access to the 10% dataset.*/
    AnitaDataset (int run, bool decimated = false, WaveCalType::WaveCalType_t cal = WaveCalType::kDefault, int anita_version = -1 ); 

    /** Change the calibration type */ 
    void setCalType(WaveCalType::WaveCalType_t cal) { fCalType = cal; } 

    /** Destructor */ 
    virtual ~AnitaDataset(); 


    /** Loads run. Can use decimated to load the 10% data file */ 
    bool loadRun(int run, bool decimated = false, int anita_version = -1); 

    /** loads the desired eventNumber and returns the current entry**/ 
    int getEvent(int eventNumber); 

    /** loads the desired entry within the tree. Returns the current entry afterwards.  */
    int getEntry(int entryNumber); 

    // returns the currently selected entry
    int current() { return fDecimated ? fDecimatedEntry : fWantedEntry; }

    /** gets the next entry and returns the current entry afterwards */
    int next() { return getEntry(fDecimated ? fDecimatedEntry +1  : fWantedEntry+1); }

    /** gets the previous entry and returns the current entry afterwards */
    int previous() { return getEntry(fDecimated ? fDecimatedEntry - 1 : fWantedEntry-1); }

    /** loads the first entry */ 
    int first() { return getEntry(0); } 

    /** loads the last entry */
    int last() { return getEntry(N()-1); }

    /** returns the number of entries */
    int N() const; 


    /** Loads the firstEvent (in case the entries are not in order). returns the entry of it */ 
    int firstEvent(); 

    /** Loads next event (in case the entries are not in order). returns the entry of it */ 
    int nextEvent(); 

    /** Loads previous event (in case the entries are not in order). returns the entry of it */ 
    int previousEvent(); 

    /** Loads last event (in case the entries are not in order). returns the entry of it. */ 
    int lastEvent(); 

    /** Returns the nth event (in case the entries are not in order). returns the entry of it. */ 
    int nthEvent(int i); 


    /** Applies a cut to the entire dataset. Supercedes any previous cut. 
     * Once you apply a cut, you may use NInCut, firstInCut(), nextInCut(), previousInCut(), lastInCut()
     * to iterate.  The cut applies to the headTree. Returns the number of event sin the cut */
    int setCut(const TCut & cut); 

    /** The number of events in the cut (or -1 if no cut is applied) */
    int NInCut() const; 

    /** Loads the first event passing the cut. Returns the entry number or -1 if no cut applied */
    int firstInCut(); 

    /** Loads the last event passing the cut. Returns the entry number or -1 if no cut applied */
    int lastInCut(); 

    /** Loads the next passing the cut. Returns the entry number or -1 if no cut applied */
    int nextInCut(); 

   /** Loads the previous passing the cut. Returns the entry number or -1 if no cut applied */
    int previousInCut(); 

    /** Loads the nth event passing the cut. Returns the entry number or -1 if no cut applied */
    int nthInCut(int i); 


    /** Loads the useful event. If force_reload is true, 
     * the event will be reloaded from the tree (in case you made some changes
     * and want a fresh copy). This will either be created from the
     * CalibratedAnitaEvent or RawAnitaEvent, whichever the Dataset found */ 
    UsefulAnitaEvent * useful(bool force_reload = false); 


    /** Loads the raw event. If force_reload is true, the event will be reloaded from the tree. */
    RawAnitaEvent * raw(bool force_reload = false); 

    /** Loads the calibrated event. This may be read directly from disk if available or otherwise created on the fly. 
     * If force_reload is true, the event will be reloaded from the tree. */
    CalibratedAnitaEvent * calibrated(bool force_reload = false); 


    /** Loads the GPS. This is either from the gpsEventTree or the best-timed
     * PAT from the gpsTree. force_reload will reload it even if it has already
     * been loaded */
    Adu5Pat * gps(bool force_reload = false); 

    /** Loads the PrettyAnitaHk. Optionally can force a reload */
    PrettyAnitaHk * hk(bool force_reload = false);

    /** Loads the Header. This will preferentially be from the timedHeader tree
     * but will fall back to the less glamorous one if need be. If the
     * decimated run was loaded, the decimated header tree is used.  Optionally
     * can force a reload */

    RawAnitaHeader * header(bool force_reload = false);


    /** Loads the TURF Rate. This is indexed so that the fTurf->realTime is as close as possible to the header triggerTime */ 
    TurfRate * turf(bool force_reload = false); 

    /** Loads the SURF Hk. This is indexed so that the the payload times match as closely as possible */ 
    SurfHk * surf(bool force_reload = false); 




  private: 
    void unloadRun(); 
    TTree * fHeadTree; 
    TTree * fDecimatedHeadTree; //only used when using decimated 
    Int_t * fIndices; 
    Int_t fIndex; 
    RawAnitaHeader * fHeader;
    TTree *fEventTree; 
    CalibratedAnitaEvent * fCalEvent; 
    RawAnitaEvent * fRawEvent; 
    UsefulAnitaEvent * fUseful; 
    Bool_t fUsefulDirty; 
    Bool_t fCalDirty;  // used only with raw data
    Bool_t fGpsDirty;  // used only with gpsFile data 
    TTree* fGpsTree; 
    Adu5Pat * fGps; 
    TTree* fHkTree; 
    PrettyAnitaHk * fHk; 

    TTree * fTurfTree; 
    TurfRate * fTurf; 
    Bool_t fTurfDirty; 

    TTree * fSurfTree; 
    SurfHk * fSurf; 
    Bool_t fSurfDirty; 


    Int_t fWantedEntry; 
    Int_t fDecimatedEntry; 
    Bool_t fHaveGpsEvent; 
    Bool_t fHaveCalibFile; 
    WaveCalType::WaveCalType_t fCalType; 
    std::vector<TFile *> filesToClose; 
    bool fDecimated; 
    TEventList * fCutList; 
    int fCutIndex; 
}; 

#endif 
