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
 * Now it supports blinding as well. Despite Ben's argument that it is not a property of the data, 
 * I think it's best that all the data is accessed consistently. 
 *
 *
 *  
 *
 *  Cosmin Deaconu <cozzyd@kicp.uchicago.edu>
 *  Ben Strutt <strutt@physics.ucla.edu> 
 *
 **/

#include <vector>
#include "AnitaConventions.h"
#include "TString.h"
#include "TRandom3.h"

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
class TruthAnitaEvent;

class AnitaDataset
{
  public:


  // All the blinding options.
  // The plan is to have each one represented by a bit so multiple strategies can be set at the same time.
  // Any output files should probably save the blinding strategy to the tree.
    enum BlindingStrategy
    {
      kNoBlinding = 0x00, 
      kInsertedVPolEvents = 0x01, 
      kInsertedHPolEvents = 0x02, 
      kRandomizePolarity = 0x04, 
      kDefault = kNoBlinding
    }; 


    // various data directories that might be searched by AnitaDataset
    // Note that ANITA_ROOT_DATA is reverted to searched if none of the others work
    enum DataDirectory
    {
      ANITA_ROOT_DATA = -1, 
      ANITA_MC_DATA= 0, 
      ANITA1_ROOT_DATA=1,
      ANITA2_ROOT_DATA=2,
      ANITA3_ROOT_DATA=3, 
      ANITA4_ROOT_DATA=4
    }; 

 /**
   * Get a one line description of the blinding strategy
   * (please update this in AnitaDataset.cxx when you add a strategy.
   * @param strat is the strategy to describe.
   * @return the description
   */
  static TString getDescription(BlindingStrategy strat); 


  /**
   * Set the current strategy (see AnitaDataset.h) for strategy options
   * @param strat is the strategy to use. This can be a combination e.g. (kInsertedVPolEvents | kAnotherStrategy)
   * @return the strategy that was set
   */
   BlindingStrategy setStrategy(BlindingStrategy strat);


  /**
   * Get the currently set strategy
   * @return the strategy that was set
   */
   BlindingStrategy getStrategy();

  /** 
   * Get the description string for the currently enabled strategy
   * 
   * @return the description for the current strategy
   */
  inline TString describeCurrentStrategy(){
    return getDescription(getStrategy());
  }


  /** 
   * @brief Get the run that contains the eventNumber.
   * 
   * Uses AnitaVersion::get(), if it's not correct you won't get the right answer!
   * 
   * @param eventNumber is the eventNumber
   * 
   * @return the run
   */
   static int getRunContainingEventNumber(UInt_t eventNumber);


    /** Get the data directory for the anita version based on environmental variables.  */
    static const char * getDataDir(DataDirectory dir = ANITA_ROOT_DATA);

    /** Returns the run at the requested time */ 
    static int getRunAtTime(double t);

  /** 
   * Check against the list of known Hi-Cal events.
   * Warning... this list is likely incomplete!
   * @todo implement this for ANITA-4
   * 
   * @param eventNumber the eventNumber
   * @param anita which ANITA flight? (optional: uses AnitaVersion::get() if not supplied.)
   * 
   * @return true if the event is on the list, false otherwise.
   */
    static bool isKnownHiCalEvent(UInt_t eventNumber, Int_t anita=AnitaVersion::get());


    /** Constructor loading a run with calibration type and anita version.  If decimated is true, the decimated header file is read and you only have access to the 10% dataset.
     * 
     *  cal changes the calibration type. 
     *
     *  version is:
     *    negative to read ANITA_ROOT_DATA
     *    postive # to read ANITA#_ROOT_DATA
     *    0 to read ANITA_MC_DATA 
     *
     *  strat is the blinding strategy. 
     *
     * */
    AnitaDataset (int run, bool decimated = false, WaveCalType::WaveCalType_t cal = WaveCalType::kDefault, 
                  DataDirectory dir = ANITA_ROOT_DATA , BlindingStrategy strat = AnitaDataset::kDefault);

    /** Change the calibration type */
    void setCalType(WaveCalType::WaveCalType_t cal) { fCalType = cal; }

    /** Destructor */
    virtual ~AnitaDataset();


    /** Loads run. Can use decimated to load the 10% data file 
     *
     *
     **/

    bool loadRun(int run, bool decimated = false, DataDirectory dir  = ANITA_ROOT_DATA );

    /** loads the desired eventNumber and returns the current entry
     * If quiet is true, won't print out a warning about changing runs or missing events. 
     * **/
    int getEvent(int eventNumber, bool quiet = false);

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

    /** Loads a playlist for your dataset.Playlist format is RUN EVENTNUMBER\n or EVENTNUMBER\n
     * After applying playlist, use these to iterate through. Need runToEv files from MagicDisplay */
    int setPlaylist(const char* playlist);

    /** The number of events in the playlist (or -1 if no playlist) */
    int NInPlaylist() const;

    /** Loads the first event in the playlist. Returns the entry number or -1 if no playlist */
    int firstInPlaylist();

    /** Loads the last event in the playlist. Returns the entry number or -1 if no playlist */
    int lastInPlaylist();

    /** Loads the next playlist event. Returns the entry number or -1 if no playlist */
    int nextInPlaylist();

   /** Loads the previous playlist event. Returns the entry number or -1 if no playlist */
    int previousInPlaylist();

    /** Loads the nth playlist event. Returns the entry number or -1 if no playlist */
    int nthInPlaylist(int i);

