#! /usr/bin/env python 


###############################################################################
#
# trivial^G^G^G^G^G^G^Gshould-have-beeen-trivial script to convert a some of
# the ROOT data  to numpy arrays.  Uses ANITA_ROOT_DATA directory for ROOT data
# and takes as arguments a list of run numbers.  Outputs either to
# ANITA_NUMPY_DATA or $ANITA_ROOT_DATA/../numpy/ 
# 
# The reason this is non-trival is because, at least as of writing, root_numpy
# is too dumb to handle objects of simple types properly, so I ended up having
# to do the work it should be doing and iterate through the branches building
# up the branch names. This solution is probably not general enough for all
# possible TTree's, but should be good enough for our simple trees. 
#
# Also, because loading large files uncompressed in memory is idiotic, it
# splits the output into chunks of max_output_length (see below for value). 
#
# The file naming convention is $ANITA_NUMPY_DATA/run%d/treeName%d[_%d].npz
# where the _%d suffix is only added if there are multiple chunks. 
#
# To load something in python, you can e.g.:   
#
#  import numpy 
#  headTreeAsAnArray = numpy.load('headTree123_0.npz')['headTree'] 
#
#
# Cosmin Deaconu <cozzyd@kicp.uchicago.edu>  Jan 2017
##############################################################################

from root_numpy import tree2array
import ROOT 
import os
import os.path
import sys
import numpy 
import math




# This is the maximum number of events per file. This is be
max_output_length = 10000

# This is the files that are currently converted. Can add more, of course.
files_to_convert = [ "headFile",  "gpsFile", "surfHkFile", "turfRateFile" , "eventFile" ]

assert'ANITA_ROOT_DATA' in os.environ, "Need to define ANITA_ROOT_DATA" 

root_dir = os.environ['ANITA_ROOT_DATA'] 


numpy_dir = ""

if 'ANITA_NUMPY_DATA' in os.environ: 
  numpy_dir = os.environ['ANITA_NUMPY_DATA']
else: 
  numpy_dir = os.path.join(root_dir,"../numpy")


def usage():  
  print "numpyfy.py run1 [run2] [run3] ... [runN]" 


ROOT.gSystem.Load("%s/lib/libRootFftwWrapper.so" % os.environ['ANITA_UTIL_INSTALL_DIR']) 
ROOT.gSystem.Load("%s/lib/libAnitaEvent.so" % os.environ['ANITA_UTIL_INSTALL_DIR']) 


def build_branches(tree, prefix = None): 

  branches = [] 

  branch_list = tree.GetListOfBranches() 

  for branch in branch_list: 

    name = branch.GetName() 
    # exclude stupid TObject things that we certainly don't want 
    if  name == "TObject": 
      continue

    
    prefixed_name = name

    if prefix:
      prefixed_name = "%s.%s" % (prefix,name) 
   
    if branch.GetListOfBranches().GetEntries() > 0:
      branches += build_branches(branch, prefixed_name) # lol  duck typing for the win 
    
    else:
      #we have to remove the [ ] at the end

      if prefixed_name.count('['): 
       prefixed_name = prefixed_name[0: prefixed_name.find('[') ] 


      print  "     Adding branch %s" % prefixed_name
      branches.append( prefixed_name) 

  return branches


def numpyfy(run): 

  for f in files_to_convert: 
    fname = "%s/run%d/%s%d.root" % (root_dir, run,f,run)
    print " Processing %s ..." % (fname )

    rfile = ROOT.TFile(fname)

    for key in rfile.GetListOfKeys(): 

      if key.GetClassName() == "TTree": 
        name = key.GetName() 
        print "  Converting Tree %s " % name 
        tree = rfile.Get(name) 

        #Ok, apparently root_numpy is not smart enough to convert composite objects, 
        # but we can give it a list of branches instead that we can automatically generate.
        # Not sure why it doesn't do it itself... 

        branch_list = build_branches(tree) 

        size = tree.GetEntries() 

        if size == 0: 
          continue 

        nchunks = int(math.ceil(size / float(max_output_length))) 

        outdir = "%s/run%d" % (numpy_dir, run)
        if not os.path.exists(outdir): 
          os.makedirs(outdir)

        for chunk in range(nchunks): 
          arrays = {} 
          arrays[tree.GetName()] = tree2array(tree, branch_list, start = chunk * max_output_length, stop = (chunk+1) * max_output_length)
          outfile = "" 
          if nchunks > 1:
            outfile = "%s/%s%d_%d.npz" % (outdir, tree.GetName(), run,chunk)
          else:
            outfile = "%s/%s%d.npz" % (outdir, tree.GetName(), run)

          print "Creating %s " % outfile 

          numpy.savez_compressed(outfile, **arrays) 
      else: 
        print " No trees found... skipping" 
    
if __name__ == "__main__": 

  if len(sys.argv) < 2:
    usage() 

  for i in sys.argv[1:]: 
    numpyfy( int(i)) 
