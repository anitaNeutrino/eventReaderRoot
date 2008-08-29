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


class MonitorHk: public TObject
{
 public:
  MonitorHk();		 
  ~MonitorHk();

  MonitorHk(Int_t trun, Int_t trealTime, MonitorStruct_t *monPtr);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime; 
   UShort_t        diskSpace[8]; ///<In units of 10 MegaBytes
   Char_t          sataminiLabel[12];
   Char_t          satabladeLabel[12];
   Char_t          usbLabel[12];
   UShort_t        eventLinks[NUM_PRIORITIES]; ///<10 Priorities
   UShort_t        hkLinks[21]; ///<Needs to be finalised once everything is settled
   UInt_t          utime[NUM_PROCESSES];
   UInt_t          stime[NUM_PROCESSES];
   UInt_t          vsize[NUM_PROCESSES];

   char *getDriveName(int driveInd);
   int getDriveCapacity(int driveInd);
   char *getHkQueueName(int hkInd);
   char *getProcName(int procInd);

  ClassDef(MonitorHk,10);
};


#endif //MONITORHK_H
