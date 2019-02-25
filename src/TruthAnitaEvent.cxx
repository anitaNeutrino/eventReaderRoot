//////////////////////////////////////////////////////////////////////////////
/////  TruthAnitaEvent.cxx      Truth ANITA event class                  /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing truth ANITA info in a TTree         /////
/////  Author: Linda Cremonesi (l.cremonesi@hep.ucl.ac.uk)               /////
//////////////////////////////////////////////////////////////////////////////

#include "TruthAnitaEvent.h"

ClassImp(TruthAnitaEvent);

TruthAnitaEvent::TruthAnitaEvent()
{
  //cheating, don't do this at home! 
  memset(&run, 0, size_t(&weight) + sizeof(weight) - size_t(&run)); 

  //payloadPhi = -999; 
  //payloadTheta = -999; 

  //Default Constructor
}

TruthAnitaEvent::~TruthAnitaEvent() {
   //Default Destructor
}
