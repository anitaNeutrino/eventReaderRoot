/* 
 * TuffStatus.h     TUFF Notch status and temperature
 * 
 *   Stores infromation about TUFF notches
 */

#ifndef ANITA_TUFF_NOTCH_STATUS_H
#define ANITA_TUFF_NOTCH_STATUS_H

#include "TObject.h" 
#include "simpleStructs.h" 
#include "AnitaConventions.h" 


class TuffNotchStatus : public TObject
{
  public:

    TuffNotchStatus(Int_t run, const TuffNotchStatus_t * status); 
    TuffNotchStatus() { ; } 
    virtual ~TuffNotchStatus() {;} 

    char getIRFCMTemperature(int i) const { return temperatures[i]; } 
    char getStartSector(int inotch) const { return startSectors[inotch] == 16 ? -1 : startSectors[inotch]; }
    char getEndSector(int inotch) const { return endSectors[inotch] == 16 ? -1 : endSectors[inotch]; }
    unsigned int getUnixTime() const { return unixTime; } 
    unsigned int getNotchSetTime() const { return notchSetTime; } 
    unsigned short getNotchedSectors(int inotch) const; 
    Int_t getRun() const { return run; } 

  private:
    Int_t run;
    unsigned int notchSetTime;
    unsigned int unixTime; 
    unsigned char startSectors[NUM_TUFF_NOTCHES]; 
    unsigned char endSectors[NUM_TUFF_NOTCHES]; 
    char temperatures[NUM_RFCM]; 



    ClassDef(TuffNotchStatus, 1); 

}; 

      

#endif
