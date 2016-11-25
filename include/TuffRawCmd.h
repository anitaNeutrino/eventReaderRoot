/* 
 *  TUFF Raw Command telemetry
 *  Ideally we should never have to use these. 
 */ 

#ifndef TUFF_RAW_CMD_HH
#define TUFF_RAW_CMD_HH


#include "TObject.h" 
#include "simpleStructs.h" 


class TuffRawCmd: public TObject
{
  public: 

    TuffRawCmd(int run, const TuffRawCmd_t * raw) ; 
    virtual ~TuffRawCmd() { ; } 

    unsigned int getRequestedTime() const { return requestedTime; }
    unsigned int getEnactedTime() const  { return enactedTime; } 
    unsigned short getCmd() const { return cmd; } 
    unsigned char getirfcm() const { return irfcm; }   
    unsigned char getTuffStack() const { return tuffStack; }
    int getRun() const { return run; } 
 
 


  private: 
    unsigned int requestedTime; 
    unsigned int enactedTime; 
    unsigned short cmd; 
    unsigned char irfcm;   
    unsigned char tuffStack;
    int run;

    ClassDef(TuffRawCmd,1); 
  };




#endif
