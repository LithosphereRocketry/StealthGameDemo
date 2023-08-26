# Semi-automated C/C++ makefile

# This should be all you need to change
TARGETS = stealthgame drawtest boxbounce test

# Header files go here
INCDIR = include/
# C/C++ files go here
SRCDIR = src/
# Generated object files go here
OBJDIR = obj/
# main() C++ files go here
TGTDIR = targets/
# Output goes here
BINDIR = bin/
# Tests go here, same folder structure
TESTDIR = tests/

LINKS = SDL2 SDL2main SDL2_image

CPPOPTS_ALWAYS = -pedantic -Wall -Wextra -std=c++14
CPPOPTS_DEBUG = -O0 -g -Wno-unused-parameter -Wno-unused-variable
CPPOPTS_RELEASE = -O2 -flto
LINKOPTS_ALWAYS = $(foreach link,$(LINKS),-l$(link))
LINKOPTS_DEBUG = 
LINKOPTS_RELEASE = -flto

CPPOPTS = $(CPPOPTS_ALWAYS) $(CPPOPTS_DEBUG)
LINKOPTS = $(LINKOPTS_ALWAYS) $(LINKOPTS_DEBUG)

# if you just type "make", this happens
.DEFAULT_GOAL: debug
# these rules don't refer to real files, so they shouldn't get timestamp-checked
.PHONY: all clean debug release test

debug: __DEBUG all

release: CPPOPTS = $(CPPOPTS_ALWAYS) $(CPPOPTS_RELEASE)
release: LINKOPTS = $(LINKOPTS_ALWAYS) $(LINKOPTS_RELEASE)
release: __RELEASE all test

__DEBUG:
	$(MAKE) clean
	rm -f __RELEASE
	touch __DEBUG

__RELEASE:
	$(MAKE) clean
	rm -f __DEBUG
	touch __RELEASE

TGTSRC = $(wildcard $(TGTDIR)*.cpp)
# targets = sources in targets/ but as executables
TARGETS = $(patsubst $(TGTDIR)%.cpp,$(BINDIR)%,$(TGTSRC))

TESTSRC = $(wildcard $(TESTDIR)$(TGTDIR)*.cpp)
TESTS = $(patsubst $(TESTDIR)$(TGTDIR)%.cpp,$(TESTDIR)$(BINDIR)%,$(TESTSRC))
# includes = everything in INCDIR with a .h file extension
INC = $(wildcard $(INCDIR)*.h)
# C++ source files = SRCDIR with a .cpp file extension
CPPSRC = $(wildcard $(SRCDIR)*.cpp)
# List of filenames from CPPSRC but change them to OBJDIR/whatever.o for compiled objects
DEPOBJ = $(patsubst $(SRCDIR)%.cpp,$(OBJDIR)%.o,$(CPPSRC))
# Target objects = things that need libraries given to them = convert targets to OBJDIR/whatever.o
TGTOBJ = $(patsubst $(TGTDIR)%.cpp,$(OBJDIR)%.o,$(TGTSRC))
TESTOBJ = $(patsubst $(TESTDIR)$(TGTDIR)%.cpp,$(OBJDIR)%.o,$(TESTSRC))
# make all = make (all of the final targets given)
all: $(TARGETS) $(TESTS)

test: $(TESTS)
	$(foreach test,$(TESTS),$(test);)
	$(info All tests passed!)

# To build a target:
# - Require its corresponding object and all of the dependency objects to be up to date
# - Throw everything to the system's default C++ compiler
$(TARGETS): $(BINDIR)%: $(OBJDIR)%.o $(DEPOBJ) Makefile | $(BINDIR)
	$(CXX) $(CPPOPTS) -o $@ $< $(DEPOBJ) $(LINKOPTS)

$(TESTS): $(TESTDIR)$(BINDIR)%: $(OBJDIR)%.o $(DEPOBJ) Makefile | $(TESTDIR)$(BINDIR)
	$(CXX) $(CPPOPTS) -o $@ $< $(DEPOBJ) $(LINKOPTS)

# To build an object:
# - require its .cpp file to be up to date, all includes to be up to date, and the OBJDIR folder to exist
# - Throw it and the includes to default C++ compiler
$(DEPOBJ): $(OBJDIR)%.o: $(SRCDIR)%.cpp $(INC) Makefile | $(OBJDIR)
	$(CXX) $(CPPOPTS) -I$(INCDIR) -c $< -o $@
$(TGTOBJ): $(OBJDIR)%.o: $(TGTDIR)%.cpp $(INC) Makefile | $(OBJDIR)
	$(CXX) $(CPPOPTS) -I$(INCDIR) -c $< -o $@
$(TESTOBJ): $(OBJDIR)%.o: $(TESTDIR)$(TGTDIR)%.cpp $(INC) Makefile | $(OBJDIR)
	$(CXX) $(CPPOPTS) -I$(INCDIR) -c $< -o $@

# If the object directory doesn't exist, make it
$(OBJDIR):
	mkdir $@
$(BINDIR):
	mkdir $@
$(TESTDIR)$(BINDIR):
	mkdir $@

# If everything else fails, warn the user
% ::
	$(error File $@ missing or not buildable)

# clean deletes all objects and target executables
clean:
	rm -f $(OBJDIR)* $(BINDIR)* $(TESTDIR)$(BINDIR)* __DEBUG __RELEASE