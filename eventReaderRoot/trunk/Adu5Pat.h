//////////////////////////////////////////////////////////////////////////////
/////  Adu5Pat.h        Pretty ANITA hk class                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing pretty ADU5 PAT objects in a TTree  /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ADU5PAT_H
#define ADU5PAT_H

//Includes
#include <TObject.h>
#include "simpleStructs.h"

//!  Adu5Pat -- The ADU5 Position and Attitude Data
/*!
  This is a class to hold the position and attitude data from the ADU5's.
*/
class Adu5Pat: public TObject
{
 public:
  Adu5Pat();		 
  ~Adu5Pat();

  Adu5Pat(Int_t           trun,
	  UInt_t          trealTime,
	  GpsAdu5PatStruct_t *gpsStruct);

  Int_t           run; 
   UInt_t          realTime; ///<Time from the GPS unit
   UInt_t          readTime; ///<Time of readout by the CPU
   UInt_t          payloadTime;
   UInt_t          payloadTimeUs;
   UInt_t          timeOfDay; ///<in ms since the start of the day
   Float_t         latitude; ///< In degrees
   Float_t         longitude; ///<In degrees
   Float_t         altitude; ///<In metres
   Float_t         heading; ///< 0 is facing north, 180 is facing south
   Float_t         pitch; ///< in degrees
   Float_t         roll; /// in degrees
   Float_t         mrms; ///<phase measurement rms error in metres
   Float_t         brms; ///<Baseline rms error in metres
   UInt_t          attFlag; ///< 0 is good attitude, 1 is bad attitude
   Int_t           intFlag; ///< for raw data = 0, otherwise shows interpolation time
   
  ClassDef(Adu5Pat,10);
};


#endif //ADU5PAT_H
