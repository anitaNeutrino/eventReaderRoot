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


//!  GpsdStart -- The start up packet for GPSd
/*!
  The startup packet for GPSd
  \ingroup rootclasses
*/
class GpsdStart: public TObject
{
 public:
   GpsdStart(); ///< Default constructor 
   ~GpsdStart(); ///< Destructor

   GpsdStart(Int_t trun, Int_t trealTime, GpsdStartStruct_t *startPtr); ///< Assignment constructor

   Int_t           run; ///< Run number asssigned offline
   UInt_t          realTime; ///< Time in unixTime
   UInt_t          payloadTime;  ///< Time in unixTime
   UChar_t         ackCount[3]; ///< Number of acknowledge messages from each GPS
   UChar_t         nakCount[3];///< Number of NAK messages from each GPS
   //! RIO Bit mask
   /*! 
     \li \c 0 G12 Matches
     \li \c 1 ADU5A Matches 
     \li \c 2 ADU5B Matches 
     \li \c 5 is ADU5A has inactive sensor
     \li \c 6 is ADU5B has inactive sensor
   */
   UChar_t         rioBitMask; 
   //!  tstBitMask
   /*!
     \li \c 0 ADU5A failed battery test
     \li \c 1 ADU5A failed parameter test
     \li \c 2 ADU5A failed fatal error test --- sounds bad
     \li \c 3 ADU5A failed power up test -- also bad sounding
     \li \c 4 ADU5B failed battery test
     \li \c 5 ADU5B failed parameter test
     \li \c 6 ADU5B failed fatal error test --- sounds bad
     \li \c 7 ADU5B failed power up test -- also bad sounding
   */
   UChar_t         tstBitMask; 
   
  ClassDef(GpsdStart,10);
};


#endif //GPSDSTART_H
