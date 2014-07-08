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



#ifndef NUM_HK_TELEM_QUEUES
#define NUM_HK_TELEM_QUEUES 21
#endif
#ifndef NUM_TELEM_QUEUES
#define NUM_TELEM_QUEUES NUM_HK_TELEM_QUEUES+NUM_PRIORITIES
#endif
#ifndef NUM_DISK_SPACES
#define NUM_DISK_SPACES 8
#endif

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

   //!  The free space left on the disks in units of 10 Megabytes
   /*!
     The free space left on the disks in units of 10 Megabytes. The eight disks  and their capacities are:
    -  "[0]" The ramdisk (8GB)
    -  "[1]" The var partition (3GB)
    -  "[2]" The home partition (4GB)
    -  "[3]" The root partiiton (8GB)
    -  "[4]" The helium1 drive (6000GB)
    -  "[5]" The helium2 drive (6000GB)
    -  "[6]" The USB drive (128GB)
    -  "[7]" The NTU SSD drive (1000GB)
   */
   UShort_t        diskSpace[8]; ///<In units of 10 MegaBytes
   Char_t          ntuLabel[12]; ///< Label of the currently mounted ntu drive
   Char_t          otherLabel[12]; ///< Label of the currently mounted other drive
   Char_t          usbLabel[12]; ///< Defunct
   UShort_t        eventLinks[NUM_PRIORITIES]; ///< The number of links in the 10 prioritity queues.
    //!  The number of links in the housekeeping queues
   /*!
     The number of links in the housekeeping queues
     <ol><li>LosCmd</li><li>SipCmd</li><li>Monitor</li><li>Header</li><li>Acromag</li>
     <li>Adu5aSat</li><li>Adu5bSat</li><li>G12Sat</li><li>Adu5aPat</li><li>Adu5bPat</li><li>G12Pos</li>
     <li>Adu5aVtg</li><li>Adu5bVtg</li><li>G12Gga</li><li>Adu5aGga</li><li>Adu5bGga</li><li>SurfHk</li>
     <li>TurfHk</li><li>Other</li><li>Pedestal</li><li>Request</li></ol>
   */
   UShort_t        hkLinks[NUM_HK_TELEM_QUEUES]; // Links in the 21 hk telemetry queues
   //!  The user space CPU time used by the flight software processes.
   /*!
     The user time used by the flight software processes.
     <ol><li>Acqd</li><li>Archived</li><li>Calibd</li><li>Cmdd</li><li>Eventd</li><li>GPSd</li><li>Hkd</li><li>LOSd</li><li>Prioritizerd</li><li>SIPd</li><li>Monitord</li><li>Playbackd</li><li>Logwatchd</li><li>Neobrickd</li><li>n/a</li><li>n/a</li></ol>
   */
   UInt_t          utime[NUM_PROCESSES]; // utime of each process
    //!  The system space CPU time used by the flight software processes.
   /*!
     Thqe system space time used by the flight software processes.
     <ol><li>Acqd</li><li>Archived</li><li>Calibd</li><li>Cmdd</li><li>Eventd</li><li>GPSd</li><li>Hkd</li><li>LOSd</li><li>Prioritizerd</li><li>SIPd</li><li>Monitord</li><li>Playbackd</li><li>Logwatchd</li><li>Neobrickd</li><li>n/a</li><li>n/a</li></ol>
   */
   UInt_t          stime[NUM_PROCESSES]; // stime of each process
   //!  The memory used by the flight software processes.
   /*!
     The memory used by the flight software processes.
     <ol><li>Acqd</li><li>Archived</li><li>Calibd</li><li>Cmdd</li><li>Eventd</li><li>GPSd</li><li>Hkd</li><li>LOSd</li><li>Prioritizerd</li><li>SIPd</li><li>Monitord</li><li>Playbackd</li><li>Logwatchd</li><li>Neobrickd</li><li>n/a</li><li>n/a</li></ol>
   */
   UInt_t          vsize[NUM_PROCESSES]; ///< vsize of each process

   const char *getDriveName(int driveInd); ///< Returns drive name by index
   int getDriveCapacity(int driveInd); ///< Returns drive capacity by index
   int getDiskSpaceAvailable(int driveInd); ///< Returns drive capacity by index
   const char *getHkQueueName(int hkInd); ///< Returns Hk Queue name
   const char *getProcName(int procInd); ///< Returns process name

  ClassDef(MonitorHk,10);
};


#endif //MONITORHK_H
