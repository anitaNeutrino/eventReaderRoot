#############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################
include Makefile.arch

#Site Specific  Flags
SYSINCLUDES	=
SYSLIBS         = 

ifdef ANITA_UTIL_INSTALL_DIR
ANITA_UTIL_LIB_DIR=${ANITA_UTIL_INSTALL_DIR}/lib
ANITA_UTIL_INC_DIR=${ANITA_UTIL_INSTALL_DIR}/include
LD_ANITA_UTIL=-L$(ANITA_UTIL_LIB_DIR)
INC_ANITA_UTIL=-I$(ANITA_UTIL_INC_DIR)
ANITA_UTIL_CALIB_DIR=$(ANITA_UTIL_INSTALL_DIR)/share/anitaCalib
else
ANITA_UTIL_LIB_DIR=/usr/local/lib
ANITA_UTIL_INC_DIR=/usr/local/include
ANITA_UTIL_CALIB_DIR=/usr/local/share/anitaCalib
ifdef EVENT_READER_DIR
LD_ANITA_UTIL=-L$(EVENT_READER_DIR)
INC_ANITA_UTIL=-I$(EVENT_READER_DIR)
endif
endif

#Toggles the FFT functions on and off
#USE_FFT_TOOLS=1

ifdef USE_FFT_TOOLS
FFTLIBS = -lRootFftwWrapper -lfftw3
FFTFLAG = -DUSE_FFT_TOOLS
else
FFTLIBS =
FFTFLAG =
endif

#Generic and Site Specific Flags
CXXFLAGS     += $(ROOTCFLAGS) $(FFTFLAG) $(SYSINCLUDES) $(INC_ANITA_UTIL)
LDFLAGS      += -g $(ROOTLDFLAGS) 


LIBS          = $(ROOTLIBS) -lMathMore -lMinuit $(SYSLIBS) $(LD_ANITA_UTIL) $(FFTLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

#Now the bits we're actually compiling
ROOT_LIBRARY = libAnitaEvent.${DLLSUF}
LIB_OBJS = RawAnitaEvent.o UsefulAnitaEvent.o  AnitaEventCalibrator.o AnitaGeomTool.o RawAnitaHeader.o PrettyAnitaHk.o Adu5Pat.o Adu5Vtg.o SurfHk.o TurfRate.o RawDataReader.o AnitaConventions.o TimedAnitaHeader.o SummedTurfRate.o AveragedSurfHk.o AcqdStart.o GpsdStart.o LogWatchdStart.o RunStart.o G12Pos.o GpsGga.o G12Sat.o Adu5Sat.o CommandEcho.o MonitorHk.o OtherMonitorHk.o RawHk.o CalibratedHk.o AnitaPacketUtil.o eventDict.o
CLASS_HEADERS = RawAnitaEvent.h UsefulAnitaEvent.h RawAnitaHeader.h PrettyAnitaHk.h Adu5Pat.h Adu5Vtg.h SurfHk.h TurfRate.h AnitaEventCalibrator.h AnitaConventions.h AnitaGeomTool.h TimedAnitaHeader.h SummedTurfRate.h AveragedSurfHk.h AcqdStart.h GpsdStart.h LogWatchdStart.h RunStart.h G12Pos.h GpsGga.h G12Sat.h Adu5Sat.h CommandEcho.h MonitorHk.h OtherMonitorHk.h RawHk.h CalibratedHk.h AnitaPacketUtil.h simpleStructs.h


all : $(ROOT_LIBRARY)

tests: printStructSizes

readRawData : $(ROOT_LIBRARY) readRawData.$(SRCSUF)
	@echo "<**Compiling**> "  
	$(LD)  $(CXXFLAGS) $(LDFLAGS) readRawData.$(SRCSUF) $(ROOT_LIBRARY) $(LIBS) -o $@


printStructSizes : $(ROOT_LIBRARY) printStructSizes.$(SRCSUF)
	@echo "<**Compiling**> "  
	$(LD)  $(CXXFLAGS) $(LDFLAGS) printStructSizes.$(SRCSUF) $(ROOT_LIBRARY) $(LIBS) -o $@


#The library
$(ROOT_LIBRARY) : $(LIB_OBJS) 
	@echo "Linking $@ ..."
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
	$(LD)   $(SOFLAGS) $^ $(OutPutOpt) $@
ifeq ($(MACOSX_MINOR),4)
	ln -sf $@ $(subst .$(DLLSUF),.so,$@)
else
	$(LD) -bundle -undefined $(UNDEFOPT)  $(LDFLAGS) $^ \
	 $(OutPutOpt) $(subst .$(DLLSUF),.so,$@)
endif
else
	$(LD) $(SOFLAGS) $(LDFLAGS) $(LIBS) $(LIB_OBJS) -o $@
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

install: $(ROOT_LIBRARY)
ifeq ($(PLATFORM),macosx)
	install -c -m 755 $(ROOT_LIBRARY) $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY)) $(ANITA_UTIL_LIB_DIR)
else
	install -c -m 755 $(ROOT_LIBRARY) $(ANITA_UTIL_LIB_DIR)
endif
	install -c -m 644  $(CLASS_HEADERS) $(ANITA_UTIL_INC_DIR)
	install -d $(ANITA_UTIL_CALIB_DIR)
	for file in calib/*.dat calib/*.csv calib/*.txt; do install -c -m 644 "$${file}" $(ANITA_UTIL_CALIB_DIR); done
	install -d $(ANITA_UTIL_CALIB_DIR)/jiwoo_timecode
	for file in calib/jiwoo_timecode/*.txt; do install -c -m 644 "$${file}" $(ANITA_UTIL_CALIB_DIR)/jiwoo_timecode ; done

clean:
	@rm -f *Dict*
	@rm -f *.${OBJSUF}
	@rm -f $(LIBRARY)
	@rm -f $(ROOT_LIBRARY)
	@rm -f $(subst .$(DLLSUF),.so,$(ROOT_LIBRARY))	
	@rm -f $(TEST)
#############################################################################



