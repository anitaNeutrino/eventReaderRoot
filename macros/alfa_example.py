#! /usr/bin/env python3 

# Example getting ALFA data (with PyRoot) 


# imports
import numpy
import ROOT 
import os 

#some constants

# ALFA is channel 104, I think 
ALFA_chan = 104 
run = 200  # a random run
useDecimated = False 

#set ANITA_ROOT_DATA if necessary: 
# os.environ["ANITA_ROOT_DATA"] = "/where/the/files/are" 

# ANITA library Loads
ROOT.gSystem.Load("${ANITA_UTIL_INSTALL_DIR}/lib/libRootFftwWrapper.so") #not sure if it's necessary 
ROOT.gSystem.Load("${ANITA_UTIL_INSTALL_DIR}/lib/libAnitaEvent.so")


# HACK HACK HACK (see below, should probably just add a nice helper to UsefulAnitaEvent... in fact should probably just add a direct pythonization  
ROOT.gInterpreter.ProcessLine(" const double  * hack_volts(const UsefulAnitaEvent * event, int chan) { return event->fVolts[chan] ;} ")
ROOT.gInterpreter.ProcessLine(" const double  * hack_times(const UsefulAnitaEvent * event, int chan) { return event->fTimes[chan] ;} ")


# open a run 
d = ROOT.AnitaDataset(run,useDecimated) 


nentries = d.N() 

print(nentries) 

for ientry in range(0, nentries):  # just the first 10 events

  d.getEntry(ientry) 


  #  Check if we want this event
  #  this filters out all RF events but you can do whatever, obviously
  if d.header().getTriggerBitRF(): 
    continue 

  # there are two ways to grab the calibrated event data, both through the UsefulAnitaEvent 
    useful = d.useful() 

  # it's often easier to use d.useful().getGraph( ) but there are some issues with that: 
  #    1) You have to set the flag not to filter out ALFA data each time  BEFORE asking for the graph on each event: 
  #    useful.setAlfaFilterFlag(False) 
  #    2) This allocates a new TGraph, which PyROOT theoretically should garbage collect. It seems to work for me,  but I don't know if it always will an all cases. It's easier to avoid the allocation entirely 
  #    3) you probalby don't want the TGraph anyway, just a numpy array, which you can get from the TGraph, but you can also get directly. 

 
  useful.setAlfaFilterFlag(False)   
  g = useful.getGraph(ALFA_chan) 
  v = numpy.frombuffer(g.GetY())   # this uses the same memory as the TGraph, you can copy it if you want to release it from the TGraph (and maybe delete the TGraph even) 
  t = numpy.frombuffer(g.GetX()) 



  #  I prefer to just directly grab the points manually, which you can do like
  #  this. It's a bit convoluted since you have to get the maximum number of valid samples yourself (we
  #  could add helpers to UsefulAnitaEvent to make this easier). 
  #  Unfortunately, if you're using ROOT 6.22 right now the most straightforward way might not work so you may have to use my hack, below,. 



  NPoints = useful.fNumPoints[ALFA_chan] 
  print("NPoints in minbias entry %d is %d " % (ientry,NPoints)) 


## This SHOULD work but crashes on the newest ROOT. Might work on an older ROOT?  They changed the way they are doing buffer views and it seems to break for multidimensional arrays. 
#  volts = numpy.frombuffer(useful.fVolts[ALFA_chan],dtype=numpy.float64,count=NPoints) 
#  times = numpy.frombuffer(useful.fTimes[ALFA_chan],dtype=numpy.float64,count=NPoints) 

  ## HACK HACK HACK to workaround ROOT 6.22 bad pythonization of multidimensional arrays by tricking PyROOT into showing us a one-dimensional array (with JITed helper function defined above) 
  volts = numpy.frombuffer(ROOT.hack_volts(useful, ALFA_chan), dtype=numpy.float64, count =NPoints)
  times = numpy.frombuffer(ROOT.hack_times(useful, ALFA_chan), dtype=numpy.float64, count =NPoints)
  
  # do something with the data, here I want to make sure the two methods give the same result
  print ("RMS computed from graph is: %g, from direct: %g\n" %( numpy.sqrt(numpy.mean(v*v)), numpy.sqrt(numpy.mean(volts*volts))))



