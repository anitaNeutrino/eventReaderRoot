//////////////////////////////////////////////////////////////////////////////
/////  Adu5Pat.cxx        ANITA ADU5 PAT reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 PAT and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "Adu5Pat.h"
#include <iostream>
#include <fstream>

ClassImp(Adu5Pat);

Adu5Pat::Adu5Pat() 
{
   //Default Constructor
}

Adu5Pat::~Adu5Pat() {
   //Default Destructor
}



Adu5Pat::Adu5Pat(Int_t           trun,
		 UInt_t          trealTime,
		 UInt_t          treadTime,
		 UInt_t          tpayloadTime,
		 UInt_t          tpayloadTimeUs,
		 UInt_t          ttimeOfDay,
		 Float_t         tlatitude,
		 Float_t         tlongitude,
		 Float_t         taltitude,
		 Float_t         theading,
		 Float_t         tpitch,
		 Float_t         troll,
		 Float_t         tmrms,
		 Float_t         tbrms,
		 UInt_t          tattFlag,
		 Int_t           tintFlag)
{

   run=trun;
   realTime=trealTime;
   readTime=treadTime;
   payloadTime=tpayloadTime;
   payloadTimeUs=tpayloadTimeUs;
   timeOfDay=ttimeOfDay;
   latitude=tlatitude;
   longitude=tlongitude;
   altitude=taltitude;
   heading=theading;
   pitch=tpitch;
   roll=troll;
   mrms=tmrms;
   brms=tbrms;
   attFlag=tattFlag;
   intFlag=tintFlag;
}
