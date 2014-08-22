//////////////////////////////////////////////////////////////////////////////
/////  CommandEcho.h        CommandEcho                                    /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for storing command echoes in a tree        /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "CommandEcho.h"
#include "anitaCommand.h"
#include <iostream>
#include <fstream>
#include <cstring>

ClassImp(CommandEcho);

CommandEcho::CommandEcho() 
{
   //Default Constructor
}

CommandEcho::~CommandEcho() {
   //Default Destructor
}


CommandEcho::CommandEcho(Int_t trun, Int_t trealTime, CommandEcho_t *echoPtr)
{

  if((echoPtr->gHdr.code&BASE_PACKET_MASK)!=PACKET_CMD_ECHO ||
     echoPtr->gHdr.verId!=VER_CMD_ECHO ||
     echoPtr->gHdr.numBytes!=sizeof(CommandEcho_t)) {
    std::cerr << "Mismatched packet\n" 
	      << "code:\t" << echoPtr->gHdr.code << "\t" << PACKET_CMD_ECHO 
	      << "\nversion:\t" << echoPtr->gHdr.verId 
	      << "\t" << VER_CMD_ECHO 
	      << "\nsize:\t" << echoPtr->gHdr.numBytes << "\t"
	      << sizeof(CommandEcho_t) << std::endl;
  }
  run=trun;
  realTime=trealTime;
  payloadTime=echoPtr->unixTime;
  goodFlag=echoPtr->goodFlag;
  numCmdBytes=echoPtr->numCmdBytes;
  memcpy(cmd,echoPtr->cmd,sizeof(UChar_t)*MAX_CMD_LENGTH);
  if(echoPtr->gHdr.code & CMD_FROM_PAYLOAD) {
    fromPayload=1;
  }
  else {
    fromPayload=0;
  }
}


