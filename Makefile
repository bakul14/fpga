.PHONY: clean

%:
	cmake -B build/$@ -DTASK=$@
	cmake --build build/$@ -j$$(nproc)
	cd build/$@ && ./$@

view:
	cd build && gtkwave

clean:
	rm -rf build
