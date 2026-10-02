# Verilator simulation builds. Each target uses its own build directory (obj_<name>).
# Usage: make dual && ./obj_dual/Vdual_nasa_log_parser <access.log>
# multi and octa drivers run several copies of the single parser (nasa_log_parser.v) from C++.

VFLAGS = --cc --exe --build

.PHONY: all single dual multi octa clean
all: single dual multi octa

single:
	verilator $(VFLAGS) --Mdir obj_single verilog/nasa_log_parser.v src/parser_driver.cpp

dual:
	verilator $(VFLAGS) --Mdir obj_dual verilog/dual_nasa_log_parser.v verilog/nasa_log_parser.v src/dual_parser_driver.cpp

multi:
	verilator $(VFLAGS) --Mdir obj_multi verilog/nasa_log_parser.v src/multi_parser_driver.cpp

octa:
	verilator $(VFLAGS) --Mdir obj_octa verilog/nasa_log_parser.v src/octa_parser_driver.cpp

clean:
	rm -rf obj_single obj_dual obj_multi obj_octa
