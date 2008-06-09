//////////////////////////////////////////////////////////////////////////////
/////  GpsdStart.h        Gpsd Start                                     /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing Gpsd Start structs in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef GPSDSTART_H
#define GPSDSTART_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"
#include "simpleStructs.h"


class GpsdStart: public TObject
{
 public:
  GpsdStart();		 
  ~GpsdStart();

  GpsdStart(Int_t trun, Int_t trealTime, GpsdStartStruct_t *startPtr);

   Int_t           run;
   UInt_t          realTime;
   UInt_t          payloadTime; 
   UChar_t         ackCount[3];
   UChar_t         nakCount[3];
   UChar_t         rioBitMask;
   UChar_t         tstBitMask;

  ClassDef(GpsdStart,10);
};


#endif //GPSDSTART_H
