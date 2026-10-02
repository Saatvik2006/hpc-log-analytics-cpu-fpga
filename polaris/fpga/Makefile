# Verilator simulation builds. Each target uses its own build directory (obj_<name>).
# Usage: make quad && ./obj_quad/quad_polaris_parser datasets/polaris_real_10.csv

VFLAGS = --cc --exe --build

.PHONY: all single dual quad eight sixteen clean
all: single dual quad eight sixteen

single:
	verilator $(VFLAGS) --Mdir obj_single verilog/polaris_log_parser.v \
	  verilator/polaris_parser_driver.cpp --top-module polaris_log_parser -o polaris_parser

dual:
	verilator $(VFLAGS) -Wno-PINMISSING --Mdir obj_dual verilog/polaris_log_parser.v verilog/dual_polaris_log_parser.v \
	  verilator/dual_polaris_parser_driver.cpp --top-module dual_polaris_log_parser -o dual_polaris_parser

quad:
	verilator $(VFLAGS) --Mdir obj_quad verilog/polaris_log_parser.v verilog/quad_polaris_log_parser.v \
	  verilator/quad_polaris_parser_driver.cpp --top-module quad_polaris_log_parser -o quad_polaris_parser

eight:
	verilator $(VFLAGS) --Mdir obj_eight verilog/polaris_log_parser.v verilog/eight_polaris_log_parser.v \
	  verilator/eight_polaris_parser_driver.cpp --top-module eight_polaris_log_parser -o eight_polaris_parser

sixteen:
	verilator $(VFLAGS) --Mdir obj_sixteen verilog/polaris_log_parser.v verilog/sixteen_polaris_log_parser.v \
	  verilator/sixteen_polaris_parser_driver.cpp --top-module sixteen_polaris_log_parser -o sixteen_polaris_parser

clean:
	rm -rf obj_single obj_dual obj_quad obj_eight obj_sixteen
