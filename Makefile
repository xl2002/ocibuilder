#
# 'make'        build executable file 'main'
# 'make clean'  removes all .o and executable files
#

# define the Cpp compiler to use
CXX = g++

# define any compile-time flags
CXXFLAGS	:= -std=c++11 -Wall -Wextra -g -O0

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -lboost_filesystem-mgw12-mt-x64-1_75 -lboost_system-mgw12-mt-x64-1_75 -lws2_32 -lboost_thread-mgw12-mt-x64-1_75 -lboost_random-mgw12-mt-x64-1_75 -lboost_json-mgw12-mt-x64-1_75 -lboost_iostreams-mgw12-mt-x64-1_75 -lssl -lcrypto -larchive -lz -lbcrypt
# LFLAGS = -lboost_system-vc141-mt-s-x64-1_75 -lboost_filesystem-vc141-mt-s-x64-1_75 -lws2_32 -lboost_thread-vc141-mt-s-x64-1_75 -lboost_random-vc141-mt-s-x64-1_75 -lboost_json-vc141-mt-s-x64-1_75 -lboost_iostreams-vc141-mt-s-x64-1_75 -lz
# define output directory
OUTPUT	:= output

# define source directory
SRC		:= src

# define include directory
# INCLUDE	:= include D:/Softwares/boost_1_75_0
INCLUDE	:= include
# define lib directory
LIB		:= lib\boost lib\zlib lib\openssl lib\libarchive
# LIB         := D:/Softwares/boost_1_75_0/lib64-msvc-14.1

ifeq ($(OS),Windows_NT)
MAIN	:= main.exe
SOURCEDIRS	:= $(SRC) $(shell dir /s /b /ad $(SRC))
INCLUDEDIRS	:= $(INCLUDE)
LIBDIRS		:= $(LIB)
FIXPATH = $(subst /,\,$1)
RM			:= del /q /f
MD	:= mkdir
else
MAIN	:= main
SOURCEDIRS	:= $(shell find $(SRC) -type d)
INCLUDEDIRS	:= $(shell find $(INCLUDE) -type d)
LIBDIRS		:= $(shell find $(LIB) -type d)
FIXPATH = $1
RM = rm -f
MD	:= mkdir -p
endif

# define any directories containing header files other than /usr/include
INCLUDES	:= $(patsubst %,-I %, $(INCLUDEDIRS:%/=%))

# define the C libs
LIBS		:= $(patsubst %,-L %, $(LIBDIRS:%/=%))

# define the C source files
SOURCES		:= $(wildcard $(patsubst %,%/*.cpp, $(SOURCEDIRS)))
# SOURCES		:= $(wildcard $(foreach dir,$(SOURCEDIRS),$(dir)/*.cpp))

# define the C object files
OBJECTS		:= $(SOURCES:.cpp=.o)

# define the dependency output files
DEPS		:= $(OBJECTS:.o=.d)

#
# The following part of the makefile is generic; it can be used to
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

OUTPUTMAIN	:= $(call FIXPATH,$(OUTPUT)/$(MAIN))

# test程序的变量
LFLAGSTEST	:= -lboost_filesystem-mgw12-mt-x64-1_75 -lboost_iostreams-mgw12-mt-x64-1_75 -lboost_system-mgw12-mt-x64-1_75 -lboost_thread-mgw12-mt-x64-1_75 -lws2_32 -lboost_json-mgw12-mt-x64-1_75 -lboost_random-mgw12-mt-x64-1_75 -lssl -lcrypto -larchive -lz -lbcrypt
INCLUDETEST	:= -I include
OUTPUTTEST		:= tests
OUTPUTMAINTEST	:= $(call FIXPATH,$(OUTPUTTEST)/$(MAIN))
SRCTEST		:= ./tests/sha356.cpp
LIBSTEST	:= -L lib\boost -L lib\openssl -L lib\libarchive
all: $(OUTPUT) $(MAIN)
# $(RM) $(call FIXPATH,$(OBJECTS))
# $(RM) $(call FIXPATH,$(DEPS))
	@echo Executing 'all' complete!

$(OUTPUT):
	$(MD) $(OUTPUT)

$(MAIN): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUTPUTMAIN) $(OBJECTS) $(LIBS) $(LFLAGS)

# include all .d files
-include $(DEPS)

test:
	$(CXX) $(CXXFLAGS) $(INCLUDETEST) -o $(OUTPUTMAINTEST) $(SRCTEST) $(LIBSTEST) $(LFLAGSTEST)
# ./$(OUTPUTMAINTEST)
# this is a suffix replacement rule for building .o's and .d's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file)
# -MMD generates dependency output files same name as the .o file
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -MMD $<  -o $@

.PHONY: clean
clean:
	$(RM) $(OUTPUTMAIN)
	$(RM) $(call FIXPATH,$(OBJECTS))
	$(RM) $(call FIXPATH,$(DEPS))
	@echo Cleanup complete!

run: all
	./$(OUTPUTMAIN) 
	@echo Executing 'run: all' complete!

