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
  switch(cmd[0]) {	
  case CMD_TAIL_VAR_LOG_MESSAGES: 
    return "tail /var/log/messages"; 
  case CMD_TAIL_VAR_LOG_ANITA: 
    return "tail /var/log/anita.log"; 
  case LOG_REQUEST_COMMAND:
    return "log request "; 
  case CMD_START_NEW_RUN: 
    return "Start New Run"; 
  case CMD_SHUTDOWN_HALT: 
    return "shutdown -h now (i.e halt the CPU)"; 
  case CMD_REBOOT: 
    return "reboot"; 
  case CMD_SIPD_REBOOT: 
    return "SIPd reboot"; 
  case CMD_KILL_PROGS: 
    return "Kill process with mask"; 
  case CMD_REALLY_KILL_PROGS: 
    return "Kill -9  process with mask"; 
  case CMD_RESPAWN_PROGS:
    return "Respawn process with mask"; 
  case CMD_START_PROGS: 
    return "Start process with mask"; 
    //  case CMD_MOUNT: 
    //    return "mount -a"; 
  case CMD_DISABLE_DISK: 
    return "Disable disk"; 
  case CMD_MOUNT_NEXT_USB: 
    return "Mount next usb drive"; 
    //  case CMD_MOUNT_NEXT_SATA: 
    //    return "Mount next sata drive"; 
  case CMD_EVENT_DISKTYPE: 
    return "Change Event Disk Mask"; 
  case CMD_HK_DISKTYPE: 
    return "Change Hk Disk Mask"; 
  case ARCHIVE_STORAGE_TYPE: 
    return "Change File Format On Disk"; 
  case ARCHIVE_PRI_DISK: 
    return "Change Disk Mask for Priority"; 
  case ARCHIVE_PPS_PRIORITIES: 
    return "Set Priority for PPS/Software triggers"; 
  case ARCHIVE_PPS_DECIMATE: 
    return "Set Decimation for PPS/Software triggers"; 
  case ARCHIVE_PRI_ENC_TYPE:
    return "Change Disk Encoding Type for Priority"; 
  case ARCHIVE_DECIMATE_PRI: 
    return "Change Decimation Fraction for Priority";  //Not implemented
  case ARCHIVE_GLOBAL_DECIMATE: 
    return "Change Global Decimation Fraction for Priority";  //Not implemented
  case TELEM_TYPE: 
    return "Change Telem Waveform Format"; 
  case TELEM_PRI_ENC_TYPE: 
    return "Change Telem Encoding Type for Priority"; 
  case CMD_TURN_GPS_ON: 
    return "Turn GPS On"; 
  case CMD_TURN_GPS_OFF: 
    return "Turn GPS Off"; 
  case CMD_TURN_RFCM_ON: 
    return "Turn RFCM On"; 
  case CMD_TURN_RFCM_OFF: 
    return "Turn RFCM Off"; 
  case CMD_TURN_CALPULSER_ON: 
    return "Turn CalPulser On"; 
  case CMD_TURN_CALPULSER_OFF: 
    return "Turn CalPulser Off"; 
  case CMD_TURN_VETO_ON: 
    return "Turn Veto On"; 
  case CMD_TURN_VETO_OFF: 
    return "Turn Veto Off"; 
  case CMD_TURN_ALL_ON: 
    return "Turn All On"; 
  case CMD_TURN_ALL_OFF: 
    return "Turn All Off"; 
  case SET_CALPULSER_SWITCH: 
    return "Set CalPulser RF Switch to Port"; 
  case SET_CALPULSER_ATTEN: 
    return "Set CalPulser Attenuation to"; 
  case SET_ATTEN_LOOP_PERIOD: 
    return "Set CalPulser Attenuation Loop Period"; 
  case SET_SWITCH_LOOP_PERIOD: 
    return "Set CalPulser RF Switch Loop Period"; 
  case SET_PULSER_OFF_PERIOD: 
    return "Set CalPulser Off Period"; 
  case SET_CALIB_WRITE_PERIOD: 
    return "Set Calibd Write Period"; 
  case SET_ADU5_PAT_PERIOD: 
    return "Set ADU5 Attitude Readout Period"; 
  case SET_ADU5_SAT_PERIOD: 
    return "Set ADU5 Satellite Info Readout Period"; 
  case SET_G12_PPS_PERIOD: 
    return "Set G12 PPS Period"; 
  case SET_G12_PPS_OFFSET: 
    return "Set G12 PPS Offset"; 
  case SET_ADU5_CALIBRATION_12: 
    return "Set ADU5 Calibration Vector 1->2"; 
  case SET_ADU5_CALIBRATION_13: 
    return "Set ADU5 Calibration Vector 1->3"; 
  case SET_ADU5_CALIBRATION_14: 
    return "Set ADU5 Calibration Vector 1->4"; 
  case SET_ADU5_VTG_PERIOD: 
    return "Set ADU5 Velocity Readout Period"; 
  case SET_G12_POS_PERIOD: 
    return "Set G12 Position Readout Period"; 
  case SET_HK_PERIOD: 
    return "Set Acromag Housekeeping Readout Period"; 
  case SET_HK_CAL_PERIOD: 
    return "Set Acromag Calibration Period"; 
  case CLEAN_DIRS: 
    return "Clean Packet Dirs"; 
  case SEND_CONFIG: 
    return "Send Config File Over Telemetry"; 
  case DEFAULT_CONFIG:
    return "Switch to Default Config"; 
  case SWITCH_CONFIG:
    return "Switch to Alternate Config File "; 
  case LAST_CONFIG:
    return "Switch to Last Config File"; 
  case ACQD_ADU5_TRIG_FLAG: 
    return "Set ADU5 PPS Trigger Flag"; 
  case ACQD_G12_TRIG_FLAG: 
    return "Set G12 PPS Trigger Flag"; 
  case ACQD_SOFT_TRIG_FLAG: 
    return "Set Software Trigger Flag"; 
  case ACQD_SOFT_TRIG_PERIOD: 
    return "Set Software Trigger Period"; 
  case ACQD_PEDESTAL_RUN: 
    return "Start Pedestal Run"; 
  case ACQD_THRESHOLD_SCAN:
    return "Start Threshold Scan"; 
  case ACQD_REPROGRAM_TURF: 
    return "Enable TURF Reprogramming"; 
  case ACQD_SURFHK_PERIOD: 
    return "Set SurfHk Write Period"; 
  case ACQD_SURFHK_TELEM_EVERY: 
    return "Set SurfHK Telemetry Duty Factor"; 
  case ACQD_TURFHK_TELEM_EVERY:
    return "Set Turf Rate Telemetry Duty Factor"; 
  case ACQD_NUM_EVENTS_PEDESTAL: 
    return "Set Number of Pedestal Events"; 
  case ACQD_THRESH_SCAN_STEP_SIZE: 
    return "Set Threshold Scan Step Size"; 
  case ACQD_THRESH_SCAN_POINTS_PER_STEP: 
    return "Set Threshold Scan Points Per Step"; 
  case GPSD_EXTRA_COMMAND:
    return "Extra GPSd command"; 
  case SET_HK_TELEM_EVERY:
    return "Set Hk. Telem Every"; 
  case SIPD_CONTROL_COMMAND:
    return "SIPd Control command"; 
  case LOSD_CONTROL_COMMAND:
    return "LOSd Control command"; 
  case CLEAR_RAMDISK:
    return "Clear Ramdisk"; 
  case SAVE_CONFIG:
    return "Save config"; 
  case MONITORD_RAMDISK_KILL_ACQD:
    return "Ramdisk space when we stop Acqd"; 
  case MONITORD_RAMDISK_DUMP_DATA:
    return "Ramdisk space when we clear the ramdisk"; 
  case MONITORD_MAX_ACQD_WAIT:
    return "Max time to wait before restarting Acqd"; 
  case MONITORD_PERIOD:
    return "Time between CPU Monitor packets"; 
  case MONITORD_USB_THRESH:
    return "Threshold to switch USBs"; 
    //  case MONITORD_SATA_THRESH:
    //    return "Threshold to switch SATAs"; 
  case MONITORD_MAX_QUEUE:
    return "Maximum inks before deleting in telem queues"; 
  case MONITORD_INODES_KILL_ACQD:
    return "Ramdisk inodes remaining before stopping Acqd"; 
  case MONITORD_INODES_DUMP_DATA:
    return "Ramdisk inodes remaining before clearing"; 
  case ACQD_EXTRA_COMMAND:
    return "Extra Acqd Commands"; 
  case ACQD_RATE_COMMAND:
    return "Acqd Rate Control Command"; 
  case EVENTD_MATCH_GPS:
    return "Eventd Match GPS"; 
  case GPS_PHI_MASK_COMMAND:
    return "GPSd Phi Mask Command"; 
  case PRIORITIZERD_COMMAND:
    return "Jim's Prioritizer Tweaking command"; 
  case PLAYBACKD_COMMAND:
    return "Playback Command"; 
  default:
    return "Unknown Command";
    

  }  
} 
