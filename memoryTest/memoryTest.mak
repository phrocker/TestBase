# SlickEdit generated file.  Do not edit this file except in designated areas.

# Make command to use for dependencies
MAKE=gmake
RM=rm
MKDIR=mkdir

# -----Begin user-editable area-----

# -----End user-editable area-----

# If no configuration is specified, "Debug" will be used
ifndef "CFG"
CFG=Debug
endif

#
# Configuration: Debug
#
ifeq "$(CFG)" "Debug"
OUTDIR=Debug
OUTFILE=$(OUTDIR)/memoryTest
CFG_INC=
CFG_LIB=../CommonLibrary/Debug/CommonLibrary.a -largtable2 
CFG_OBJ=
COMMON_OBJ=$(OUTDIR)/memoryTest.o $(OUTDIR)/pageAllocator.o 
OBJ=$(COMMON_OBJ) $(CFG_OBJ)
ALL_OBJ=$(OUTDIR)/memoryTest.o $(OUTDIR)/pageAllocator.o \
	../CommonLibrary/Debug/CommonLibrary.a -largtable2 

COMPILE=gcc -c   -g -o "$(OUTDIR)/$(*F).o" $(CFG_INC) "$<"
LINK=gcc  -g -static -o "$(OUTFILE)" $(ALL_OBJ)

# Pattern rules
$(OUTDIR)/%.o : %.c
	$(COMPILE)

# Build rules
all: $(OUTFILE)

$(OUTFILE): $(OUTDIR)  $(OBJ)
	$(LINK)

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
OUTFILE=$(OUTDIR)/memoryTest
CFG_INC=
CFG_LIB=../CommonLibrary/Debug/CommonLibrary.a -largtable2 
CFG_OBJ=
COMMON_OBJ=$(OUTDIR)/memoryTest.o $(OUTDIR)/pageAllocator.o 
OBJ=$(COMMON_OBJ) $(CFG_OBJ)
ALL_OBJ=$(OUTDIR)/memoryTest.o $(OUTDIR)/pageAllocator.o \
	../CommonLibrary/Debug/CommonLibrary.a -largtable2 

COMPILE=gcc -c   -o "$(OUTDIR)/$(*F).o" $(CFG_INC) "$<"
LINK=gcc  -static -o "$(OUTFILE)" $(ALL_OBJ)

# Pattern rules
$(OUTDIR)/%.o : %.c
	$(COMPILE)

# Build rules
all: $(OUTFILE)

$(OUTFILE): $(OUTDIR)  $(OBJ)
	$(LINK)

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