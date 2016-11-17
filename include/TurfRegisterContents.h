//////////////////////////////////////////////////////////////////////////////
/////  TurfRegisterContents.h        Turf Register Contents                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 VTG objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef TURFREGISTERCONTENTS_H
#define TURFREGISTERCONTENTS_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"

//!  TurfRegisterContents -- The Turf Register Contents
/*!
  The ROOT implementation of the TURF Register Contents -- Unifinished
  \ingroup rootclasses
*/
class TurfRegisterContents: public TObject
{
 public:
   TurfRegisterContents(); ///< Default constructor
   ~TurfRegisterContents(); ////< Destructor

  //   TurfRegisterContents(Int_t trun, Int_t trealTime, TurfRegContentsStruct_t *ratePtr); ///< Assignment constructor
   TurfRegisterContents(Int_t trun, Int_t trealTime, TurfRegisterContents_t *ratePtr); ///< Assignment constructor  
  

   
  ClassDef(TurfRegisterContents,16);
};


#endif //TURFREGISTERCONTENTS_H
