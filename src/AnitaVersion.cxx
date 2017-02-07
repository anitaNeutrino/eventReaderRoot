#include "AnitaVersion.h" 

/*  Use TSD here for thread safety. */
static __thread int anitaVersion = 4; 

int AnitaVersion::get() { return anitaVersion; } 
void AnitaVersion::set(int v) { anitaVersion = v; } 

