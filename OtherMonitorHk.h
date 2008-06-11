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


class OtherMonitorHk: public TObject
{
 public:
  OtherMonitorHk();		 
  ~OtherMonitorHk();

  OtherMonitorHk(Int_t trun, Int_t trealTime, OtherMonitorStruct_t *otherPtr);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime; 
   UInt_t          ramDiskInodes;
   UInt_t          runStartTime;
   UInt_t          runStartEventNumber; ///<Start eventNumber
   UInt_t          runNumber; ///<Run number
   UShort_t        dirFiles[3]; ///< /tmp/anita/acqd /tmp/anita/eventd /tmp/anita/prioritizerd
   UShort_t        dirLinks[3]; ///< /tmp/anita/acqd /tmp/anita/eventd /tmp/anita/prioritizerd
   UShort_t        processBitMask;
   UShort_t        reserved;

  ClassDef(OtherMonitorHk,10);
};


#endif //OTHERMONITORHK_H
