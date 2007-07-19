//////////////////////////////////////////////////////////////////////////////
/////  Adu5Vtg.cxx        ANITA ADU5 VTG reading class                   /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class that reads in ADU5 VTG and produces trees       ///// 
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "Adu5Vtg.h"
#include <iostream>
#include <fstream>

ClassImp(Adu5Vtg);

Adu5Vtg::Adu5Vtg() 
{
   //Default Constructor
}

Adu5Vtg::~Adu5Vtg() {
   //Default Destructor
}

Adu5Vtg::Adu5Vtg(Int_t           trun,
		 UInt_t          trealTime,
		 UInt_t          tpayloadTime,
		 UInt_t          tpayloadTimeUs,
		 Float_t         ttrueCourse,
		 Float_t         tmagneticCourse,
		 Float_t         tspeedInKnots,
		 Float_t         tspeedInKPH,
		 Int_t           tintFlag)
{

   run=trun;
   realTime=trealTime;
   payloadTime=tpayloadTime;
   payloadTimeUs=tpayloadTimeUs;
   trueCourse=ttrueCourse;
   magneticCourse=tmagneticCourse;
   speedInKnots=tspeedInKnots;
   speedInKPH=tspeedInKPH;
   intFlag=tintFlag;
}
