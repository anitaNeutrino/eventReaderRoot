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
   UShort_t        processBitMask; ///< Bit mask of currently running processes
   UShort_t        reserved; ///< Reserved

   char *getDirName(int dirInd); ///< Returns dir name

  ClassDef(OtherMonitorHk,10);
};


#endif //OTHERMONITORHK_H
