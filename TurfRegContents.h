//////////////////////////////////////////////////////////////////////////////
/////  TurfRegContents.h        Turf Register Contents                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 VTG objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef TURFRATE_H
#define TURFRATE_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"


class TurfRegContents: public TObject
{
 public:
  TurfRegContents();		 
  ~TurfRegContents();

  TurfRegContents(Int_t trun, Int_t trealTime, TurfRegContentsStruct_t *ratePtr);
  

   
  ClassDef(TurfRegContents,16);
};


#endif //TURFRATE_H
