#############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################
include Makefile.arch

#Site Specific  Flags
SYSINCLUDES	= -I/sw/include
SYSLIBS         = -L/sw/lib

#Now the bits we're actually compiling

ROOT_LIBRARY = libAnitaEvent.${DLLSUF}
LIB_OBJS = RawAnitaEvent.o UsefulAnitaEvent.o  AnitaEventCalibrator.o AnitaGeomTool.o RawAnitaHeader.o PrettyAnitaHk.o Adu5Pat.o Adu5Vtg.o SurfHk.o TurfRate.o RawDataReader.o AnitaConventions.o TimedAnitaHeader.o eventDict.o
CLASS_HEADERS = RawAnitaEvent.h UsefulAnitaEvent.h RawAnitaHeader.h PrettyAnitaHk.h Adu5Pat.h Adu5Vtg.h SurfHk.h TurfRate.h AnitaEventCalibrator.h AnitaConventions.h AnitaGeomTool.h TimedAnitaHeader.h

PROGRAMS = generateSecondaries generateNeutrinos

all : $(ROOT_LIBRARY)

readRawData : $(ROOT_LIBRARY) readRawData.$(SRCSUF)
	@echo "<**Compiling**> "  
	$(LD)  $(CXXFLAGS) $(LDFLAGS) readRawData.$(SRCSUF) $(ROOT_LIBRARY) $(LIBS) -o $@

#The library
$(ROOT_LIBRARY) : $(LIB_OBJS) 
	@echo "Linking $@ ..."
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
	$(LD) $(SOFLAGS) $^ $(OutPutOpt) $@
ifeq ($(MACOSX_MINOR),4)
	ln -sf $@ $(subst .$(DLLSUF),.so,$@)
else
	$(LD) -bundle -undefined $(UNDEFOPT) $(LDFLAGS) $^ \
	 $(OutPutOpt) $(subst .$(DLLSUF),.so,$@)
endif
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $(LIB_OBJS) -o $@
endif

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
	@rm -f $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY))	
	@rm -f $(TEST)
#############################################################################



