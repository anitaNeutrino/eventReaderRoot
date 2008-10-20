//////////////////////////////////////////////////////////////////////////////
/////  MonitorHk.h        CPU Monitor                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Monitor structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef MONITORHK_H
#define MONITORHK_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"


//!  MonitorHk -- The CPU Monitor data
/*!
  The ROOT implementation of the CPU monitoring data
  \ingroup rootclasses
*/
class MonitorHk: public TObject
{
 public:
   MonitorHk(); ///< Default constructor
   ~MonitorHk(); ///< Destructor

   MonitorHk(Int_t trun, Int_t trealTime, MonitorStruct_t *monPtr); ///< Assignment constructor

   Int_t           run; ///< Run number assigned offline
   UInt_t          realTime; ///< Time in unixTime
   UInt_t          payloadTime; ///< Time in unixTime
   UShort_t        diskSpace[8]; ///<In units of 10 MegaBytes
   Char_t          sataminiLabel[12]; ///< Label of the currently mounted satamini drive
   Char_t          satabladeLabel[12]; ///< Label of the currently mounted satablade drive
   Char_t          usbLabel[12]; ///< Defunct
   UShort_t        eventLinks[NUM_PRIORITIES]; ///< Links in the 10 priorities queues
   UShort_t        hkLinks[21]; ///< Links in the 21 hk telemetry queues
   UInt_t          utime[NUM_PROCESSES]; ///< utime of each process
   UInt_t          stime[NUM_PROCESSES]; ///< stime of each process
   UInt_t          vsize[NUM_PROCESSES]; ///< vsize of each process

   char *getDriveName(int driveInd); ///< Returns drive name by index
   int getDriveCapacity(int driveInd); ///< Returns drive capacity by index
   char *getHkQueueName(int hkInd); ///< Returns Hk Queue name
   char *getProcName(int procInd); ///< Returns process name

  ClassDef(MonitorHk,10);
};


#endif //MONITORHK_H
