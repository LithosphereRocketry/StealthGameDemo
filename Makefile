# Semi-automated C/C++ makefile

# This should be all you need to change
TARGETS = stealthgame

# Header files go here
INCDIR = include/
# C/C++ files go here
SRCDIR = src/
# Generated object files go here
OBJDIR = obj/

LINKS = SDL2 SDL2main SDL2_image

CPPOPTS_ALWAYS = -pedantic -Wall -Wextra -std=c++14
CPPOPTS_DEBUG = -g -Wno-unused-parameter -Wno-unused-variable
CPPOPTS_RELEASE = -O2 -flto
LINKOPTS_ALWAYS = $(foreach link,$(LINKS),-l$(link))
LINKOPTS_DEBUG = 
LINKOPTS_RELEASE = -flto

CPPOPTS = $(CPPOPTS_ALWAYS) $(CPPOPTS_DEBUG)
LINKOPTS = $(LINKOPTS_ALWAYS) $(LINKOPTS_DEBUG)

# if you just type "make", this happens
.DEFAULT_GOAL: debug
# these rules don't refer to real files, so they shouldn't get timestamp-checked
.PHONY: all clean debug release

debug: __DEBUG all

release: CPPOPTS = $(CPPOPTS_ALWAYS) $(CPPOPTS_RELEASE)
release: LINKOPTS = $(LINKOPTS_ALWAYS) $(LINKOPTS_RELEASE)
release: __RELEASE all

__DEBUG:
	$(MAKE) clean
	rm -f __RELEASE
	touch __DEBUG

__RELEASE:
	$(MAKE) clean
	rm -f __DEBUG
	touch __RELEASE

# make all = make (all of the final targets given)
all: $(TARGETS)
# includes = everything in INCDIR with a .h file extension
INC = $(wildcard $(INCDIR)*.h)
# C source files = SRCDIR with a .c file extension
# C++ source files = SRCDIR with a .cpp file extension
CPPSRC = $(wildcard $(SRCDIR)*.cpp)
# List of filenames from CPPSRC but change them to OBJDIR/whatever.o for compiled objects
OBJ = $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(CPPSRC))
# Target objects = things that need libraries given to them = convert targets to OBJDIR/whatever.o
TGTOBJ = $(patsubst %,$(OBJDIR)%.o,$(TARGETS))
# Dependency objects = things that should be given to targets = whatever objects aren't targets
DEPOBJ = $(filter-out $(TGTOBJ),$(OBJ))

# To build a target:
# - Require its corresponding object and all of the dependency objects to be up to date
# - Throw everything to the system's default C++ compiler
$(TARGETS): %: $(OBJDIR)%.o $(DEPOBJ)
	$(CXX) $(CPPOPTS) -o $@ $< $(DEPOBJ) $(LINKOPTS)

# To build an object:
# - require its .cpp file to be up to date, all includes to be up to date, and the OBJDIR folder to exist
# - Throw it and the includes to default C++ compiler
$(OBJDIR)%.o: $(SRCDIR)%.cpp $(INC) | $(OBJDIR)
	$(CXX) $(CPPOPTS) -I$(INCDIR) -c $< -o $@

# If the object directory doesn't exist, make it
$(OBJDIR):
	mkdir $@

# If everything else fails, warn the user
% ::
	@echo "File $@ missing or not buildable"

# clean deletes all objects and target executables
clean:
	rm -f $(OBJDIR)* $(TARGETS) __DEBUG __RELEASE