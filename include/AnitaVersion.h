#ifndef _ANITA_VERSION_H
#define _ANITA_VERSION_H

/******************************************************************************
 *
 *     AnitaVersion.h
 *
 *     Cosmin Deaconu <cozzyd@kicp.uchicago.edu> Jan 2017
 *
 *    INTRODUCTION
 *  
 * This file  is part of an attempt at allowing multiple ANITA versions with
 * the same code base.
 *
 * In this header the interface for setting and querying the ANITA version is
 * exposed.  This header file will also serve as the documentation for the
 * versioning implementation progress 
 *
 * Initially 3 and 4 will be supported. 2 Could also be made to be supported
 * with a bit more work. 
 *
 *
 *   USAGE 
 *
 * You can always set the current anita version by using AnitaVersion::set()
 * (but keep in mind that it might get set again automatically). Use
 * AnitaVersion::get() to find out what the current version is set to. 
 *
 * In some cases (e.g. when using AnitaDataset, or constructing a
 * UsefulAnitaEvent from a header), the ANITA version may be automatically
 * detected and set appropriately. 
 *
 *
 * Some singleton classes have been converted to one instance per anita version
 * (i.e. AnitaEventCalibrator, AnitaGeomTool).  One must beware of reusing the
 * pointer to the instance if one is switch between ANITA versions within the
 * same context.  It is always safe to call e.g. AnitaGeomTool::Instance() and
 * that will dispatch the currently set version, albeit perhaps at the cost of
 * a performance penalty. 
 *
 * By default, the implementation is not thread-safe. If you plan on processing
 * different ANITA verisons in different threads you may define the
 * THREADSAFE_ANITA_VERSION compile-time-flag. Note that this requires compiler
 * and standard library support for thread-specific-storage and may impose a
 * small performance penalty. 
 *
 *
 *   PROGRESS MADE SO FAR
 *
 *        AnitaGeomTool dispatches Instances based on version, also checks for correct constants
 *
 *        AnitaEventCalibrator dispatches Instances based on version, also checks for correct constants
 *
 *        AnitaDataset sets version based on header each time. 
 *
 *        UsefulAnitaEvent sets version sometimes (if constructed with something that has a time). Also AlfaFilterFlag set based on version. 
 *
 *        RawAnitaHeader manual schema migration 
 *
 *        TurfRate manual schema migration and compatibility structure. 
 *
 *        SurfHK added methods for both A3 and A4 for l1 scalers.  
 *
 *          
 *   TODO 
 *      
 *      Need phase centers and such for A4 
 *  
 *      Many of the constants in AnitaConventions need to be dispatched according to version. 
 *
 *      Handle the following, which are not currently backward's compatible: 
 *
 *        CalibratedHk 
 *        Probably other stuff... 
 *        
 ******************************************************************************/ 



/** The default ANITA version may be overriden at compile time. */ 
#ifndef DEFAULT_ANITA_VERSION
#define DEFAULT_ANITA_VERSION 4 
#endif 


 /** Define this macro so other code can act accordingly*/ 
#define MULTIVERSION_ANITA_ENABLED 1


namespace AnitaVersion 
{

/** Set the ANITA Version to use for e.g. calibration.
   *  If eventReaderRoot is compiled with THREADSAFE_ANITA_VERSION,
   *  it is safe to use different ANITA versions in different threads 
   */ 

  void set(int anitaVersion); 

  /** Retrieve the current ANITA Version*/ 
  int get(); 


  /** Utility function to retrieve the anita version from the unix time. Used
   * when processing data etc. */ 
  int getVersionFromUnixTime(unsigned time); 

  /** Sets the version from unix time  (equivalent to calling set(getVersionFromUnixTime(t)) )
   *
   * returns 0 if successful, 1 if time is suspicious
   * */ 
  int setVersionFromUnixTime(unsigned time); 


} 



#endif 
