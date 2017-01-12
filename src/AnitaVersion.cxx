#include "AnitaVersion.h" 

//uncomment this to get a whole bunch of printouts  (or you can define it while compiling) 
//#define ANITA_VERSION_DEBUG 1

#include <stdio.h>




/** This is the thread-safe version, where multiple threads may be working
* with different ANITA versions at the same time.  It may have a
* performance penalty. Also, if you're using a MacOs X system and this
* doesn't work, try to get a better clang (either via Homebrew or XCode 8
* supposedly finally supports thread-local-storage, despite open-source
* clang supporting it for years) */

#ifdef THREADSAFE_ANITA_VERSION
  static __thread int version = DEFAULT_ANITA_VERSION;

#else 

/* If accessing different versions with different threads is unnnecessary, we
* can use a slightly optimized version here. More importantly, this is more
* likely to compile on someone's MacOS X system. 
*
*
*  I tried making this an extern in the header file, but that didn't work so well with inlining 
*  which would mostly defeat the point. 
*/

  static int version = DEFAULT_ANITA_VERSION; 
#endif 
 

/** These are the boundaries for setting the ANITA version based on timestamp */ 
static const unsigned version_cutoffs[] = 
{ 0,    //Just padding
  1104537600,    //Jan 1, 2005  , ANITA-1 data will be newer
  1199145600,   // Jan 1, 2008, ANITA-2 data will be newer
  1388534400,   // Jan 1, 2014, ANITA-3 data will be newer
  1451606400   // Jan 1, 2016, ANITA-4 data will be newer
}; 



void AnitaVersion::set(int v) 
{ 
#ifdef ANITA_VERSION_DEBUG 
  printf(":::DEBUG:::   Called AnitaVersion::set(%d)\n", v); 
#endif
  version = v; 
} 

int AnitaVersion::get() 
{ 
  return version; 
} 

int AnitaVersion::setVersionFromUnixTime(unsigned t) 
{
  int v = getVersionFromUnixTime(t); 
  if (v>0) 
  {
    set(v); 
    return 0; 
  }
  else
  {
    fprintf(stderr,"Attempting to set a version based on suspicious timestamp: %u\n", t); 
    return 1; 
  }
}

int AnitaVersion::getVersionFromUnixTime(unsigned t) 
{
  for (int v = 4; v>0; v--)
  {
    if (t > version_cutoffs[v]) 
    {
      return v; 
    }
  }

  return -1; 
}

