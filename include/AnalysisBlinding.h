/* -*- C++ -*-.*********************************************************************************************
 Author: Ben Strutt
 Email: strutt@physics.ucla.edu

 Description:
 Separate the blinding implementation from AnitaEventCalibrator.
 To add a blinding strategy please update the strategy_t enum, and the description function.
 You will also need to add a function to manipulate the UsefulAnitaEvent you want to blind.
 The actual blinding functions are currently not accessible outside AnalysisBlindling.cxx.
***********************************************************************************************************/

#ifndef ANALYSIS_BLINDING_H
#define ANALYSIS_BLINDING_H

#include <iostream>
#include "UsefulAnitaEvent.h"
#include "TString.h"

namespace AnalysisBlinding{


  // All the blinding options.
  // The plan is to have each one represented by a bit so multiple strategies can be set at the same time.
  // Any output files should probably save the blinding strategy to the tree.
  enum strategy {
    kNoBlinding                          = 0x00,
    kInsertedEvents                      = 0x01
    // kAnotherStrategy                     = 0x02,
    // kYetAnotherStrategy                  = 0x04,
    // kYetAnotherStill                     = 0x08
  };

  // define the bitwise or operator | to combine blinding strategies
  inline strategy operator|(strategy strat1,  strategy strat2){
    return static_cast<strategy>(static_cast<UInt_t>(strat1) | static_cast<UInt_t>(strat2));
  }
  // define the bitwise and opterator & to decode the blinding strategies
  inline strategy operator&(strategy strat1,  strategy strat2){
    return static_cast<strategy>(static_cast<UInt_t>(strat1) & static_cast<UInt_t>(strat2));
  }







  /**
   * Get a one line description of the blinding strategy
   * (please update this in AnalysisBlinding.cxx when you add a strategy.
   * @param strat is the strategy to describe.
   * @return the description
   */
  TString getDescription(strategy strat);


  /**
   * Set the current strategy (see AnalysisBlinding.h) for strategy options
   * @param strat is the strategy to use. This can be a combination e.g. (kIntertedEvents | kAnotherStrategy)
   * @return the strategy that was set
   */
  strategy setStrategy(strategy strat);


  /**
   * Get the currently set strategy
   * @return the strategy that was set
   */
  strategy getStrategy();


  /**
   * Applies the currently set blinding strategy to the events
   * @param event is the event to be blinded
   */
  void applyBlinding(UsefulAnitaEvent* event);



};





#endif //ANALYSIS_BLINDING_H
