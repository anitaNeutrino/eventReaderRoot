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

ROOTLIBS      = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --libs) -lMinuit -lTreePlayer
ROOTGLIBS     = -L$(ROOTSYS)/lib $(shell $(ROOTSYS)/bin/root-config --glibs)
LIBS		= -lz -lm $(ROOTLIBS)



#ROOT stuff




ROOT_LIBRARY = libAnitaEvent.so
TREE_MAKER = makeHeadTreeRunFiles makePrettyHkRunFiles makeAdu5PatFiles makeAdu5VtgFiles makeSurfhkFiles makeTurfRateFiles
LIB_OBJS = RawAnitaEvent.o UsefulAnitaEvent.o  AnitaEventCalibrator.o AnitaGeomTool.o RawAnitaHeader.o PrettyAnitaHk.o Adu5Pat.o Adu5Vtg.o SurfHk.o TurfRate.o RawDataReader.o AnitaConventions.o eventDict.o
CLASS_HEADERS = RawAnitaEvent.h UsefulAnitaEvent.h RawAnitaHeader.h PrettyAnitaHk.h Adu5Pat.h Adu5Vtg.h SurfHk.h TurfRate.h AnitaEventCalibrator.h AnitaConventions.h AnitaGeomTool.h



all : $(ROOT_LIBRARY) 
raw: readRawData

readRawData : $(ROOT_LIBRARY) readRawData.$(SRCSUF)
	@echo "<**Compiling**> "  
	$(LD)  $(CXXFLAGS) $(LDFLAGS) readRawData.$(SRCSUF) $(ROOT_LIBRARY) $(LIBS) -o $@


$(ROOT_LIBRARY) : $(LIB_OBJS) 
	@echo "Linking $@ ..."
	$(LD) $(SOFLAGS) $(LDFLAGS) $(LIB_OBJS) -o $@


%.$(OBJSUF) : %.$(SRCSUF)
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) -c $< -o  $@

%.$(OBJSUF) : %.C
	@echo "<**Compiling**> "$<
	$(CXX) $(CXXFLAGS) $ -c $< -o  $@


eventDict.C: $(CLASS_HEADERS)
	@echo "Generating dictionary ..."
	@ rm -f *Dict* 
	rootcint $@ -c $(CLASS_HEADERS) LinkDef.h

clean:
	@rm -f *Dict*
	@rm -f *.${OBJSUF}
	@rm -f $(LIBRARY)
	@rm -f $(ROOT_LIBRARY)
	@rm -f $(TEST)
