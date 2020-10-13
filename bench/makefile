#simple makefile
CC = /usr/bin/g++
CFLAGS = -std=c++11 -g

CUR_DIR = /home/yl20/3LA/testbench/hlscnn

INCDIR = -I. -I./json_helper

CFLAGS+= $(INCDIR)

all: extract_input extract_sim_result uninterpreted_func.o conv_testbench.out

clean:
	rm -f *.out *.o

extract_input: ./tool/extract_input_seq.cc 
	$(CC) $(CFLAGS) ./tool/extract_input_seq.cc -o extract_input_seq.out
extract_sim_result: ./tool/extract_sim_result.cc
	$(CC) $(CFLAGS) ./tool/extract_sim_result.cc -o extract_sim_result.out

HLSCNN_SIM_DIR = /home/yl29/3LA/hlscnn-ila/build/sim_model
HLSCNN_MODEL_DIR = $(HLSCNN_SIM_DIR)/build/CMakeFiles/hlscnn.dir/src

HLSCNN_ROOT = /home/yl29/3LA/HLSCNN_Accel
HLSCNN_HOME = $(HLSCNN_ROOT)/cmod
BOOST_HOME = /usr/local
SYSTEMC_HOME = /home/yl29/local/systemc-2.3.3

CATAPULT_HOME = /home/yl29/3LA/MentorGraphics2020/Catapult_Synthesis_10.5a-871028
MC_SCV_INCLUDE = /home/yl29/3LA/MentorGraphics2020/Catapult_Synthesis_10.5a-871028/Mgc_home/shared/include
CRT_LIB_DIR = /usr/lib/x86_64-linux-gnu
STDCXX_LIB_DIR = /usr/lib/gcc/x86_64-linux-gnu
BOOST_INC_DIR = /home/yl29/local/boost/include
BOOST_LIB_DIR = /home/yl29/local/boost/lib

export LIBRARY_PATH=$(CRT_LIB_DIR):$LIBRARY_PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(BOOST_LIB_DIR):$(SYSTEMC)/lib


INCDIR+= -I$(SYSTEMC_HOME)/include -I$(BOOST_HOME)/include 
INCDIR+= -I$(HLSCNN_SIM_DIR)/include

INCDIR+= -I$(MC_SCV_INCLUDE) -I$(BOOST_INC_DIR)
INCDIR+= -I$(HLSCNN_HOME)/include
INCDIR+= -I$(HLSCNN_HOME)/harvard/top

LIBDIR = -L. -L$(SYSTEMC_HOME)/lib -Wl,-rpath=$(SYSTEMC_HOME)/lib -L$(BOOST_HOME)/lib
LIBDIR+= -L$(STDCXX_LIB_DIR)

CFLAGS_HLSCNN = $(INCDIR) $(LIBDIR) -std=c++11 -g

LIBS = -lstdc++ -lsystemc -lm -lpthread -lboost_chrono -lboost_system

uninterpreted_func.o: uninterpreted_func.cc $(HLSCNN_SIM_DIR)/include/*
	$(CC) $(CFLAGS_HLSCNN) -c uninterpreted_func.cc -o uninterpreted_func.o $(LIBS)

conv_testbench.out: uninterpreted_func.o conv_testbench.cc $(HLSCNN_MODEL_DIR)/*.o
	$(CC) $(CFLAGS_HLSCNN) conv_testbench.cc uninterpreted_func.o $(HLSCNN_MODEL_DIR)/*.o -o conv_testbench.out $(LIBS)

