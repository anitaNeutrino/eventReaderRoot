//////////////////////////////////////////////////////////////////////////////
/////  OtherMonitorHk.h        CPU Monitor                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Monitor structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef OTHERMONITORHK_H
#define OTHERMONITORHK_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"

//!  OtherMonitorHk -- The other CPU Monitor data
/*!
  The ROOT implementation of the other CPU monitoring data
  \ingroup rootclasses
*/
class OtherMonitorHk: public TObject
{
 public:
   OtherMonitorHk(); ///<Default constructor
   ~OtherMonitorHk(); ///<Destructor

   OtherMonitorHk(Int_t trun, Int_t trealTime, OtherMonitorStruct_t *otherPtr); ///< Assigment constructor

   Int_t           run; ///< Run number, assigned offline
   UInt_t          realTime; ///< Time in unixTime
   UInt_t          payloadTime;  ///< Time in unixTime
   UInt_t          ramDiskInodes; ///< Inodes remaining on ramdisk
   UInt_t          runStartTime; ///< Start time of the current run
   UInt_t          runStartEventNumber; ///<Start eventNumber of the current run
   UInt_t          runNumber; ///< Current run number
   UShort_t        dirFiles[3]; ///< Number of files in /tmp/anita/acqd /tmp/anita/eventd /tmp/anita/prioritizerd
   UShort_t        dirLinks[3]; ///< Number of links in /tmp/anita/acqd /tmp/anita/eventd /tmp/anita/prioritizerd
   //!  A bit mask of the flight software processes currently running.
   /*!
     A bit mask of the flight software processes currently running. If the bit is set that process is running. The bits are:
     <ol>
     <li>Acqd</li>
     <li>Archived</li>
     <li>Calibd</li>
     <li>Cmmd</li>
     <li>Eventd</li>
     <li>GPSd</li>
     <li>Hkd</li>
     <li>LOSd</li>
     <li>Prioritizerd</li>
     <li>SIPd</li>
     <li>Monitord</li>
     <li>Playbackd</li>
     <li>Logwatchd</li>
     <li>Neobrickd</li>
     </ol>
*/
   UShort_t        processBitMask; ///< Bit mask of currently running processes
   UShort_t        reserved; ///< Reserved
   
   int isInProcessMask(int procInd); ///< Returns 1 or 0 if process is in mask

   const char *getDirName(int dirInd); ///< Returns dir name
   const char *getProcName(int procInd); ///< Returns process name (using 0 as the index of the first bit).
  ClassDef(OtherMonitorHk,10);
};


#endif //OTHERMONITORHK_H
