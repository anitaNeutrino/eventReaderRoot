//////////////////////////////////////////////////////////////////////////////
/////  AnitaGeomTool.h        ANITA Geom Tool                            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for converting between physical channel (ring,  /////
/////     antenna, phi, etc.) and logical channel (surf, chan, etc.)     /////
/////     It should be noted that phi, antenna, etc are numbered from 0  /////
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
  

  //Generally useful function
  static int getChanIndex(int surf, int chan)
     {return chan+(9*surf);}

  static int getChanIndexFromRingPhiPol(AnitaRing::AnitaRing_t ring,
					int phi,
					AnitaPol::AnitaPol_t pol);
  static int getChanIndexFromAntPol(int ant,
				    AnitaPol::AnitaPol_t pol);


  static int getSurfChanFromChanIndex(int chanIndex, // input channel index
				      int &surf,int &chan); // output surf and channel
  
  static int getAntPolFromSurfChan(int surf,int chan,int &ant,AnitaPol::AnitaPol_t &pol);
  
  static int getAzimuthPartner(int rx); // output the antenna that points to the same place in phi as the input antenna
  
  static void getThetaPartners(int rx,int& rxleft,int& rxright); // output the antenna that points to the same place in phi as the input antenna

  static int getPhiSector(int rx); // phi sector of this antenna.  rx runs from 0 to 31.

  static float getDirectionwrtNorth(int phi,float heading); // get direction that a phi sector is pointing wrt north.  Also takes heading as a input.





 protected:
   static AnitaGeomTool *fgInstance;  
   // protect against multiple instances


};


#endif //ANITAGEOMTOOL_H
