.PHONY: clean distclean

TIMESCALE ?= 1ns/1ps

task_4: TOP = iir_filter

%:
	cmake -B build/$@ -DTASK=$@ -DTIMESCALE=$(TIMESCALE) -DTOP=$(TOP)
	cmake --build build/$@ -j$$(nproc)
	cd build/$@ && ./$@

view:
	cd build && gtkwave

clean:
	rm -rf build

distclean: clean
	rm -rf .deps
	rm -f semester_*/*/task_*/.clangd
