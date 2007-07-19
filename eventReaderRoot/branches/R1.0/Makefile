#############################################################################
####


#Global Thingies
CC 	= g++
LD	= g++
SOFLAGS	= -shared
OBJSUF	= o
SRCSUF	= cxx
OPT	= -O2 -g --debug  -Wall -fPIC

ROOTINCLUDES      = -I$(ROOTSYS)/include
INCLUDES	= 
CXXFLAGS	= $(EXCEPTION) $(OPT) $(CXXOPT) -fPIC $(INCLUDES) $(ROOTINCLUDES)

ROOTLIBS      = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs) -lMinuit
ROOTGLIBS     = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs)
LIBS		= -lz -lm $(ROOTLIBS)



#ROOT stuff




ROOT_LIBRARY = libAnitaEvent.so
TREE_MAKER = makeBodyTree
LIB_OBJS = RawAnitaEvent.${OBJSUF} RawAnitaEventDict.$(OBJSUF) UsefulAnitaEvent.$(OBJSUF) UsefulAnitaEventDict.$(OBJSUF) AnitaEventCalibrator.$(OBJSUF) AnitaEventCalibratorDict.$(OBJSUF) AnitaGeomTool.$(OBJSUF)
ROOT_OBJS = eventdict.o


all : $(ROOT_LIBRARY) 

$(TREE_MAKER) : $(ROOT_LIBRARY) $(TREE_MAKER).$(SRCSUF)
	@echo "<**Linking**> "  
	$(LD)  $(CXXFLAGS) $(LDFLAGS) $(TREE_MAKER).$(SRCSUF) $(ROOT_LIBRARY) $(LIBS) -o $@

$(ROOT_LIBRARY) : $(LIB_OBJS) 
	@echo "Linking $@ ..."
	$(LD) $(SOFLAGS) $(LDFLAGS) $(LIB_OBJS) -o $@


%.$(OBJSUF) : %.$(SRCSUF)
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) -c $< -o  $@

%.$(OBJSUF) : %.C
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) $ -c $< -o  $@


%Dict.C: %.h
	@echo "Generating dictionary ..."
#	@ rm -f *Dict* 
	rootcint $@ -c $<

clean:
	@rm -f *Dict*
	@rm -f *.${OBJSUF}
	@rm -f $(LIBRARY)
	@rm -f $(ROOT_LIBRARY)
	@rm -f $(TEST)