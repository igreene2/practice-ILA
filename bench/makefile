CC = g++
CFLAGS = -std=c++11 -g

INCDIR = -I. -I./json_helper
CURDIR = $(shell pwd)

CFLAGS += $(INCDIR)

all: conv_testbench.out 

clean:
	rm -rf *.out *.o

MAX_SIM_DIR = /home/workspace/practice-ILA/build/sim_model
MAX_MODEL_DIR = $(VTA_SIM_DIR)/build/CMakeFiles/MAX.dir/src

AP_INCLUDE = $(CURDIR)/ap_include
SYSTEMC_HOME = /ibuild/systemc-2.3.3
BOOST_HOME = /usr/local

INCDIR += -I$(MAX_SIM_DIR)/include
INCDIR += -I$(AP_INCLUDE)

INCDIR += -I$(SYSTEMC_HOME)/include
INCDIR += -I$(BOOST_HOME)/include

LIBDIR = -L. -L$(SYSTEMC_HOME)/lib -Wl,-rpath=$(SYSTEMC_HOME)/lib
LIBDIR += -L$(BOOST_HOME)/lib

LIBS = -lstdc++ -lsystemc -lboost_chrono -lboost_system

CFLAGS_MAX = $(INCDIR) $(LIBDIR) -std=c++11 -g

conv_testbench.out : conv_testbench.cc $(MAX_MODEL_DIR)/*.o $(MAX_SIM_DIR)/include/*.h
	$(CC) $(CFLAGS_MAX) testbench.cc $(MAX_MODEL_DIR)/*.o -o conv_testbench.out $(LIBS)