export HLSCNN_DIR=/home/yl29/3LA/HLSCNN_Accel/cmod/harvard/top
cp $HLSCNN_DIR/test_conv.log ./sim_info/
make all
./extract_input_seq.out ./sim_info/test_conv.log ./sim_info/test_input_conv.json
./extract_sim_result.out ./sim_info/test_conv.log ./sim_info/sim_result.json
./conv_testbench.out

