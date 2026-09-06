.PHONY: clean distclean

%:
	cmake -B build/$@ -DTASK=$@
	cmake --build build/$@ -j$$(nproc)
	cd build/$@ && ./$@

view:
	cd build && gtkwave

clean:
	rm -rf build

distclean: clean
	rm -rf .deps
	rm -f semester_*/*/task_*/.clangd
