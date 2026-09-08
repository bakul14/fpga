.PHONY: clean distclean

TIMESCALE ?= 1ns/1ps

%:
	cmake -B build/$@ -DTASK=$@ -DTIMESCALE=$(TIMESCALE)
	cmake --build build/$@ -j$$(nproc)
	cd build/$@ && ./$@

view:
	cd build && gtkwave

clean:
	rm -rf build

distclean: clean
	rm -rf .deps
	rm -f semester_*/*/task_*/.clangd
