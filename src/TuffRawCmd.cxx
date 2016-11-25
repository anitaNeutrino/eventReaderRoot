#include "TuffRawCmd.h" 



ClassImp(TuffRawCmd); 


TuffRawCmd::TuffRawCmd(int run, const TuffRawCmd_t * raw) 
  : run(run) 
{
  requestedTime = raw->requestedTime; 
  enactedTime = raw->enactedTime; 
  cmd = raw->cmd; 
  irfcm = raw->irfcm; 
  tuffStack = raw->tuffStack; 
}
