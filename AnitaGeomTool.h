//////////////////////////////////////////////////////////////////////////////
/////  AnitaGeomTool.h        ANITA Event Calibrator              /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for calibrating UsefulAnitaEvents               /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#ifndef ANITAGEOMTOOL_H
#define ANITAGEOMTOOL_H

//Includes
#include <TObject.h>
#include "AnitaConventions.h"


class UsefulAnitaEvent;

class AnitaGeomTool 
{
 public:
   
  AnitaGeomTool();
  ~AnitaGeomTool();

  //Instance generator
  static AnitaGeomTool*  Instance();
  
  //Calibration Routine
  int calibrateUsefulEvent(UsefulAnitaEvent *eventPtr,
			   WaveCalType::WaveCalType_t calType);

  //Generally useful function
  static int getChanIndex(int surf, int chan)
     {return chan+(9*surf);}

  static int getChanIndexFromRingPhiPol(AnitaRing::AnitaRing_t ring,
					int phi,
					AnitaPol::AnitaPol_t pol);
  static int getChanIndexFromAntPol(int ant,
				    AnitaPol::AnitaPol_t pol);


 protected:
   static AnitaGeomTool *fgInstance;  
   // protect against multiple instances


};


#endif //ANITAGEOMTOOL_H
