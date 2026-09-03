.PHONY: clean

%:
	cmake -B build/$@ -DTASK=$@
	cmake --build build/$@ -j$$(nproc)
	cd build/$@ && ./$@

clean:
	rm -rf build
