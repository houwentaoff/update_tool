# SlickEdit generated file.  Do not edit this file except in designated areas.

# Make command to use for dependencies
MAKE=gmake
RM=rm
MKDIR=mkdir

# -----Begin user-editable area-----

# -----End user-editable area-----

# If no configuration is specified, "Debug" will be used
ifndef CFG
CFG=Debug
endif

#
# Configuration: Debug
#
ifeq "$(CFG)" "Debug"
OUTDIR=Debug
OUTFILE=$(OUTDIR)/FiUpdateLoader
CFG_INC=
CFG_LIB=-Wl,-Bdynamic -lpthread -lrt -ldl -Wl,-Bstatic -lomniORB4 \
	-lomnithread -lomniDynamic4 -Wl,-Bdynamic
CFG_OBJ=
COMMON_OBJ=$(OUTDIR)/Markup_linux.o $(OUTDIR)/Md5.o \
	$(OUTDIR)/utility.o $(OUTDIR)/QueryUpdateSK.o \
	$(OUTDIR)/FiUpdateAssistant.o $(OUTDIR)/main.o \
	$(OUTDIR)/SAXParserHandler.o 
OBJ=$(COMMON_OBJ) $(CFG_OBJ)
ALL_OBJ=$(OUTDIR)/Markup_linux.o $(OUTDIR)/Md5.o $(OUTDIR)/utility.o \
	$(OUTDIR)/QueryUpdateSK.o $(OUTDIR)/FiUpdateAssistant.o \
	$(OUTDIR)/main.o $(OUTDIR)/SAXParserHandler.o $(CFG_LIB)

COMPILE=g++ -c  -x c++  -g -w -fpermissive -o "$(OUTDIR)/$(*F).o" $(CFG_INC) $<
LINK=g++  -g -w -o "$(OUTFILE)" $(ALL_OBJ)

# Pattern rules
$(OUTDIR)/%.o : %.cpp
	$(COMPILE)

$(OUTDIR)/%.o : ../comm/%.cpp
	$(COMPILE)

$(OUTDIR)/%.o : ../idl/%.cpp
	$(COMPILE)

$(OUTDIR)/%.o : %.cc
	$(COMPILE)

$(OUTDIR)/%.o : ../comm/%.cc
	$(COMPILE)

$(OUTDIR)/%.o : ../idl/%.cc
	$(COMPILE)

# Build rules
all: $(OUTFILE)

$(OUTFILE): $(OUTDIR)  $(OBJ)
	$(LINK)
	-cp ./Debug/FiUpdateLoader ./

$(OUTDIR):
	$(MKDIR) -p "$(OUTDIR)"

# Rebuild this project
rebuild: cleanall all

# Clean this project
clean:
	$(RM) -f $(OUTFILE)
	$(RM) -f $(OBJ)

# Clean this project and all dependencies
cleanall: clean
endif

#
# Configuration: Release
#
ifeq "$(CFG)" "Release"
OUTDIR=Release
OUTFILE=$(OUTDIR)/FiUpdateLoader
CFG_INC=
CFG_LIB=-Wl,-Bdynamic -lpthread -lrt -ldl -Wl,-Bstatic -lomniORB4 \
	-lomnithread -lomniDynamic4 -Wl,-Bdynamic
CFG_OBJ=
COMMON_OBJ=$(OUTDIR)/Markup_linux.o $(OUTDIR)/Md5.o \
	$(OUTDIR)/utility.o $(OUTDIR)/QueryUpdateSK.o \
	$(OUTDIR)/FiUpdateAssistant.o $(OUTDIR)/main.o \
	$(OUTDIR)/SAXParserHandler.o 
OBJ=$(COMMON_OBJ) $(CFG_OBJ)
ALL_OBJ=$(OUTDIR)/Markup_linux.o $(OUTDIR)/Md5.o $(OUTDIR)/utility.o \
	$(OUTDIR)/QueryUpdateSK.o $(OUTDIR)/FiUpdateAssistant.o \
	$(OUTDIR)/main.o $(OUTDIR)/SAXParserHandler.o $(CFG_LIB)

COMPILE=g++ -c  -x c++  -w -fpermissive -o "$(OUTDIR)/$(*F).o" $(CFG_INC) $<
LINK=g++  -w -o "$(OUTFILE)" $(ALL_OBJ)

# Pattern rules
$(OUTDIR)/%.o : %.cpp
	$(COMPILE)

$(OUTDIR)/%.o : ../comm/%.cpp
	$(COMPILE)

$(OUTDIR)/%.o : ../idl/%.cpp
	$(COMPILE)

$(OUTDIR)/%.o : %.cc
	$(COMPILE)

$(OUTDIR)/%.o : ../comm/%.cc
	$(COMPILE)

$(OUTDIR)/%.o : ../idl/%.cc
	$(COMPILE)

# Build rules
all: $(OUTFILE)

$(OUTFILE): $(OUTDIR)  $(OBJ)
	$(LINK)
	-cp ./Debug/FiUpdateLoader ./

$(OUTDIR):
	$(MKDIR) -p "$(OUTDIR)"

# Rebuild this project
rebuild: cleanall all

# Clean this project
clean:
	$(RM) -f $(OUTFILE)
	$(RM) -f $(OBJ)

# Clean this project and all dependencies
cleanall: clean
endif