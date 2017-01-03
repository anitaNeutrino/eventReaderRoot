#ifndef _ANITA_VERSION_H
#define _ANITA_VERSION_H

/** 
 * This is an attempt at allowing 
 * multiple ANITA versions with the same code base. This file doesn't do much
 * by itself, but other things can use set() and get() to select an ANITA 
 * version or do something version-dependent. 
 *
 * Initially 3 and 4 will be supported. 2 Could also be made to be 
 * supported with a bit more work. 
 *
 *
 *  Cosmin Deaconu <cozzyd@kicp.uchicago.edu> 
 */ 


namespace AnitaVersion
{
  /** Set the ANITA Version to use for e.g. calibration.
   *  This is a thread-specifi value, so multiple threads
   *  could process different ANITA versions.   
   */ 
  void set(int anitaVersion); 


  /** Retrieve the current ANITA Version*/ 
  int get(); 



} 



#endif 