    /** Loads the useful event. If force_reload is true,
     * the event will be reloaded from the tree (in case you made some changes
     * and want a fresh copy). This will either be created from the
     * CalibratedAnitaEvent or RawAnitaEvent, whichever the Dataset found */
     virtual UsefulAnitaEvent * useful(bool force_reload = false);


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

    virtual RawAnitaHeader * header(bool force_reload = false);


    /** Loads the TURF Rate. This is indexed so that the fTurf->realTime is as close as possible to the header triggerTime */
    TurfRate * turf(bool force_reload = false);

    /** Loads the SURF Hk. This is indexed so that the the payload times match as closely as possible */
    SurfHk * surf(bool force_reload = false);


    /** Loads the MCTruth. This will be NULL if there is no truth (like if you're working with real data. */ 
    TruthAnitaEvent * truth(bool force_reload = true); 
    
    /** Loads the calibInfo file. These are computed by makeCalibratedEventInfo in anitaTreeMaker so that you can have consistent calibratedEventFiles that can be generated on the fly */
    std::vector<Double_t>* calibInfo(bool force_reload = false);

    /** Lets you check to see if you have a header and event file actually loaded, or if it failed loading */
    bool fRunLoaded;

    /** Want to see what run you previously loaded?  Look no further */
    int getCurrRun() { return currRun; };

    /* Wraps the random number generator for polarity inversion so it is derministic regardless of event processing order */
    bool maybeInvertPolarity(UInt_t eventNumber);

    /* Where was HiCal at a particular time?*/
    static void hiCal(UInt_t unixTime, Double_t& longitude,  Double_t& latitude, Double_t& altitude);

    /* Where was hical? Uses the current header realTime*/ 
    void hiCal(Double_t& longitude,  Double_t& latitude, Double_t& altitude);

  protected:
    void unloadRun();
    TTree * fHeadTree;
    TTree * fDecimatedHeadTree; //only used when using decimated
    Long64_t * fIndices;
    Long64_t fIndex;
    RawAnitaHeader * fHeader;
    TTree *fEventTree;
    TTree* fCalibInfoTree;
    CalibratedAnitaEvent * fCalEvent;
    RawAnitaEvent * fRawEvent;
    UsefulAnitaEvent * fUseful;
    std::vector<Double_t>* fCalibInfo;
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

    TTree * fTruthTree; 
    TruthAnitaEvent * fTruth;


    /* place to store the current run but that can't be confusingly changed */
    int currRun;

    Long64_t fWantedEntry;
    Long64_t fDecimatedEntry;
    Bool_t fHaveGpsEvent;
    Bool_t fHaveCalibFile;
    Bool_t fHaveCalibInfo;
    WaveCalType::WaveCalType_t fCalType;
    std::vector<TFile *> filesToClose;
    bool fDecimated;
    TEventList * fCutList;
    int fCutIndex;

    static void loadRunToEv(int anita); // read runToEvA*.txt
    static void loadHiCalGps(); /// Where was HiCal?
    int loadPlaylist(const char* playlist);
    int fPlaylistIndex;
    std::vector<std::vector<long> > fPlaylist;
    int getPlaylistRun() { return fPlaylist[fPlaylistIndex][0]; }
    Long64_t getPlaylistEvent() { return fPlaylist[fPlaylistIndex][1]; }


    /* Blinding stuff */ 
    void zeroBlindPointers();
    void loadBlindTrees();

    BlindingStrategy theStrat; ///!< Currently selected blinding strategy
    TRandom3 fRandy; ///!< for deciding whether to do polarity flipping (eventNumber is used as seed)

    bool loadedBlindTrees; ///!< Have we loaded the tree of events to insert?
    Int_t needToOverwriteEvent(AnitaPol::AnitaPol_t pol, UInt_t eventNumber);
    void overwriteHeader(RawAnitaHeader* header, AnitaPol::AnitaPol_t pol, Int_t fakeTreeEntry);
    void overwriteEvent(UsefulAnitaEvent* useful, AnitaPol::AnitaPol_t pol, Int_t fakeTreeEntry);

    // fake things
    TFile* fBlindFile; ///!< Pointer to file containing tree of UsefulAnitaEvents to insert
    TTree* fBlindEventTree[AnitaPol::kNotAPol]; ///!< Tree of UsefulAnitaEvents to insert
    TTree* fBlindHeadTree[AnitaPol::kNotAPol]; ///!< Tree of headers to insert

    UsefulAnitaEvent* fBlindEvent[AnitaPol::kNotAPol]; ///!< Pointer to fake UsefulAnitaEvent
    RawAnitaHeader* fBlindHeader[AnitaPol::kNotAPol]; ///!< Pointer to fake header

    // Here we have a set of three vectors that should all be the same length as the elements of each match up.
    // They are filled in loadBlindTrees
    std::vector<UInt_t> eventsToOverwrite;
    std::vector<AnitaPol::AnitaPol_t> polarityOfEventToInsert;
    std::vector<Int_t> fakeTreeEntries;


  
};

// define the bitwise or operator | to combine blinding strategies
inline AnitaDataset::BlindingStrategy operator|(AnitaDataset::BlindingStrategy strat1,  AnitaDataset::BlindingStrategy strat2){
  return static_cast<AnitaDataset::BlindingStrategy>(static_cast<UInt_t>(strat1) | static_cast<UInt_t>(strat2));
}
// define the bitwise and opterator & to decode the blinding strategies
inline AnitaDataset::BlindingStrategy operator&(AnitaDataset::BlindingStrategy strat1,  AnitaDataset::BlindingStrategy strat2){
  return static_cast<AnitaDataset::BlindingStrategy>(static_cast<UInt_t>(strat1) & static_cast<UInt_t>(strat2));
}



#endif
