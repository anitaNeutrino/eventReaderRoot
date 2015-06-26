//////////////////////////////////////////////////////////////////////////////
/////  AnitaEventCalibrator.cxx        ANITA Event Calibrator            /////
/////                                                                    /////
/////  Description:                                                      /////
/////     A simple class for calibrating UsefulAnitaEvents               /////
/////  Author: Ryan Nichol (rjn@hep.ucl.ac.uk)                           /////
//////////////////////////////////////////////////////////////////////////////

#include "AnitaEventCalibrator.h"
#include "UsefulAnitaEvent.h"

ClassImp(AnitaEventCalibrator);

AnitaEventCalibrator*  AnitaEventCalibrator::fgInstance = nullptr;

AnitaEventCalibrator::AnitaEventCalibrator(){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;
  std::cout << "AnitaEventCalibrator::AnitaEventCalibrator()" << std::endl;
  loadCalib();

  const Int_t bufferSize = 3600; ///< An hour of averging at 1Hz, worked for calibration
  clockPeriodRingBuffer = new RingBuffer(bufferSize); 

  firstHitBusRcoLatchLimit = 40; ///< How closely to trust RCO.
  // dtInterp = 0.001; ///< in nanoseconds, for clock alignment interpolation  
  dtInterp = 0.01; ///< in nanoseconds, for clock alignment interpolation  
  nominalDeltaT = 1./2.6; ///< in nanoseconds

  // Set up empty vectors for clock alignment
  for(Int_t surf=0; surf<NUM_SURF; surf++){
    defaultClocksToAlign.push_back(surf);
    clockAlignment.push_back(0);

    grClocks.push_back(NULL);
    grClockInterps.push_back(NULL);
    grCorClock.push_back(NULL);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////
  fTempFactorGuess = 1; ///< Very important this doesn't start at 0, things will break if it does!
  ////////////////////////////////////////////////////////////////////////////////////////////////
  fClockProblem = 0; ///< If we get more than reasonable zero crossings raise flag to skip updating temp correction

}

AnitaEventCalibrator::~AnitaEventCalibrator(){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;
  delete clockPeriodRingBuffer;
  deleteClockAlignmentTGraphs();
};


//______________________________________________________________________________
AnitaEventCalibrator*  AnitaEventCalibrator::Instance(){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;
  if(fgInstance==nullptr){
    fgInstance = new AnitaEventCalibrator();
  }

  return fgInstance;
}





Int_t AnitaEventCalibrator::calibrateUsefulEvent(UsefulAnitaEvent *eventPtr, 
						 WaveCalType::WaveCalType_t calType){  


  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;
  //! The plan for calibration happiness:
  //! Step 1: figure out from WaveCalType_t calType exactly what we're going to do
  //! Step 2: Figure out RCO phase from clock period (if bin-to-bin dts required)
  //! Step 3: Update rolling temperature correction (copy to event)
  //! Step 4: Unwrap all channels (if requested)
  //! Step 5: Apply bin-to-bin timing (if requested)
  //! Step 6: Apply voltage calib (if requested)
  //! Step 7: Find trigger jitter correction
  //! Step 8: Zero mean all non-clock channels
  //! Step 9: Apply channel-to-channel cable delays
  //! Step 10: Copy everything to UsefulAnitaEvent





  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //! Step 1: figure out from WaveCalType_t calType exactly what we're going to do.
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Initial states for calibration flags, modified based on calType below
  Bool_t fUnwrap = false;
  Bool_t fBinToBinDts = false;
  Bool_t fNominal = false;
  Bool_t fApplyTempCorrection = true;
  Bool_t fVoltage = false;
  Bool_t fApplyTriggerJitterCorrection = false;
  Bool_t fApplyCableDelays = false;
  Bool_t fZeroMeanNonClockChannels = false;
  Bool_t fFlipRcoPhase = false;
  Bool_t fFirmwareRcoNoLatch = false;

  switch(calType){

  case WaveCalType::kNotACalib:
    std::cerr << "Warning in " << __PRETTY_FUNCTION__ << ", calling kNotACalib will be treated as kNoCalib"
	      << std::endl;

  case WaveCalType::kNoCalib: 
    fApplyTempCorrection = false;
      break;

  case WaveCalType::kNominal:
    fBinToBinDts = true;
    fApplyTempCorrection = true;
    break;

  case WaveCalType::kJustTimeNoUnwrap:
    fBinToBinDts = true;
    fApplyTempCorrection = true;
    break;

  case WaveCalType::kJustUnwrap:
    fUnwrap = true;
    break;

  case WaveCalType::kNoTriggerJitterNoZeroMean:
    fUnwrap = true;
    fBinToBinDts = true;
    fVoltage = true;
    fApplyTempCorrection = true;
    break;

  case WaveCalType::kNoTriggerJitterNoZeroMeanFlipRco:
    fUnwrap = true;
    fBinToBinDts = true;
    fVoltage = true;
    fFlipRcoPhase = true;
    break;

  case WaveCalType::kNoTriggerJitterNoZeroMeanFirmwareRco:
    fUnwrap = true;
    fBinToBinDts = true;
    fVoltage = true;
    fApplyTempCorrection = true;
    fFirmwareRcoNoLatch = true;
    break;

  case WaveCalType::kNoTriggerJitterNoZeroMeanFirmwareRcoFlipped:
    fUnwrap = true;
    fBinToBinDts = true;
    fVoltage = true;
    //    fApplyTempCorrection = false;
    fFirmwareRcoNoLatch = true;
    fFlipRcoPhase = true;
    break;    

  case WaveCalType::kNoChannelToChannelDelays:
    fUnwrap = true;
    fBinToBinDts = true;
    fApplyTempCorrection = true;
    fVoltage = true;
    fApplyTriggerJitterCorrection = true;
    fApplyCableDelays = false;
    fZeroMeanNonClockChannels = true;
    break;

  case WaveCalType::kFull:
    fUnwrap = true;
    fBinToBinDts = true;
    fApplyTempCorrection = true;
    fVoltage = true;
    fApplyTriggerJitterCorrection = true;
    fApplyCableDelays = true;
    fZeroMeanNonClockChannels = true;
    break;

  // case WaveCalType::kDefault:
  //   std::cerr << "It seems like WaveCal::kDefault isn't handled by any of the options in " 
  // 	      << __PRETTY_FUNCTION__ << std::endl;

  }




  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //! Step 2: Figure out RCO phase from clock period (if bin-to-bin dts required)
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  fClockProblem = 0; ///< If we have an issue with the clock (more than 5 zero crossings!) then we raise this flag to not update rolling average temperature correction.
  if(fBinToBinDts==true){
    // If we want deltaTs then we definitely need to know what RCO phase we are in
    if(eventPtr->fFromCalibratedAnitaEvent==0){ // Figure out the RCO if we don't have calibrated input 
      guessRco(eventPtr);
    }
    else{// If have calibrated anita event, then copy result and don't guess
      for(Int_t surf=0; surf<NUM_SURF; surf++){
	rcoArray[surf] = eventPtr->fRcoArray[surf];
      }
    }

    // For calibration purposes...
    if(fFirmwareRcoNoLatch==true){
      for(Int_t surf=0; surf<NUM_SURF; surf++){
	rcoArray[surf] = eventPtr->getRcoCorrected(surf*NUM_CHAN + 8);
      }
    }
    // Sanity check for calibration purposes... don't ever use this.
    if(fFlipRcoPhase==true){
      for(Int_t surf=0; surf<NUM_SURF; surf++){
	rcoArray[surf] = 1 - rcoArray[surf];
      }
    }

    // Copy to rco guessing array in UsefulAnitaEvent
    for(Int_t surf=0; surf<NUM_SURF; surf++){
      eventPtr->fRcoArray[surf] = rcoArray[surf];
    }
  }






  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //! Step 3: Update rolling temperature correction (copy to usefulAnitaEvent)
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(eventPtr->fFromCalibratedAnitaEvent==0){
    if(fClockProblem!=0){
      std::cerr << "Clock problem found in eventNumber = " << eventPtr->eventNumber
		<< ", skipped temperature correction" << std::endl;
    }
    else{
      updateTemperatureCorrection(); ///< This is pretty important as RCO guess needs accurate clock 
    }
    fTempFactorGuess = getTempFactor();
    eventPtr->fTempFactorGuess = fTempFactorGuess; 
  }
  else{
    fTempFactorGuess = eventPtr->fTempFactorGuess;
  }






  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //! Steps 4: Unwrap all channels (if requested)
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(fUnwrap==true){
    // Then we call the unwrapping function for every channel

    for(Int_t surf = 0; surf < NUM_SURF; surf++){
      for(Int_t chan = 0; chan < NUM_CHAN; chan++){
	numPointsArray[surf] = unwrapChannel(eventPtr, surf, chan,
					     rcoArray[surf], fApplyTempCorrection,
					     voltsArray[surf][chan],
					     timeArray[surf],
					     scaArray[surf]);
      }
    }
  }
  else if(fUnwrap==false){// Then we fill in the volt/time arrays with linearly increasing data points 

    Double_t tempFactor = fTempFactorGuess; //getTempFactor();
    for(Int_t surf = 0; surf < NUM_SURF; surf++){
      numPointsArray[surf] = NUM_SAMP;
      Int_t clockIndex = surf*NUM_CHAN + 8;
      Int_t labChip = eventPtr->getLabChip(clockIndex);
      for(Int_t samp=0; samp < numPointsArray[surf]-1; samp++){
	timeArray[surf][samp+1] = (timeArray[surf][samp] 
				   + deltaTs[surf][labChip][rcoArray[surf]][samp]*tempFactor);
      }

      for(Int_t chan = 0; chan < NUM_CHAN; chan++){
	Int_t chanIndex = surf*NUM_CHAN + chan;
	for(Int_t samp=0; samp < numPointsArray[surf]; samp++){
	  voltsArray[surf][chan][samp] = eventPtr->data[chanIndex][samp];
	  scaArray[surf][samp] = samp;
	}
      }
    }
  }








  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //! Step 5: Apply bin-to-bin timing (if requested)
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Actually all the bin-to-bin timings are put in the time array by default..
  // so if they're not wanted we can take them out again.
  if(fBinToBinDts==false){
    if(fNominal==true){
      for(Int_t surf = 0; surf < NUM_SURF; surf++){
	for(Int_t samp=0; samp < numPointsArray[surf]; samp++){
	  timeArray[surf][samp] = nominalDeltaT*samp;
	}
      }
    }
    else{ // just sample number
      for(Int_t surf = 0; surf < NUM_SURF; surf++){
	for(Int_t samp=0; samp < numPointsArray[surf]; samp++){
	  timeArray[surf][samp] = samp;
	}
      }
    }
  }








  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //! Step 6: Apply voltage calib (if requested)
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(fVoltage==true){
    applyVoltageCalibration(eventPtr);
  }











  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //! Step 7: Find trigger jitter correction
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(fApplyTriggerJitterCorrection==true){
    if(eventPtr->fFromCalibratedAnitaEvent==0){ 
      // If we don't have the calibration then get correction
      getClockAlignment(eventPtr, numPointsArray, voltsArray, timeArray);
      // And copy to relevant array
      for(Int_t surf=0; surf<NUM_SURF; surf++){
	eventPtr->fClockPhiArray[surf] = clockAlignment.at(surf);
      }
    }
    else{ // If we have the calibration then just copy the result
      for(Int_t surf=0; surf<NUM_SURF; surf++){
	clockAlignment.at(surf) = eventPtr->fClockPhiArray[surf];
      }
    }

    for(Int_t surf=0; surf<NUM_SURF; surf++){
      for(Int_t samp=0; samp < numPointsArray[surf]; samp++){
	// Update time base for each SURF with the trigger jitter
	timeArray[surf][samp] -= clockAlignment.at(surf);
      }
    }
  }








  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //! Step 8: Zero mean all non-clock channels
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  if(fZeroMeanNonClockChannels==true){
    zeroMeanNonClockChannels();
  }









  // Combine last two steps...
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  //! Step 9: Apply channel-to-channel cable delays
  //! Step 10: Copy voltage, time (with cable delays) & capacitor arrays to UsefulAnitaEvent
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(Int_t surf=0; surf<NUM_SURF; surf++){
    eventPtr->fRcoArray[surf] = rcoArray[surf];
    for(Int_t chan=0; chan<NUM_CHAN; chan++){
      Int_t chanIndex = surf*CHANNELS_PER_SURF + chan;

      eventPtr->fNumPoints[chanIndex] = numPointsArray[surf];

      Double_t cableDelay = 0;
      if(fApplyCableDelays==true){
	cableDelay = relativeChannelDelays[surf][chan];
      }

      for(Int_t samp=0; samp<numPointsArray[surf]; samp++){
	eventPtr->fTimes[chanIndex][samp] = timeArray[surf][samp] + cableDelay;
	eventPtr->fVolts[chanIndex][samp] = voltsArray[surf][chan][samp];
	eventPtr->fCapacitorNum[chanIndex][samp] = scaArray[surf][samp];
      }
    }
  }

  eventPtr->fCalType = calType;
  eventPtr->fClockProblem = fClockProblem;

  // Enjoy your calibrated event


  return 0;
}




void AnitaEventCalibrator::applyVoltageCalibration(UsefulAnitaEvent* eventPtr){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;
  for(Int_t surf=0; surf<NUM_SURF; surf++){
    for(Int_t chan=0; chan<RFCHAN_PER_SURF; chan++){
      Int_t chanIndex = surf*NUM_CHAN + chan;
      Int_t labChip = eventPtr->getLabChip(chanIndex);
      for(Int_t samp=0; samp<numPointsArray[surf]; samp++){
	// std::cout << voltsArray[surf][chan][samp] << "\t";
	voltsArray[surf][chan][samp]*=mvCalibVals[surf][chan][labChip];
	// std::cout << voltsArray[surf][chan][samp] << "\tsurf=" << surf << "\tchan=" << chan 
	// 	  << "\tchanIndex=" << chanIndex << "\tlabChip=" << labChip << "\tnumPoints=" 
	// 	  << numPointsArray[surf] << "\tRFCHAN_PER_SURF=" << RFCHAN_PER_SURF
	// 	  << "\tmvCalibVals[" << surf << "][" << chan << "]["<< labChip << "] = " 
	// 	  << mvCalibVals[surf][chan][labChip] << std::endl;
      }
    }
  }
}



void AnitaEventCalibrator::zeroMeanNonClockChannels(){
  for(Int_t surf=0; surf<NUM_SURF; surf++){
    for(Int_t chan=0; chan<RFCHAN_PER_SURF; chan++){
      Double_t chanMean = TMath::Mean(numPointsArray[surf], voltsArray[surf][chan]);
      for(Int_t samp=0; samp<numPointsArray[surf]; samp++){
	voltsArray[surf][chan][samp] -= chanMean;
      }
    }    
  }
}


std::vector<Double_t> AnitaEventCalibrator::getClockAlignment(UsefulAnitaEvent* eventPtr,
							      Int_t numPoints[NUM_SURF],
							      Double_t volts[NUM_SURF][NUM_CHAN][NUM_SAMP],
							      Double_t times[NUM_SURF][NUM_SAMP]){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;
  return getClockAlignment(eventPtr, numPoints, volts, times, defaultClocksToAlign);
}



void AnitaEventCalibrator::deleteClockAlignmentTGraphs(){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;
  for(Int_t surf=0; surf<NUM_SURF; surf++){
    // std::cout << surf << std::endl;
    clockAlignment.at(surf) = 0;
    if(grClocks.at(surf)) {
      delete grClocks.at(surf);
      grClocks.at(surf) = NULL;
    }
    if(grClockInterps.at(surf)) {
      delete grClockInterps.at(surf);
      grClockInterps.at(surf) = NULL;
    }
    if(grCorClock.at(surf)){
      delete grCorClock.at(surf);
      grCorClock.at(surf) = NULL;
    }
  }

  // std::cout << "deleting clock0s" << std::endl;
  for(std::map<Double_t, TGraph*>::iterator itr = grClock0s.begin(); itr != grClock0s.end(); ++itr){
    // std::cout << itr->first << ", " << itr->second << "\t" << itr->second->GetN() << std::endl;

    delete itr->second; // delete object pointed to by current key
    itr->second = NULL;
  }

  // finally delete all key and pointer pairs from the map
  grClock0s.erase(grClock0s.begin(), grClock0s.end());

  // std::cout << "finished deleting..." << std::endl;
}

std::vector<Double_t> AnitaEventCalibrator::getClockAlignment(UsefulAnitaEvent* eventPtr,
							      Int_t numPoints[NUM_SURF],
							      Double_t volts[NUM_SURF][NUM_CHAN][NUM_SAMP],
							      Double_t times[NUM_SURF][NUM_SAMP],
							      std::vector<Int_t> listOfClockNums){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;

  // for(int surf=0; surf<NUM_SURF; surf++){
  //   std::cout << surf << "\t" << numPoints[surf] << std::endl;
  // }



  // Want these to hang around in memory during calibration so I can examine them.
  // Therefore delete previous clock alignment graphs only when this function is called.
  deleteClockAlignmentTGraphs();


  for(UInt_t surfInd=0; surfInd<listOfClockNums.size(); surfInd++){
    Int_t surf = listOfClockNums.at(surfInd);
    if(surf==0){
      continue;
    }

    // Interpolate clock
    grClocks.at(surf) = new TGraph(numPoints[surf], times[surf], volts[surf][8]);
    grClockInterps.at(surf) = FFTtools::getInterpolatedGraph(grClocks.at(surf), dtInterp);

    Int_t lab = eventPtr->getLabChip(surfInd*NUM_CHAN + 8);
    Double_t deltaClockKeepNs = clockKeepTime[surf][lab];
    
    TGraph* grClock0 = nullptr;
    if(grClock0s.find(deltaClockKeepNs)==grClock0s.end()){
      // Not already made/played around with this amount of ringing
      TGraph* grTemp0 = new TGraph(numPoints[0], times[0], volts[0][8]); // make clock
      grClock0 = FFTtools::getInterpolatedGraph(grTemp0, dtInterp);
      delete grTemp0;
      grClock0s[deltaClockKeepNs] = grClock0; // add key and pointer into map

      keepOnlySomeTimeAfterClockUptick(grClock0, deltaClockKeepNs); // process
    }
    else{
      grClock0 = grClock0s.find(deltaClockKeepNs)->second;
    }
    keepOnlySomeTimeAfterClockUptick(grClockInterps.at(surf), deltaClockKeepNs);
    
    // Correlate clocks once they've both been processed
    grCorClock.at(surf) = FFTtools::getCorrelationGraph(grClockInterps.at(surf), 
							grClock0);

    Double_t maxVal = -1001;
    Double_t clockAlignmentTemp = 0;
    for(Int_t samp=0; samp<grCorClock.at(surf)->GetN(); samp++){
      if(TMath::Abs(grCorClock.at(surf)->GetX()[samp]) <= AnitaClock::meanPeriodNs/2){
	if(grCorClock.at(surf)->GetY()[samp] > maxVal){
	  maxVal = grCorClock.at(surf)->GetY()[samp];
	  clockAlignmentTemp = grCorClock.at(surf)->GetX()[samp];
	}
      }
    }
    clockAlignment.at(surf) = clockAlignmentTemp;

    // not necessary but nice if printing graphs to be able to show alignment...
    for(Int_t samp=0; samp < grClockInterps.at(surf)->GetN(); samp++){
      grClockInterps.at(surf)->GetX()[samp] -= clockAlignment.at(surf);
    }
    for(Int_t samp=0; samp < grClocks.at(surf)->GetN(); samp++){
      grClocks.at(surf)->GetX()[samp] -= clockAlignment.at(surf);
    }
  }

  // if(fClockProblem!=0){
  //   for(int surf=1; surf<NUM_SURF; surf++){
  //     grClockInterps.at(surf)->SetName(TString::Format("grClockInterps_%d", surf));
  //     grClockInterps.at(surf)->Write();

  //     grClocks.at(surf)->SetName(TString::Format("grClocks_%d", surf));
  //     grClocks.at(surf)->Write();

  //   }
  //   exit(-1);
  // }




  return clockAlignment;

}







void AnitaEventCalibrator::keepOnlySomeTimeAfterClockUptick(TGraph* grClock, Double_t deltaClockKeepNs){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;

  if(deltaClockKeepNs > 0){ // Here 0 means don't actually do anything to the clock

    // Try only keeping things near the upgoing clock
    Double_t timeZeroCrossings[AnitaClock::maxNumZcs] = {0};
    Int_t sampZeroCrossings[AnitaClock::maxNumZcs] = {0};

    Int_t numZc = getTimeOfUpwardsClockTicksCrossingZero(grClock->GetN(),
							 grClock->GetX(),
							 grClock->GetY(),
							 timeZeroCrossings,
							 sampZeroCrossings,
							 false);

    if(fClockProblem==0){
      Int_t thisZc = 0;
      for(Int_t samp=0; samp<grClock->GetN(); samp++){
      
	if(grClock->GetX()[samp] - timeZeroCrossings[thisZc] < deltaClockKeepNs &&
	   grClock->GetX()[samp] - timeZeroCrossings[thisZc] >= 0){
	}
	else{
	  grClock->GetY()[samp] = 0;
	}
      
	if(timeZeroCrossings[thisZc] < grClock->GetX()[samp] - deltaClockKeepNs){
	  if(thisZc < numZc-1){
	    thisZc++;
	  }
	}
      }
    }    
  }
}







Double_t AnitaEventCalibrator::getTempFactor(){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;
  Double_t oldMeanClockPeriod = clockPeriodRingBuffer->getMean();
  Double_t tempFactor = 1;
  if(oldMeanClockPeriod > 1){ // Check we actually have a mean value
    tempFactor = AnitaClock::meanPeriodNs/oldMeanClockPeriod;
  }
  // std::cout << "output = " << oldMeanClockPeriod << "\t" << tempFactor << "\t" << std::endl;
  // std::cout << tempFactor << std::endl;
  // std::cout << clockPeriodRingBuffer->getSum() << std::endl;
  return tempFactor;
}








void AnitaEventCalibrator::updateTemperatureCorrection(){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;
  //! N.B. This function relies on the results of the guessRCO() to have the measured clock periods
  
  // First need to remove effect of previous temp correction...

  if(fClockProblem==0){

    Double_t currentTempFactor = fTempFactorGuess; //getTempFactor();

    Double_t meanClockDt = 0;
    Int_t count = 0;
    for(Int_t surf=0; surf<NUM_SURF; surf++){
      for(Int_t zc=0; zc<AnitaClock::maxNumZcs; zc++){
	// std::cout << surf << "\t" << rcoArray[surf] << "\t" << zc << "\t" 
	// 		<< measuredClockPeriods[surf][rcoArray[surf]][zc] << std::endl;
	if(measuredClockPeriods[surf][rcoArray[surf]][zc] > 0){
	  meanClockDt += measuredClockPeriods[surf][rcoArray[surf]][zc]/currentTempFactor;
	  count++;
	}
      }
    }
  
    meanClockDt /= count;
    // std::cout << meanClockDt << std::endl;
    clockPeriodRingBuffer->insert(meanClockDt);
  }
}















void AnitaEventCalibrator::guessRco(UsefulAnitaEvent* eventPtr){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;
  
  /*
    This function works by unwrapping the clock channels in both RCO phases
    and compares the upgoing clock periods to the (two) clock period(s).
  */

  // Arrays to hold the unwrapped clock channels in both RCO phases
  Double_t clockVolts[NUM_SURF][NUM_RCO][NUM_SAMP] = {{{0}}};
  Double_t clockTimes[NUM_SURF][NUM_RCO][NUM_SAMP] = {{{0}}};
  Int_t clockCapacitorNums[NUM_SURF][NUM_RCO][NUM_SAMP] = {{{0}}};
  Int_t clockNumPoints[NUM_SURF][NUM_RCO] = {{0}};

  // Here we do the unwrapping
  for(Int_t surf=0; surf<NUM_SURF; surf++){
    for(Int_t rco=0; rco<NUM_RCO; rco++){
      clockNumPoints[surf][rco] = unwrapChannel(eventPtr, surf, 8, rco, true, 
						clockVolts[surf][rco], clockTimes[surf][rco],
						clockCapacitorNums[surf][rco]);
      // std::cout << "surf = " << surf << "\trco = " << rco << "\tnumPoints = " 
      // 		<< clockNumPoints[surf][rco] << ":" << std::endl;
      // for(Int_t samp=0; samp<clockNumPoints[surf][rco]; samp++){
      // 	std::cout << clockVolts[surf][rco][samp] << ", ";
      // 	// std::cout << clockCapacitorNums[surf][rco][samp] << ", ";
      // }
      // std::cout << std::endl << std::endl;
      // for(Int_t samp=0; samp<clockNumPoints[surf][rco]; samp++){
      // 	std::cout << clockTimes[surf][rco][samp] << ", ";
      // }
      // std::cout << std::endl << std::endl;
    }
  }
  
  // Now we have the unwrapped clocks we need to examine their periods as measured by the SURFs. 
  // (We will use the time of the zero crossings to match them up across the SURFs later so get that too.)

  Double_t timeZeroCrossings[NUM_SURF][NUM_RCO][AnitaClock::maxNumZcs] = {{{0}}};
  Int_t sampZeroCrossings[NUM_SURF][NUM_RCO][AnitaClock::maxNumZcs] = {{{0}}};
  std::fill(&measuredClockPeriods[0][0][0],
	    &measuredClockPeriods[0][0][0]+sizeof(measuredClockPeriods)/sizeof(Double_t), 0);
  Int_t numZcs[NUM_SURF][NUM_RCO] = {{0}};

  for(Int_t surf=0; surf<NUM_SURF; surf++){    
    for(int rco=0; rco<NUM_RCO; rco++){
      numZcs[surf][rco] = getTimeOfUpwardsClockTicksCrossingZero(clockNumPoints[surf][rco],
								 clockTimes[surf][rco],
								 clockVolts[surf][rco],
								 timeZeroCrossings[surf][rco],
								 sampZeroCrossings[surf][rco],
								 true);
      
      for(Int_t zc=0; zc<numZcs[surf][rco]; zc++){	
	if(zc > 0){
	  measuredClockPeriods[surf][rco][zc-1] = timeZeroCrossings[surf][rco][zc] - timeZeroCrossings[surf][rco][zc-1]; 
	}
      }

      // if(eventPtr->eventNumber==10512383){
      // 	std::cout << "Summary at start of RCO guessing " << eventPtr->eventNumber << std::endl;
      // 	std::cout << "numZcs[" << surf << "][" << rco << "] = " << numZcs[surf][rco] << std::endl;
      // 	for(Int_t zc=0; zc<numZcs[surf][rco]; zc++){
      // 	  std::cout << "zc = " << zc << ", timeZc = " << timeZeroCrossings[surf][rco][zc] << ", sampZc = " 
      // 		    << sampZeroCrossings[surf][rco][zc] << "\t" << "AnitaClock::maxNumZcs = " 
      // 		    << AnitaClock::maxNumZcs << std::endl;
      // 	}
      // }
    }
  }

  // Now we have the time of (and times between) the clock upgoing zero crossings in both possible RCOs

  // Now we compare the periods implied by both RCO phase possibilities to the true clock period(s)
  // In ANITA-3 no-one turned off the spread spectrum modulation before flight, so we compare to
  // AnitaClock::highPeriodNs and AnitaClock::lowPeriodNs
  Double_t nearestClockPeriod[NUM_SURF][NUM_RCO][AnitaClock::maxNumZcs] = {{{0}}};
  Double_t differenceFromClockPeriod[NUM_SURF][NUM_RCO] = {{0}};

  const Int_t dangerZoneStart = 170;
  const Int_t dangerZoneEnd = 220;

  // First we use the clock period(s) to get a first pass at the RCO phase determination
  for(Int_t surf=0; surf<NUM_SURF; surf++){
    for(int rco=0; rco<NUM_RCO; rco++){
      for(int zc=0; zc<numZcs[surf][rco]-1; zc++){

	// Short variable name to make the next code snippet more readable
	Double_t dt = measuredClockPeriods[surf][rco][zc];
	if(dt <= 0){
	  std::cerr << "Idiot " << __FILE__ << ": " << __LINE__ << std::endl;
	}

	// std::cout << eventPtr->eventNumber << "\t" << surf << "\t" << rco << "\t" << zc << "\t"
	// 	  << sampZeroCrossings[surf][rco][zc] << std::endl;

	Int_t capNum = clockCapacitorNums[surf][rco][sampZeroCrossings[surf][rco][zc]];
	// In ANITA-3 no-one turned off the clock spread modulation... so compare to high and low periods
	// (We will need the nearestClockPeriods later for a second, improved period matching.)
	//	if(clockCapacitorNums[surf][rco]
	// std::cout << "capNum = " << capNum << std::endl;

	// std::cout << eventPtr->eventNumber << "\t" << capNum << "\t" << surf << "\t" << rco 
	// 	  << sampZeroCrossings[surf][rco][zc] << std::endl;

	if(capNum < dangerZoneStart || capNum > dangerZoneEnd){
	  if(TMath::Abs(dt - AnitaClock::lowPeriodNs) < TMath::Abs(dt - AnitaClock::highPeriodNs)){
	    differenceFromClockPeriod[surf][rco] += TMath::Abs(dt - AnitaClock::lowPeriodNs);
	    nearestClockPeriod[surf][rco][zc] = AnitaClock::lowPeriodNs;
	    // std::cout << differenceFromClockPeriod[surf][rco] << "\t" << nearestClockPeriod[surf][rco][zc]
	    // 	      << std::endl;
	  }
	  else{
	    differenceFromClockPeriod[surf][rco] += TMath::Abs(dt - AnitaClock::highPeriodNs);
	    nearestClockPeriod[surf][rco][zc] = AnitaClock::highPeriodNs;
	    // std::cout << differenceFromClockPeriod[surf][rco] << "\t" << nearestClockPeriod[surf][rco][zc]
	    // 	      << std::endl;
	  }
	}
      }

      if(numZcs[surf][rco] > 0){
	differenceFromClockPeriod[surf][rco]/=numZcs[surf][rco];
      }
      else{ // If we have no zero crossings we're probably missing a clock and this won't work
	std::cerr << "eventNumber " << eventPtr->eventNumber << ": Missing clock? SURF " << surf << ", RCO " << rco << ", guess 1" << std::endl;
      }
    }


    // May as well use firmware RCO latched value in the safe area
    // If the first hit bus is greater than the latch delay value defined here or the hit bus
    Int_t chanIndex = surf*NUM_CHAN + 8;
    Int_t firstHitBus = eventPtr->getFirstHitBus(chanIndex);
    Int_t lastHitBus = eventPtr->getLastHitBus(chanIndex);
    if(firstHitBus >= firstHitBusRcoLatchLimit || lastHitBus - firstHitBus > NUM_SAMP/2){
      rcoArray[surf] = eventPtr->getRcoCorrected(chanIndex);
    }
    else{
      // At this point we've compared the unwrapped clock periods to the closest of the 
      // two spread modulated clock periods...
      // Calibration work so far indicates this is a very good guess
      if(differenceFromClockPeriod[surf][0] < differenceFromClockPeriod[surf][1]){
	rcoArray[surf] = 0;
      }
      else{
	rcoArray[surf] = 1;
      }
    }
  }



  // Now this is a pretty good guess, but we can do slightly better.
  // The clock spread modulation means is the same across all SURFs... we can use that information.
  // By using the time of the first upgoing zero crossings (and assuming any trigger jitter is 
  // less than 15ns) we can match up the clock periods.

  // We find the offset of the clock periods relative to SURF 0.

  std::vector<Int_t> zeroCrossingMatchIndices;
  zeroCrossingMatchIndices.clear();
  zeroCrossingMatchIndices.assign(NUM_SURF, 0);

  const Double_t halfClockPeriod = 0.5*AnitaClock::meanPeriodNs;
  for(Int_t surf=0; surf<NUM_SURF; surf++){
    Int_t rco = rcoArray[surf];

    if(TMath::Abs(timeZeroCrossings[surf][rco][0] - timeZeroCrossings[0][rco][0]) < halfClockPeriod){
      zeroCrossingMatchIndices.at(surf) = 0; // If within half a period, there's no offset
    }

    else if(timeZeroCrossings[surf][rco][0] - timeZeroCrossings[0][rco][0] >= halfClockPeriod){
      // If first zero crossing in SURF[surf] is more than half a clock period later than current SURF[0]
      // then the timeZeroCrossing[surf][rco][0] matches up with zeroCrossing[0][rco][1]
      // and so the relative clock zero crossing index is -1
      zeroCrossingMatchIndices.at(surf) = -1;
    }
    else if(timeZeroCrossings[surf][rco][0] - timeZeroCrossings[0][rco][0] <= halfClockPeriod){
      // opposite of above.
      zeroCrossingMatchIndices.at(surf) = 1;
    }
  }
  



  // Suppose the relative SURF clock period offsets are {0, -1, 1} in a simple situation with 3 SURFs.
  // If we started at the sum at 0 then the third SURF would start at 1 and there would be a clock
  // period which didn't contribute to anything.
  // In that case we want the sum to start then at -1 so that the zero crossings compared will be {-1, -2, 0}
  // on the first loop. (Obviously we should ignore the cases when the clock indices are less than 0.)
  // Therefore, the start index is...
  Int_t startIndex = -1*TMath::MaxElement(NUM_SURF, &zeroCrossingMatchIndices[0]);

  
  // Now we need to sum over the clock indices again but with the offsets.
  // How many SURFs thought a period was high (AnitaClock::highPeriodNs) or low (AnitaClock::lowPeriodNs)?

  const Int_t maxNumOffsetZcs = AnitaClock::maxNumZcs - startIndex;
  std::vector<Int_t> numHigh(maxNumOffsetZcs, 0);
  std::vector<Int_t> numLow(maxNumOffsetZcs, 0);
  std::vector<Int_t> meanClockPeriod(maxNumOffsetZcs, 0);
  std::vector<Int_t> count(maxNumOffsetZcs, 0);
    
  Int_t alignedPeriodIndex = 0;
  for(Int_t zcIndex=startIndex; zcIndex < AnitaClock::maxNumZcs; zcIndex++){
    numHigh.at(alignedPeriodIndex)=0;
    numLow.at(alignedPeriodIndex)=0;
    for(Int_t surf=0; surf<NUM_SURF; surf++){
      Int_t rco = rcoArray[surf];
      Int_t zcThisSurf = zcIndex + zeroCrossingMatchIndices.at(surf);
      if(zcThisSurf >=0 && zcThisSurf < AnitaClock::maxNumZcs && nearestClockPeriod[surf][rco][zcThisSurf] > 0){

	meanClockPeriod.at(zcThisSurf) += measuredClockPeriods[surf][rco][zcThisSurf];
	count.at(zcThisSurf)++;
	// if(nearestClockPeriod[surf][rco][zcThisSurf]==AnitaClock::lowPeriodNs){
	//   numLow.at(alignedPeriodIndex)++;
	// }
	// else if(nearestClockPeriod[surf][rco][zcThisSurf]==AnitaClock::highPeriodNs){
	//   numHigh.at(alignedPeriodIndex)++;
	// }
	// else{
	//   std::cerr << "How did I get here? " << __FILE__ << ": line " << __LINE__ << std::endl;
	//   std::cerr << surf << "\t" << rco << "\t" << zcThisSurf << "\t" 
	// 	    << nearestClockPeriod[surf][rco][zcThisSurf] << std::endl;
	// }
      }
    }

    alignedPeriodIndex++;
  }

  // Now we just pick the most common answer for the clock period and compare only to that
  // (i.e. not to both possible (high/low) clock periods).

  const Int_t numAlignedPeriodIndices = alignedPeriodIndex;
  std::vector<Double_t> mostCommonClockPeriod;
  mostCommonClockPeriod.clear();
  mostCommonClockPeriod.assign(maxNumOffsetZcs, 0);
  
  for(Int_t alignedPeriodIndex=0; alignedPeriodIndex<numAlignedPeriodIndices; alignedPeriodIndex++){

    if(count.at(alignedPeriodIndex)){
      meanClockPeriod.at(alignedPeriodIndex)/=count.at(alignedPeriodIndex);
      if(TMath::Abs(meanClockPeriod.at(alignedPeriodIndex) - AnitaClock::lowPeriodNs) < TMath::Abs(meanClockPeriod.at(alignedPeriodIndex) - AnitaClock::highPeriodNs)){
	mostCommonClockPeriod.at(alignedPeriodIndex) = AnitaClock::lowPeriodNs;
      }
      else{
	mostCommonClockPeriod.at(alignedPeriodIndex) = AnitaClock::highPeriodNs;
      }
    }
    else{
      mostCommonClockPeriod.at(alignedPeriodIndex) = -1000;
    }
    // if(numLow.at(alignedPeriodIndex) > numHigh.at(alignedPeriodIndex)){
    //   mostCommonClockPeriod.at(alignedPeriodIndex) = AnitaClock::lowPeriodNs;
    // }
    // else if(numHigh.at(alignedPeriodIndex) > numLow.at(alignedPeriodIndex)){
    //   mostCommonClockPeriod.at(alignedPeriodIndex) = AnitaClock::highPeriodNs;
    // } 
    // else{ // In the rare case that we're really not sure, let's use the mean clock period
    //   mostCommonClockPeriod.at(alignedPeriodIndex) = AnitaClock::meanPeriodNs;
    // }
  }


  // Now we have our best guess at what the clock periods across all SURFs really are...
  // We can go through and see how far each pair of zero crossings are from the best guess across all SURFs.

  for(Int_t surf=0; surf<NUM_SURF; surf++){
    for(int rco=0; rco<NUM_RCO; rco++){
      Int_t alignedPeriodIndex = 0;
      for(int zcIndex=startIndex; zcIndex<startIndex; zcIndex++){
	Int_t zc = zcIndex + zeroCrossingMatchIndices.at(surf);

	if(zc >= 0 && zc < numZcs[surf][rco]-1 && mostCommonClockPeriod.at(alignedPeriodIndex)>0 ){
	  // Short variable name to make the next code snippet more readable
	  Double_t dt = measuredClockPeriods[surf][rco][zc];

	  if(dt <= 0){
	    std::cerr << "Idiot " << __FILE__ << ": " << __LINE__ << std::endl;
	  }

	  Int_t capNum = clockCapacitorNums[surf][rco][sampZeroCrossings[surf][rco][zc]];

	  if(capNum < dangerZoneStart || capNum > dangerZoneEnd){
	    differenceFromClockPeriod[surf][rco] += TMath::Abs(dt - mostCommonClockPeriod.at(alignedPeriodIndex));
	  }
	}
      }
      if(numZcs[surf][rco] > 0){
	differenceFromClockPeriod[surf][rco]/=numZcs[surf][rco];
      }
      else{ // If we have no zero crossings we're probably missing a clock and this won't work
	std::cerr << "eventNumber " << eventPtr->eventNumber << ": Missing clock? SURF " << surf << ", RCO " << rco << ", guess 2" << std::endl;
      }
    }

    // May as well use firmware RCO latched value in the safe area
    // If the first hit bus is greater than the latch delay value defined here or the hit bus
    Int_t chanIndex = surf*NUM_CHAN + 8;
    Int_t firstHitBus = eventPtr->getFirstHitBus(chanIndex);
    Int_t lastHitBus = eventPtr->getLastHitBus(chanIndex);
    if(firstHitBus >= firstHitBusRcoLatchLimit || lastHitBus - firstHitBus > NUM_SAMP/2){
      rcoArray[surf] = eventPtr->getRcoCorrected(chanIndex);
    }
    else{
      // At this point we've compared the unwrapped clock periods to the closest of the 
      // two spread modulated clock periods...
      // Calibration work so far indicates this is a very good guess
      if(differenceFromClockPeriod[surf][0] < differenceFromClockPeriod[surf][1]){
	rcoArray[surf] = 0;
      }
      else{
	rcoArray[surf] = 1;
      }
    }
    // std::cout << "surf = " << surf << "\trcoArray = " << rcoArray[surf] << std::endl;
  }
}

  



Int_t AnitaEventCalibrator::getTimeOfUpwardsClockTicksCrossingZero(Int_t numPoints, Double_t* times, Double_t* volts, Double_t* timeZeroCrossings, Int_t* sampZeroCrossings, bool raiseFlagIfClocksAreWeird){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;

  std::vector<Double_t> tZcs;
  std::vector<Int_t> sampZcs;
  std::vector<Int_t> prevExtremas;
  std::vector<Int_t> nextExtremas;

  std::vector<Int_t> maximaSamps;
  std::vector<Int_t> minimaSamps;

  // See explanation below for why I find the samples of the local maxima and minima
  findExtremaSamples(numPoints, volts, maximaSamps, minimaSamps);

  Int_t numZcs = 0;
  for(Int_t samp=0; samp<numPoints-1; samp++){
    Double_t y1 = volts[samp];
    Double_t y2 = volts[samp+1];
    Double_t t1 = times[samp];
    Double_t t2 = times[samp+1];
    if(y1 <= 0 && y2 > 0){

      Double_t tzc = getTimeOfZeroCrossing(t1, y1, t2, y2);
      if(TMath::IsNaN(tzc)){
	std::cerr << "Error in " __FILE__ << ": line " << __LINE__ << std::endl;
	std::cerr << samp << "\t" << numPoints << "\t" << t1 << "\t" 
		  << y1 << "\t" << t2 << "\t" << y2 << std::endl;
      }


      // OK here things get a little more complicated than I would like... here's why.
      // There were some problems with the crosstalk from SURF saturation messing this function up
      // Such a condition creates large +ve or -ve spikes in the clock channels which naively 
      // look like the clock has crossed zero (it has but not because of the clock).
      // So to combat that I've added some extra conditions a zero crossing has to fulfil to count as
      // a tick of the clock.
      // That condition is the nearest local maxmima or minima (which ever is closer but not equal) 
      // before and after the zero crossing sample MUST have a voltage difference above some threshold.
      // That threshold is defined in AnitaClock.h


      // At the start of the function I found the local maxima and minima...
      // Now I find the local maxima and minima immediately before and after the upgoing zero crossing.

      // First do the maxima, since we are looking for an upwards "tick" of the clock between
      // samp and samp+1, the current sample can't be a local maxima.
      Int_t nextMaxima = numPoints-1;
      Int_t prevMaxima = 0;
      for(UInt_t maxInd=0; maxInd < maximaSamps.size(); maxInd++){
	if(maximaSamps.at(maxInd) > samp){

	  // So long as the next maxima isn't he sample immediately following the sample before the 
	  // minima then select the next maxima. (It could be a surf saturation cross talk!)
	  if(maximaSamps.at(maxInd) > samp+1){
	    nextMaxima = maximaSamps.at(maxInd);
	  }
	  else if(maxInd < maximaSamps.size()-1){
	    nextMaxima = maximaSamps.at(maxInd+1);
	  }

	  if(maxInd > 0){
	    // If it's not the first maxima, then the previous value can be assigned
	    // (otherwise it stays zero).
	    prevMaxima = maximaSamps.at(maxInd-1);
	  }
	  break;
	}
      }

      // Now we do the minima, but need to take care that we don't accidentally select the current sample
      // if it is indeed a minima (often the case for uninterpolated clocks)
      Int_t nextMinima = numPoints-1;
      Int_t prevMinima = 0;
      for(UInt_t minInd=0; minInd < minimaSamps.size(); minInd++){
	if(minimaSamps.at(minInd) > samp){

	  nextMinima = minimaSamps.at(minInd);
 
	  // It is possible that the value of the current sample is a local minimum
	  // But I don't want that (downward spike from cross talk fits that condition!)
	  if(minInd > 0 && minimaSamps.at(minInd-1) < samp){// Require the previous minima be less than samp
	    prevMinima = minimaSamps.at(minInd-1);
	  }
	  else if(minInd > 1 && minimaSamps.at(minInd-2) < samp){ //If not, then use the prior one.
	    prevMinima = minimaSamps.at(minInd-2);
	  }
	  // If we are very close to the start of the array then the previous minima can be 0.
	  break;
	}
      }


      // Here we select the closest maxima or minima to the upwards clock tick...
      Int_t prevExtrema = prevMinima > prevMaxima ? prevMinima : prevMaxima;
      Int_t nextExtrema = nextMinima < nextMaxima ? nextMinima : nextMaxima;


      // std::cout << std::endl;

      // std::cout << "samp = " << samp << ", prevMaxima = " << prevMaxima << ", nextMaxima = " << nextMaxima 
      // 		<< ", prevMinima = " << prevMinima << ", nextMinima = " << nextMinima << std::endl;
      // std::cout << "volt = " << volts[samp] << ", prevMaxima = " << volts[prevMaxima] 
      // 		<< ", nextMaxima = " << volts[nextMaxima] << ", prevMinima = " 
      // 		<< volts[prevMinima] << ", nextMinima = " << volts[nextMinima] << std::endl;
      // std::cout << "nextExtrema = " << nextExtrema << " volts = " << volts[nextExtrema] << std::endl;
      // std::cout << "prevExtrema = " << prevExtrema << " volts = " << volts[prevExtrema] << std::endl;

      if(volts[nextExtrema] - volts[prevExtrema] > AnitaClock::upgoingThreshold){
	prevExtremas.push_back(prevExtrema);
	nextExtremas.push_back(nextExtrema);

	tZcs.push_back(tzc);
	sampZcs.push_back(samp);
	// std::cout << "SELECTED!" << std::endl;
      }
      // std::cout << std::endl;
    }
  }

  if(raiseFlagIfClocksAreWeird==true){
    if(tZcs.size() > (UInt_t)AnitaClock::maxNumZcs || tZcs.size() < (UInt_t)AnitaClock::minNumZcs){
      fClockProblem = 1;
      for(UInt_t zc=0; zc<sampZcs.size(); zc++){
	std::cout << "(" << sampZcs.at(zc) << ", " << volts[sampZcs.at(zc)] << "), (" 
		  << prevExtremas.at(zc) << ", " << volts[prevExtremas.at(zc)] << "), (" 
		  << nextExtremas.at(zc) << ", " << volts[nextExtremas.at(zc)] << "), " << std::endl;
      }
      std::cout << "fClockProblem = " << fClockProblem << std::endl;      
    }
  }

  // This guarentees we won't have any buffer overflow... 
  // but may still have crappy zero crossings.
  for(Int_t zc=0; zc<AnitaClock::maxNumZcs; zc++){
    if(zc < (Int_t)tZcs.size()){
      timeZeroCrossings[zc] = tZcs.at(zc);
      sampZeroCrossings[zc] = sampZcs.at(zc);
      numZcs++;
    }
  }


  return numZcs;
}


void AnitaEventCalibrator::findExtremaSamples(Int_t length, Double_t* volts, 
					      std::vector<Int_t>& maximaSamps, 
					      std::vector<Int_t>& minimaSamps){
  // empty vectors
  maximaSamps.clear();
  minimaSamps.clear();
  
  for(int samp=0; samp<length; samp++){
    Double_t y0 = samp > 0 ? volts[samp-1] : 0;
    Double_t y1 = volts[samp];

    int posOffset = 1;
    Double_t y2 = samp < length-1 ? volts[samp + posOffset] : 0;
    while(y2==y1 && samp + posOffset < length){
      y2 = samp < length-1 ? volts[samp + posOffset] : 0;
      posOffset++;
    }


    if(y0 > y1 && y1 < y2){
      minimaSamps.push_back(samp);
    }
    else if(y0 < y1 && y1 > y2){
      maximaSamps.push_back(samp);
    }
  }
}


Double_t AnitaEventCalibrator::getTimeOfZeroCrossing(Double_t x1, Double_t y1, Double_t x2, Double_t y2){
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;
  // Does a linear interpolation of zero crossing between (x1,y1) and (x2, y2)
  Double_t m = (y1-y2)/(x1-x2);
  Double_t c = y1 - m*x1;
  return -c/m;
}






















Int_t AnitaEventCalibrator::unwrapChannel(UsefulAnitaEvent* eventPtr, Int_t surf, Int_t chan, Int_t rco, 
					  Bool_t fApplyTempCorrection, Double_t* mvArray, 
					  Double_t* tArray, Int_t* cArray) {
  // std::cout << "Just called " << __PRETTY_FUNCTION__ << std::endl;

  /*
    With 33 MHz clock will need to unwrap waveform to measure clock frequency and guess RCO phase.
    For that reason, output array pointers are passed into function and the RCO phase to unwrap in.
    The meat of this is inherited from ANITA-2 (ANITA-1) unwrapping function.
  */

  // std::cout << "AnitaEventCalibrator::unwrapChannel(" << eventPtr << ", " << surf << ", " << chan << " ," 
  // 	    << rco << ", " << fApplyTempCorrection << ", " << mvArray << ", " << tArray << ", " 
  // 	    << cArray << ")" << std::endl;


  Int_t chanIndex = surf*CHANNELS_PER_SURF + chan;
  
  Int_t numExtras = 0; ///< Used to count how many extra capacitors to include after the wrap around 
  Int_t labChip = eventPtr->getLabChip(chanIndex); ///< Each LAB has its own set of deltaTs and epsilons
  Int_t earliestSample = eventPtr->getEarliestSample(chanIndex); ///< Given by firmware
  Int_t latestSample = eventPtr->getLatestSample(chanIndex); ///< Given by firmware

  Double_t tempFactor = 1;
  if(fApplyTempCorrection==true){
    tempFactor = fTempFactorGuess; //getTempFactor();
  }

  // Don't include first capacitor if it's the first sample
  if(earliestSample==0){
    earliestSample++;
  }

  // Don't include first capacitor if it's the last sample
  if(latestSample==0){
    latestSample=259;
  }

  Short_t* rawArray = eventPtr->data[chanIndex];
      
  //Now do the unwrapping
  Int_t index=0;
  Double_t time=0;



  if(latestSample<earliestSample) {
    // Then the waveform is wrapped around and we have two RCOs    

    Int_t nextExtra=256; ///< nextExtra used for deciding whether or not to insert capacitors after 255
    Double_t extraTime=0;///< sum of times of those extra capacitors

    if(earliestSample<256) {
      //Lets do the first segment up to the wrap around...
      for(Int_t samp = earliestSample;samp<256;samp++) {
	Int_t binRco = 1-rco;
	cArray[index] = samp;
	mvArray[index] = rawArray[samp];
	tArray[index] = time;

	// Need to store 255->256 differently to 255->0 in case we add data from extra capacitors
	if(samp == 255) {
	  extraTime = time+(deltaTs[surf][labChip][binRco][samp])*tempFactor;
	}
	else {
	  time += (deltaTs[surf][labChip][binRco][samp])*tempFactor;
	}
	index++;
      }

      time += epsilons[surf][labChip][rco]*tempFactor; ///< This is the time of the first capacitor.

      // Ignore sample zero: add time on, but don't include value in unwrapped waveform
      // ***MOVED TO HERE*** (see big chunk of comments a few lines below for explanation)
      time += (deltaTs[surf][labChip][rco][0])*tempFactor; ///< BenS - "I think moving to here is legit"
    }
    else {
      // If first sample is inside the extra capacitors then just ignore the first couple of samples.
      nextExtra=260;
      extraTime=0;
    }
	

    // Chuck of explanation:
    // I have moved this to inside the (if earliestSample<256) statement as it led to 
    // some waveforms starting with non-zero times when no inter-surf alignment was applied
    // which makes no sense at all.
    // Since all the deltaTs are identical across all channels (inc. the clock) in a SURF
    // then the non-zero start time is accounted for in the clock correction.
    // Moving it guarentees all channels start at t=0 (when no inter-SURF calibration is applied),
    // which makes it less hard to mess up calibration

    // Ignore sample zero: add time on, but don't include value in unwrapped waveform
    // time += (deltaTs[surf][labChip][rco][0])*tempFactor; // ***MOVED FROM HERE*** (see above)

    // Now unwrap from sample 1 to the end of the waveform
    for(Int_t samp=1;samp<=latestSample;samp++) {
      Int_t binRco = rco;

      if(nextExtra<260 && samp==1) {

	if(extraTime<time-0.22) { ///This is Andres's 220ps minimum sample separation

	  // Then insert the next extra capacitor
	  binRco=1-rco;
	  cArray[index] = nextExtra;
	  mvArray[index] = Double_t(rawArray[nextExtra]);
	  tArray[index] = extraTime;
	  if(nextExtra < 259) {
	    extraTime += (deltaTs[surf][labChip][binRco][nextExtra])*tempFactor;
	  }
	  nextExtra++;
	  numExtras++;
	  index++;

	  // continue in loop but undo the samp variable incrementation
	  samp--;
	  continue;
	}
      }

      cArray[index] = samp;
      mvArray[index] = Double_t(rawArray[samp]);
      tArray[index] = time;

      if(samp<259) {
	time+=(deltaTs[surf][labChip][binRco][samp])*tempFactor;
      }

      index++;
    }
  }
  else{ // Only one rco phase, entire waveform is in order in the capacitor array

    time=0;
    for(Int_t samp=earliestSample; samp<=latestSample; samp++){
      Int_t binRco=rco;
      cArray[index] = samp;
      mvArray[index] = rawArray[samp];
      tArray[index] = time;
      if(samp<259) {
	time += (deltaTs[surf][labChip][binRco][samp])*tempFactor;
      }
      index++;
    }
  }

  // Here index == numPoints in unwrapped waveform
  return index;

}


void AnitaEventCalibrator::loadCalib() {
  char calibDir[FILENAME_MAX];
  char fileName[FILENAME_MAX];
  char *calibEnv=getenv("ANITA_CALIB_DIR");
  if(!calibEnv) {
    char *utilEnv=getenv("ANITA_UTIL_INSTALL_DIR");
    if(!utilEnv)
      sprintf(calibDir,"calib");
    else
      sprintf(calibDir,"%s/share/anitaCalib",utilEnv);
  }
  else {
    strncpy(calibDir,calibEnv,FILENAME_MAX);
  }

  //Set up some default numbers
  for(Int_t surf=0;surf<NUM_SURF;surf++) {
    for(Int_t chan=0;chan<NUM_CHAN;chan++) {
      for(Int_t chip=0;chip<NUM_CHIP;chip++) {
	mvCalibVals[surf][chan][chip]=1;
      }
    }
  }
    
  for(Int_t surf=0;surf<NUM_SURF;surf++) {
    for(Int_t chip=0;chip<NUM_CHIP;chip++) {
      for(Int_t rco=0;rco<NUM_RCO;rco++) {
	epsilons[surf][chip][rco]=1.25*(rco+1);
	for(Int_t samp=0;samp<NUM_SAMP;samp++)
	  deltaTs[surf][chip][rco][samp]=1./2.6;
      }
    }
  }
    
  Int_t surf,chan,chip,rco,samp;
  Double_t calib;
  sprintf(fileName,"%s/simpleVoltageCalibrationHarm.txt",calibDir);
  std::ifstream CalibFile(fileName);
  char firstLine[180];
  CalibFile.getline(firstLine,179);

  while(CalibFile >> surf >> chan >> chip >> calib) {
      
    // std::cout << surf << "\t" << chan << "\t" << chip << "\t" << calib << std::endl;
    mvCalibVals[surf][chan][chip]=calib;
    
    // accounts for factor of -1 in the top ring since the seaveys are flipped
    Int_t ant;
    AnitaPol::AnitaPol_t pol;
    AnitaGeomTool::getAntPolFromSurfChan(surf,chan,ant, pol);
    mvCalibVals[surf][chan][chip] *= AnitaGeomTool::getAntOrientation(ant);
  }

    
  sprintf(fileName,"%s/justBinByBin.dat",calibDir);
  std::ifstream binByBinCalibFile(fileName);
  binByBinCalibFile.getline(firstLine,179);
  while(binByBinCalibFile >> surf >> chip >> rco >> samp >> calib) {
    deltaTs[surf][chip][rco][samp]=calib;
    // std::cout << "surf=" << surf << "\tchip=" << chip << "\trco=" << rco << "\tsamp=" << samp 
    // 	      << "\tdt=" << deltaTs[surf][chip][rco][samp] << std::endl;
  }


  sprintf(fileName,"%s/epsilonFromBenS.dat",calibDir);
  std::ifstream epsilonFile(fileName);
  epsilonFile.getline(firstLine,179);
  while(epsilonFile >> surf >> chip >> rco >> calib) {
    epsilons[surf][chip][rco]=calib; 
  }

  sprintf(fileName,"%s/clockKeep.dat",calibDir);
  std::ifstream clockKeep(fileName);
  clockKeep.getline(firstLine,179);
  while(clockKeep >> surf >> chip >> calib) {
    clockKeepTime[surf][chip]=calib; 
  }
    
  sprintf(fileName,"%s/RFCalibration_BRotter.txt",calibDir);
  std::ifstream BRotterRfPowPeds(fileName);
  std::string chanName;
  Double_t rfYInt,ampNoise,chanGain,rfSlope;
  Int_t surfi,chani;
  BRotterRfPowPeds.getline(firstLine,179);
  while(BRotterRfPowPeds >> chanName >> surfi >> chani >> rfSlope >> rfYInt >> ampNoise >> chanGain) {
    RfPowYInt[surfi-1][chani-1]=rfYInt;
    RfPowSlope[surfi-1][chani-1]=rfSlope;
  }
}


Double_t AnitaEventCalibrator::convertRfPowTodBm(int surf, int chan, int adc){
  return (double(adc)-RfPowYInt[surf][chan])/RfPowSlope[surf][chan];
}

Double_t AnitaEventCalibrator::convertRfPowToKelvin(int surf, int chan, int adc){
  double dBm = AnitaEventCalibrator::convertRfPowTodBm(surf, chan, adc);  
  double mW = TMath::Power(10,dBm/10.);
  double K = (mW/(1000.*1.38e-23*1e9));
  return K;
}