const char * CommandEcho::getCommandAsString() 
{
  char *cmdString;
  switch(cmd[0]) {	
  case CMD_TAIL_VAR_LOG_MESSAGES: 
    cmdString="tail /var/log/messages"; break;
  case CMD_TAIL_VAR_LOG_ANITA: 
    cmdString="tail /var/log/anita.log"; break;
  case LOG_REQUEST_COMMAND:
    cmdString="log request "; break;
  case CMD_START_NEW_RUN: 
    cmdString="Start New Run"; break;
  case CMD_SHUTDOWN_HALT: 
    cmdString="shutdown -h now (i.e halt the CPU)"; break;
  case CMD_REBOOT: 
    cmdString="reboot"; break;
  case CMD_SIPD_REBOOT: 
    cmdString="SIPd reboot"; break;
  case CMD_KILL_PROGS: 
    cmdString="Kill process with mask"; break;
  case CMD_REALLY_KILL_PROGS: 
    cmdString="Kill -9  process with mask"; break;
  case CMD_RESPAWN_PROGS:
    cmdString="Respawn process with mask"; break;
  case CMD_START_PROGS: 
    cmdString="Start process with mask"; break;
    //  case CMD_MOUNT: 
    //    cmdString="mount -a"; break;
  case CMD_DISABLE_DISK: 
    cmdString="Disable disk"; break;
  case CMD_MOUNT_NEXT_USB: 
    cmdString="Mount next usb drive"; break;
    //  case CMD_MOUNT_NEXT_SATA: 
    //    cmdString="Mount next sata drive"; break;
  case CMD_EVENT_DISKTYPE: 
    cmdString="Change Event Disk Mask"; break;
  case CMD_HK_DISKTYPE: 
    cmdString="Change Hk Disk Mask"; break;
  case ARCHIVE_STORAGE_TYPE: 
    cmdString="Change File Format On Disk"; break;
  case ARCHIVE_PRI_DISK: 
    cmdString="Change Disk Mask for Priority"; break;
  case ARCHIVE_PPS_PRIORITIES: 
    cmdString="Set Priority for PPS/Software triggers"; break;
  case ARCHIVE_PPS_DECIMATE: 
    cmdString="Set Decimation for PPS/Software triggers"; break;
  case ARCHIVE_PRI_ENC_TYPE:
    cmdString="Change Disk Encoding Type for Priority"; break;
  case ARCHIVE_DECIMATE_PRI: 
    cmdString="Change Decimation Fraction for Priority"; break; //Not implemented
  case ARCHIVE_GLOBAL_DECIMATE: 
    cmdString="Change Global Decimation Fraction for Priority"; break; //Not implemented
  case TELEM_TYPE: 
    cmdString="Change Telem Waveform Format"; break;
  case TELEM_PRI_ENC_TYPE: 
    cmdString="Change Telem Encoding Type for Priority"; break;
  case CMD_TURN_GPS_ON: 
    cmdString="Turn GPS On"; break;
  case CMD_TURN_GPS_OFF: 
    cmdString="Turn GPS Off"; break;
  case CMD_TURN_RFCM_ON: 
    cmdString="Turn RFCM On"; break;
  case CMD_TURN_RFCM_OFF: 
    cmdString="Turn RFCM Off"; break;
  case CMD_TURN_CALPULSER_ON: 
    cmdString="Turn CalPulser On"; break;
  case CMD_TURN_CALPULSER_OFF: 
    cmdString="Turn CalPulser Off"; break;
  case CMD_TURN_VETO_ON: 
    cmdString="Turn Veto On"; break;
  case CMD_TURN_VETO_OFF: 
    cmdString="Turn Veto Off"; break;
  case CMD_TURN_ALL_ON: 
    cmdString="Turn All On"; break;
  case CMD_TURN_ALL_OFF: 
    cmdString="Turn All Off"; break;
  case SET_CALPULSER_SWITCH: 
    cmdString="Set CalPulser RF Switch to Port"; break;
  case SET_CALPULSER_ATTEN: 
    cmdString="Set CalPulser Attenuation to"; break;
  case SET_ATTEN_LOOP_PERIOD: 
    cmdString="Set CalPulser Attenuation Loop Period"; break;
  case SET_SWITCH_LOOP_PERIOD: 
    cmdString="Set CalPulser RF Switch Loop Period"; break;
  case SET_PULSER_OFF_PERIOD: 
    cmdString="Set CalPulser Off Period"; break;
  case SET_CALIB_WRITE_PERIOD: 
    cmdString="Set Calibd Write Period"; break;
  case SET_ADU5_PAT_PERIOD: 
    cmdString="Set ADU5 Attitude Readout Period"; break;
  case SET_ADU5_SAT_PERIOD: 
    cmdString="Set ADU5 Satellite Info Readout Period"; break;
  case SET_G12_PPS_PERIOD: 
    cmdString="Set G12 PPS Period"; break;
  case SET_G12_PPS_OFFSET: 
    cmdString="Set G12 PPS Offset"; break;
  case SET_ADU5_CALIBRATION_12: 
    cmdString="Set ADU5 Calibration Vector 1->2"; break;
  case SET_ADU5_CALIBRATION_13: 
    cmdString="Set ADU5 Calibration Vector 1->3"; break;
  case SET_ADU5_CALIBRATION_14: 
    cmdString="Set ADU5 Calibration Vector 1->4"; break;
  case SET_ADU5_VTG_PERIOD: 
    cmdString="Set ADU5 Velocity Readout Period"; break;
  case SET_G12_POS_PERIOD: 
    cmdString="Set G12 Position Readout Period"; break;
  case SET_HK_PERIOD: 
    cmdString="Set Acromag Housekeeping Readout Period"; break;
  case SET_HK_CAL_PERIOD: 
    cmdString="Set Acromag Calibration Period"; break;
  case CLEAN_DIRS: 
    cmdString="Clean Packet Dirs"; break;
  case SEND_CONFIG: 
    cmdString="Send Config File Over Telemetry"; break;
  case DEFAULT_CONFIG:
    cmdString="Switch to Default Config"; break;
  case SWITCH_CONFIG:
    cmdString="Switch to Alternate Config File "; break;
  case LAST_CONFIG:
    cmdString="Switch to Last Config File"; break;
  case ACQD_ADU5_TRIG_FLAG: 
    cmdString="Set ADU5 PPS Trigger Flag"; break;
  case ACQD_G12_TRIG_FLAG: 
    cmdString="Set G12 PPS Trigger Flag"; break;
  case ACQD_SOFT_TRIG_FLAG: 
    cmdString="Set Software Trigger Flag"; break;
  case ACQD_SOFT_TRIG_PERIOD: 
    cmdString="Set Software Trigger Period"; break;
  case ACQD_PEDESTAL_RUN: 
    cmdString="Start Pedestal Run"; break;
  case ACQD_THRESHOLD_SCAN:
    cmdString="Start Threshold Scan"; break;
  case ACQD_REPROGRAM_TURF: 
    cmdString="Enable TURF Reprogramming"; break;
  case ACQD_SURFHK_PERIOD: 
    cmdString="Set SurfHk Write Period"; break;
  case ACQD_SURFHK_TELEM_EVERY: 
    cmdString="Set SurfHK Telemetry Duty Factor"; break;
  case ACQD_TURFHK_TELEM_EVERY:
    cmdString="Set Turf Rate Telemetry Duty Factor"; break;
  case ACQD_NUM_EVENTS_PEDESTAL: 
    cmdString="Set Number of Pedestal Events"; break;
  case ACQD_THRESH_SCAN_STEP_SIZE: 
    cmdString="Set Threshold Scan Step Size"; break;
  case ACQD_THRESH_SCAN_POINTS_PER_STEP: 
    cmdString="Set Threshold Scan Points Per Step"; break;
  case GPSD_EXTRA_COMMAND:
    cmdString="Extra GPSd command"; break;
  case SET_HK_TELEM_EVERY:
    cmdString="Set Hk. Telem Every"; break;
  case SIPD_CONTROL_COMMAND:
    cmdString="SIPd Control command"; break;
  case LOSD_CONTROL_COMMAND:
    cmdString="LOSd Control command"; break;
  case CLEAR_RAMDISK:
    cmdString="Clear Ramdisk"; break;
  case SAVE_CONFIG:
    cmdString="Save config"; break;
  case MONITORD_RAMDISK_KILL_ACQD:
    cmdString="Ramdisk space when we stop Acqd"; break;
  case MONITORD_RAMDISK_DUMP_DATA:
    cmdString="Ramdisk space when we clear the ramdisk"; break;
  case MONITORD_MAX_ACQD_WAIT:
    cmdString="Max time to wait before restarting Acqd"; break;
  case MONITORD_PERIOD:
    cmdString="Time between CPU Monitor packets"; break;
  case MONITORD_USB_THRESH:
    cmdString="Threshold to switch USBs"; break;
    //  case MONITORD_SATA_THRESH:
    //    cmdString="Threshold to switch SATAs"; break;
  case MONITORD_MAX_QUEUE:
    cmdString="Maximum inks before deleting in telem queues"; break;
  case MONITORD_INODES_KILL_ACQD:
    cmdString="Ramdisk inodes remaining before stopping Acqd"; break;
  case MONITORD_INODES_DUMP_DATA:
    cmdString="Ramdisk inodes remaining before clearing"; break;
  case ACQD_EXTRA_COMMAND:
    cmdString="Extra Acqd Commands"; break;
  case ACQD_RATE_COMMAND:
    cmdString="Acqd Rate Control Command"; break;
  case EVENTD_MATCH_GPS:
    cmdString="Eventd Match GPS"; break;
  case GPS_PHI_MASK_COMMAND:
    cmdString="GPSd Phi Mask Command"; break;
  case PRIORITIZERD_COMMAND:
    cmdString="Jim's Prioritizer Tweaking command"; break;
  case PLAYBACKD_COMMAND:
    cmdString="Playback Command"; break;
  default:
    cmdString="Unknown Command";
    break;

  }
  return cmdString;    
} 
