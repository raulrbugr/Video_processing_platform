onbreak {quit -f}
onerror {quit -f}

vsim -voptargs="+acc" -t 1ps -pli "C:/Xilinx/Vivado/2016.2/lib/win64.o/libxil_vsim.dll" -L unisims_ver -L unimacro_ver -L secureip -L xil_defaultlib -L xpm -L axi_lite_ipif_v3_0_4 -L lib_cdc_v1_0_2 -L interrupt_control_v3_1_4 -L axi_gpio_v2_0_11 -L lib_pkg_v1_0_2 -L fifo_generator_v13_1_1 -L lib_fifo_v1_0_5 -L blk_mem_gen_v8_3_3 -L lib_bmg_v1_0_5 -L lib_srl_fifo_v1_0_2 -L axi_datamover_v5_1_11 -L axi_vdma_v6_2_8 -L proc_sys_reset_v5_0_9 -L processing_system7_bfm_v2_0_5 -L fifo_generator_v13_0_3 -L v_tc_v6_1_8 -L v_vid_in_axi4s_v4_0_3 -L v_axi4s_vid_out_v4_0_3 -L generic_baseblocks_v2_1_0 -L axi_infrastructure_v1_1_0 -L axi_register_slice_v2_1_9 -L axi_data_fifo_v2_1_8 -L axi_crossbar_v2_1_10 -L axi_protocol_converter_v2_1_9 -lib xil_defaultlib xil_defaultlib.zybo_hdmi_in xil_defaultlib.glbl

do {wave.do}

view wave
view structure
view signals

do {zybo_hdmi_in.udo}

run -all

quit -force
